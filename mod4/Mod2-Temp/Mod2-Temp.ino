/*
Project 2 - Temperature Sensor with Interrupts
Alex Shah
2/18/24
*/

//using a tmp36gz on pin a0
int iPin = A0;

volatile float temperatureF = 0;
volatile bool newReadingAvailable = false; 

void setup() {
  Serial.begin(9600);
  //use AREF port to enhance precision
  analogReference(EXTERNAL);
  attachInterrupt(digitalPinToInterrupt(2), readTemperature, FALLING);
  cli(); //suspend interrupts
  //clear timers
  TCCR1A = 0;
  TCCR1B = 0;
  //set cs12, cs10 bits for prescalar 1024
  TCCR1B |= B00000101;
  //use compare match by setting OCIE1A to 1
  TIMSK1 |= B00000010;
  //set compare register A ~ 10 sec
  OCR1A = 156250;
  //Reset Timer 1 value to 0
  TCNT1 = 0;
  sei(); //resume interrupts
}

void loop() {
  if (newReadingAvailable) {
    Serial.print(millis());
    Serial.print(", ");
    Serial.println(temperatureF);
    newReadingAvailable = false;
  }
}

void readTemperature() {
  int sensorValue = analogRead(iPin); 
  //external reference voltage and resolution
  float fRef = 3.3;
  float fRes = 1024.0;
  // celsuis = (sensor value * (voltage in mw / resolution) - offset) * 100
  float fVolt = sensorValue * (fRef / fRes);
  float fTempC = (fVolt - 0.5) * 100;
  temperatureF = (fTempC * 9 / 5) + 32;
  newReadingAvailable = true;
}

ISR(TIMER1_COMPA_vect) {
  TCNT1  = 0;
  readTemperature();
}
