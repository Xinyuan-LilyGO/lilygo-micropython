
#include "py/runtime.h"
#include "py/obj.h"

#include "mphalport.h"

#include "modepd.h"

STATIC const mp_rom_map_elem_t mp_module_epd_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_epd) },

    { MP_ROM_QSTR(MP_QSTR_EPD47), MP_ROM_PTR(&epd47_make_new_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_epd_globals, mp_module_epd_globals_table);


const mp_obj_module_t mp_module_epd = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_epd_globals,
};

MP_REGISTER_MODULE(MP_QSTR_epd, mp_module_epd, 1);
