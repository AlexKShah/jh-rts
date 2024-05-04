#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
 
/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
 
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.
 
   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).
 
   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3-5V DC
   Connect GROUND to common ground
 
   History
   =======
   2015/MAR/03  - First release (KTOWN)
   2015/AUG/27  - Added calibration and system status helpers
 
   @Author Modified by Addison Sears-Collins
   @Date   April 17, 2019
*/
 
/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)
 
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
 
// Flag used to stop the program
bool done = false;
 
// Make the Arduino a slave to the Raspberry Pi
int SLAVE_ADDRESS = 0X04;
 
// Toggle in-built LED for verifying program is working
int ledPin = 13;
 
// Data to send back to Raspberry Pi
byte imu_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
// Variables used for digit extraction
int roll = 0;
int pitch = 0;
int yaw = 0;
 
// Initialize the LED. This is used for testing.
boolean ledOn = false;
 
/**************************************************************************/
/*
    This function ends the program
*/
/**************************************************************************/
 
void end_program() {
     
    // Used for reading data from the serial monitor
    char ch;
 
    // Check to see if ! is available to be read
    if (Serial.available()) {
 
    // Read the character
    ch = Serial.read();
 
    // End the program if exclamation point is entered in the serial monitor
    if (ch == '!') {
      done = true;
      Serial.println("Finished recording Roll+Pitch+Yaw data. Goodbye.");
    }
  } 
}
 
/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}
 
/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
void displaySensorStatus(void)
{
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);
 
  /* Display the results in the Serial Monitor */
  Serial.println("");
  Serial.print("System Status: 0x");
  Serial.println(system_status, HEX);
  Serial.print("Self Test:     0x");
  Serial.println(self_test_results, HEX);
  Serial.print("System Error:  0x");
  Serial.println(system_error, HEX);
  Serial.println("");
  delay(500);
}
 
/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
 
  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }
 
  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
}
 
/**************************************************************************/
/*
    Callback for received data
*/
/**************************************************************************/
 
void processMessage(int n) {
 
  char ch = Wire.read();
  if (ch == 'l') {
    toggleLED();
  }
}
 
/**************************************************************************/
/*
    Method to toggle the LED. This is used for testing.
*/
/**************************************************************************/
 
void toggleLED() {
  ledOn = ! ledOn;
  digitalWrite(ledPin, ledOn);
   
}
 
/**************************************************************************/
/*
    Code that executes when request is received from Raspberry Pi
*/
/**************************************************************************/
 
void sendIMUReading() {
  Wire.write(imu_data, 12); 
}
 
/**************************************************************************/
/*
    Retrieves the digit of any position in an integer. The rightmost digit
    has position 0. The second rightmost digit has position 1, etc.
    e.g. Position 3 of integer 245984 is 5.
*/
/**************************************************************************/
 
byte getDigit(int num, int n) {
  int int_digit, temp1, temp2;
  byte byte_digit;
 
  temp1 = pow(10, n+1);
  int_digit = num % temp1;
 
  if (n > 0) {
    temp2 = pow(10, n);
    int_digit = int_digit / temp2;
  }
 
  byte_digit = (byte) int_digit;
 
  return byte_digit;
}
 
     
/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
    Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("Orientation Sensor Starting...");
  if (!bno.begin()) {
    Serial.print("No BNO055 detected !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    while (1)
      ;
  }
 
  delay(1000);
 
  /* Display some basic information on this sensor */
  displaySensorDetails();
 
  /* Optional: Display current status */
  displaySensorStatus();
 
  bno.setExtCrystalUse(true);
 
  pinMode(ledPin, OUTPUT); // This is used for testing.
 
  Wire.begin(SLAVE_ADDRESS); // Set up the Wire library and make Arduino the slave
   
  /* Define the callbacks for i2c communication */
  Wire.onReceive(processMessage); // Used to specify a function when data received from Master
  Wire.onRequest(sendIMUReading); // Used to specify a function when the Master requests data
   
}
 
/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete
*/
/**************************************************************************/
void loop(void)
{
 
  while (!done) {
    /* Get a new sensor event */
    sensors_event_t event;
    bno.getEvent(&event);
 
    /* Display the floating point data */
    Serial.print("Yaw: ");
    yaw = (int) event.orientation.x;
    Serial.print(yaw);
    if (yaw < 0) {
      imu_data[8] = 1;  // Capture the sign information
      yaw = abs(yaw);
    }
    else {
      imu_data[8] = 0;
    }
    if (yaw > 360) {
      yaw = yaw - 360; // Calculate equivalent angle
    } 
     
    Serial.print("\tPitch: ");
    pitch = (int) event.orientation.y;
    Serial.print(pitch);
    if (pitch < 0) {
      imu_data[4] = 1;   // Capture the sign information
      pitch = abs(pitch);
    }
    else {
      imu_data[4] = 0;
    }
     
    Serial.print("\tRoll: ");
    roll = (int) event.orientation.z; 
    Serial.print(roll);
    if (roll < 0) {
      imu_data[0] = 1;    // Capture the sign information
      roll = abs(roll);
    }    
    else {
      imu_data[0] = 0;
    }
 
    /* Optional: Display calibration status */
    displayCalStatus();
 
    /* Optional: Display sensor status (debug only) */
    //displaySensorStatus();
 
    /* New line for the next sample */
    Serial.println("");
 
    /* Update the IMU data by extracting each digit from the raw data */
    imu_data[1] = getDigit(roll, 2);
    imu_data[2] = getDigit(roll, 1);
    imu_data[3] = getDigit(roll, 0);
    imu_data[5] = getDigit(pitch, 2);
    imu_data[6] = getDigit(pitch, 1);
    imu_data[7] = getDigit(pitch, 0);
    imu_data[9] = getDigit(yaw, 2);
    imu_data[10] = getDigit(yaw, 1);
    imu_data[11] = getDigit(yaw, 0);
 
    /* Wait the specified delay before requesting nex data */
    delay(BNO055_SAMPLERATE_DELAY_MS);
 
    end_program();
 
  }
          
  // Do nothing
  while (true){};
 
}