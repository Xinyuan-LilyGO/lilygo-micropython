# Copyright (C) 2022 Lilygo https://github.com/Xinyuan-LilyGO
# Author: liangying
# Contact: 1102390310@qq.com

from time import ticks_ms
from at.sim7000 import *

__version__ = (0, 1, 0)

def constructAT(base_class):

    class AT(base_class):
        DISCONNECT = const(0)
        CONNECT    = const(1)
        BUSY       = const(2)

        def __init__(self, stream):
            # check arg
            if not hasattr(stream, "write") or not hasattr(stream, "readline"):
                raise Exception("Invalid Argument", stream)
            super(AT, self).__init__()
            self.stream = stream
            self.__status = DISCONNECT

        def sendCmd(self, cmd):
            # check arg
            if not isinstance("a", str):
                raise Exception("Invalid Argument", cmd)

            # check cmd
            if cmd.find('AT') == -1:
                raise Exception("String 'AT' not found ", cmd)
            if cmd.find('AT') != 0:
                raise Exception("Command must start with 'AT'", cmd)

            if cmd.find('\r') != (len(cmd) - 2) or \
            cmd.find('\n') != (len(cmd) - 1):
                cmd = cmd + '\r\n'

            self.stream.write(cmd)

        def waitResponse(self, rsp1 = 'OK\r\n', rsp2 = 'ERROR\r\n', timeout=1000):
            rsp = bytearray()
            startMillis = ticks_ms()
            while (ticks_ms() - startMillis) < timeout:
                if self.stream.any():
                    rsp.extend(bytearray(self.stream.readline()))
                    if rsp.decode('utf-8').endswith(rsp1):
                        return True, rsp
                    if rsp.decode('utf-8').endswith(rsp2):
                        return False, rsp
            return False, rsp

        def connect(self, timeout=20000):
            res = False
            startMillis = ticks_ms()
            while not res:
                self.sendCmd("AT\r\n")
                res, data = self.waitResponse()
                if (ticks_ms() - startMillis) > timeout:
                    raise Exception("Timeout")
            self.__status = CONNECT

        def parseResponse(self, data, head):
            # check cmd
            if not (isinstance(data, str) or isinstance(data, bytearray)):
                raise Exception("Invalid Argument", data)
            if not isinstance(head, str):
                raise Exception("Invalid Argument", head)

            s = ''
            if isinstance(data, bytearray):
                s = data.decode('utf-8')
            s = s[s.find(head) + len(head):]
            s = s[:s.find('\r\n')]
            return s.lstrip()

    return AT



