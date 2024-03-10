// Include the library
#include <Servo.h>

#define DIRECTIONPIN 13

// Create the servo object
Servo myservo;
Servo myservo_c;

int pos;
int pos_c;

void slowturn(int startpos, int endpos, Servo servo, float delaytime) {
  if (startpos > 180) {
    startpos = 180;
  }
  if (startpos < 0) {
    startpos = 0;
  }
  if (endpos > 180) {
    endpos = 180;
  }
  if (endpos < 0) {
    endpos = 0;
  }
  if (startpos < endpos) {
    for (int p = startpos; p < endpos; p++) {
      servo.write(p);
      delay(delaytime);
    }
  }
  else {
    for (int p = startpos; p > endpos; p -= 1) {
      servo.write(p);
      delay(delaytime);
    }
  }
}


// Setup section to run once
void setup() {
  pinMode(DIRECTIONPIN, INPUT);
  myservo.attach(9);
  myservo_c.attach(10);
  myservo.write(90);
  myservo_c.write(0);
  delay(5000);
  move_ball();
}

// Loop to keep the motor turning!
void loop() {

}

void contact_hitter() {
  slowturn(0, 180, myservo_c, 2.75);
}

void celebration() {
  slowturn(180, 0, myservo_c, 2.75);
  slowturn(0, 180, myservo_c, 2.75);
}

void move_ball() {
  int direction = -digitalRead(DIRECTIONPIN)*2+1;
  for (int curhole = 0; curhole < 3; curhole++){
    slowturn(90+direction*30*curhole, 90+direction*30*(curhole+1), myservo, 15);
  delay(100);
  } 
}