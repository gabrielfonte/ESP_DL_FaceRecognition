# ESP DL Face Recognition

This project is a implementation of a face recognition system, designed to work with ESP32. It is based on the [ESP-IDF](https://github.com/espressif/esp-idf) framework and [ESP-DL](https://github.com/espressif/esp-dl) library and also [ESP-Camera](https://github.com/espressif/esp32-camera) library.

It also uses some utilities components of [ESP-WHO](https://github.com/espressif/esp-who) framework, which will be removed in a future release to provide better compatibility with different versions of ESP32 other than ESP-EYE.


## How to Use

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-S3/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A compatible camera such as OV2640, for image acquisition.
* For larger frame sizes, you may need External RAM. 

**Notes** It has been tested on an ESP-EYE DevKit (ESP32 Microcontroller with OV2640 Integrated Camera). For different versions, it needs to be tested. If you have any problems, create an issue describing and the needed changes will be implemented. 


### Configure the Project

Open the project configuration menu (`idf.py menuconfig`). 

In the `Camera Pins` menu:

* Select a Camera from the list or enter the `Custom Camera Pinout` option

In the `WiFi Configuration` menu:

* Insert your SSID in `WiFi SSID` option
* Insert your Password in `WiFi PASSWORD` option
* If needed, you may want to change the number of connection retries on `Maximum Retry`


### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

