#include "py/dynruntime.h"


mp_obj_type_t gsm_type;

#include "gsm_ppp.c"

mp_map_elem_t gsm_locals_dict_table[5];
STATIC MP_DEFINE_CONST_DICT(gsm_locals_dict, gsm_locals_dict_table);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args)
{
    MP_DYNRUNTIME_INIT_ENTRY

    gsm_type.base.type = (void*)&mp_type_type;
    gsm_type.name = MP_QSTR_GSM;
    gsm_type.make_new = gsm_make_new;

    gsm_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_active),     MP_OBJ_FROM_PTR(&gsm_active_obj)       };
    gsm_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_status),     MP_OBJ_FROM_PTR(&gsm_status_obj)       };
    gsm_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_connect),    MP_OBJ_FROM_PTR(&gsm_connect_obj)      };
    gsm_locals_dict_table[3] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_OBJ_FROM_PTR(&gsm_disconnect_obj)   };
    gsm_locals_dict_table[4] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig),   MP_OBJ_FROM_PTR(&gsm_ifconfig_obj) };
    gsm_type.locals_dict = (void*)&gsm_locals_dict;

    mp_store_global(MP_QSTR_GSM, MP_OBJ_FROM_PTR(&gsm_type));

    MP_DYNRUNTIME_INIT_EXIT
}
