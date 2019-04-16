import serial 
import matplotlib.pyplot as plt
pts = []
plt.figure()
plt.ylim([0,100])
plt.xlim([0,20])
while(1):
	with serial.Serial('/dev/ttyACM0', 115200, timeout=1, ) as ser:
		# Valor lido
		line = ser.readline()   # read a '\n' terminated line


		print(line[0:4])
		if line[0:4] == b'TEMP':
			pts.append(int(line[-3:-1]))
			print('got temp:', line[-3:-1]);			
			plt.plot(pts)
			plt.pause(0.05)
		if len(pts)>=20:
			pts = []
plt.show()
		
			
