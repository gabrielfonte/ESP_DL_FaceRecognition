/*
 * websocket.h
 *
 *  Created on: 20 de jan. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_WEBSOCKET_WEBSOCKET_H_
#define MAIN_WEBSOCKET_WEBSOCKET_H_

#include "esp_websocket_client.h"
#include "../detection/detection.h"

#define WEBSOCKET_TIMEOUT_DEFAULT (TickType_t) (100/portTICK_PERIOD_MS) //default timeout configured as 100ms

typedef struct {
	const char *uri;
    const char *host;
    int port;/*!< Port to connect, default depend on esp_websocket_transport_t (80 or 443) */
    const char *path; /*!< HTTP Path, if not set, default is `/` */
    const char *username;
    const char *password;
    bool disable_auto_reconnect;
    const char *cert_pem;/*!< Pointer to certificate data in PEM or DER format for server verify (with SSL), default is NULL, not required to verify the server. PEM-format must have a terminating NULL-character. DER-format requires the length to be passed in cert_len. */
    size_t cert_len; /*!< Length of the buffer pointed to by cert_pem. May be 0 for null-terminated pem */
    esp_websocket_transport_t transport;/*!< Websocket transport type, see `esp_websocket_transport_t */
    TickType_t send_timeout;
} websocket_client_config_t;

typedef struct{
    uint8_t *data;
    int data_len;
} websocket_data_t;

typedef enum {
	CMD_REGISTER_FACE = 1,
	CMD_REGISTER_FACE_WITH_IMG,
    CMD_EVALUATE_REG_FACE = 99,
    CMD_EVALUATE_INFER_FACE,
}websocket_commands_e;

typedef struct {
    int command;
    std::string name;
    int message_id;
    int status;
    int time_ms;
    std::list<dl::detect::result_t> detect_result;
    face_info_t recognition_result;
    uint8_t* img;
    int img_size;
}websocket_response_t;

#ifdef __cplusplus
extern "C" {
#endif

void websocket_init(websocket_client_config_t *config);
void websocket_client_start(QueueHandle_t xQueue, QueueHandle_t xQueueEvaluate, QueueHandle_t xWebsocketInputQueue, QueueHandle_t xWebsocketOutputQueue);
void websocket_client_stop();
void websocket_client_destroy();
void websocket_client_send(uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_WEBSOCKET_WEBSOCKET_H_ */
