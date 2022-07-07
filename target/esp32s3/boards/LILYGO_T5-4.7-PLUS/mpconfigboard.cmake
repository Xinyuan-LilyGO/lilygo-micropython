set(IDF_TARGET esp32s3)

set(SDKCONFIG_DEFAULTS boards/sdkconfig.base
                       boards/sdkconfig.usb
                       ${MICROPY_BOARD_DIR}/sdkconfig.board
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py
                                ${EXTMOD_FROZEN_DIR}/touch/manifest.py
                                ${EXTMOD_FROZEN_DIR}/display/framebuf1/manifest.py)
endif()
set(EXTMOD_EPD 1)
