/*
 * websocket.h
 *
 *  Created on: 20 de jan. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_WEBSOCKET_WEBSOCKET_H_
#define MAIN_WEBSOCKET_WEBSOCKET_H_

#include "esp_websocket_client.h"

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


#ifdef __cplusplus
extern "C" {
#endif

void websocket_init(websocket_client_config_t *config);
void websocket_client_start();
void websocket_client_stop();
void websocket_client_destroy();
void websocket_client_send(uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_WEBSOCKET_WEBSOCKET_H_ */
