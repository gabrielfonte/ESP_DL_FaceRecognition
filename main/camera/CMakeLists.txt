#
# Camera functions CMakeLists
#

idf_component_register(
    SRCS
        "image_provider.cc"
        "app_camera_esp.c"
        "utils.cpp"
    PRIV_REQUIRES console spi_flash freertos
    INCLUDE_DIRS "")
