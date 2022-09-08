set(IDF_TARGET esp32s3)

#
# Generate sdkconfig.partitions file
#
file(WRITE ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
           "CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y\n")
file(APPEND ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
            "CONFIG_PARTITION_TABLE_CUSTOM=y\n")
file(APPEND ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
            "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"${CMAKE_CURRENT_LIST_DIR}/partitions.csv\"\n")

set(SDKCONFIG_DEFAULTS boards/sdkconfig.base
                       boards/sdkconfig.usb
                       boards/sdkconfig.ble
                       boards/sdkconfig.240mhz
                       ${MICROPY_BOARD_DIR}/sdkconfig.board
                       ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.partitions
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py
                                ${EXTMOD_FROZEN_DIR}/touch/manifest.py
                                ${EXTMOD_FROZEN_DIR}/display/framebuf1/manifest.py)
endif()

set(EXTMOD_EPD 1)
