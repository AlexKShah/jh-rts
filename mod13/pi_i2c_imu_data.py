from smbus2 import SMBus
from gps import *
import time
import threading
from flask import Flask, render_template, Response, jsonify
import cv2

app = Flask(__name__)

# for RPI version 1, use bus = SMBus(0)
bus = SMBus(1)

# This is the address we setup in the Arduino Program
SLAVE_ADDRESS = 0x04

gpsd = None

# Create a GPS Poller class
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

def request_reading():
  # Read a block of 12 bytes starting at SLAVE_ADDRESS, offset 0
  reading = bus.read_i2c_block_data(SLAVE_ADDRESS, 0, 12)

  # Extract the IMU reading data
  if reading[0] < 1:
    roll_sign = "+"
  else:
    roll_sign = "-"
  roll_1 = reading[1]
  roll_2 = reading[2]
  roll_3 = reading[3]

  if reading[4] < 1:
    pitch_sign = "+"
  else:
    pitch_sign = "-"   
  pitch_1 = reading[5]
  pitch_2 = reading[6]
  pitch_3 = reading[7]

  if reading[8] < 1:
    yaw_sign = "+"
  else:
    yaw_sign = "-"
  yaw_1 = reading[9]
  yaw_2 = reading[10]
  yaw_3 = reading[11]

  # Create a dictionary with the IMU and GPS data
  data = {
    'roll': roll_sign + str(roll_1) + str(roll_2) + str(roll_3),
    'pitch': pitch_sign + str(pitch_1) + str(pitch_2) + str(pitch_3),
    'yaw': yaw_sign + str(yaw_1) + str(yaw_2) + str(yaw_3),
    'latitude': str(gpsd.fix.latitude),
    'longitude': str(gpsd.fix.longitude),
    'altitude': str(gpsd.fix.altitude / .3048)
  }

  return data

def gen_frames():
    camera = cv2.VideoCapture(0)  # Use 0 for the first connected USB camera
    while True:
        success, frame = camera.read()
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def data():
    # Read the IMU and GPS data
    imu_gps_data = request_reading()
    
    # Return the data as JSON
    return jsonify(imu_gps_data)

if __name__ == '__main__':
    gpsp = GpsPoller()
    try:
        gpsp.start()
        app.run(host='0.0.0.0', port=5000)
    except(KeyboardInterrupt, SystemExit):
        gpsp.running = False
        gpsp.join()
        bus.close()