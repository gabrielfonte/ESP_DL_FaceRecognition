/*
 * websocket.cpp
 *
 *  Created on: 20 de jan. de 2022
 *      Author: gabriel
 */

#include "websocket.h"

static esp_websocket_client_config_t client_config;
static esp_websocket_client_handle_t client_handle;
static TickType_t websocketTimeout = WEBSOCKET_TIMEOUT_DEFAULT;

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
}

void websocket_client_start(){
	esp_websocket_client_start(client_handle);
}

void websocket_client_stop(){
	esp_websocket_client_stop(client_handle);
}

void websocket_client_destroy(){
	esp_websocket_client_destroy(client_handle);
}

void websocket_client_send(uint8_t *data, int len){
	printf("Sending %i bytes to Websocket\n", len);
	esp_websocket_client_send(client_handle, (char*) data, (int)len, websocketTimeout);
}

