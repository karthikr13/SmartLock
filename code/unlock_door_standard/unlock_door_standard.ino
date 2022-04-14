#include <Servo.h>

Servo myservo;

int SERVO_PIN = 6;

//pause time
const int PAUSE_TIME = 5000; // time servo waits before locking the door again, units of ms

//servo constants
const int LOCKED_POS = 0;
const int UNLOCKED_POS = 45;


void setup() {

  myservo.attach(SERVO_PIN);  // attach servo to board
  myservo.write(LOCKED_POS); // start in locked position

}


// INTEGRATE THIS LOGIC WITH RFID CREDENTIALS BEING VERIFIED IN OTHER ARDUINO FILE
void loop() {
  delay(1000);
  unlock_door();
  
}

void unlock_door() {

  //rotate to desired angle
  myservo.write(UNLOCKED_POS);

  //wait in unlocked state
  delay(PAUSE_TIME);

  //rotate back to locked state
  myservo.write(LOCKED_POS);


  
}
