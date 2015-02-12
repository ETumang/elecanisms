import pprint
import random
import matplotlib.pyplot as plt
import numpy as np
import scipy.stats as stats #commenting for use on ubuntu server
import pickle

sensorReadings = [59184, 58419, 
    58462, 
#    58511, 
    59060, 59151, 59187, 58409, 57669, 57133]
positionValues = [0,10,20,30,40,50,60,70,80]

def makePlt(x,y):
    x=np.array(x); y=np.array(y)
    plt.plot(x,y,'co')
    
    m,b = np.polyfit(x, y, 1) 
    plt.plot(x, m*x+b, 'b',linewidth=3.0) 
    
    plt.ylabel('Sensor Output (Analog to Digital Units)', fontsize=17)
    plt.xlabel('Handle Position (Degrees)', fontsize=17)
    plt.title('Calibration of Angle Sensor', fontsize=30)
    plt.savefig('calibrationAngleSensor.png')
    plt.clf()    
    
print len(positionValues)
print len(sensorReadings)
makePlt(positionValues,sensorReadings)
