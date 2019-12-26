LED Bulb web thing.
====================
This project tries to emulate a LED buld using `esp-webthing` component on Esp32. After completion you should be able to Turn on/off the onboard LED on the WROVER board from the things console.

## Components:
* ESP-32 WROVER module or any other Esp development board.

## Configuration:
WiFi : change the value of EXAMPLE_ESP_WIFI_SSID and EXAMPLE_ESP_WIFI_PASSWORD to that of your Access Point in `main.c` .

LED GPIO Pin: The example project uses GPIO2 which is connected to an onboard blue LED , change this by changing the value of LED_GPIO in `main.c`.

## Building and Flashing:
### Eclipse:
Open and flash the project using eclipse as mentioned [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/eclipse-setup.html).

### Command line:
Make sure tool chain and IDF_PATH are set properly. 
Enter the command `make menuconfig` and choose EXIT in case you dont want to change the defaults.
Enter the command `make flash monitor` to build and flash the firmware.

*Code in this repository is in the Public Domain (or CC0 licensed, at your option.)
Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.*
