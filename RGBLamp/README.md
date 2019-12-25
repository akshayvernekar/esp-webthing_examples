# Mozilla web thing example for esp-idf


## How to use example

### Prerequiste

Make sure you have downloaded the latest release of v3.3.1 and are able to compile and run the hello_world code. You can find the getting started guide for esp-idf here:https://docs.espressif.com/projects/esp-idf/en/latest/

### Clone and Configure the project

Checkout the example project outside the esp-idf folder and set the Wifi SSID and PASSWORD .

```
EXAMPLE_ESP_WIFI_SSID      "YOUR_SSID"
EXAMPLE_ESP_WIFI_PASS      "YOUR_PASSWORD"
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make flash monitor
```

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### After Flashing

Make sure that the Gateway and Esp are configured to the same network . Open up the gateway's webpage and press the "+" option to add new device , the example device should get discovered as "RGB light", add it and start controlling from the gateway's webpage.

## Example Output

There is the console output if device connects to AP and device controls are changed:
```
I (1770) RGB Light: got ip:192.168.8.210
I (1770) web_thing_adapter: Starting webthing Server
I (1780) web_thing_adapter: url:/things/89110d8000b6/properties/on
I (1780) web_thing_adapter: url:/things/89110d8000b6/properties/color
I (1790) web_thing_adapter: url:/things/89110d8000b6/properties/brightness
I (13040) RGB Light: Light was turned on
I (17640) RGB Light: Light brighness is 100
I (24620) RGB Light: Light color is #ff0000
```

