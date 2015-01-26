import serial

ser = serial.Serial('/dev/tty.usbmodem427921', 115200, timeout=1)

def command(motor, delta):
	if delta < 0:
		s = '-'
	else:
		s = '+'
	command = 't'+str(motor)+s+str(delta)+'\n'
	ser.write(command)


def main():
	command(1,-500)
	command(2,-500)
	command(3,-500)
	command(4,-500)



main()