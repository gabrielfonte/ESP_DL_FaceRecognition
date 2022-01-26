
/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "image_provider.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "app_camera_esp.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

camera_fb_t* fb = NULL;
static const char* TAG = "app_camera";

/* Function to initialize camera */
DLStatus InitCamera() {
  int ret = app_camera_init();
  if (ret != 0) {
    ESP_LOGE(TAG, "Camera init failed\n");
    return DLStatusError;
  }
  ESP_LOGI(TAG, "Camera Initialized\n");
  return DLStatusOk;
}

/* Function to Capture an Image */
extern "C" int capture_image() {
  fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
    return -1;
  }
  return 0;
}

/* Capture an image and wait until finished */
DLStatus PerformCapture(uint8_t *image_data) {
  int ret = capture_image();

  if (ret != 0) {
    return DLStatusError;
    ESP_LOGE(TAG, "Capture Failed\n");
  }

  memcpy(image_data, fb->buf, fb->len);

  //image = esp_camera_fb_get();
  ESP_LOGI(TAG, "Image Captured\n");
  esp_camera_fb_return(fb);

  return DLStatusOk;
}

// Get an image from the camera module
DLStatus GetImage(int image_width, int image_height, int channels, uint8_t *data){
  static bool g_is_camera_initialized = false;
  if (!g_is_camera_initialized) {
	  DLStatus_e init_status = InitCamera();
    if (init_status != DLStatusOk) {
      ESP_LOGE(TAG, "InitCamera failed\n");
      return DLStatusError;
    }
    g_is_camera_initialized = true;
  }
  /* Camera Captures Image of size 96 x 96  which is of the format grayscale
   * thus, no need to crop or process further , directly send it to tf */
  DLStatus_e capture_status = PerformCapture(data);

  if (capture_status != DLStatusOk) {
	ESP_LOGE(TAG, "PerformCapture failed\n");
    return capture_status;
  }

  return DLStatusOk;
}

DLStatus ClearBuffer(){
	esp_camera_fb_return(fb);
	return DLStatusOk;
}
