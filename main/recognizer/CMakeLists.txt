#
# Camera functions CMakeLists
#

idf_component_register(
    SRCS "recognizer.cpp"
    PRIV_REQUIRES console esp-dl camera esp32-camera spi_flash
    INCLUDE_DIRS "")
