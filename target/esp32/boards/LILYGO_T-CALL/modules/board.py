from machine import Pin
from time import sleep

class BoardV20190610():
    MODEM_RST_PIN    = 5
    MODEM_PWRKEY_PIN = 4
    MODEM_POWER_PIN  = 23
    MODEM_TX         = 27
    MODEM_RX         = 26
    LED_PIN          = 13

    def __init__(self):
        self.version = "20190610"
        self.pmu = "IP5306"

    def info(self):
        print("  _      _____ _  __     _______  ____   ")
        print(" | |    |_   _| | \ \   / / ____|/ __ \  ")
        print(" | |      | | | |  \ \_/ / |  __| |  | | ")
        print(" | |      | | | |   \   /| | |_ | |  | | ")
        print(" | |____ _| |_| |____| | | |__| | |__| | ")
        print(" |______|_____|______|_|  \_____|\____/  ")
        print("                                         ")
        print("                                         ")
        print("Board: LILYGO T-CALL SIM800")
        print("")
        print("Version: %s" %(self.version))
        print("")
        print("PMU: %s" %(self.pmu))

    def pinmap(self):
        print("Pin Map:")
        print("")
        print("                             -----------------------                 ")
        print("                       3V3  |     -------------     | GND            ")
        print("                       NC   |    |             |    | 23   VSPI_MOSI ")
        print("        ADC0   GPIO36  SVP  |    |             |    | 22   SCL       ")
        print("        ADC3   GPIO39  SVN  |    |    ESP32    |    | TXD  GPIO1     ")
        print("        ADC6   GPIO34  34   |    |             |    | RXD  GPIO3     ")
        print("        ADC7   GPIO35  35   |    |             |    | 21   SDA       ")
        print("TOUCH9  ADC4   GPIO32  32   |     -------------     | GND            ")
        print("TOUCH8  ADC5   GPIO33  32   |                       | 19   VSPI_MISO ")
        print(" DAC1   ADC18  GPIO24  24   |                       | 18   VSPI_SCK  ")
        print(" DAC2   ADC19  GPIO26  26   |                       | 5    VSPI_SS   ")
        print("TOUCH7  ADC17  GPIO27  27   |                       | NC             ")
        print("TOUCH6  ADC16  GPIO14  14   |                       | NC             ")
        print("TOUCH5  ADC15  GPIO12  12   |                       | 4    GPIO4     ADC10 TOUCH4")
        print("                       GND  |                       | 0    GPIO11    ADC11 TOUCH1")
        print("TOUCH4  ADC14  GPIO13  13   |     -------------     | 2    GPIO12    ADC12 TOUCH2")
        print("                       SD2  |    |             |    | 15   GPIO13    ADC13 TOUCH3")
        print("                       SD3  |    |             |    | SD1            ")
        print("                       CMD  |    |   SIM800    |    | SD0            ")
        print("                       5V   |    |             |    | CLK            ")
        print("                       SPK+ |    |             |    | MIC-           ")
        print("                       SPK- |     -------------     | MIC+           ")
        print("                             -----------------------                ")

    def sim800_restart(self):
        '''Reset SIM800 module

        复位SIM800模块

        Electrical Connections:

        | ESP32 | SIM800 |
        | ----- | ------ |
        | GPIO5 |     49 |
        '''
        RST_PIN = Pin(self.MODEM_RST_PIN, Pin.OUT)
        RST_PIN.value(1)
        RST_PIN.value(0)
        sleep(0.2)
        RST_PIN.value(1)
        del RST_PIN

    def sim800_power(self, active):
        '''Control the power supply of SIM800 module

        控制SIM800模块的电源
        '''
        POWER_PIN = Pin(self.MODEM_POWER_PIN, Pin.OUT)
        POWER_PIN.value(active)
        del POWER_PIN

    def sim800_boot(self):
        '''sim800 module boot

        sim800模块开机启动
        '''
        PWRKEY_PIN = Pin(self.MODEM_PWRKEY_PIN, Pin.OUT)

        PWRKEY_PIN.value(1)
        sleep(0.5)
        PWRKEY_PIN.value(0)
        sleep(1)
        PWRKEY_PIN.value(1)

        del PWRKEY_PIN

    def sim800_shutdown(self):
        '''sim800 module shutdown

        sim800模块关机
        '''
        PWRKEY_PIN = Pin(self.MODEM_PWRKEY_PIN, Pin.OUT)

        PWRKEY_PIN.value(1)
        sleep(0.1)
        PWRKEY_PIN.value(0)
        sleep(3)
        PWRKEY_PIN.value(1)

        del PWRKEY_PIN

    def sim800_init(self):
        '''sim800 initialization

        sim800初始化
        '''
        self.sim800_power(True)
        self.sim800_boot()
        self.status_light(True)

    def status_light(self, onoff):
        led = Pin(self.LED_PIN, Pin.OUT)
        led.value(onoff)
        del led

class BoardV20200811(BoardV20190610):
    MODEM_PWRKEY_PIN = 4
    MODEM_POWER_PIN  = 23
    MODEM_TX         = 27
    MODEM_RX         = 26
    MODEM_DTR        = 32
    MODEM_RI         = 33
    LED_PIN          = 13

    def __init__(self):
        self.version = "20200327"
        self.pmu = "IP5306"

class BoardV20200327(BoardV20190610):
    MODEM_RST_PIN    = 5
    MODEM_PWRKEY_PIN = 4
    MODEM_POWER_PIN  = 25
    MODEM_TX         = 27
    MODEM_RX         = 26
    MODEM_DTR        = 32
    MODEM_RI         = 33
    LED_PIN          = 13

    def __init__(self):
        self.version = "20200327"
        self.pmu = "AXP192"

class BoardV20200609(BoardV20190610):
    MODEM_PWRKEY_PIN = 4
    MODEM_POWER_PIN  = 23
    MODEM_TX         = 27
    MODEM_RX         = 26
    MODEM_DTR        = 32
    MODEM_RI         = 33
    LED_PIN          = 12

    def __init__(self):
        self.version = "20200609"
        self.pmu = "AXP192"

    def sim800_restart(self):
        pass

if __name__ == '__main__':
    b = BoardV20190610()
    b.sim800_init()
