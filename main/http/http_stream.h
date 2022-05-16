#ifndef MAIN_HTTP_HTTP_STREAM_
#define MAIN_HTTP_HTTP_STREAM_

#include <cstdint>
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

void start_http_stream_server(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb);
bool is_http_streaming();

#endif
