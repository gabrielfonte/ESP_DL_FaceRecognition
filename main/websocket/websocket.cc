/*
 * websocket.cpp
 *
 *  Created on: 20 de jan. de 2022
 *      Author: gabriel
 */

#include "websocket.h"
#include "esp_log.h"
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "cstring"

static esp_websocket_client_config_t client_config;
static esp_websocket_client_handle_t client_handle;
static TickType_t websocketTimeout = WEBSOCKET_TIMEOUT_DEFAULT;
static QueueHandle_t xQueueWebsocket = NULL;
static QueueHandle_t xQueueWebsocketInput = NULL;
static QueueHandle_t xQueueWebsocketOutput = NULL;
static QueueHandle_t xQueueRegister = NULL;
static QueueHandle_t xQueueEvaluation = NULL;
static const char *TAG = "WEBSOCKET";
static char receive_buffer[1024] = {0};
static TaskHandle_t xWebsocketTaskHandle = NULL;
static websocket_response_t response;
static uint8_t *img_buffer = NULL;

void websocketTask(void *pvParameters){
	for(;;){
		esp_websocket_event_data_t ws_data;
		if(xQueueReceive(xQueueWebsocket, &ws_data, portTICK_PERIOD_MS)){
			//ESP_LOGI("REGISTER", "Received by the Register Queue");

			bool validation;
			bool evaluate_reg, evaluate_infer;

			cJSON* data = cJSON_Parse(ws_data.data_ptr);
			printf("%s", cJSON_Print(data));

			cJSON* command = cJSON_GetObjectItemCaseSensitive(data, "command");
			cJSON* name = cJSON_GetObjectItemCaseSensitive(data, "name");

			face_register_t face_info;

			if(command->valueint == CMD_REGISTER_FACE){
				ESP_LOGI("REGISTER", "Received CMD_REGISTER_FACE");
				face_info.command = command->valueint;
				face_info.name = name->valuestring;
				validation = true;
			}
			else if(command->valueint == CMD_REGISTER_FACE_WITH_IMG){
				ESP_LOGI("REGISTER", "Received CMD_REGISTER_FACE_WITH_IMG");
				cJSON* img = cJSON_GetObjectItemCaseSensitive(data, "img");
				cJSON* shape = cJSON_GetObjectItemCaseSensitive(data, "shape");
				int img_size = cJSON_GetArraySize(img);
				face_info.command = command->valueint;
				face_info.name = name->valuestring;
				//face_info.img.data = (uint8_t*) command->valuestring;
				//face_info.img.size = img_size;
				//face_info.img.shape = {cJSON_GetArrayItem(shape, 0)->valueint, cJSON_GetArrayItem(shape, 1)->valueint};
				validation = true;
			}
			else if((command->valueint == CMD_EVALUATE_REG_FACE) || (command->valueint == CMD_EVALUATE_INFER_FACE)){
				cJSON* img = cJSON_GetObjectItemCaseSensitive(data, "data");
				cJSON* shape = cJSON_GetObjectItemCaseSensitive(data, "shape");
				face_info.command = command->valueint;
				face_info.name = name->valuestring;
				cJSON* w = cJSON_GetArrayItem(shape, 0);
				cJSON* h = cJSON_GetArrayItem(shape, 1);
				cJSON* c = cJSON_GetArrayItem(shape, 2);
				face_info.img.shape[0] = (w->valueint);
				face_info.img.shape[1] = (h->valueint);
				face_info.img.shape[2] = (c->valueint);
				std::vector<BYTE> decoded_image = base64_decode(std::string(img->valuestring));
				int image_size = (int) decoded_image.size();
				face_info.img.data = (uint8_t*) malloc(image_size);
				face_info.img.size = image_size;
				memcpy((uint8_t*) face_info.img.data, (uint8_t*) decoded_image.data(), image_size);
				validation = false;

				printf("Image has the format: {%d, %d, %d}, len: %d and address: %p\n", w->valueint, h->valueint, c->valueint, image_size, face_info.img.data);
				/*
				printf("Image: [");
				for(int i=0; i<image_size; i++){
					printf("%d, ", face_info.img.data[i]);
				}
				printf("]\n");*/

				if(command->valueint == CMD_EVALUATE_REG_FACE){
					ESP_LOGI("EVALUATE", "Received CMD_EVALUATE_REG_FACE");
				}
				else{
					ESP_LOGI("EVALUATE", "Received CMD_EVALUATE_INFER_FACE");
				}
				xQueueSend(xQueueEvaluation, &face_info, portMAX_DELAY);
			}
			else{
				ESP_LOGE(TAG, "INVALID COMMAND");
				validation = false;
			}

			if(validation == true){
				if(xQueueSend(xQueueRegister, &face_info, portMAX_DELAY)){
					//ESP_LOGI("REGISTER", "Sent to Register Queue");
				}
			}

			//memset(receive_buffer, 0, sizeof(receive_buffer));
			cJSON_Delete(data);
		}

		else if(xQueueReceive(xQueueWebsocketInput, &response, portTICK_PERIOD_MS)){
			//ESP_LOGI("QUEUE", "Received by the Websocket Input Queue");

			cJSON* packet = cJSON_CreateObject();

			if(response.command != 0){
				cJSON_AddNumberToObject(packet, "command", response.command);
			}

			if(response.img != NULL){
				//websocket_client_send(response.img, response.img_size);
			}

			if(response.message_id != 0){
				cJSON_AddNumberToObject(packet, "id", response.message_id);
			}

			if(response.time_ms != 0){
				cJSON_AddNumberToObject(packet, "latency", response.time_ms);
			}

			if(response.name != ""){
				cJSON_AddStringToObject(packet, "name", response.name.c_str());
			}

			if(response.status != 0){
				cJSON_AddNumberToObject(packet, "status", response.status);
			}

			char *formated_response = cJSON_PrintUnformatted(packet);
			int response_len = (int) strlen(formated_response);

			websocket_client_send((uint8_t*) formated_response, response_len);

			ESP_LOGI("QUEUE", "Message sent to Websocket");

			cJSON_Delete(packet);
		}

		//free((void*) ws_data.data_ptr);
		ws_data.data_ptr = NULL;
	}

}

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        //ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        //ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
        ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);
		if(data->op_code == 0 || data->op_code == 1 || data->op_code == 2){
			if((data->data_len+data->payload_offset) <= data->payload_len){
				if(data->payload_offset == 0){
					img_buffer = (uint8_t*) malloc(data->payload_len);
				}
				if(img_buffer != NULL){
					memcpy((uint8_t*) img_buffer+(data->payload_offset), (uint8_t*)(data->data_ptr), data->data_len);
					if((data->data_len+data->payload_offset) == data->payload_len){
						data->data_ptr = (const char*) img_buffer;
						xQueueSend(xQueueWebsocket, data, portMAX_DELAY);
						img_buffer = NULL;
					}
				}
				else{
					ESP_LOGE(TAG, "IMG BUFFER COULD NOT BE ALLOCATED");
				}
			}
			else{
				if(xQueueSend(xQueueWebsocket, data, portMAX_DELAY) != pdTRUE){
					ESP_LOGE(TAG, "Failed to Queue New Data");
				}
			}
		}
		break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}

void websocket_init(websocket_client_config_t *config){
	client_config.uri = config->uri;
	client_config.host = config->host;
	client_config.port = config->port;
	client_config.path = config->path;
	client_config.cert_pem = config->cert_pem;
	client_config.cert_len = config->cert_len;
	client_config.username = config->username;
	client_config.password = config->password;
	client_config.disable_auto_reconnect = config->disable_auto_reconnect;
	client_config.transport = config->transport;

	if(config->send_timeout != 0)
		websocketTimeout = config->send_timeout;

	client_handle = esp_websocket_client_init(&client_config);
	esp_websocket_register_events(client_handle, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client_handle);
}

void websocket_client_start(QueueHandle_t xQueue, QueueHandle_t xQueueEvaluate, QueueHandle_t xWebsocketInputQueue, QueueHandle_t xWebsocketOutputQueue){
	esp_websocket_client_start(client_handle);
	xQueueWebsocket = xQueueCreate(1, sizeof(esp_websocket_event_data_t));
	xQueueRegister = xQueue;
	xQueueEvaluation = xQueueEvaluate;
	xQueueWebsocketInput = xWebsocketInputQueue;
	xQueueWebsocketOutput = xWebsocketOutputQueue;
	xTaskCreate(websocketTask, "Websocket", 8*1024, NULL, 6, &xWebsocketTaskHandle);
}

void websocket_client_stop(){
	esp_websocket_client_stop(client_handle);
	if(xQueueWebsocket != NULL){
		vQueueDelete(xQueueWebsocket);
	}
}

void websocket_client_destroy(){
	esp_websocket_client_destroy(client_handle);
}

void websocket_client_send(uint8_t *data, int len){
	printf("Sending %i bytes to Websocket\n", len);
	esp_websocket_client_send(client_handle, (char*) data, (int)len, websocketTimeout);
}

