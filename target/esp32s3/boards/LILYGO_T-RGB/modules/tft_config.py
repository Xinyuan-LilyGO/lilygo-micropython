import machine
import xl9535
import time
from collections import namedtuple
import lcd
import gc

__TP_RES_PIN   = const(1)
__PWR_EN_PIN   = const(2)
__LCD_CS_PIN   = const(3)
__LCD_SDA_PIN  = const(4)
__LCD_CLK_PIN  = const(5)
__LCD_RST_PIN  = const(6)
__SD_CS_PIN    = const(7)

InitData = namedtuple("InitData", ["cmd", "data", "delay"])

st_init_cmds = [
    InitData(cmd=0xFF, data=b'\x77\x01\x00\x00\x10', delay=0),
    InitData(cmd=0xC0, data=b'\x3b\x00', delay=0),
    InitData(cmd=0xC1, data=b'\x0b\x02', delay=0),
    InitData(cmd=0xC2, data=b'\x07\x02', delay=0),
    InitData(cmd=0xCC, data=b'\x10', delay=0),
    InitData(cmd=0xCD, data=b'\x08', delay=0), # 用565时屏蔽    666打开
    InitData(cmd=0xb0, data=b'\x00\x11\x16\x0e\x11\x06\x05\x09\x08\x21\x06\x13\x10\x29\x31\x18', delay=0),
    InitData(cmd=0xb1, data=b'\x00\x11\x16\x0e\x11\x07\x05\x09\x09\x21\x05\x13\x11\x2a\x31\x18', delay=0),
    InitData(cmd=0xFF, data=b'\x77\x01\x00\x00\x11', delay=0),
    InitData(cmd=0xb0, data=b'\x6d', delay=0),
    InitData(cmd=0xb1, data=b'\x37', delay=0),
    InitData(cmd=0xb2, data=b'\x81', delay=0),
    InitData(cmd=0xb3, data=b'\x80', delay=0),
    InitData(cmd=0xb5, data=b'\x43', delay=0),
    InitData(cmd=0xb7, data=b'\x85', delay=0),
    InitData(cmd=0xb8, data=b'\x20', delay=0),
    InitData(cmd=0xc1, data=b'\x78', delay=0),
    InitData(cmd=0xc2, data=b'\x78', delay=0),
    InitData(cmd=0xc3, data=b'\x8c', delay=0),
    InitData(cmd=0xd0, data=b'\x88', delay=0),
    InitData(cmd=0xe0, data=b'\x00\x00\x02', delay=0),
    InitData(cmd=0xe1, data=b'\x03\xa0\x00\x00\x04\xa0\x00\x00\x00\x20\x20', delay=0),
    InitData(cmd=0xe2, data=b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00', delay=0),
    InitData(cmd=0xe3, data=b'\x00\x00\x11\x00', delay=0),
    InitData(cmd=0xe4, data=b'\x22\x00', delay=0),
    InitData(cmd=0xe5, data=b'\x05\xec\xa0\xa0\x07\xee\xa0\xa0\x00\x00\x00\x00\x00\x00\x00\x00', delay=0),
    InitData(cmd=0xe6, data=b'\x00\x00\x11\x00', delay=0),
    InitData(cmd=0xe7, data=b'\x22\x00', delay=0),
    InitData(cmd=0xe8, data=b'\x06\xed\xa0\xa0\x08\xef\xa0\xa0\x00\x00\x00\x00\x00\x00\x00\x00', delay=0),
    InitData(cmd=0xeb, data=b'\x00\x00\x40\x40\x00\x00\x00', delay=0),
    InitData(cmd=0xed, data=b'\xff\xff\xff\xba\x0a\xbf\x45\xff\xff\x54\xfb\xa0\xab\xff\xff\xff', delay=0),
    InitData(cmd=0xef, data=b'\x10\x0d\x04\x08\x3f\x1f', delay=0),
    InitData(cmd=0xFF, data=b'\x77\x01\x00\x00\x13', delay=0),
    InitData(cmd=0xef, data=b'\x08', delay=0),
    InitData(cmd=0xFF, data=b'\x77\x01\x00\x00\x00', delay=0),
    InitData(cmd=0x36, data=b'\x08', delay=0),
    InitData(cmd=0x3a, data=b'\x66', delay=0),
    InitData(cmd=0x11, data=b'\x00', delay=100),
    # InitData(cmd=0xFF,  data=b'\x77\x01\x00\x00\x12', delay=0),
    # InitData(cmd=0xd1,  data=b'\x81', delay=0),
    # InitData(cmd=0xd2,  data=b'\x06', delay=0),
    InitData(cmd=0x29, data=b'\x00', delay=100)
]


def __lcd_send_data(xl, data):
    for _ in range(8):
        if (data & 0x80):
            xl.digitalWrite(__LCD_SDA_PIN, 1);
        else:
            xl.digitalWrite(__LCD_SDA_PIN, 0);
        data <<= 1;
        xl.digitalWrite(__LCD_CLK_PIN, 0);
        xl.digitalWrite(__LCD_CLK_PIN, 1);

def __lcd_cmd(xl, cmd):
    xl.digitalWrite(__LCD_CS_PIN, 0)
    xl.digitalWrite(__LCD_SDA_PIN, 0)
    xl.digitalWrite(__LCD_CLK_PIN, 0)
    xl.digitalWrite(__LCD_CLK_PIN, 1)
    __lcd_send_data(xl, cmd)
    xl.digitalWrite(__LCD_CS_PIN, 1)

def __lcd_data(xl, data):
    for i in range(len(data)):
        xl.digitalWrite(__LCD_CS_PIN, 0)
        xl.digitalWrite(__LCD_SDA_PIN, 1)
        xl.digitalWrite(__LCD_CLK_PIN, 0)
        xl.digitalWrite(__LCD_CLK_PIN, 1)
        __lcd_send_data(xl, data[i])
        xl.digitalWrite(__LCD_CS_PIN, 1)

def tft_init(xl):
    xl.digitalWrite(__LCD_CS_PIN, 1)
    xl.digitalWrite(__LCD_SDA_PIN, 1)
    xl.digitalWrite(__LCD_CLK_PIN, 1)

    # Reset the display
    xl.digitalWrite(__LCD_RST_PIN, 1)
    time.sleep(0.2)
    xl.digitalWrite(__LCD_RST_PIN, 0)
    time.sleep(0.2)
    xl.digitalWrite(__LCD_RST_PIN, 1)
    time.sleep(0.2)
    for i in st_init_cmds:
        __lcd_cmd(xl, i.cmd)
        __lcd_data(xl, i.data)
        if i.delay != 0:
            time.sleep(0.1)
    # print("Register setup complete")

def config():
    i2c = machine.I2C(0, scl=machine.Pin(48), sda=machine.Pin(8))
    xl = xl9535.XL9535(i2c)
    pin = (1 << __PWR_EN_PIN) | \
      (1 << __LCD_CS_PIN) | \
      (1 << __TP_RES_PIN) | \
      (1 << __LCD_SDA_PIN) | \
      (1 << __LCD_CLK_PIN) | \
      (1 << __LCD_RST_PIN) | \
      (1 << __SD_CS_PIN)
    xl.pinMode8(0, pin, xl.OUT)
    xl.digitalWrite(__PWR_EN_PIN, 1)
    tft_init(xl)
    del i2c
    del xl
    gc.collect()
    return lcd.RGB(data = (machine.Pin(7),   \
                           machine.Pin(6),   \
                           machine.Pin(5),   \
                           machine.Pin(3),   \
                           machine.Pin(2),   \
                           machine.Pin(14),  \
                           machine.Pin(13),  \
                           machine.Pin(12),  \
                           machine.Pin(11),  \
                           machine.Pin(10),  \
                           machine.Pin(9),   \
                           machine.Pin(21),  \
                           machine.Pin(18),  \
                           machine.Pin(17),  \
                           machine.Pin(16),  \
                           machine.Pin(15)), \
                   hsync = machine.Pin(47),          \
                   vsync = machine.Pin(41),          \
                   de = machine.Pin(45),             \
                   pclk_pin = machine.Pin(42),       \
                   timings = (1, 30, 50, 1, 30, 20), \
                   backlight = machine.Pin(46),      \
                   pclk = 10 * 1000 * 1000,          \
                   width = 480,                      \
                   height = 480)
