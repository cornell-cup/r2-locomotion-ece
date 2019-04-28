#!/usr/bin/env python3
'''Records measurments to a given file. Usage example:
$ ./record_measurments.py out.txt'''
import sys
from rplidar import RPLidar


PORT_NAME = '/dev/ttyUSB0'


def run_lidar():
    '''Main function'''
    lidar = RPLidar(PORT_NAME)
    #outfile = open('lidarResults.txt', 'w')
    
    print('Recording measurments... Press Crl+C to stop.')
<<<<<<< HEAD
    print(len(lidar.iter_measurments()))
    '''
=======
    length = 0
>>>>>>> 57d72081231675b4173a7efae82cb18ac65b108d
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
<<<<<<< HEAD
    '''
    print("before stop")
    lidar.stop()
    lidar.disconnect()
=======
        if length > 3:
            lidar.stop()
            lidar.disconnect()
        length = length + 1
>>>>>>> 57d72081231675b4173a7efae82cb18ac65b108d
    #outfile.close()
