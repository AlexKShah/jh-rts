#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

void TaskReadIMU(void *pvParameters);
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup(void) {
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("Orientation Sensor Test");
  if (!bno.begin()) {
    Serial.print("No BNO055 detected !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    while (1)
      ;
  }

  // Create task
  if (xTaskCreate(TaskReadIMU, "ReadIMU", 256, NULL, 3, NULL) != pdFAIL) {
    Serial.println("Task created successfully.");
    delay(500);
  } else {
    Serial.println("Failed to create task.");
  }
  bno.setExtCrystalUse(true);
}

void loop(void) {
  // do tasks
}

void TaskReadIMU(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

    Serial.print("X/PITCH: ");
    Serial.println(euler.x());
    Serial.print("Y/ROLL: ");
    Serial.println(euler.y());
    Serial.print("Z/YAW: ");
    Serial.println(euler.z());

    delay(BNO055_SAMPLERATE_DELAY_MS);
  }
}