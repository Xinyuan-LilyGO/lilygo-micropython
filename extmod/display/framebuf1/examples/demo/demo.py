if __name__ == "__main__":
    from FiraSansRegular10pt import FiraSansRegular10pt
    from framebuf1 import FrameBuffer

    buffer = bytearray(int(960 * 540 / 2))
    fb = FrameBuffer(buffer, 960, 540)
    fb.fill(0)

    fb.hline(100, 100, 100, 255)

    fb.vline(100, 100, 100, 255)

    fb.circle(200, 200, 50, 255)
    fb.fill_circle(400, 200, 50, 255)

    fb.rect(600, 200, 100, 100, 255)
    fb.fill_rect(800, 200, 100, 100, 255)

    fb.line(100, 300, 400, 500, 255)
    fb.line(400, 500, 100, 300, 255)
    fb.line(400, 300, 100, 500, 255)
    fb.line(100, 500, 400, 300, 255)

    fb.triangle(200, 400, 150, 500, 250, 400, 255)
    fb.fill_triangle(600, 400, 600, 500, 700, 500, 255)

    fb.text(FiraSansRegular10pt, "1234", 50, 50)
    try:
        from epd import EPD47
        e = EPD47()
        e.power(True)
        e.clear()
        e.bitmap(buffer, 0, 0, 960, 540)
    except:
        print("The current parser is not micropython")
