#include "http_stream.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "http_parser.h"
#include "esp_http_server.h"

static const char *TAG = "camera_httpd";
#define PART_BOUNDARY "123456789000000000000987654321"

httpd_handle_t stream_httpd = NULL;
static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static bool gReturnFB = true;
static bool streaming = false;

static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *frame = NULL;
    struct timeval _timestamp;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[128];
    streaming = true;

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

    while (true)
    {
        if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
        {
            printf("Received an HTTP Frame\n");
            _timestamp.tv_sec = frame->timestamp.tv_sec;
            _timestamp.tv_usec = frame->timestamp.tv_usec;

            if (frame->format == PIXFORMAT_JPEG)
            {
                _jpg_buf = frame->buf;
                _jpg_buf_len = frame->len;
            }
            else if (!frame2jpg(frame, 80, &_jpg_buf, &_jpg_buf_len))
            {
                ESP_LOGE(TAG, "JPEG compression failed");
                res = ESP_FAIL;
            }
        }
        else
        {
            streaming = false;
            res = ESP_FAIL;
        }

        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }

        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }

        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }

        if (frame->format != PIXFORMAT_JPEG)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }

        if (xQueueFrameO)
        {
            xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
        }
        else if (gReturnFB)
        {
            esp_camera_fb_return(frame);
        }
        else
        {
            free(frame);
        }

        if (res != ESP_OK)
        {
            break;
        }
    }

    return res;
}

void start_http_stream_server(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb){
    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    gReturnFB = return_fb;

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL};

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}

bool is_http_streaming(){
    return streaming;
}