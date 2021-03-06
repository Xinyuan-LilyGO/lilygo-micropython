set(SDKCONFIG_DEFAULTS boards/sdkconfig.base
                       boards/sdkconfig.ble
                       boards/sdkconfig.spiram
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py
                                ${MICROPY_BOARD_DIR}/manifest.py
                                ${EXTMOD_FROZEN_DIR}/network/at/manifest.py
    )
endif()