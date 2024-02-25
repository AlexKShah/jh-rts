/*
Alexander Shah
Project 3 - IR Tach
2/25/24
*/
//IR Photodiode
const int sensorPin = 2;
volatile unsigned int rpscount = 0;
// Flag for interrupt
volatile bool newDataAvailable = false;

void count_rps() {
  rpscount++; // count rpms on interrupt
}

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  //Reset timers and counter
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  //Set compare match for every ~1 second
  OCR1A = 15624;
  // Turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  //interrupt with sensor pin reading on falling
  attachInterrupt(digitalPinToInterrupt(sensorPin), count_rps, FALLING);
}

void loop() {
  if (newDataAvailable) {
    //use interrupt disabling during main loop
    noInterrupts();
    //reading rotations every ~1 second = rps * 60 = rpm
    unsigned int rpm = rpscount * 60;

    Serial.println(rpm);

    //reset and resume interrupts
    rpscount = 0;
    newDataAvailable = false;
    interrupts();
  }
}

ISR(TIMER1_COMPA_vect) {
  //set flag to enter main loop
  newDataAvailable = true;
}