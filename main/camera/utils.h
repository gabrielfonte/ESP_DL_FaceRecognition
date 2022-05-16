/*
 * utils.h
 *
 *  Created on: 19 de jan. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_CAMERA_UTILS_H_
#define MAIN_CAMERA_UTILS_H_

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <string>
#include <list>
#include "dl_detect_define.hpp"
#include "esp_camera.h"

typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgb;

typedef unsigned char BYTE;

uint8_t cvtImgRGB565ToRGB888(uint8_t *img_input, uint8_t *img_output, uint32_t size);
uint8_t cvtImgRGB565ToRGB888_16(uint16_t *img_input, uint8_t *img_output, uint32_t size);
uint8_t cvtImgRGB565ToBGR888(uint8_t *img_input, uint8_t *img_output, uint32_t size);
uint8_t cvtfbRGB565TofbBGR888(camera_fb_t *fb_input, camera_fb_t *fb_output);
uint8_t cvtfbRGB565TofbRGB888(camera_fb_t *fb_input, camera_fb_t *fb_output);

std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);

/**
 * @brief Draw detection result on RGB565 image.
 * 
 * @param image_ptr     image
 * @param image_height  height of image
 * @param image_width   width of image
 * @param results       detection results
 */
void draw_detection_result(uint16_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results);

/**
 * @brief Draw detection result on RGB888 image.
 * 
 * @param image_ptr     image
 * @param image_height  height of image
 * @param image_width   width of image
 * @param results       detection results
 */

void draw_detection_result(uint8_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results);

/**
 * @brief Print detection result in terminal
 * 
 * @param results detection results
 */
void print_detection_result(std::list<dl::detect::result_t> &results);

/**
 * @brief Decode fb , 
 *        - if fb->format == PIXFORMAT_RGB565, then return fb->buf
 *        - else, then return a new memory with RGB888, don't forget to free it
 * 
 * @param fb 
 */
void *app_camera_decode(camera_fb_t *fb);


#endif /* MAIN_CAMERA_UTILS_H_ */
