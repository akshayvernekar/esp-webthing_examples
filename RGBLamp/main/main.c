/* Esp webthing Example
   This code attempts to create a RGB led light with brightness and color control.
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
#include "ws2812_control.h"
#include <stdlib.h>

// Replace with your ssid and password
#define EXAMPLE_ESP_WIFI_SSID      "YOUR_SSID"
#define EXAMPLE_ESP_WIFI_PASS      "YOUR_PASSWORD"

#define EXAMPLE_ESP_MAXIMUM_RETRY  10

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "RGB Lamp";
static int s_retry_num = 0;

static ThingPropertyValue currentColor ;
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


void set_led_color(char* color)
{
    if(!color && (strlen(color)!= 7))
        return;

    char GRB[7] = {0};
    GRB[0] = color[3];
    GRB[1] = color[4];

    GRB[2] = color[1];
    GRB[3] = color[2];

    GRB[4] = color[5];
    GRB[5] = color[6];
    GRB[6] = '\0';
    int intval = strtol(GRB,NULL,16);
    ESP_LOGI(TAG,"Light color str:%s,hex=%6x",GRB,intval);

    //Now set all the Neopixel leds to same color
    struct led_state new_state;
    for(int i = 0 ; i < NUM_LEDS; i++)
    {
        new_state.leds[i] = intval;
     }

    ws2812_write_leds(new_state);
}

void onOffCallback(ThingPropertyValue value)
{
    ESP_LOGI(TAG,"Light was turned %s",value.boolean ? "on" : "off");
    if(value.boolean)
    {
        set_led_color(currentColor.string);
    }
    else
    {
        set_led_color("#000000");
    }
}

void colorChangeCallback(ThingPropertyValue value)
{
    ESP_LOGI(TAG,"Light color is %s",value.string);
    currentColor = value;
    set_led_color(currentColor.string);
}

void brightnessCallback(ThingPropertyValue value)
{
    ESP_LOGI(TAG,"Light brighness is %d",(int)value.number);
}

void app_main()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    ws2812_control_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    Thing* rgbDevice = createThing("RGB Lamp",&deviceTypes);

    PropertyInfo on_info = {
        .type = eON_OFF,
        .value.boolean = false,
    } ;
    ThingProperty* property = createProperty("Power",on_info,&onOffCallback);
    addProperty(rgbDevice,property);

    currentColor.string = "#ffffff";
    PropertyInfo color_info = {
        .type = eCOLOR,
        .value = currentColor,
    } ;
    property = createProperty("Light Color",color_info,&colorChangeCallback);
    addProperty(rgbDevice,property);

    PropertyInfo brightness_info = {
        .type = eBRIGHTNESS,
        .value.number = 50,
    } ;
    property = createProperty("Brightness",brightness_info,&brightnessCallback);
    addProperty(rgbDevice,property);

    initAdapter(rgbDevice);
    wifi_init_sta();

    //wait for device to connect to network
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    startAdapter();
}
