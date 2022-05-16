#
# Camera functions CMakeLists
#

idf_component_register(
    SRCS "http_stream.cpp" "app_mdns.c"
    PRIV_REQUIRES  esp32-camera esp-dl nvs_flash esp_http_server
    INCLUDE_DIRS "")
