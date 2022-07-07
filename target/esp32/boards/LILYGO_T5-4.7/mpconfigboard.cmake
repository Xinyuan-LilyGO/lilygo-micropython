set(SDKCONFIG_DEFAULTS boards/sdkconfig.base
                       boards/sdkconfig.ble
                       boards/sdkconfig.spiram
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py
                                ${EXTMOD_FROZEN_DIR}/touch/manifest.py
                                ${EXTMOD_FROZEN_DIR}/display/framebuf1/manifest.py
    )
endif()

set(EXTMOD_EPD 1)