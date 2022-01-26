#
# Camera functions CMakeLists
#

idf_component_register(
    SRCS
        "image_provider.cc"
        "app_camera_esp.c"
        "utils.c"
    PRIV_REQUIRES console esp32-camera spi_flash
    INCLUDE_DIRS "")
