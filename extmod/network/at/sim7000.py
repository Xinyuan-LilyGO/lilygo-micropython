from at.gps import GPS
from at.gprs import GPRS
from at.modem import MODEM

class SIM7000(GPS, GPRS, MODEM):
    
    def setNetworkMode(self, mode):
        '''
        2 Automatic
        13 GSM only
        38 LTE only
        51 GSM and LTE only
        '''
        self.sendCmd("AT+CNMP=" + str(mode))
        res, data = self.waitResponse()
        return res

    def isNetworkConnected(self):
        status = self.getRegistrationStatus()
        return status == 1 or status == 5

