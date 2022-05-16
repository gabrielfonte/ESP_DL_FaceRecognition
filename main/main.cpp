#include "main.h"

using namespace std;

static const char *TAG = "Main";
static websocket_client_config_t config;
static TaskHandle_t xCameraTaskHandle = NULL;
static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueRecognition = NULL;
static QueueHandle_t xQueueHTTP = NULL;
static QueueHandle_t xQueueRegister = NULL;
static QueueHandle_t xQueueEvaluate = NULL;
static QueueHandle_t xQueueWebsocket = NULL;
static QueueHandle_t xWebsocketResponseQueue = NULL;
static SemaphoreHandle_t xFrameSemaphore = NULL;

void captureTask(void *pvParameters){

	bool recognize = true;
	camera_fb_t *frame = NULL;
	int height = 0, width = 0;
	uint8_t *decoded_image = NULL;
	websocket_response_t response = {0};
	face_register_t new_face;
	bool register_face = false;

	register_camera(PIXFORMAT_YUV422, FRAMESIZE_VGA, 1, xQueueRecognition, xFrameSemaphore);

	while(1){
		bool received_frame = false;
		if(xQueueReceive(xQueueRecognition, &frame, portTICK_PERIOD_MS)){

			height = (int) frame->height, width = (int) frame->width;
			decoded_image = (uint8_t *) app_camera_decode(frame);

			if(frame->format != PIXFORMAT_RGB565){
				frame->format = PIXFORMAT_RGB888;
				frame->len = height*width*3;
				free(frame->buf);	
				frame->buf = (uint8_t*) decoded_image;
			}

			received_frame = true;
		}

		if(xQueueReceive(xQueueRegister, &new_face, portTICK_PERIOD_MS)){
			cout << "New register required for " << new_face.name << endl;
			if(new_face.command == CMD_REGISTER_FACE){
				register_face = true;
			}
		}

		if(received_frame){
		
			std::list<dl::detect::result_t> detect_results;
			uint32_t time_ms = 0;

			if(Inference(decoded_image, height, width, &detect_results)){
				if(recognize){
					if(register_face){
						response.command = new_face.command;
						response.name = new_face.name;
						if(EnrollIDtoFlash(decoded_image, height, width, detect_results, new_face.name) > 0){
							response.detect_result = detect_results;
							response.status = 200;
						}
						else{
							response.status = 507;
						}
					}
					else{
						response.detect_result = detect_results;
						response.recognition_result = Recognize(decoded_image, height, width, detect_results, &time_ms);
						if(response.recognition_result.id > 0){
							response.status = 200;
							fb_gfx_printf(frame, detect_results.front().box[0], detect_results.front().box[1]-30, 0xF800, "%s %.2f",
							response.recognition_result.name.c_str(), response.recognition_result.similarity); 
						}
						else{
							response.status = 404;
							fb_gfx_printf(frame, detect_results.front().box[0], detect_results.front().box[1]-30, 0x07E0, "%s %.2f", 
							response.recognition_result.name.c_str(), response.recognition_result.similarity);
						}
					}
					xQueueSend(xQueueWebsocket, &response, (TickType_t) 0);
				}
				draw_detection_result(decoded_image, height, width, detect_results);
				//response.img = (uint8_t*) decoded_image;
				//response.img_size = frame->len;
			}
			else{
				response.command = new_face.command;
				response.name = new_face.name;
				response.status = 404;
				xQueueSend(xQueueWebsocket, &response, (TickType_t) 0);
			}

			if(is_http_streaming()){
				if(xQueueSend(xQueueHTTP, &frame, (TickType_t) portMAX_DELAY) != pdTRUE){
					esp_camera_fb_return(frame);
				}
			}
			else{
				esp_camera_fb_return(frame);
			}
			//esp_camera_fb_return(frame);
			decoded_image = NULL;

		}
	}
}

void evaluationTask(void *pvParameters){

	camera_fb_t *frame = NULL;
	int height = 0, width = 0, channels = 0;
	websocket_response_t response = {0};
	face_register_t new_face;

	while(1){
		if(xQueueReceive(xQueueEvaluate, &new_face, portMAX_DELAY)){

			cout << "Received by xQueueEvaluate" << endl;
			//vTaskSuspend(xCameraTaskHandle);

			width = new_face.img.shape[0]; height = new_face.img.shape[1], channels = new_face.img.shape[2];
			printf("Received image from queue with size {%d, %d, %d}, size %d and address: %p \n", width, height, channels, new_face.img.size, new_face.img.data);

			uint8_t *decoded_image = (uint8_t *) malloc(width*height*channels);

			if(fmt2rgb888((uint8_t*) new_face.img.data, new_face.img.size, PIXFORMAT_JPEG, decoded_image)){
				ESP_LOGI("CONVERSION", "END OF JPEG->RGB CONVERSION");
			}
			else{
				ESP_LOGE("CONVERSION", "CONVERSION FAILED");
			}

			std::list<dl::detect::result_t> detect_results;
			uint32_t time_ms = 0;

			if(Inference(decoded_image, height, width, &detect_results)){
				if(new_face.command == CMD_EVALUATE_REG_FACE){
					response.command = new_face.command;
					response.name = new_face.name;
					if(EnrollIDtoFlash(decoded_image, height, width, detect_results, new_face.name) > 0){
						response.detect_result = detect_results;
						response.status = 200;
					}
					else{
						response.status = 507;
					}
				}
				else{
					response.detect_result = detect_results;
					response.command = new_face.command;
					response.recognition_result = Recognize(decoded_image, height, width, detect_results, &time_ms);
					if(response.recognition_result.id > 0){
						response.status = 200;

					}
					else{
						response.status = 404;
					}
					response.name = new_face.name;
					response.time_ms = (int) time_ms;
				}
				xQueueSend(xQueueWebsocket, &response, (TickType_t) 0);
				draw_detection_result(decoded_image, height, width, detect_results);
				//response.img = (uint8_t*) decoded_image;
				//response.img_size = height*width*3;
				free(decoded_image);
			}
			else{
				response.command = new_face.command;
				response.name = new_face.name;
				response.status = 404;
				xQueueSend(xQueueWebsocket, &response, (TickType_t) 0);
			}
			free(new_face.img.data);
			new_face.img.data = NULL;
		}
	}
}


extern "C" void app_main(void)
{
	wifi_init();
	//pp_wifi_main();
    app_mdns_main();
	//printf("End configuration of Wi-fi\n");

	config.uri = "ws://192.168.0.16:3000";
	config.transport = WEBSOCKET_TRANSPORT_OVER_TCP;
	config.send_timeout = portMAX_DELAY;
	//config.username = "ESP32";
	//config.password = "123456";

    //xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
	xQueueWebsocket = xQueueCreate(1, sizeof(websocket_response_t));
	xWebsocketResponseQueue = xQueueCreate(1, sizeof(bool));
    xQueueRecognition = xQueueCreate(2, sizeof(camera_fb_t *));
	xQueueHTTP = xQueueCreate(2, sizeof(camera_fb_t *));
	xQueueRegister = xQueueCreate(1, sizeof(face_register_t));
	xQueueEvaluate = xQueueCreate(1, sizeof(face_register_t));
    xFrameSemaphore = xSemaphoreCreateBinary();

    xSemaphoreGive(xFrameSemaphore);

	websocket_init(&config);
	websocket_client_start(xQueueRegister, xQueueEvaluate, xQueueWebsocket, xWebsocketResponseQueue);

    recognizer_init();
	delete_all_ids();
	start_http_stream_server(xQueueHTTP, NULL, true);
	//register_httpd(xQueueHTTP, NULL, true);

	//xTaskCreate(captureTask, "Capture", 4*1024, NULL, tskIDLE_PRIORITY, &xCameraTaskHandle);
	xTaskCreate(evaluationTask, "Evaluation", 4*1024, NULL, tskIDLE_PRIORITY, NULL);

	while(1){

		printf("\n=========== START HEAP =================\n\t"
			   "Free heap: %d bytes \n\t"
			   "Watermark: %d bytes \n"
			   "=========== STOP HEAP ==================\n\n",
			   esp_get_free_heap_size(), xPortGetMinimumEverFreeHeapSize());

	    vTaskDelay(10000/portTICK_PERIOD_MS);

	}
}