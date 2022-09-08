

#
# Generate sdkconfig.partitions file
#
file(WRITE ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
           "CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y\n")
file(APPEND ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
            "CONFIG_PARTITION_TABLE_CUSTOM=y\n")
file(APPEND ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
            "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"${CMAKE_CURRENT_LIST_DIR}/partitions.csv\"\n")

set(SDKCONFIG_DEFAULTS ${MICROPY_PORT_DIR}/boards/sdkconfig.240mhz
                       ${MICROPY_PORT_DIR}/boards/sdkconfig.base
                       ${MICROPY_PORT_DIR}/boards/sdkconfig.ble
                       ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.board
                       ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
)

set(EXTMOD_ST7789 1)
