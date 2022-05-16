#
# Main component of TF Micro project 'person_detection'.
#

idf_component_register(
    SRCS
        "main.cpp"
        "camera/app_camera_esp.c"
        "camera/utils.cpp"
        "websocket/websocket.cc"
        "wifi/wi-fi.c"
        "detection/detection.cpp"
        "recognizer/recognizer.cpp"
        "http/app_mdns.c"
        "http/http_stream.cpp"
    PRIV_REQUIRES esp32-camera esp-dl spi_flash json nvs_flash freertos esp_websocket_client esp_wifi fb_gfx esp_http_server mdns
    INCLUDE_DIRS "")