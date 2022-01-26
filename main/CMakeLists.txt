#
# Main component of TF Micro project 'person_detection'.
#

idf_component_register(
    SRCS
        "main.cpp"
        "camera/image_provider.cc"
        "camera/app_camera_esp.c"
        "camera/utils.c"
        "websocket/websocket.cc"
        "wifi/wi-fi.c"
        "json/cjson.cc"
    PRIV_REQUIRES esp32-camera esp-dl spi_flash nvs_flash esp_websocket_client esp_wifi json
    INCLUDE_DIRS "")
