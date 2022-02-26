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
    # power(True)

    uart = UART(1, baudrate=115200, rx=26, tx=27)
    g = constructAT(SIM7000)(uart)
    print(g)
    #g = GSM(uart)
    g.connect()

    while True:
        g.init()
        isConnected = False
        for i in [ 2, 13, 38, 51 ]:
            '''
            2,  # Automatic
            13, # GSM only
            38, # LTE only
            51  # GSM and LTE only
            '''
            g.setNetworkMode(i)
            sleep(3)
            tryCount = 60
            while tryCount:
                print(g.getSignalQuality())
                isConnected = g.isNetworkConnected()
                print("CONNECT" if isConnected else "NO CONNECT")
                if isConnected:
                    break
                tryCount -= 1
            if isConnected:
                break

        print("Device is connected .")
        print("=====Inquiring UE system information=====")
        g.sendCmd("AT+CPSI?")
        res, data = g.waitResponse()
        print(data)
        break
