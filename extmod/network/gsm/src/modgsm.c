/*
 * This file is part of the MicroPython ESP32 project, https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 LoBo (https://github.com/loboris)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// #ifdef CONFIG_MICROPY_USE_GSM

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tcpip_adapter.h"

#include "py/runtime.h"
#include "py/obj.h"
#include "shared/netutils/netutils.h"
#include "mphalport.h"
#include "libGSM.h"

#include "modgsm.h"

STATIC const mp_rom_map_elem_t mp_module_gsm_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),   MP_ROM_QSTR(MP_QSTR_gsm)               },

    { MP_ROM_QSTR(MP_QSTR_GSM), MP_ROM_PTR(&gsm_make_new_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_gsm_globals, mp_module_gsm_globals_table);


const mp_obj_module_t mp_module_gsm = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_gsm_globals,
};

MP_REGISTER_MODULE(MP_QSTR_gsm, mp_module_gsm, 1);

// #endif
