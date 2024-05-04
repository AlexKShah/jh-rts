from gps import *
import time
import threading
 
# Source: Donat, Wolfram. "Make a Raspberry Pi-controlled Robot :
# Building a Rover with Python, Linux, Motors, and Sensors.
# Sebastopol, CA: Maker Media, 2014. Print.
# Modified by Addison Sears-Collins
# Date April 17, 2019
 
# Open a new file to log the GPS data
f = open("gps_data.txt", "w")
 
gpsd = None
 
# Create a GPS Poller class. 
class GpsPoller(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
    global gpsd
    gpsd = gps(mode=WATCH_ENABLE)
    self.current_value = None
    self.running = True
 
  def run(self):
    global gpsd
    while gpsp.running:
      gpsd.next()
 
if __name__ == '__main__':
  gpsp = GpsPoller()
  try:
    gpsp.start()
    while True:
      f.write("Lat: " + str(gpsd.fix.latitude) # Log the latitude data
      + "\tLon: " + str(gpsd.fix.longitude) # Log the longitude data
      + "\tAlt: " + str(gpsd.fix.altitude / .3048) # Log the altitude in feet
      + "\n")
      time.sleep(5)
  except(KeyboardInterrupt, SystemExit):
    f.close()
    gpsp.running = False
    gpsp.join()