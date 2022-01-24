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

// #include "sdkconfig.h"

// #ifdef CONFIG_MICROPY_USE_GSM

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objtype.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"
#include "lwip/dns.h"
#include "mphalport.h"
#include "libGSM.h"

#include "tcpip_adapter.h"

typedef struct gsm_if_obj_t {
    mp_obj_base_t base;
    ts_GSM       *gsm;
} gsm_if_obj_t;

const mp_obj_type_t gsm_if_type;

STATIC mp_obj_t gsm_make_new(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        {MP_QSTR_uart_num, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1}           },
        {MP_QSTR_tx,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1}           },
        {MP_QSTR_rx,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1}           },
        {MP_QSTR_rts,      MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = -1}           },
        {MP_QSTR_cts,      MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = -1}           },
        {MP_QSTR_baudrate, MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 115200}       },
        {MP_QSTR_apn,      MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none}},
        {MP_QSTR_roaming,  MP_ARG_KW_ONLY  | MP_ARG_BOOL,{.u_bool = false}       },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    int32_t err = 0;
    ts_GSM *psGSM = NULL;

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[0].u_int < 0 || \
        args[1].u_int < 0 || \
        args[2].u_int < 0)
    {
        err = -11;
        goto exit;
    }

    psGSM = ppposInit(args[0].u_int, // uart num
                      args[1].u_int, // tx pin
                      args[2].u_int, // rx pin
                      args[3].u_int, // rts pin
                      args[4].u_int, // cts pin
                      args[5].u_int, // baudrate
                      mp_obj_str_get_str(args[6].u_obj), // apn
                      args[7].u_bool); // roaming
    if (psGSM)
    {
        gsm_if_obj_t *self = m_new_obj_with_finaliser(gsm_if_obj_t);

        self->base.type = &gsm_if_type;
        self->gsm = psGSM;

        return MP_OBJ_FROM_PTR(self);
    }

exit:
    mp_printf(&mp_plat_print, "Error %d\n", err);
    return mp_const_false;
}
MP_DEFINE_CONST_FUN_OBJ_KW(gsm_make_new_obj, 3, gsm_make_new);


STATIC mp_obj_t gsm_active(size_t n_args, const mp_obj_t *args)
{
    gsm_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (mp_obj_is_true(args[1]))
        ppposActive(self->gsm, true);
    else
        ppposActive(self->gsm, false);

    return mp_obj_new_bool(self->gsm->active);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gsm_active_obj, 1, 2, gsm_active);


STATIC mp_obj_t gsm_status(mp_obj_t self_in)
{
    gsm_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t tuple[2] = { mp_const_none, mp_const_none };
    char state[20] = {'\0'};
    int gsm_state = ppposStatus(self->gsm, NULL, NULL, NULL);

    tuple[0] = mp_obj_new_int(gsm_state);

    if (gsm_state == E_GSM_STATUS_INIT)
        sprintf(state, "Not started");
    else if (gsm_state == E_GSM_STATUS_ACTIVE)
        sprintf(state, "Idle");
    else if (gsm_state == E_GSM_STATUS_NO_DATA_LINK)
        sprintf(state, "Disconnected");
    else if (gsm_state == E_GSM_STATUS_DATA_LINK_TO_PPP)
        sprintf(state, "Connected");
    else
        sprintf(state, "Unknown");
    tuple[1] = mp_obj_new_str(state, strlen(state));

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gsm_status_obj, gsm_status);


STATIC mp_obj_t gsm_connect(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    gsm_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_authmode, MP_ARG_INT, {.u_int = PPPAUTHTYPE_NONE} },
        { MP_QSTR_username, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_password, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    if (!self->gsm->active) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("must be active"));
    }
    if (self->gsm->client_task_handle != NULL) {
        mp_raise_OSError(MP_EALREADY);
    }

    int32_t ret = ppposConnect(self->gsm,
                               parsed_args[0].u_int,
                               mp_obj_str_get_str(parsed_args[1].u_obj),
                               mp_obj_str_get_str(parsed_args[2].u_obj));
    if (ret == -1)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid auth"));
    }
    else if (ret == -2)
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("set default failed"));
    }
    else if (ret == -3)
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("connect failed"));
    }
    else if (ret == -4)
    {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to create worker task"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(gsm_connect_obj, 1, gsm_connect);


STATIC mp_obj_t gsm_disconnect(mp_obj_t self_in)
{
    gsm_if_obj_t *self = MP_OBJ_TO_PTR(self_in);

    ppposDisconnect(self->gsm);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gsm_disconnect_obj, gsm_disconnect);


STATIC mp_obj_t gsm_ifconfig(size_t n_args, const mp_obj_t *args)
{
    gsm_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // get
        const ip_addr_t *dns;
        if (self->gsm->pcb != NULL) {
            dns = dns_getserver(0);
            struct netif *pppif = ppp_netif(self->gsm->pcb);
            mp_obj_t tuple[4] = {
                netutils_format_ipv4_addr((uint8_t *)&pppif->ip_addr, NETUTILS_BIG),
                netutils_format_ipv4_addr((uint8_t *)&pppif->gw,      NETUTILS_BIG),
                netutils_format_ipv4_addr((uint8_t *)&pppif->netmask, NETUTILS_BIG),
                netutils_format_ipv4_addr((uint8_t *)dns,             NETUTILS_BIG),
            };
            return mp_obj_new_tuple(4, tuple);
        } else {
            mp_obj_t tuple[4] = { mp_const_none, mp_const_none, mp_const_none, mp_const_none };
            return mp_obj_new_tuple(4, tuple);
        }
    } else {
        ip_addr_t dns;
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[3], (uint8_t *)&dns.u_addr.ip4, NETUTILS_BIG);
        dns_setserver(0, &dns);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gsm_ifconfig_obj, 1, 2, gsm_ifconfig);


STATIC const mp_rom_map_elem_t gsm_if_locals_dict_table[] = {
    // method
    { MP_ROM_QSTR(MP_QSTR_active),     MP_ROM_PTR(&gsm_active_obj)       },
    { MP_ROM_QSTR(MP_QSTR_status),     MP_ROM_PTR(&gsm_status_obj)       },
    { MP_ROM_QSTR(MP_QSTR_connect),    MP_ROM_PTR(&gsm_connect_obj)      },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&gsm_disconnect_obj)   },
    { MP_ROM_QSTR(MP_QSTR_ifconfig),   MP_ROM_PTR(&gsm_ifconfig_obj) },

    // const
    { MP_ROM_QSTR(MP_QSTR_AUTH_NONE), MP_ROM_INT(PPPAUTHTYPE_NONE) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_PAP),  MP_ROM_INT(PPPAUTHTYPE_PAP)  },
    { MP_ROM_QSTR(MP_QSTR_AUTH_CHAP), MP_ROM_INT(PPPAUTHTYPE_CHAP) },
};
STATIC MP_DEFINE_CONST_DICT(gsm_if_locals_dict, gsm_if_locals_dict_table);


const mp_obj_type_t gsm_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_GSM,
    .locals_dict = (mp_obj_dict_t *)&gsm_if_locals_dict,
};

// #endif
