/*
 * utils.c
 *
 *  Created on: 19 de jan. de 2022
 *      Author: gabriel
 */

#include "utils.h"

#define R_BITS 0b1111100000000000
#define G_BITS 0b0000011111100000
#define B_BITS 0b0000000000011111

rgb __convertRgb565ToRgb888(uint16_t pixel){
	rgb rgb_pixel;

	rgb_pixel.r = (pixel & R_BITS) >> 8;
	rgb_pixel.g = (pixel & G_BITS) >> 3;
	rgb_pixel.b = (pixel & B_BITS) << 3;

	return rgb_pixel;
}

uint8_t cvtImgRGB565ToRGB888(uint8_t *img_input, uint8_t *img_output, uint32_t size){
	uint32_t j = 0;
	for(uint32_t i = 0; i < size; i+=2, j+=3){
		uint16_t img_pixel = (img_input[i] << 8) | (img_input[i+1] << 0);
		rgb rgb_pixel = __convertRgb565ToRgb888(img_pixel);
		img_output[j] = rgb_pixel.r;
		img_output[j+1] = rgb_pixel.g;
		img_output[j+2] = rgb_pixel.b;
	}
	return 0;
}

uint8_t cvtImgRGB565ToRGB888_16(uint16_t *img_input, uint8_t *img_output, uint32_t size){
	printf("Started Conversion of RGB565 format (uint16_t[%u]) to RGB888 format (uint8_t[%u])\n", size, size*3);
	uint32_t j = 0;
	for(uint32_t i = 0; i < size; i++, j+=3){
		uint16_t img_pixel = img_input[i];
		rgb rgb_pixel = __convertRgb565ToRgb888(img_pixel);
		img_output[j] = rgb_pixel.r;
		img_output[j+1] = rgb_pixel.g;
		img_output[j+2] = rgb_pixel.b;
	}
	return 0;
}
