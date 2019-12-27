RGB Lamp web thing.
====================
This project tries to emulate a RGB LED lamp using `esp-webthing` component on Esp32. After completion you should be able to Turn on/off and change the color of the LED Ring from the things console.

![Alt Text](../images/rgbLight.GIF)

## Components:
* ESP-32 WROVER module or any other Esp development board.
* NeoPixel Ring - 8 x WS2812.

## Configuration:
WiFi : Change the value of EXAMPLE_ESP_WIFI_SSID and EXAMPLE_ESP_WIFI_PASSWORD to that of your Access Point.

NeoPixel: The example project uses GPIO14 as NeoPixel's TX pin , change this by changing the value of LED_RMT_TX_GPIO in `ws2812_control.h`.

## Building and Flashing:
### Eclipse:
Open and flash the project using eclipse as mentioned [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/eclipse-setup.html).

### Command line:
Make sure tool chain and IDF_PATH are set properly. <br/>
Enter the command `idf.py menuconfig` and choose EXIT in case you dont wish to change the defaults.<br/>
Enter the command `idf.py build` to build <br/>
Enter the command `idf.py -p PORT [-b BAUD] flash` to flash<br/>

*Code in this repository is in the Public Domain (or CC0 licensed, at your option.)
Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.*
