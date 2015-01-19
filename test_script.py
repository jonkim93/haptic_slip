import serial

ser = serial.Serial('/dev/tty.usbmodem427921', 115200, timeout=1)


def main():
	for x in xrange(0,100):
		ser.write("t+100+0+0+0")


main()