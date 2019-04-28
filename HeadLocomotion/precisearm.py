import serial
import time

ser_precise = 0
wave_commands = ["M2D90E", "M3D60E", "M5D90E", "M3D90E", "M3D30E", "M3D90E", "M3D30E", "M3D90E", "M3D30E", "M0D0E"]

def open():
    ser_precise = serial.Serial('/dev/precisearm', 9600)
    
def reset():
    ser_precise.write("M0D0E".encode('utf-8'))
    ser_precise.close()
    
def wave():
    for command in wave_commands:
        ser_precise.write(command.encode('utf-8')
    ser_precise.close()

