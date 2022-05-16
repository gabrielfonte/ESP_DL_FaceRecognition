
#
# Main component of TF Micro project 'person_detection'.
#

idf_component_register(
    SRCS "wi-fi.c" "esp_mdns.c"
    PRIV_REQUIRES esp_wifi spi_flash nvs_flash http mdns
    INCLUDE_DIRS "")
