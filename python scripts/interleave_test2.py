import serial
import time 
from random import shuffle


ser = serial.Serial('/dev/tty.usbmodem427921', 115200, timeout=1)
for i in xrange(0,100):
	ser.write("t+1000+1000+1000+1000")