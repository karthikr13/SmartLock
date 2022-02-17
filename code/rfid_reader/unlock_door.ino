#include <Servo.h>

Servo myservo;

int SERVO_PIN = -1;
const int START_POS = 0;
const int END_POS = 180; // NEEDS to be experimented with
const int SERVO_STEP_SIZE = 1;
const int PAUSE_TIME = 5000; // time servo waits before locking the door again, units of ms


void setup() {
  myservo.attach(SERVO_PIN);  // attach servo to board
}


// INTEGRATE THIS LOGIC WITH RFID CREDENTIALS BEING VERIFIED IN OTHER ARDUINO FILE
void loop() {
  
  unlock_door();
  
}

void unlock_door() {
   
   int pos = 0;    // variable to store running servo position

   for (pos = START_POS; pos <= END_POS; pos += SERVO_STEP_SIZE) { // goes from start degrees to end degrees
    myservo.write(pos);              // tell servo to go to position pos
    delay(15);                       // wait for the servo to reach new position
  }

  delay(PAUSE_TIME);
  
  for (pos = END_POS; pos >= START_POS; pos -= SERVO_STEP_SIZE) { // goes from end degrees to start degrees
    myservo.write(pos);              // tell servo to go to position pos
    delay(15);                       // wait for the servo to reach new position
  }
  
}
