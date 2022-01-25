# epd Module

## Class EPD47

### Constructors

#### class epd.EPD47()

### Methods

#### EPD47.power(active)

打开或者关闭电子纸的电源

#### EPD47.bitmap(framebuffer, x, y, width, height)

刷写位图

#### EPD47.text(text, x, y, font_size=12)

显示文字

#### EPD47.clear([x=0, y=0, width=960, height=540)

清楚屏幕

#### EPD47.width()

返回电子纸的宽度

#### EPD47.height()

返回电子纸的高度

### example

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
