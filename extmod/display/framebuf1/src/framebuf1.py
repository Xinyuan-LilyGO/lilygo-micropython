# from math import floor, ceil
# from collections import namedtuple
from zlib import decompress

'''
GFXfont = namedtuple("GFXfont", ["bitmap", "glyph", "intervals", \
                                 "interval_count", "compressed", "advance_y", \
                                 "ascender", "descender"])

UnicodeInterval = namedtuple("UnicodeInterval", ["first", "last", "offset"])

GFXglyph = namedtuple("GFXglyph", ["width", "height", "advance_x", "left", \
                                   "top", "compressed_size", "data_offset"])
'''


class FrameBuffer():
    '''This module provides a general frame buffer which can be used to create 
    bitmap images, which can then be sent to a display.
    '''

    def __init__(self, buffer: bytearray, width: int, height: int):
        self.fb = buffer
        self.width = width
        self.height = height

    def fill(self, color: int):
        '''Fill the entire FrameBuffer with the specified color.
        '''
        color = 255 - color
        color = color >> 4 | color
        for i in range(0, self.width * int(self.height / 2)):
            self.fb[i] = color

    def pixel(self, x: int, y: int, color: int):
        '''Draw a pixel a given framebuffer.

        Args:
            x: Horizontal position in pixels.
            y: Vertical position in pixels.
            color: The gray value of the line (0-255).
        '''
        if x < 0 or x >= self.width:
            return
        if y < 0 or y >= self.height:
            return
        color = 255 - color
        pos = y * int(self.width / 2) + int(x / 2)
        if int(x % 2):
            self.fb[pos] = (self.fb[pos] & 0x0F) | (color & 0xF0)
        else:
            self.fb[pos] = (self.fb[pos] & 0xF0) | (color >> 4 & 0x0F)

    def hline(self, x: int, y: int, length: int, color: int):
        '''Draw a horizontal line to a given framebuffer.

        Args:
            x: Horizontal start position in pixels.
            y: Vertical start position in pixels.
            length: Length of the line in pixels.
            color: The gray value of the line (0-255);
        '''
        for i in range(0, length):
            self.pixel(x + i, y, color)

    def vline(self, x: int, y: int, length: int, color: int):
        '''Draw a horizontal line to a given framebuffer.

        Args:
            x: Horizontal start position in pixels.
            y: Vertical start position in pixels.
            length: Length of the line in pixels.
            color: The gray value of the line (0-255);
        '''
        for i in range(0, length):
            self.pixel(x, y + i, color)

    def line(self, x0: int, y0: int, x1: int, y1: int, color: int):
        '''Draw a line

        Args:
            x0  Start point x coordinate
            y0  Start point y coordinate
            x1  End point x coordinate
            y1  End point y coordinate
            color: The gray value of the line (0-255)
        '''
        if x0 == x1:
            if (y0 > y1):
                y0, y1 = self._swap_int(y0, y1)
            self.vline(x0, y0, y1 - y0 + 1, color)
        elif y0 == y1:
            if x0 > x1:
                x0, x1 = self._swap_int(x0, x1)
            self.hline(x0, y0, x1 - x0 + 1, color)
        else:
            self._write_line(x0, y0, x1, y1, color)

    def rect(self, x: int, y: int, w: int, h: int, color: int):
        '''Draw a rectanle with no fill color

        Args:
            x: Top left corner x coordinate
            y: Top left corner y coordinate
            w: Width in pixels
            h: Height in pixels
            color: The gray value of the line (0-255);
        '''
        self.hline(x, y, w, color)
        self.hline(x, y + h - 1, w, color)
        self.vline(x, y, h, color)
        self.vline(x + w - 1, y, h, color)

    def fill_rect(self, x: int, y: int, w: int, h: int, color: int):
        '''Draw a rectanle with fill color

        Args:
            x: Top left corner x coordinate
            y: Top left corner y coordinate
            w: Width in pixels
            h: Height in pixels
            color: The gray value of the line (0-255)
        '''
        for i in range(x, x + w):
            self.vline(i, y, h, color)

    def circle(self, x: int, y: int, r: int, color: int):
        '''Draw a circle with given center and radius

        Args:
            x: Center-point x coordinate
            y: Center-point y coordinate
            r: Radius of the circle in pixels
            color: The gray value of the line (0-255)
        '''
        f = 1 - r
        ddF_x = 1
        ddF_y = -2 * r
        xx = 0
        yy = r

        self.pixel(x,     y + r, color)
        self.pixel(x,     y - r, color)
        self.pixel(x + r,     y, color)
        self.pixel(x - r,     y, color)

        while xx < yy:
            if (f >= 0):
                yy -= 1
                ddF_y += 2
                f += ddF_y
            xx += 1
            ddF_x += 2
            f += ddF_x

            self.pixel(x + xx, y + yy, color)
            self.pixel(x - xx, y + yy, color)
            self.pixel(x + xx, y - yy, color)
            self.pixel(x - xx, y - yy, color)
            self.pixel(x + yy, y + xx, color)
            self.pixel(x - yy, y + xx, color)
            self.pixel(x + yy, y - xx, color)
            self.pixel(x - yy, y - xx, color)

    def fill_circle(self, x: int, y: int, r: int, color: int):
        '''Draw a circle with fill with given center and radius

        Args:
            x: Center-point x coordinate
            y: Center-point y coordinate
            r: Radius of the circle in pixels
            color: The gray value of the line (0-255)
        '''
        self.vline(x, y - r, 2 * r + 1, color)
        self._fill_circle_helper(x, y, r, 3, 0, color)

    def triangle(self, x0: int, y0: int, x1: int, y1: int, x2: int, y2: int, color: int):
        '''Draw a triangle with no fill color

        Args:
            x0: Vertex #0 x coordinate
            y0: Vertex #0 y coordinate
            x1: Vertex #1 x coordinate
            y1: Vertex #1 y coordinate
            x2: Vertex #2 x coordinate
            y2: Vertex #2 y coordinate
            color: The gray value of the line (0-255)
        '''
        self.line(x0, y0, x1, y1, color)
        self.line(x1, y1, x2, y2, color)
        self.line(x2, y2, x0, y0, color)

    def fill_triangle(self, x0: int, y0: int, x1: int, y1: int, x2: int,  y2: int, color: int):
        '''Draw a triangle with color-fill

        Args:
            x0: Vertex #0 x coordinate
            y0: Vertex #0 y coordinate
            x1: Vertex #1 x coordinate
            y1: Vertex #1 y coordinate
            x2: Vertex #2 x coordinate
            y2: Vertex #2 y coordinate
            color: The gray value of the line (0-255)
        '''
        a = b = y = last = 0

        if y0 > y1:
            y0, y1 = self._swap_int(y0, y1)
            x0, x1 = self._swap_int(x0, x1)

        if y1 > y2:
            y2, y1 = self._swap_int(y2, y1)
            x2, x1 = self._swap_int(x2, x1)

        if y0 > y1:
            y0, y1 = self._swap_int(y0, y1)
            x0, x1 = self._swap_int(x0, x1)

        if y0 == y2:
            a = b = x0
            if x1 < a:
                a = x1
            elif x1 > b:
                b = x1
            if x2 < a:
                a = x2
            elif x2 > b:
                b = x2
            self.hline(a, y0, b - a + 1, color)
            return

        dx01 = x1 - x0
        dy01 = y1 - y0
        dx02 = x2 - x0
        dy02 = y2 - y0
        dx12 = x2 - x1
        dy12 = y2 - y1
        sa = sb = 0

        last = y1 if y1 == y2 else y1 -1

        for y in range(y0, last + 1):
            a = x0 + int(sa / dy01)
            b = x0 + int(sb / dy02)
            sa += dx01
            sb += dx02
            if a > b:
                a, b = self._swap_int(a, b)
            self.hline(a, y, b - a + 1, color)

        y = last + 1

        sa = dx12 * (y - y1)
        sb = dx02 * (y - y0)
        for i in range(y, y2 + 1):
            a = x1 + int(sa / dy12)
            b = x0 + int(sb / dy02)
            sa += dx12
            sb += dx02

            if (a > b):
                a, b = self._swap_int(a, b)
            self.hline(a, i, b - a + 1, color)

    def text(self, gfx, s: str, x: int, y: int):
        '''Write a (multi-line) string to FrameBuffer.
        '''
        line_start = x
        a = s.splitlines()
        for i in a:
            x, y = self.writeln(gfx, i, x, y)
            y += gfx.advance_y
            x = line_start
        return x, y

    def writeln(self, gfx, s: str, x: int, y: int):
        if s is None or len(s) == 0:
            return

        # x1 = y1 = w = h = 0
        # tmp_cur_x = x
        # tmp_cur_y = y
        # tmp_cur_x, tmp_cur_y, x1, y1, w, h = self.get_text_bounds(gfx, s,
        #     tmp_cur_x, tmp_cur_y, x1, y1, w, h)

        local_cursor_x = x
        local_cursor_y = y

        cursor_x_init = local_cursor_x
        cursor_y_init = local_cursor_y

        for ch in s:
            local_cursor_x = self.draw_char(gfx, local_cursor_x, local_cursor_y,
                ord(ch))

        x += local_cursor_x - cursor_x_init
        y += local_cursor_y - cursor_y_init
        return x, y

    def get_text_bounds(self, gfx: GFXfont, s: str, x: int, y: int, x1: int, y1: int, w: int, h: int):
        if len(s) == 0:
            return x, y, x, y, 0, 0

        minx = 100000
        miny = 100000
        maxx = -1
        maxy = -1
        original_x = x

        for ch in s:
            x, y, minx, miny, maxx, maxy = self.get_char_bounds(gfx, ord(ch), x,
                y, minx, miny, maxx, maxy)
        x1 = min(original_x, minx)
        w = maxx - x1
        y1 = miny
        h = maxy - miny
        return x, y, x1, y1, w, h

    def get_char_bounds(self, gfx: GFXfont, codepoint: int, x: int, y: int, minx: int, miny: int, maxx: int, maxy: int):
        glyph = self.get_glyph(gfx, codepoint)

        if glyph is None:
            return

        x1 = x + glyph.left
        y1 = y + (glyph.top - glyph.height)
        x2 = x1 + glyph.width
        y2 = y1 + glyph.height

        if x1 < minx:
            minx = x1
        if y1 < miny:
            miny = y1
        if x2 > maxx:
            maxx = x2
        if y2 > maxy:
            maxy = y2

        x += glyph.advance_x
        return x, y, minx, miny, maxx, maxy

    def get_glyph(self, gfx: GFXfont, code_point: int):
        glyph = None
        for interval in gfx.intervals:
            if code_point >= interval.first and code_point <= interval.last:
                glyph = gfx.glyph[interval.offset + (code_point - interval.first)]
        return glyph

    def draw_char(self, gfx: GFXfont, cursor_x: int, cursor_y: int, cp: int):
        glyph = self.get_glyph(gfx, cp)

        if glyph is None:
            return

        offset = glyph.data_offset
        width = glyph.width
        height = glyph.height
        left = glyph.left

        byte_width = int(width / 2) + width % 2
        bitmap_size = byte_width * height

        if gfx.compressed:
            try:
                bitmap = decompress(
                    bytes(gfx.bitmap[offset: offset + glyph.compressed_size]),
                    bufsize = bitmap_size)
            except:
                bitmap = decompress(
                    bytes(gfx.bitmap[offset: offset + glyph.compressed_size]),
                    bitmap_size)
        else:
            bitmap = gfx.bitmap[offset: offset + bitmap_size]

        color_lut = [ max(0, min(15, 15 + c * int(-15 / 15))) for c in range(0, 16) ]

        for y in range(0, height):
            yy = cursor_y - glyph.top + y
            if yy < 0 or yy >= self.height:
                continue
            start_pos = cursor_x + left
            byte_complete = start_pos % 2
            x = max(0, -start_pos)
            max_x = min(start_pos + width, self.width)
            for xx in range(start_pos, max_x):
                buf_pos = yy * int(self.width/2) + int(xx / 2)
                old = self.fb[buf_pos]
                bm = bitmap[y * byte_width + int(x / 2)]
                bm = bm & 0xF if x & 1 == 0 else bm >> 4

                if xx & 1 == 0:
                    self.fb[buf_pos] = (old & 0xF0) | color_lut[bm]
                else:
                    self.fb[buf_pos] = (old & 0x0F) | (color_lut[bm] << 4)
                byte_complete = ~byte_complete
                x += 1

        cursor_x += glyph.advance_x
        return cursor_x

    def _fill_circle_helper(self, x0: int, y0: int, r: int, corners: int, delta: int, color: int):
        f = 1 - r
        ddF_x = 1
        ddF_y = -2 * r
        x = 0
        y = r
        px = x
        py = y

        delta += 1

        while x < y:
            if f >= 0:
                y -= 1
                ddF_y += 2
                f += ddF_y
            x += 1
            ddF_x += 2
            f += ddF_x
            if x < (y + 1):
                if corners & 1:
                    self.vline(x0 + x, y0 - y, 2 * y + delta, color)
                if corners & 2:
                    self.vline(x0 - x, y0 - y, 2 * y + delta, color)
            if y != py:
                if corners & 1:
                    self.vline(x0 + py, y0 - px, 2 * px + delta, color)
                if corners & 2:
                    self.vline(x0 - py, y0 - px, 2 * px + delta, color)
                py = y
            px = x

    def _write_line(self, x0: int, y0: int, x1: int, y1: int, color: int):
        '''Write a line. Bresenham's algorithm - thx wikpedia

        Args:
            x0  Start point x coordinate
            y0  Start point y coordinate
            x1  End point x coordinate
            y1  End point y coordinate
            color: The gray value of the line (0-255)
        '''
        steep = abs(y1 - y0) > abs(x1 - x0)
        if steep:
            x0, y0 = self._swap_int(x0, y0)
            x1, y1 = self._swap_int(x1, y1)

        if x0 > x1:
            x0, x1 = self._swap_int(x0, x1)
            y0, y1 = self._swap_int(y0, y1)

        dx = x1 - x0
        dy = abs(y1 - y0)

        err = int(dx / 2)
        ystep = 1 if y0 < y1 else -1

        for i in range(x0, x1 + 1):
            if steep:
                self.pixel(y0, i, color)
            else:
                self.pixel(i, y0, color)
            err -= dy
            if err < 0:
                y0 += ystep
                err += dx

    @staticmethod
    def _swap_int(a: int, b: int):
        return b, a
