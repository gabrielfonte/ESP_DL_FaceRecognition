/* ESP-DL Face Reconition Project */
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include "face_recognition_tool.hpp"
#include "face_recognition_112_v1_s16.hpp"
#include "face_recognition_112_v1_s8.hpp"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "camera/utils.h"
#include "camera/app_camera_esp.h"
#include "websocket/websocket.h"
#include "wifi/wi-fi.h"
#include "json/cjson.h"
#include "app_httpd.hpp"
#include "app_mdns.h"
#include "app_wifi.h"
#include "esp_log.h"
#include "who_ai_utils.hpp"
#include "dl_image.hpp"
#include "fb_gfx.h"

#define QUANT_TYPE 0 //1 for S16 model and 0 for S8

using namespace std;
using namespace dl;

/* Model import */
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"

static const char *TAG = "Main";

#define CONFIG_S8 1

using namespace std;

websocket_client_config_t config;
TaskHandle_t xCameraTaskHandle = NULL;

static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueHttpFrame = NULL;


#if CONFIG_S8
	FaceRecognition112V1S8 recognizer;
#elif CONFIG_S16
	FaceRecognition112V1S16 recognizer;
#endif


uint8_t Inference(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> *results){

    HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
    HumanFaceDetectMNP01 detector2(0.4F, 0.3F, 10);

	ESP_LOGI(TAG, "Inference initialized");

	std::list<dl::detect::result_t> &detect_candidates = detector.infer(img_p, {height, width, 3});
	std::list<dl::detect::result_t> &detect_results = detector2.infer(img_p, {height, width, 3}, detect_candidates);

	ESP_LOGI(TAG, "Inference finished");

    if(detect_results.size() == 1){
    	ESP_LOGI(TAG, "Detected Face");
        //print_detection_result(detect_results);
        //draw_detection_result(img_p, height, width, detect_results);
        *results = detect_results;
        return 1;
    }
    else{
    	ESP_LOGI(TAG, "No Faces Detected");
    	return 0;
    }

}

void Recognize(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results){

	/*
	face_info_t recognize_result;
	recognize_result = recognizer.recognize(img_p, {height, width, 3}, detect_results.front().keypoint);
	ESP_LOGI("RECOGNIZE", "Face ID %i recognized: Probability %f", recognize_result.id, recognize_result.similarity);
	*/
}

void EnrollIDtoFlash(uint8_t* img_p, int height, int width, std::list<dl::detect::result_t> detect_results, std::string name){

	face_info_t recognize_result;
    print_detection_result(detect_results);

    vector<int> landmarks = detect_results.front().keypoint;
    Tensor<uint8_t> image;

    image.set_element(img_p).set_shape({112, 112, 3}).set_auto_free(false);

    int id = recognizer.enroll_id(image, landmarks, name, false);

    cout << "name: " << name << ", id: " << id << "\n";

    recognize_result = recognizer.recognize(image);

    ESP_LOGI("RECOGNIZE", "Face ID %i recognized: Probability %f", recognize_result.id, recognize_result.similarity);

    /*
    int id = recognizer.enroll_id(img_p, {height, width, 3}, detect_results.front().keypoint, name, false);
    ESP_LOGI("ENROLL", "ID %d is enrolled", id);
    */
}

void captureTask(void *pvParameters){

    camera_fb_t *frame = NULL;
    camera_fb_t frame888 = {0};
    uint8_t *buffer888 = NULL;

	register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 1, xQueueAIFrame);
    register_httpd(xQueueHttpFrame, NULL, false);

	while(1){

		if(xQueueReceive(xQueueAIFrame, &frame, portMAX_DELAY)){

			frame888.len = (frame->len)*1.5;
			frame888.format = PIXFORMAT_RGB888;
			frame888.timestamp = frame->timestamp;
			frame888.width = frame -> width;
			frame888.height = frame -> height;

			buffer888 = (uint8_t*) heap_caps_malloc(frame888.len, MALLOC_CAP_SPIRAM);

			if(cvtImgRGB565ToBGR888(frame->buf, buffer888, frame->len) == 0){
				printf("End of conversion\n");
			}

			frame888.buf = buffer888;

			std::list<dl::detect::result_t> detect_results;
			string name = "Gabriel";


			if(Inference((uint8_t*) frame888.buf, (int) frame888.height, (int) frame888.width, &detect_results)){

				vTaskDelay(pdMS_TO_TICKS(1000));

				EnrollIDtoFlash(frame888.buf, (int) frame888.height, (int) frame888.width, detect_results, name);

				vTaskDelay(pdMS_TO_TICKS(1000));

				//Recognize((uint16_t*) frame->buf, (int) frame->height, (int) frame->width, detect_results);
			}

		}
		/*
		if(xQueueSend(xQueueHttpFrame, &frame, portMAX_DELAY)){
			esp_camera_fb_return(frame);
		}
		*/
		websocket_client_send(frame888.buf, frame888.len);

		esp_camera_fb_return(frame);

		free(buffer888);

		vTaskDelay(pdMS_TO_TICKS(10));

	}
}


extern "C" void app_main(void)
{
	wifi_init();
	printf("End configuration of Wi-fi\n");

	config.uri = "ws://192.168.0.15:3000";
	config.transport = WEBSOCKET_TRANSPORT_OVER_TCP;
	config.send_timeout = portMAX_DELAY;
	//config.username = "ESP32";
	//config.password = "123456";

	websocket_init(&config);
	websocket_client_start();

    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueHttpFrame = xQueueCreate(2, sizeof(camera_fb_t *));

    recognizer.set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    recognizer.set_ids_from_flash();

    //app_wifi_main();
    //app_mdns_main();

	xTaskCreate(captureTask, "Capture", 4*1024, NULL, tskIDLE_PRIORITY, &xCameraTaskHandle);

	while(1){

		printf("\n=========== START HEAP =================\n\t"
			   "Free heap: %d bytes \n\t"
			   "Watermark: %d bytes \n"
			   "=========== STOP HEAP ==================\n\n",
			   esp_get_free_heap_size(), xPortGetMinimumEverFreeHeapSize());


	    vTaskDelay(2000/portTICK_PERIOD_MS);

	}

    /*
    // load ids from flash partition96
    recognizer.set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    recognizer.set_ids_from_flash();

    // prepare image data
    Tensor<uint8_t> image_rgb888_316_1;
    image_rgb888_316_1.set_element((uint8_t *)image_bgr_316_1).set_shape({240, 320, 3}).set_auto_free(false);

    Tensor<uint8_t> image_rgb888_319_1;
    image_rgb888_319_1.set_element((uint8_t *)image_bgr_319_1).set_shape({240, 320, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_316_2;
    aligned_face_rgb888_316_2.set_element((uint8_t *)aligned_face112_image_bgr_316_2).set_shape({112, 112, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_316_3;
    aligned_face_rgb888_316_3.set_element((uint8_t *)aligned_face112_image_bgr_316_3).set_shape({112, 112, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_319_2;
    aligned_face_rgb888_319_2.set_element((uint8_t *)aligned_face112_image_bgr_319_2).set_shape({112, 112, 3}).set_auto_free(false);

    Tensor<uint8_t> aligned_face_rgb888_319_3;
    aligned_face_rgb888_319_3.set_element((uint8_t *)aligned_face112_image_bgr_319_3).set_shape({112, 112, 3}).set_auto_free(false);

    // enroll id
    cout << "\nenroll id ...\n";
    string name_316 = "Sandra";
    string name_319 = "Jiong";
    int id316, id319 = -1;

    //using the bgr888 image with landmarks of human face to enroll face id to flash
    if (recognizer.get_enrolled_id_num() == 0)
    {
        id316 = recognizer.enroll_id(image_rgb888_316_1, landmarks_316_1, name_316, true);
        cout << "name: " << name_316 << ", id: " << id316 << "\n";
    }
    else
    {
        vector<face_info_t> ids = recognizer.get_enrolled_ids();
        for (std::vector<face_info_t>::iterator i = ids.begin(); i != ids.end(); ++i)
        {
            if((*i).name == name_316)
            {
                id316 = (*i).id;
                cout << "ids from flash, name: " << name_316 << ", id: " << id316 << "\n";
            }
        }

    }

    // recognize face
    cout << "\nrecognize face ...\n";

    //recognize the aligned face with format bgr888 (make sure: aligned_face.shape == recognizer.get_input_shape())
    face_info_t recognize_316 = recognizer.recognize(aligned_face_rgb888_316_2);
    cout << "[recognition result] id: " << recognize_316.id << ", name: " << recognize_316.name << ", similarity: " << recognize_316.similarity << "\n";

    // recognizer information
    cout << "\nrecognizer information ...\n";

    cout << "recognizer threshold: " << recognizer.get_thresh() << "\n";
    vector<int> input_shape = recognizer.get_input_shape();
    cout << "input shape: " << input_shape[0] << ", " << input_shape[1] << ", " << input_shape[2] << "\n";

    // face id information
    cout << "\nface id information ...\n";

    cout << "number of enrolled ids: " << recognizer.get_enrolled_id_num() << "\n";
    vector<face_info_t> ids = recognizer.get_enrolled_ids();
    for (std::vector<face_info_t>::iterator i = ids.begin(); i != ids.end(); ++i)
    {
        cout << "id: " << (*i).id << ", name: " << (*i).name << "\n";
    }

    // write enrolled ids to flash partition.
    cout << "write " << recognizer.write_ids_to_flash() << " ids to flash.\n";

    // recognize face
    cout << "\nrecognize face ...\n";
    recognize_316 = recognizer.recognize(aligned_face_rgb888_316_3);
    cout << "[recognition result] id: " << recognize_316.id << ", name: " << recognize_316.name << ", similarity: " << recognize_316.similarity << "\n";

	*/
}
