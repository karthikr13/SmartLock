#include <Servo.h>

Servo myservo;

int SERVO_PIN = 6;

//angle constants
const int START_POS = 0;
const int END_POS = 270; // NEEDS to be experimented with
const int SERVO_STEP_SIZE = 1;

//pause time
const int PAUSE_TIME = 3000; // time servo waits before locking the door again, units of ms

//servo constants
const int CLOCKWISE = 0;
const int COUNTER_CLOCKWISE = 180;
const int NO_MOTION = 90;
int ROTATION_TIME = 0;



void setup() {
  Serial.begin(9600);
  myservo.attach(SERVO_PIN);  // attach servo to board
  myservo.write(NO_MOTION);
  ROTATION_TIME = time_per_angle(END_POS);
  
}


// INTEGRATE THIS LOGIC WITH RFID CREDENTIALS BEING VERIFIED IN OTHER ARDUINO FILE
void loop() {
  Serial.print(ROTATION_TIME, DEC);
  delay(1000);
  unlock_door();
  
}

void unlock_door() {

  //rotate to desired angle
  myservo.write(CLOCKWISE);
  delay(ROTATION_TIME);

  //wait in unlocked state
  myservo.write(NO_MOTION);
  delay(PAUSE_TIME);

  //rotate back to locked state
  myservo.write(COUNTER_CLOCKWISE);
  delay(ROTATION_TIME);
  myservo.write(NO_MOTION);


  
}

int time_per_angle(int angle){
  return int(0.1167/60 * angle * 1000); //for ms
}
