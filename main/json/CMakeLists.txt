#
# Json CMakeLists
#

idf_component_register(
    SRCS
        "cjson.cc"
    PRIV_REQUIRES json esp_system
    INCLUDE_DIRS "")
