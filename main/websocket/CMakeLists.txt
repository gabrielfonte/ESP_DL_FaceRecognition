#
# Websocket CMakeLists
#

idf_component_register(
    SRCS
        "websocket.cc"
    PRIV_REQUIRES esp_websocket_client
    INCLUDE_DIRS "")
