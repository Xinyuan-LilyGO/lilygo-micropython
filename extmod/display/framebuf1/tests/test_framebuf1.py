import unittest
import epd
from framebuf1 import FrameBuffer
import time

class TestFrameBuffer(unittest.TestCase):

    def __init__(self):
        self.e = epd.EPD47()
        self.buffer = bytearray(int(960 * 540 / 2))
        self.fb = FrameBuffer(self.buffer, 960, 540)

    def setUpClass(self):
        self.e.power(True)
        self.e.clear()

    def tearDownClass(self):
        self.e.power(False)
        del self.e
        del self.fb
        del self.buffer

    def setUp(self):
        self.e.clear()
        self.fb.fill(0)

    def tearDown(self):
        self.e.bitmap(self.buffer, 0, 0, 960, 540)
        time.sleep(1)

    def test_hline(self):
        self.fb.hline(430, 270, 100, 255)

    def test_vhine(self):
        self.fb.vline(480, 220, 100, 255)
    
    def test_line(self):
        self.fb.line(430, 220, 530, 320, 255)
        self.fb.line(430, 320, 530, 220, 255)
    
    def test_rect(self):
        self.fb.rect(430, 220, 100, 100, 255)

    def test_fill_rect(self):
        self.fb.fill_rect(430, 220, 100, 100, 255)
    
    def test_circle(self):
        self.fb.circle(480, 270, 50, 255)
    
    def test_fill_circle(self):
        self.fb.fill_circle(480, 270, 50, 255)
    
    def test_triangle(self):
        self.fb.triangle(430, 220, 430, 320, 530, 320, 255)
    
    def test_fill_triangle(self):
        self.fb.fill_triangle(430, 220, 430, 320, 530, 320, 255)

if __name__ == "__main__":
    unittest.main()
