from __future__ import print_function
import serial
import time

class OpticFlowSensor(serial.Serial):

    def __init__(self, port='/dev/ttyACM0'):
        super(OpticFlowSensor,self).__init__(port=port,baudrate=115200) 
        print('Initializing sensor')
        time.sleep(4.0)
        print('done')

    def start(self):
        self.write('r')

    def stop(self):
        self.write('s')

    def readData(self):
        dataList = []
        while self.inWaiting() > 0:
            rawData = self.readline()
            rawData = rawData.strip()
            rawData = rawData.split(',')
            try:
                dataList.append([int(x) for x in rawData])
            except:
                pass
        return dataList


# -----------------------------------------------------------------------------
if __name__ == '__main__':


    port = '/dev/ttyACM0'
    filename = 'data.txt'
    numPoints = 1000

    sensor = OpticFlowSensor(port)
    sensor.start()
    with open(filename,'w') as f:
        count = 0
        while count < numPoints:
            data = sensor.readData()
            count += len(data)
            for vals in data:
                print(vals)
                try:
                    f.write('{0} {1}\n'.format(vals[0], vals[1]))
                except IndexError:
                    pass


