/*
 * main.h
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include <stdio.h>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "camera/utils.h"
#include "camera/app_camera_esp.h"
#include "websocket/websocket.h"
#include "wifi/wi-fi.h"
#include "http/app_mdns.h"
//#include "app_httpd.hpp"
//#include "who_motion_detection.hpp"
//#include "app_mdns.h"
//#include "app_wifi.h"
#include "esp_log.h"
//#include "who_ai_utils.hpp"
#include "dl_image.hpp"
#include "fb_gfx.h"
#include "detection/detection.h"
#include "http/http_stream.h"

typedef struct{
    uint16_t id;
    std::string name;
    uint8_t *img;
    std::vector<int> shape;
}new_person;

/* ESP-DL Face Recognition Project */

#endif
