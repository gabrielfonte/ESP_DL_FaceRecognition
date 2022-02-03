/*
 * detection.h
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_DETECTION_DETECTION_H_
#define MAIN_DETECTION_DETECTION_H_

#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include "../recognizer/recognizer.h"
#include "esp_log.h"
#include "who_ai_utils.hpp"
#include "dl_image.hpp"
#include "fb_gfx.h"

/* Model import */
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"

uint8_t Inference(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> *results);
uint8_t Inference(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> *results);

#endif /* MAIN_DETECTION_DETECTION_H_ */
