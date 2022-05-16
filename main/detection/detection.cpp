/*
 * detection.cpp
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */
#include "detection.h"
#include "face_recognition_tool.hpp"

using namespace std;
using namespace dl;
using namespace face_recognition_tool;

static const char *TAG = "DETECTION";
static dl::tool::Latency latency;


HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
HumanFaceDetectMNP01 detector2(0.4F, 0.3F, 10);

uint8_t Inference(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> *results){

	ESP_LOGI(TAG, "Inference initialized");
    latency.start();

	std::list<dl::detect::result_t> &detect_candidates = detector.infer(img_p, {height, width, 3});
	std::list<dl::detect::result_t> &detect_results = detector2.infer(img_p, {height, width, 3}, detect_candidates);

	ESP_LOGI(TAG, "Inference finished");
    latency.end();
    latency.print("Inference latency");

    if(detect_results.size() == 1){
    	ESP_LOGI(TAG, "Detected Face");
        print_detection_result(detect_results);
        *results = detect_results;
        return 1;
    }
    else{
    	ESP_LOGI(TAG, "No Faces Detected");
    	return 0;
    }

}

uint8_t Inference(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> *results){

	ESP_LOGI(TAG, "Inference initialized");
    latency.start();

	std::list<dl::detect::result_t> &detect_candidates = detector.infer(img_p, {height, width, 3});
	std::list<dl::detect::result_t> &detect_results = detector2.infer(img_p, {height, width, 3}, detect_candidates);

	ESP_LOGI(TAG, "Inference finished");
    latency.end();
    latency.print("Inference latency");


    if(detect_results.size() == 1){
    	ESP_LOGI(TAG, "Detected Face");
        print_detection_result(detect_results);
        *results = detect_results;
        return 1;
    }
    else{
    	ESP_LOGI(TAG, "No Faces Detected");
    	return 0;
    }

}


