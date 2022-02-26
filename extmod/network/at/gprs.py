from time import sleep, ticks_ms

class GPRS():

    '''
    SIM card functions
    '''

    def simUnlock(self):
        '''
        Unlocks the SIM

        TODO: to be realized
        '''
        pass

    def getSimCCID(self):
        '''
        Gets the CCID of a sim card via AT+CCID

        TODO: to be realized
        '''
        pass

    def getIMEI(self):
        '''
        Asks for TA Serial Number Identification (IMEI)

        TODO: to be realized
        '''
        pass

    def getIMSI(self):
        '''
        Asks for International Mobile Subscriber Identity IMSI

        TODO: to be realized
        '''
        pass

    def getSimStatus(self, timeout = 10000):
        '''
        TODO: Not Tested
        '''
        startMillis = ticks_ms()
        while (ticks_ms() - startMillis) < timeout:
            self.sendCmd("AT+CPIN?")
            res, data = self.waitResponse()
            if not res:
                sleep(1)
                continue
            s = self.parseResponse(data, "+CPIN:")
            if s == 'READY':
                return (1, 'READY')
            elif s == 'SIM PIN':
                return (2, 'SIM PIN')
            elif s == 'SIM PUK':
                return (3, 'SIM PUK')
            elif s == 'PH_SIM PIN':
                return (4, 'PH_SIM PIN')
            elif s == 'PH_SIM PUK':
                return (5, 'PH_SIM PUK')
            elif s == 'SIM PIN2':
                return (5, 'SIM PIN2')
            elif s == 'SIM PUK2':
                return (5, 'SIM PUK2')
        return (0, "")

    '''
    GPRS functions
    '''
    def gprsConnect(self, apn = "", user = "", pwd = ""):
        '''
        TODO: to be realized
        '''
        pass

    def gprsDisconnect(self):
        '''
        TODO: to be realized
        '''
        pass

    def isGprsConnected(self):
        '''
        Checks if current attached to GPRS/EPS service

        TODO: to be realized
        '''
        pass

    def getOperator(self):
        '''
        Gets the current network operator

        TODO: to be realized
        '''
        pass

    def getRegistrationStatus(self):
        self.sendCmd("AT+CEREG?")
        res, data = self.waitResponse()
        if res:
            s = self.parseResponse(data, "+CEREG:")
            a = s.split(',')
            return int(a[1])
        return -1
