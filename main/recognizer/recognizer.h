/*
 * recognizer.h
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_RECOGNIZER_RECOGNIZER_H_
#define MAIN_RECOGNIZER_RECOGNIZER_H_

#include <cstdint>
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>

#include "face_recognition_tool.hpp"
#include "face_recognizer.hpp"
#include "face_recognition_112_v1_s16.hpp"
#include "face_recognition_112_v1_s8.hpp"
#include "who_ai_utils.hpp"
#include "dl_image.hpp"
#include "fb_gfx.h"

#include "esp_log.h"
#include "../detection/detection.h"

#define CONFIG_MFN_V1 1
#define CONFIG_S16 1

void recognizer_init();

void EnrollIDtoFlash(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name);
void EnrollIDtoFlash(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name);
void Recognize(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results);
void Recognize(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results);


#endif /* MAIN_RECOGNIZER_RECOGNIZER_H_ */
