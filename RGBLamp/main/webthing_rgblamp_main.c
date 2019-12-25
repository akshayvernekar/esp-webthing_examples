/* Web Thing Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "web_thing.h"
#include "web_thing_adapter.h"
#include "mdns.h"

/* 
    Change the EXAMPLE_ESP_WIFI_SSID and EXAMPLE_ESP_WIFI_PASS to SSID and password of your Router
*/
#define EXAMPLE_ESP_WIFI_SSID      "YOUR_SSID"
#define EXAMPLE_ESP_WIFI_PASS      "YOUR_PASSWORD"
#define EXAMPLE_ESP_MAXIMUM_RETRY  10

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event 
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "web thing";

static int s_retry_num = 0;

static const char* deviceTypes[] = {"Light", "OnOffSwitch", "ColorControl", NULL};

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        {
            if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
                esp_wifi_connect();
                xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                s_retry_num++;
                ESP_LOGI(TAG,"retry to connect to the AP");
            }
            ESP_LOGI(TAG,"connect to the AP fail\n");
            break;
        }
    default:
        break;
    }
    mdns_handle_system_event(ctx, event);
    return ESP_OK;
}

void wifi_init_sta()
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

void onOffCallback(ThingPropertyValue value)
{
    ESP_LOGI(TAG,"Light was turned %s",value.boolean ? "on" : "off");
}

void colorChangeCallback(ThingPropertyValue value)
{
    ESP_LOGI(TAG,"Light color is %s",value.string);
}

void brightnessCallback(ThingPropertyValue value)
{
    ESP_LOGI(TAG,"Light brighness is %d",(int)value.number);
}

void app_main()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ThingPropertyValue defOn ;
    defOn.boolean = false;

    ThingPropertyValue defColor ;
    defColor.string = "#ffffff";

    ThingPropertyValue defBrightness ;
    defBrightness.number = 50;

    Thing* smallthing = createThing("RGB light",&deviceTypes);
    
    ThingProperty* property = createProperty(eON_OFF,defOn,false,eNONE,0,0,&onOffCallback);
    addProperty(smallthing,property);

    property = createProperty(eCOLOR,defColor,false,eNONE,0,0,&colorChangeCallback);
    addProperty(smallthing,property);

    property = createProperty(eBRIGHTNESS,defBrightness,false,eNONE,0,0,&brightnessCallback);
    addProperty(smallthing,property);

    initAdapter(smallthing);
    wifi_init_sta();

    //wait for device to connect to network 
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    startAdapter();
   // cleanUpThing(smallthing);
}
