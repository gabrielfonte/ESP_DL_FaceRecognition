/*
 * recognizer.h
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_RECOGNIZER_RECOGNIZER_H_
#define MAIN_RECOGNIZER_RECOGNIZER_H_

#include <cstdint>
#include <cstddef>
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "face_recognition_tool.hpp"
#include "face_recognizer.hpp"
//#include "who_ai_utils.hpp"
#include "dl_detect_define.hpp"
#include "dl_image.hpp"
#include "esp_log.h"
#include "../detection/detection.h"

typedef struct{
    uint8_t *data;
    int size;
    int shape[3]; // <Width, Height, Channels>
}img_type_t;

typedef struct{
    int command;
    std::string name;
    img_type_t img;
}face_register_t;

typedef struct{
    std::string name;
    double score;
}reco_result_t;

void recognizer_init();
void delete_all_ids();
int EnrollIDtoFlash(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name);
int EnrollIDtoFlash(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name);
face_info_t Recognize(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, uint32_t* time_ms);
face_info_t Recognize(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, uint32_t* time_ms);
void AlignFace(uint8_t* src_image, std::vector<int> src_shape, uint8_t *dst_image, std::vector<int> dst_shape, std::list<dl::detect::result_t> detect_results);

#endif /* MAIN_RECOGNIZER_RECOGNIZER_H_ */
