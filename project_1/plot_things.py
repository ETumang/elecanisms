import serial
import numpy as numpy
import matplotlib.pyplot as plt

num_points = 50


ser = serial.Serial('/dev/ttyUSB1', 19200)
def main():

	buffer = ''
	plot = []
	ser.flushInput()

	while num_points > len(plot):
		read_values = ser.readline()
		buffer = read_values
		plot.append(abs(int(buffer))/.001)

		#plot.append(int(buffer))
		#print plot
	ser.close()

	plt.plot(plot)
	plt.title("Motor Spin-Down")
	plt.xlabel("Time (s/10)")
	plt.ylabel("Degrees per Second")
	plt.show()

	


if __name__=='__main__':
	main()



