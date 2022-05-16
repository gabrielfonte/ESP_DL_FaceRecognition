/*
 * recognizer.c
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */

#include "recognizer.h"
#include "face_recognition_112_v1_s16.hpp"
#include "face_recognition_112_v1_s8.hpp"
#include "fb_gfx.h"
#include "esp_log.h"

using namespace std;
using namespace dl;
using namespace face_recognition_tool;

#define CONFIG_S8 1 

#if CONFIG_S8
    FaceRecognition112V1S8 *recognizer = new FaceRecognition112V1S8();
#elif CONFIG_S16
    FaceRecognition112V1S16 *recognizer = new FaceRecognition112V1S16();
#endif

static dl::tool::Latency latency;

void recognizer_init(){

    if(recognizer->set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr") == 1){
    	ESP_LOGI("MAIN", "ESP PARTITION SET SUCCESSFULLY");
    }
    else {
    	ESP_LOGE("MAIN", "ESP PARTITION NOT SET");
    }

    recognizer->set_ids_from_flash();
}

int EnrollIDtoFlash(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name){
	Tensor<uint8_t> input_face;
	int id;

    vector<int> &landmarks = detect_results.front().keypoint;

    latency.start();

    input_face.set_element(img_p).set_shape({height, width, 3}).set_auto_free(false);
    cout << "Input face ";
    input_face.print_shape();

    //align_face(&input_face, &aligned_face, landmarks);

    if(recognizer->check_partition()>-2){
    	ESP_LOGI("ENROLL", "Partition Found");
        id = recognizer->enroll_id(input_face, landmarks, name, true);
        recognizer->write_ids_to_flash();
        cout << "name: " << name << ", id: " << id << "\n";
    }
    else{
    	ESP_LOGE("ENROLL", "Partition Not Found");
        id = recognizer->enroll_id(input_face, landmarks, name, false);
        cout << "name: " << name << ", id: " << id << "\n";
    }

    latency.end();
    latency.print("Enroll latency");

    return id;
}

int EnrollIDtoFlash(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name){
	int id;

    vector<int> &landmarks = detect_results.front().keypoint;

    latency.start();

    if(recognizer->check_partition()>-1){
    	ESP_LOGI("ENROLL", "Partition Found");
        id = recognizer->enroll_id(img_p, {height, width, 2} ,landmarks, name, false);
        //recognizer->write_ids_to_flash();
        cout << "name: " << name << ", id: " << id << "\n";
    }
    else{
    	ESP_LOGE("ENROLL", "Partition Not Found");
        id = recognizer->enroll_id(img_p, {height, width, 2} ,landmarks, name, false);
        cout << "name: " << name << ", id: " << id << "\n";
    }

    latency.end();
    latency.print("Enroll latency");

    return id;
}

face_info_t Recognize(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, uint32_t* time_ms){
	Tensor<uint8_t> input_face;
    vector<int> &landmarks = detect_results.front().keypoint;
    latency.start();

    input_face.set_element(img_p).set_shape({height, width, 3}).set_auto_free(false);

    cout << "Input face ";
    input_face.print_shape();

	face_info_t recognize_result;
	recognize_result = recognizer->recognize(input_face, landmarks);
	cout << "RECOGNIZE: Face ID " << recognize_result.id << " recognized - " << recognize_result.name << " Similarity: " <<recognize_result.similarity << endl;

    latency.end();
    *time_ms = latency.get_average_period()/1000;
    latency.print("Recognize latency");

    return recognize_result;
}

face_info_t Recognize(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, uint32_t* time_ms){
    vector<int> &landmarks = detect_results.front().keypoint;
    latency.start();

	face_info_t recognize_result;
	recognize_result = recognizer->recognize(img_p, {height, width, 2}, landmarks);
	cout << "RECOGNIZE: Face ID " << recognize_result.id << " recognized - " << recognize_result.name << " Similarity: " <<recognize_result.similarity << endl;

    latency.end();
    latency.print("Recognize latency");

    return recognize_result;
}

void AlignFace(uint8_t* src_image, std::vector<int> src_shape, uint8_t *dst_image, std::vector<int> dst_shape, std::list<dl::detect::result_t> detect_results){

	ESP_LOGI("ALIGN", "Started Face Align");
	int dst_width = dst_shape.at(0), dst_height = dst_shape.at(1), dst_channel = dst_shape.at(2);
	int src_width = src_shape.at(0), src_height = src_shape.at(1), src_channel = src_shape.at(2);
	int total_faces = detect_results.size();

	dst_image = (uint8_t*) heap_caps_malloc(112*112*3, MALLOC_CAP_SPIRAM); //malloc memory for image in spiram

	for(int face=0; face<total_faces; face++){
		ESP_LOGI("ALIGN", "Started Face Crop and Resize, %i of %i", face+1, total_faces);
		dl::image::crop_and_resize(dst_image, dst_width, dst_channel, 0, dst_height, 0, dst_width, src_image, src_height, src_width, src_channel,
				detect_results.front().box[3],
				detect_results.front().box[1],
				detect_results.front().box[0],
				detect_results.front().box[2]);

	/*
		new_kp->at(face).category = detect_results.front().category;
		new_kp->at(face).score = detect_results.front().score;
		new_kp->at(face).box = []
	*/
	}

	ESP_LOGI("ALIGN", "Finished Face Alignment");


}

void delete_all_ids(){
    recognizer->clear_id(true);
}
