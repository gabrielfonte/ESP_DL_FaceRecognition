/*
 * recognizer.c
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */

#include "recognizer.h"

using namespace std;
using namespace dl;
using namespace face_recognition_tool;

#if CONFIG_MFN_V1
#if CONFIG_S8
    FaceRecognition112V1S8 recognizer;
#elif CONFIG_S16
    FaceRecognition112V1S16 recognizer;
#endif
#endif

static dl::tool::Latency latency;

void recognizer_init(){

    if(recognizer.set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr") == 1){
    	ESP_LOGI("MAIN", "ESP PARTITION SET SUCCESSFULLY");
    }
    else {
    	ESP_LOGE("MAIN", "ESP PARTITION NOT SET");
    }

    recognizer.set_ids_from_flash();
}

void EnrollIDtoFlash(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name){
	Tensor<uint8_t> input_face;
	Tensor<uint8_t> aligned_face;
	int id;

    vector<int> &landmarks = detect_results.front().keypoint;

    latency.start();

    input_face.set_element(img_p).set_shape({height, width, 3}).set_auto_free(false);
    cout << "Input face ";
    input_face.print_shape();

    //align_face(&input_face, &aligned_face, landmarks);

    //cout << "Aligned face ";
    //aligned_face.print_shape();

    if(recognizer.check_partition()>-1){
    	ESP_LOGI("ENROLL", "Partition Found");
        id = recognizer.enroll_id(input_face, landmarks, name, true);
        cout << "name: " << name << ", id: " << id << "\n";
    }
    else{
    	ESP_LOGE("ENROLL", "Partition Not Found");
        id = recognizer.enroll_id(input_face, landmarks, name, false);
        cout << "name: " << name << ", id: " << id << "\n";
    }

    latency.end();
    latency.print("Enroll latency");
    /*
	recognize_result = recognizer->recognize(aligned_face);
	ESP_LOGI("RECOGNIZE", "Face ID %i recognized: Probability %f", recognize_result.id, recognize_result.similarity);
	*/
}

void EnrollIDtoFlash(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name){
	Tensor<uint8_t> input_face;
	Tensor<uint8_t> aligned_face;
	int id;

    latency.start();

    vector<int> &landmarks = detect_results.front().keypoint;

    face_recognition_tool::align_face(img_p, {height, width, 3},&aligned_face, landmarks);

    if(recognizer.check_partition()>-1){
    	ESP_LOGI("ENROLL", "Partition Found");
        id = recognizer.enroll_id(aligned_face, landmarks, name, true);
        cout << "name: " << name << ", id: " << id << "\n";
    }
    else{
    	ESP_LOGE("ENROLL", "Partition Not Found");
        id = recognizer.enroll_id(aligned_face, landmarks, name, false);
        cout << "name: " << name << ", id: " << id << "\n";
    }

    latency.end();
    latency.print("Enroll latency");
}

void Recognize(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results){
	Tensor<uint8_t> input_face;
    vector<int> &landmarks = detect_results.front().keypoint;
    latency.start();

    input_face.set_element(img_p).set_shape({height, width, 3}).set_auto_free(false);
    cout << "Input face ";
    input_face.print_shape();

	face_info_t recognize_result;
	recognize_result = recognizer.recognize(input_face, landmarks);
	ESP_LOGI("RECOGNIZE", "Face ID %i recognized: Probability %f", recognize_result.id, recognize_result.similarity);

    latency.end();
    latency.print("Enroll latency");
}

void Recognize(uint16_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results){
	face_info_t recognize_result;
	recognize_result = recognizer.recognize(img_p, {height, width, 3}, detect_results.front().keypoint);
	ESP_LOGI("RECOGNIZE", "Face ID %i recognized: Probability %f", recognize_result.id, recognize_result.similarity);
}
