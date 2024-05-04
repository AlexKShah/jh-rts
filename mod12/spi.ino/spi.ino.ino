#include <Adafruit_BNO055.h>
#include <SPI.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

const int slaveSelectPin = 10;

struct IMUData {
  float roll;
  float pitch;
  float yaw;
};

IMUData imuData;

void setup() {
  bno.begin();
  SPI.begin();
  pinMode(slaveSelectPin, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);
}

void loop() {
  sensors_event_t event;
  bno.getEvent(&event);
  imuData.roll = event.orientation.x;
  imuData.pitch = event.orientation.y;
  imuData.yaw = event.orientation.z;

  sendDataOverSPI();
  delay(10);
}

void sendDataOverSPI() {
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(slaveSelectPin, LOW);

  SPI.transfer((uint8_t*)&imuData, sizeof(IMUData));

  digitalWrite(slaveSelectPin, HIGH);
  SPI.endTransaction();
}