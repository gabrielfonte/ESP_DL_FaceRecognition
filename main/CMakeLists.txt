#
# Main component of TF Micro project 'person_detection'.
#

idf_component_register(
    SRCS
        "main.cpp"
        "camera/app_camera_esp.c"
        "camera/utils.c"
        "websocket/websocket.cc"
        "wifi/wi-fi.c"
        "json/cjson.cc"
        "detection/detection.cpp"
        "recognizer/recognizer.cpp"
    PRIV_REQUIRES esp32-camera esp-dl spi_flash nvs_flash mdns esp_websocket_client esp_http_server esp_wifi json nghttp esp-modules bus screen fb_gfx
    INCLUDE_DIRS "")