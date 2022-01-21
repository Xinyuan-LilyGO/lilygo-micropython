import epd
from time import sleep
import random

epd47 = epd.EPD47()

fn_size = 0
buf = []
with open('logo.jpg', 'rb') as f:
    while True:
        content = f.read(512)
        if len(content) == 0:
            break
        # print("", len(content))
        fn_size = fn_size + len(content)
        buf.extend(list(bytes(content)))

while True:
    epd47.power(True)
    sleep(0.1)
    x = random.randint(0, 960-200)
    y = random.randint(0, 540-200)
    epd47.jpeg(buf, x, y, 200, 200)
    epd47.power(False)
    sleep(1)
