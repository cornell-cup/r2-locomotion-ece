#!/usr/bin/env python3
'''Records measurments to a given file. Usage example:
$ ./record_measurments.py out.txt'''
import sys
from rplidar import RPLidar


PORT_NAME = '/dev/ttyUSB0'


def run():
    '''Main function'''
    lidar = RPLidar(PORT_NAME)
    outfile = open('lidarResults.txt', 'w')
    try:
        print('Recording measurments... Press Crl+C to stop.')
        for measurment in lidar.iter_measurments():
            i = 0
            for distance in measurment:
                if distance == False and i != 0:
                    return False
                i = i + 1
    except KeyboardInterrupt:
        print('Stopping.')
    lidar.stop()
    lidar.disconnect()
    outfile.close()

if __name__ == '__main__':
    run()
