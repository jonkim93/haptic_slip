import serial
import time 
from random import shuffle

K_p_VALS = (1,5)
K_i_VALS = (1,5)
K_d_VALS = (1,5)
#SAMPLES = (0,720,0,30,0,10,0,5,0,900,0)
SAMPLES = (20,0,30,0,10,0,5,0,40,0,35,0)
PARAM_SET = [(16,0.005,0.01),
						 (16,0.002,0.01),
	           (32,0.005,0.01),
	           (32,0.002,0.01)]

def timeCommand(K_p, K_i, K_d, angle):
	ser = serial.Serial('/dev/tty.usbmodem427921', 115200, timeout=1)
	start = time.time()
	command = str(K_p)+","+str(K_i)+","+str(K_d)+","+str(angle)+'\n'
	ser.write(command)
	result_time = ser.readline()
	while len(result_time) < 2:
		result_time = ser.readline()
	result_angle = ser.readline()
	end = time.time()*1000
	return result_time, result_angle

def pidTest(K_p, K_i, K_d):
	result_times = [] 
	for x in SAMPLES:
		t, a = timeCommand(K_p, K_i, K_d, x)
		result_times.append(int(t[:-1]))
	print "PARAMS: ", K_p, K_i, K_d
	print "\tANGLES:   ", SAMPLES
	print "\tTIME(ms): ", result_times
	print "\n\tAVG TIME: ", float(sum(result_times))/float(len(result_times))

def main():
	x = PARAM_SET
	shuffle(x)
	for p in x:
		pidTest(p[0], p[1], p[2])

main()