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

typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgb;


#ifdef __cplusplus
extern "C" {
#endif
uint8_t cvtImgRGB565ToRGB888(uint8_t *img_input, uint8_t *img_output, uint32_t size);
uint8_t cvtImgRGB565ToRGB888_16(uint16_t *img_input, uint8_t *img_output, uint32_t size);
#ifdef __cplusplus
}
#endif

#endif /* MAIN_CAMERA_UTILS_H_ */
