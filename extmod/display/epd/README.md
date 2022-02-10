# `epd` -- Parallel e-Paper

该模块为并口墨水屏提供驱动接口。目前支持的墨水屏显示器有`ED047TC1`。相关产品有[TTGO T5-4.7 Inch E-paper](https://www.aliexpress.com/item/1005002006058892.html).

## Class EPD47

### Constructors

#### class epd.EPD47()

返回EPD47的对象

### Methods

#### EPD47.power(active)

打开或者关闭电子纸的电源，并返回当前的电源状态。

在使用此类上的任何其他方法之前，必须先激活电源。

#### EPD47.bitmap(framebuffer, x, y, width, height)

显示位图数据。

- `framebuffer`: List 类型。位图数据。

- `x`: Numbers 类型。x坐标，范围为0 - 960。

- `y`: Numbers 类型。y坐标，范围为0 - 540。

- `width`: Numbers 类型。显示区域的宽度，范围为0 - 960。

- `height`: Numbers 类型。显示区域的高度，范围为0 - 540。

#### EPD47.jpeg(pathname, x, y)

显示jpeg图像。

- `pathname`: String 类型。jpeg图像文字在micropython的绝对路径。

- `x`: Numbers 类型。x坐标，范围为0 - 960。

- `y`: Numbers 类型。y坐标，范围为0 - 540。

#### EPD47.text(text, x, y, font_size=12)

显示文字。目前内置FiraSans字体，支持9、12、18、24四种字体大小。

- `text`: String 类型。显示的文本。

- `x`: Numbers 类型。x坐标，范围为0 - 960。

- `y`: Numbers 类型。y坐标，范围为0 - 540。

- `font_size`: Numbers 类型。字体大小，取值只能是9、12、18、24。

#### EPD47.clear([x=0, y=0, width=960, height=540])

清除屏幕

#### EPD47.width()

返回电子纸的宽度

#### EPD47.height()

返回电子纸的高度

### example

文本显示：

```python
from epd import EPD47

e = EPD47()
e.power(True)
e.clear()
e.text("hello world", 100, 100, font_size=9)
e.text("hello world", 100, 120, font_size=12)
e.text("hello world", 100, 150, font_size=18)
e.text("hello world", 100, 190, font_size=24)

del e
```

jpeg图像显示：

```python
from epd import EPD47
from random import randint
from time import sleep

e = EPD47()

e.power(True)

e.clear()

while True:
    x = randint(0, 960-200)
    y = randint(0, 540-200)
    e.jpeg("/logo.jpeg", x, y)
    sleep(1)
    e.jpeg("/cat.jpg", 0, 0)
    sleep(1)
    e.clear()
    sleep(1)
```