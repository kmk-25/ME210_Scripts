// Include the library
#include <Servo.h>

// Create the servo object
Servo myservo;

// Setup section to run once
void setup() {
  myservo.attach(6); // attach the servo to our servo object
  myservo.write(90); // stop the motor
}

// Loop to keep the motor turning!
void loop() {
  myservo.write(180);
  delay(2000);
  myservo.write(90);
  delay(2000);
  myservo.write(0);
  delay(2000);
}