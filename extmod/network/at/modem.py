
class MODEM():
    '''
    Basic functions
    '''

    def init(self):
        self.sendCmd('AT+CLTS=1')
        res, data = self.waitResponse()
        if not res:
            return False

        self.sendCmd('AT+CBATCHK=1')
        res, data = self.waitResponse()
        if not res:
            return False

        return True

    def setBaud(self, baud):
        '''
        Set TE-TA Fixed Local Rate

        TODO: Not Tested
        '''
        cmd = "AT+IPR=" + baud
        self.sendCmd(cmd)
        self.waitResponse()

    def testAT(self):
        '''
        Test response to AT commands

        TODO: Not Tested
        '''
        self.sendCmd("AT")
        self.waitResponse()

    def getModemInfo(self):
        '''
        Asks for modem information via the V.25TER standard ATI command

        NOTE: The actual value and style of the response is quite varied

        TODO: Not Tested
        '''
        self.sendCmd("ATI")
        res, data = self.waitResponse()
        if res:
            return data
        return ""

    def getModemName(self):
        '''
        Gets the modem name (as it calls itself)

        TODO: Not Tested
        '''
        s = ""
        self.sendCmd("AT+CGMI")
        res, data = self.waitResponse()
        if res:
            data = data.replace("\r\nOK\r\n", "")
            s += data.replace("\rOK\r", "")
        else:
            return ""
        self.sendCmd("AT+GMM")
        res, data = self.waitResponse()
        if res:
            data = data.replace("\r\nOK\r\n", "")
            s += data.replace("\rOK\r", "")
        else:
            return ""
        return s

    def factoryDefault(self):
        '''
        TODO
        '''
        pass

    '''
    Power functions
    '''
    def restart(self):
        '''
        not anything no single thing
        '''
        pass

    def poweroff(self):
        '''
        TODO: Not Tested
        '''
        self.sendCmd("+CPOWD=1")
        res = self.waitResponse(rsp1 = 'NORMAL POWER DOWN\r\n')
        return res

    def radioOff(self):
        pass

    def sleepEnable(self, enable = True):
        '''
        During sleep, the SIM70xx module has its serial communication disabled.

        In order to reestablish communication pull the DRT-pin of the SIM70xx
        module LOW for at least 50ms.

        Then use this function to disable sleep mode.

        The DTR-pin can then be released again.

        TODO: Not Tested
        '''
        self.sendCmd("+CSCLK=" + "1" if enable else "0")
        res, data = self.waitResponse()
        return res

    def setPhoneFunctionality(self, fun, rst):
        '''
        TODO: Not Tested
        '''
        cmd = "AT+CFUN=" + fun + ",1" if rst else ""
        self.sendCmd(cmd)
        res, data = self.waitResponse()
        return res

    def getSignalQuality(self):
        '''
        Gets signal quality report
        '''
        self.sendCmd("AT+CSQ")
        res, data = self.waitResponse()
        if res:
            s = self.parseResponse(data, '+CSQ:')
            a = s.split(',')
            if int(a[0]) == 0:
                return (0, '-115 dBm or less')
            elif int(a[0]) == 1:
                return (1, '-111 dBm')
            elif int(a[0]) >= 2 and int(a[0]) <= 31:
                dBm = (-110+2 * (int(a[0]) - 2 ))
                return ( int(a[0]), "{} dBm".format(dBm))
            elif int(a[0]) == 31:
                return ( 31, '-52 dBm or greater')
        return (99, 'not known or not detectable')

