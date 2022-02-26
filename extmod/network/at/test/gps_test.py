from at import constructAT, SIM7000
from time import sleep

if __name__ == "__main__":
    from machine import UART
    from machine import Pin

    def power(active):
        if active == True:
            GSM_PWR.value(1)
            sleep(0.5)
            GSM_PWR.value(0)
            sleep(0.5)
            GSM_PWR.value(1)
            sleep(10)
        else:
            GSM_PWR.value(0)
            sleep(1)

    GSM_PWR = Pin(4, Pin.OUT)
    power(True)

    uart = UART(1, baudrate=115200, rx=26, tx=27)
    g = constructAT(SIM7000)(uart)
    print(g)
    g.connect()

    g.sendCmd("AT+SGPIO=0,4,1,1");
    res, data = g.waitResponse(timeout = 10000)
    if not res:
        print("error")
    g.enableGPS()

    while True:
        print(g.getGPS())
        print(g.getGPSTime())
        print(g.getGNSSMode())
        sleep(1)
