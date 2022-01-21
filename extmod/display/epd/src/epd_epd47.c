#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objtype.h"
#include "py/mperrno.h"

#include "epd_driver.h"
#include "ed097oc4.h"
#include "libjpeg/libjpeg.h"

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
#if 0
    self->jpeg_buf = m_malloc(540 * 960);
    if (!self->jpeg_buf)
    {
        mp_raise_TypeError(MP_ERROR_TEXT("malloc fail"));
        return mp_const_none;
    }

    libjpeg_init(self->jpeg_buf);
#endif
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


STATIC mp_obj_t epd47_jpeg(size_t n_args, const mp_obj_t *args)
{
    // epd47_if_obj_t *self = NULL;
    Rect_t area;
    size_t len;
    mp_obj_t *items;
    uint8_t *data = NULL;

    if (n_args < 2 || n_args > 6) return mp_const_none;

    area.x      = mp_obj_get_int(args[2]);
    area.y      = mp_obj_get_int(args[3]);
    area.width  = mp_obj_get_int(args[4]);
    area.height = mp_obj_get_int(args[5]);

    mp_obj_list_get(args[1], &len, &items);
    data = m_malloc(len);
    if (!data)
    {
        mp_printf(&mp_plat_print, "malloc fail\n");
        mp_raise_TypeError(MP_ERROR_TEXT("malloc fail"));
        return mp_const_none;
    }
    mp_printf(&mp_plat_print, "len: %d\n", len);
    for (size_t i = 0; i < len; i++)
    {
        data[i] = mp_obj_get_int(items[i]);
    }

    show_area_jpg_from_buff(data, len, area);
    m_free(data);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epd47_jpeg_obj, 6, 6, epd47_jpeg);


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


STATIC const mp_rom_map_elem_t epd47_if_locals_dict_table[] = {
    // method
    { MP_ROM_QSTR(MP_QSTR_power), MP_ROM_PTR(&epd47_power_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&epd47_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_jpeg), MP_ROM_PTR(&epd47_jpeg_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&epd47_clear_obj) },
};
STATIC MP_DEFINE_CONST_DICT(epd47_if_locals_dict, epd47_if_locals_dict_table);

const mp_obj_type_t epd47_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_EPD47,
    .locals_dict = (mp_obj_dict_t *)&epd47_if_locals_dict,
};