import uos
from flashbdev import bdev


def check_bootsec():
    buf = bytearray(bdev.ioctl(5, 0))  # 5 is SEC_SIZE
    bdev.readblocks(0, buf)
    empty = True
    for b in buf:
        if b != 0xFF:
            empty = False
            break
    if empty:
        return True
    fs_corrupted()


def fs_corrupted():
    import time

    while 1:
        print(
            """\
The filesystem appears to be corrupted. If you had important data there, you
may want to make a flash snapshot to try to recover it. Otherwise, perform
factory reprogramming of MicroPython firmware (completely erase flash, followed
by firmware programming).
"""
        )
        time.sleep(3)


def setup():
    check_bootsec()
    print("Performing initial setup")
    uos.VfsLfs2.mkfs(bdev)
    vfs = uos.VfsLfs2(bdev)
    uos.mount(vfs, "/")
    with open("boot.py", "w") as f:
        f.write(
            """\
import lcd
from machine import Pin
import logo
import gc
import time

gc.enable()

i8080 = lcd.I8080(data=(Pin(39), Pin(40), Pin(41), Pin(42), Pin(45), Pin(46), Pin(47), Pin(48)),
                     command=Pin(7),
                     write=Pin(8),
                     read=Pin(9),
                     cs=Pin(6),
                     pclk=2 * 1000 * 1000,
                     width=320,
                     height=170,
                     swap_color_bytes=False,
                     cmd_bits=8,
                     param_bits=8)

st = lcd.ST7789(i8080, reset=Pin(5), backlight=Pin(38))

st.reset()
st.init()
st.invert_color(True)
st.swap_xy(True)
st.mirror(False, True)
st.set_gap(0, 35)
st.backlight_on()

st.bitmap(0, 0, logo.WIDTH, logo.HEIGHT, logo.BITMAP)

time.sleep(0.1)
del st
i8080.deinit()
gc.collect()
"""
        )
    return vfs
