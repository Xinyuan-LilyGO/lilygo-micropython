#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/stream.h"
#include "py/objtype.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"

#include "epd_driver.h"
#include "ed097oc4.h"
#include "libjpeg/libjpeg.h"
#include "font/FiraSans.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct epd47_if_obj_t {
    mp_obj_base_t base;
    uint8_t *jpeg_buf;
} epd47_if_obj_t;

const mp_obj_type_t epd47_if_type;

STATIC mp_obj_t epd47_make_new()
{
    epd_init();

    epd47_if_obj_t *self = m_new_obj_with_finaliser(epd47_if_obj_t);
    if (!self)
    {
        mp_raise_TypeError(MP_ERROR_TEXT("malloc fail"));
        return mp_const_none;
    }

    self->base.type = &epd47_if_type;

    self->jpeg_buf = m_malloc(540 * 960);
    if (!self->jpeg_buf)
    {
        mp_raise_TypeError(MP_ERROR_TEXT("malloc fail"));
        return mp_const_none;
    }

    libjpeg_init(self->jpeg_buf);

    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_0(epd47_make_new_obj, epd47_make_new);


STATIC mp_obj_t epd47_power(mp_obj_t self_in, mp_obj_t onoff_in)
{
    // epd47_if_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (mp_obj_is_true(onoff_in))
        epd_poweron();
    else
        epd_poweroff();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(epd47_power_obj, epd47_power);


STATIC mp_obj_t epd47_bitmap(size_t n_args, const mp_obj_t *args)
{
    // epd47_if_obj_t *self = NULL;
    Rect_t area;
    size_t len;
    mp_obj_t *items;
    uint8_t *data = NULL;

    if (n_args < 2 || n_args > 6) return mp_const_none;

    // self = MP_OBJ_TO_PTR(args[0]);
    area.x      = mp_obj_get_int(args[2]);
    area.y      = mp_obj_get_int(args[3]);
    area.width  = mp_obj_get_int(args[4]);
    area.height = mp_obj_get_int(args[5]);

    data = m_malloc((area.width * area.height) / 2);
    if (!data)
    {
        mp_printf(&mp_plat_print, "malloc fail\n");
        mp_raise_TypeError(MP_ERROR_TEXT("malloc fail"));
        return mp_const_none;
    }

    mp_obj_list_get(args[1], &len, &items);
    for (size_t i = 0; i < len; i++)
    {
        data[i] = mp_obj_get_int(items[i]);
    }

    epd_clear_area(area);
    epd_draw_grayscale_image(area, (uint8_t *)data);
    m_free(data);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epd47_bitmap_obj, 6, 6, epd47_bitmap);


//
// jpeg file name
// area.x
// area.y
//
STATIC mp_obj_t epd47_jpeg(size_t n_args, const mp_obj_t *args)
{
    int errcode;
    mp_uint_t len;
    Rect_t area;
    epd47_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t vfs_args[2] = {
        args[1],
        MP_OBJ_NEW_QSTR(MP_QSTR_rb),
    };
    mp_obj_t o = mp_vfs_open(MP_ARRAY_SIZE(vfs_args), &vfs_args[0], (mp_map_t *)&mp_const_empty_map);

    len = mp_stream_rw(o, self->jpeg_buf, 540 * 960, &errcode, MP_STREAM_RW_READ);

    area.x = mp_obj_get_int(args[2]);
    area.y = mp_obj_get_int(args[3]);

    show_area_jpg_from_buff(self->jpeg_buf, len, area);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epd47_jpeg_obj, 2, 4, epd47_jpeg);


STATIC mp_obj_t epd47_text(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        {MP_QSTR_text,      MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        {MP_QSTR_cursorx,   MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1}            },
        {MP_QSTR_cursory,   MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1}            },
        // {MP_QSTR_font,      MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        {MP_QSTR_font_size, MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 12}            },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];

    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int x = args[1].u_int;
    int y = args[2].u_int;
    const GFXfont *gfx = &FiraSansRegular12pt;

    if (args[0].u_obj == mp_const_none || x == -1 || y == -1)
    {
        return mp_const_none;
    }

    if (x < 0 || x > EPD_WIDTH || y < 0 || y > EPD_HEIGHT)
    {
        return mp_const_none;
    }

    if (args[3].u_int == 9)
        gfx = &FiraSansRegular9pt;
    if (args[3].u_int == 18)
        gfx = &FiraSansRegular18pt;
    if (args[3].u_int == 24)
        gfx = &FiraSansRegular24pt;

    writeln((GFXfont *)gfx,
            mp_obj_str_get_str(args[0].u_obj),
            &x,
            &y,
            NULL);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(epd47_text_obj, 3, epd47_text);


STATIC mp_obj_t epd47_clear(size_t n_args, const mp_obj_t *args)
{
    Rect_t area = {.x = 0, .y = 0, .width = EPD_WIDTH, .height = EPD_HEIGHT};

    if (n_args < 1 || n_args > 5) return mp_const_none;

    if (n_args > 1 && mp_obj_get_int(args[1]) > 0 && mp_obj_get_int(args[1]) < EPD_WIDTH)
    {
        area.x = mp_obj_get_int(args[1]);
    }

    if (n_args > 2 && mp_obj_get_int(args[2]) > 0 && mp_obj_get_int(args[2]) < EPD_HEIGHT)
    {
        area.y = mp_obj_get_int(args[2]);
    }

    if (n_args > 3 && mp_obj_get_int(args[3]) > 0 && mp_obj_get_int(args[3]) < EPD_WIDTH)
    {
        area.width = mp_obj_get_int(args[3]);
    }

    if (n_args > 4 && mp_obj_get_int(args[4]) > 0 && mp_obj_get_int(args[4]) < EPD_HEIGHT)
    {
        area.height = mp_obj_get_int(args[4]);
    }

    epd_clear_area(area);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epd47_clear_obj, 1, 5, epd47_clear);


STATIC mp_obj_t epd47_width(mp_obj_t self_in)
{
    return mp_obj_new_int(EPD_WIDTH);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(epd47_width_obj, epd47_width);


STATIC mp_obj_t epd47_height(mp_obj_t self_in)
{
    return mp_obj_new_int(EPD_HEIGHT);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(epd47_height_obj, epd47_height);


STATIC mp_obj_t epd47_delete(mp_obj_t self_in)
{
    epd47_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    epd_deinit();
    libjpeg_deinit();
    m_free(self->jpeg_buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(epd47_delete_obj, epd47_delete);


STATIC const mp_rom_map_elem_t epd47_if_locals_dict_table[] = {
    // method
    { MP_ROM_QSTR(MP_QSTR_power),   MP_ROM_PTR(&epd47_power_obj)  },
    { MP_ROM_QSTR(MP_QSTR_bitmap),  MP_ROM_PTR(&epd47_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_jpeg),    MP_ROM_PTR(&epd47_jpeg_obj)   },
    { MP_ROM_QSTR(MP_QSTR_text),    MP_ROM_PTR(&epd47_text_obj)   },
    { MP_ROM_QSTR(MP_QSTR_clear),   MP_ROM_PTR(&epd47_clear_obj)  },
    { MP_ROM_QSTR(MP_QSTR_width),   MP_ROM_PTR(&epd47_width_obj)  },
    { MP_ROM_QSTR(MP_QSTR_height),  MP_ROM_PTR(&epd47_height_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&epd47_delete_obj) },
};
STATIC MP_DEFINE_CONST_DICT(epd47_if_locals_dict, epd47_if_locals_dict_table);

const mp_obj_type_t epd47_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_EPD47,
    .locals_dict = (mp_obj_dict_t *)&epd47_if_locals_dict,
};