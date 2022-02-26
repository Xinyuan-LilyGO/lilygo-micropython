from collections import namedtuple

gpsinfo = namedtuple("gpsinfo", ["lat",      # Latitude
                                 "lon",      # Longitude
                                 "speed",    # Speed Over Ground. Unit is knots.
                                 "alt",      # MSL Altitude. Unit is meters
                                 "vsat",     # GNSS Satellites in View
                                 "usat",     # GNSS Satellites Used
                                 "accuracy", # Horizontal Dilution Of Precision
                                 "year",     # Four digit year
                                 "month",    # Two digit month
                                 "day",      # Two digit day
                                 "hour",     # Two digit hour
                                 "minute",   # Two digit minute
                                 "second"])  # 6 digit second with subseconds

class GPS():
    '''
    GPS/GNSS/GLONASS Basic Class
    '''

    def enableGPS(self):
        '''
        enable GPS
        '''
        self.sendCmd("AT+CGNSPWR=1")
        res, data = self.waitResponse()
        return res

    def disableGPS(self):
        '''
        disable GPS
        '''
        self.sendCmd("AT+CGNSPWR=0")
        res, data = self.waitResponse()
        return res

    def getGPSRAW(self):
        '''
        get the RAW GPS output
        '''
        self.sendCmd("AT+CGNSINF")
        res, data = self.waitResponse(timeout = 10000)
        if not res:
            return ""
        return data

    def getGPS(self):
        '''
        get GPS informations
        '''
        data = self.getGPSRAW()
        s = self.parseResponse(data, "+CGNSINF:")
        a = s.split(',')
        if int(a[0]) == 1 and int(a[1]) == 1:
            return gpsinfo(
                lat      = float(a[3])      if a[3]  != '' else float(0), # Latitude
                lon      = float(a[4])      if a[4]  != '' else float(0), # Longitude
                speed    = float(a[6])      if a[6]  != '' else float(0), # Speed Over Ground. Unit is knots.
                alt      = float(a[5])      if a[5]  != '' else float(0), # MSL Altitude. Unit is meters
                vsat     = int(a[14])       if a[14] != '' else int(0),   # GNSS Satellites in View
                usat     = int(a[16])       if a[16] != '' else int(0),   # GNSS Satellites Used
                accuracy = float(a[10])     if a[10] != '' else float(0), # Horizontal Dilution Of Precision
                year     = int(a[2][0:4])   if a[2]  != '' else int(0),   # Four digit year
                month    = int(a[2][4:6])   if a[2]  != '' else int(0),   # Two digit month
                day      = int(a[2][6:8])   if a[2]  != '' else int(0),   # Two digit day
                hour     = int(a[2][8:10])  if a[2]  != '' else int(0),   # Two digit hour
                minute   = int(a[2][10:12]) if a[2]  != '' else int(0),   # Two digit minute
                second   = int(a[2][12:14]) if a[2]  != '' else int(0),   # 6 digit second with subseconds
            )
        return None

    def getGPSTime(self):
        '''
        get GPS Time informations
        '''
        data = self.getGPSRAW()
        s = self.parseResponse(data, "+CGNSINF:")
        a = s.split(',')
        if int(a[0]) == 1 and int(a[1]) == 1:
            return (
                int(a[2][0:4])   if a[2]  != '' else int(0),   # Four digit year
                int(a[2][4:6])   if a[2]  != '' else int(0),   # Two digit month
                int(a[2][6:8])   if a[2]  != '' else int(0),   # Two digit day
                int(a[2][8:10])  if a[2]  != '' else int(0),   # Two digit hour
                int(a[2][10:12]) if a[2]  != '' else int(0),   # Two digit minute
                int(a[2][12:14]) if a[2]  != '' else int(0),   # 6 digit second with subseconds
            )
        return None

    def setGNSSMode(self, gps, glonass, beidou, galilean):
        '''
        GNSS Work Mode Set

        TODO: Not Tested
        '''
        cmd = 'AT+CGNSMOD=' + str(gps) + ',' + str(glonass) + ',' + str(beidou) + ',' + str(galilean)
        self.sendCmd(cmd)
        res, data = self.waitResponse(timeout = 10000)
        return res

    def getGNSSMode(self):
        '''
        GNSS Work Mode Get
        '''
        self.sendCmd("AT+CGNSMOD?")
        res, data = self.waitResponse(timeout = 10000)
        if res:
            s = self.parseResponse(data, "+CGNSMOD:")
            a = s.split(',')
            return (
                int(a[0]) if a[0] != '' else int(-1), # GPS work mode
                int(a[1]) if a[1] != '' else int(-1), # GLONASS work mode
                int(a[2]) if a[2] != '' else int(-1), # BEIDOU work mode
                int(a[3]) if a[3] != '' else int(-1)  # GALILEAN work mode
            )
        return None

