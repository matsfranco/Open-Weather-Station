import sys
import os
import time
import math
import datetime
import MySQLdb as mdb
import numpy

# so matplotlib has to have some of the setup parameters _before_ pyplot
import matplotlib
matplotlib.use('agg')

#matplotlib.rcParams['figure.dpi'] = 100
#matplotlib.rcParams['figure.figsize'] = [10.24, 7.68]
matplotlib.rcParams['lines.linewidth'] = 3.0
matplotlib.rcParams['axes.color_cycle'] = ['b','c','b','k']
matplotlib.rcParams['axes.labelsize'] = 'large'
matplotlib.rcParams['font.size'] = 8
matplotlib.rcParams['grid.linestyle']='-'

import matplotlib.pyplot as plt

anomalies = 0

print "GraphTH.py V1.69 12/04/2013 WPNS",time.asctime(),

# open the database connection, read the last <many> seconds of data, put them in a Numpy array called Raw
DBconn = mdb.connect('localhost', 'root', '85ae79f52b', 'weatherparams')
cursor = DBconn.cursor()
sql = "select time,humi,rain from Log"
cursor.execute(sql)
Raw = numpy.fromiter(cursor.fetchall(), count=-1, dtype=[('', numpy.float)]*3)
Raw = Raw.view(numpy.float).reshape(-1, 3)
(samples,ports)=Raw.shape
print 'Samples: {}, DataPoints: {}'.format(samples,ports),
plotme=numpy.zeros((samples,ports-1)) # make an array the same shape minus the epoch numbers

for y in range(ports-1):
#    print y
    for x in range(samples-1):  # can't do last one, there's no (time) delta from previous sample
        seconds = Raw[x+1,0]-Raw[x,0]
        # if the number didn't overflow the counter
        plotme[x,y] = Raw[x,y+1]

    plotme[samples-1,y] = None # set last sample to "do not plot"

    for x in range(samples-1):                   # go thru the dataset again
        if (Raw[x+1,1] == -0.1):                 # if values are "reading failed" flag
            plotme[x+1,0] = plotme[x,0]          # copy current sample over it
            plotme[x+1,1] = plotme[x,1]          # for temperature and humidity both
            anomalies += 1

print "Anomalies: ",anomalies,

#print plotme

# get an array of adatetime objects (askewchan from stackoverflow, above)
dts = map(datetime.datetime.fromtimestamp, Raw[:,0])

# set up the plot details we want
plt.grid(True)
plt.ylabel('RH %%, Rain')
plt.axis(ymax=100,ymin=0)
plt.xlabel(time.asctime())
plt.title("<Unifesp_lab402_Forgers> Rain (Ciano), Humidity (Blue)")
plt.hold(True)

# and some fiddly bits around formatting the X (date) axis
plt.gca().xaxis.set_major_formatter(matplotlib.dates.DateFormatter('%m/%d %H:%M'))
plt.gca().xaxis.set_major_locator(matplotlib.dates.HourLocator())
lines = plt.plot(dts,plotme)
plt.gcf().autofmt_xdate()

FileName = '/home/pi/WeatherStation/Python/Graphics/HR.jpg'
plt.savefig(FileName)

print 'Copy to Web Server...',
Destination = '/var/www/images/'
os.system("sudo cp //home/pi/WeatherStation/Python/Graphics/HR.jpg {}".format(Destination))
print 'Done at',time.asctime()
