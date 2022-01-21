import epd
import logo
from time import sleep
import random

epd47 = epd.EPD47()

while True:
    print("display")
    epd47.power(True)
    sleep(0.1)
    x = random.randint(0, 960-200)
    y = random.randint(0, 540-200)
    epd47.bitmap(logo.logo_data, x, y, logo.logo_width, logo.logo_height)
    epd47.power(False)
    sleep(1)
