#!/usr/bin/env python3
'''Records measurments to a given file. Usage example:
$ ./record_measurments.py out.txt'''
import sys
from rplidar import RPLidar


PORT_NAME = '/dev/ttyUSB0'


def run():
    '''Main function'''
    lidar = RPLidar(PORT_NAME)
    #outfile = open('lidarResults.txt', 'w')
    try:
        print('Recording measurments... Press Crl+C to stop.')
        for measurment in lidar.iter_measurments():
            i = 0
            for distance in measurment:
                if i == 2:
                    angle = distance
                    print("angle " + str(angle))
                if i == 3:
                    distance = distance / 25.4
                    print("distance " + str(distance))
                    if distance != 0 and distance < 12 and (angle > 245 or angle < 115):
                        print("in return")
                        return False
                i = i + 1
    except KeyboardInterrupt:
        print('Stopping.')
    lidar.stop()
    lidar.disconnect()
    #outfile.close()

if __name__ == '__main__':
    run()
