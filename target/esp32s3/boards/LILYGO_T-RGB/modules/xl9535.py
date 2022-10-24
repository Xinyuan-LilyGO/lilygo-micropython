

__XL9535_IIC_ADDRESS = const(0X20)

__XL9535_INPUT_PORT_0_REG     = const(0X00)
__XL9535_INPUT_PORT_1_REG     = const(0X01)
__XL9535_OUTPUT_PORT_0_REG    = const(0X02)
__XL9535_OUTPUT_PORT_1_REG    = const(0X03)
__XL9535_INVERSION_PORT_0_REG = const(0X04)
__XL9535_INVERSION_PORT_1_REG = const(0X05)
__XL9535_CONFIG_PORT_0_REG    = const(0X06)
__XL9535_CONFIG_PORT_1_REG    = const(0X07)


class XL9535():
    OUT = const(3)
    IN = const(1)
    def __init__(self, bus: I2C, a0: bool = False, a1: bool = False, a2: bool = False):
        self.__bus = bus
        self.__address = __XL9535_IIC_ADDRESS | \
                         (int(a2) << 3) | \
                         (int(a1) << 2) | \
                         (int(a0) << 1)
        self.__buf = bytearray(1)
        try:
            self.__bus.scan().index(self.__address)
        except:
            raise Exception("XL9535 not found")

    def pinMode(self, pin, mode):
        if pin > 7:
            self.__bus.readfrom_mem_into(self.__address,
                                         __XL9535_CONFIG_PORT_1_REG,
                                         self.__buf)
            if mode == self.OUT:
                self.__buf[0] = self.__buf[0] & (~(1 << (pin - 10)))
            else:
                self.__buf[0] = self.__buf[0] | (1 << (pin - 10))
            self.__bus.writeto_mem(self.__address,
                                   __XL9535_CONFIG_PORT_1_REG,
                                   self.__buf)
        else:
            self.__bus.readfrom_mem_into(self.__address,
                                         __XL9535_CONFIG_PORT_0_REG,
                                         self.__buf)
            if mode == self.OUT:
                self.__buf[0] = self.__buf[0] & (~(1 << pin))
            else:
                self.__buf[0] = self.__buf[0] | (1 << pin)
            self.__bus.writeto_mem(self.__address,
                                   __XL9535_CONFIG_PORT_0_REG,
                                   self.__buf)

    def pinMode8(self, port, pin, mode):
        self.__buf[0] = pin if mode != self.OUT else ~pin
        if port:
            self.__bus.writeto_mem(self.__address,
                                   __XL9535_CONFIG_PORT_1_REG,
                                   self.__buf)
        else:
            self.__bus.writeto_mem(self.__address,
                                   __XL9535_CONFIG_PORT_0_REG,
                                   self.__buf)

    def digitalWrite(self, pin, val):
        if pin > 7:
            self.__bus.readfrom_mem_into(self.__address,
                                         __XL9535_CONFIG_PORT_1_REG,
                                         self.__buf)
            self.__buf[0] = self.__buf[0] & (~(1 << (pin - 10)))
            self.__buf[0] = self.__buf[0] | val << (pin - 10)
            self.__bus.writeto_mem(self.__address,
                                   __XL9535_CONFIG_PORT_1_REG,
                                   bytes(self.__buf))
        else:
            self.__bus.readfrom_mem_into(self.__address,
                                         __XL9535_OUTPUT_PORT_0_REG,
                                         self.__buf)
            self.__buf[0] = self.__buf[0] & (~(1 << pin))
            self.__buf[0] = self.__buf[0] | val << pin
            self.__bus.writeto_mem(self.__address,
                                   __XL9535_OUTPUT_PORT_0_REG,
                                   self.__buf)
