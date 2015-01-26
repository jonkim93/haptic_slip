import serial
from time import sleep

ser = serial.Serial('/dev/tty.usbmodem427921', 115200, timeout=1)

#ser.write(bytearray[116,43,14,43,14,43,14,43,14])
ser.write(bytearray(b'\x74\x2B\x64\x2B\x64\x2B\x64\x2B\x64\x0A'))
#ser.write("t+14-14+14-14\n")
"""
#for x in xrange(0,5):
#print(x)
#ser.write("t+14-14+14-14")
#sleep(1)
"""
