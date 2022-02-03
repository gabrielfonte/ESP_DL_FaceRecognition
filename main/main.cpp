#include "main.h"

using namespace std;

static const char *TAG = "Main";

static websocket_client_config_t config;
static TaskHandle_t xCameraTaskHandle = NULL;
static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueHttpFrame = NULL;
static SemaphoreHandle_t xFrameSemaphore = NULL;

void captureTask(void *pvParameters){

    camera_fb_t *frame = NULL;
    uint8_t *decoded_image = NULL;
    int height = 0, width = 0;

    bool id_registered = false;

	register_camera(PIXFORMAT_JPEG, FRAMESIZE_240X240, 1, xQueueAIFrame, xFrameSemaphore);
    register_httpd(xQueueHttpFrame, NULL, false);

	while(1){

		if(xQueueReceive(xQueueAIFrame, &frame, portMAX_DELAY)){

			decoded_image = (uint8_t*) app_camera_decode(frame);
			height = (int) frame->height, width = (int) frame->width;
			esp_camera_fb_return(frame);

			std::list<dl::detect::result_t> detect_results;
			string name = "Gabriel";

			if(Inference(decoded_image, height, width, &detect_results)){

				if(!id_registered){
					EnrollIDtoFlash(decoded_image, height, width, detect_results, name);
					id_registered = true;
				}

				Recognize(decoded_image, height, width, detect_results);
			}

		}
		/*
		if(xQueueSend(xQueueHttpFrame, &frame, portMAX_DELAY)){
			esp_camera_fb_return(frame);
		}
		*/
		websocket_client_send(decoded_image, height*width*3);

		free(decoded_image);

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

    xQueueAIFrame = xQueueCreate(5, sizeof(camera_fb_t *));
    xQueueHttpFrame = xQueueCreate(5, sizeof(camera_fb_t *));
    xFrameSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xFrameSemaphore);

    recognizer_init();

    //app_wifi_main();
    //app_mdns_main();


	xTaskCreate(captureTask, "Capture", 4*1024, NULL, tskIDLE_PRIORITY, &xCameraTaskHandle);

	while(1){

		/*
		printf("\n=========== START HEAP =================\n\t"
			   "Free heap: %d bytes \n\t"
			   "Watermark: %d bytes \n"
			   "=========== STOP HEAP ==================\n\n",
			   esp_get_free_heap_size(), xPortGetMinimumEverFreeHeapSize());
		*/

	    vTaskDelay(10000/portTICK_PERIOD_MS);

	}
}
