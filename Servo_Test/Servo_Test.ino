
// Include the library
#include <Servo.h>

// Create the servo object
// Servo myservo;
Servo myservo_c;
// int pos;
int pos_c;


// Setup section to run once
void setup() {
  Serial.begin(9600);

  // myservo.attach(9); // attach the servo to our servo object
  myservo_c.attach(10);
  // myservo.write(180); // stop the motor
  myservo_c.write(180);
  delay(1000);
}

// Loop to keep the motor turning!
void loop() {
  // slow_prog();
  // delay(5000);
  // rev_slow_prog();
  // delay(5000);
  // move_ball();
  // delay(5000);
  // myservo.write(180); // stop the motor
  // delay(5000);

  // delay(5000);
  // contact_hitter();
  celebration();

}

// void slow_prog() {
//   for (pos = 90; pos >= 0; pos-= 1) {
//     myservo.write(pos);
//     Serial.println(pos);
//     delay(500);
//   }
// }
// void rev_slow_prog() {
//   for (pos = 0; pos <= 90; pos+= 1) {
//     myservo.write(pos);
//     Serial.println(pos);
//     delay(500);
//   }
// }

// void move_ball() {
//   for (pos = 180; pos >= 145; pos-= 1) {
//     myservo.write(pos);
//     // Serial.println(pos);
//     delay(15);
//   }
//   delay(1000);
//   for (pos = 145; pos >= 115; pos-= 1) {
//     myservo.write(pos);
//     // Serial.println(pos);
//     delay(15);
//   }
//   delay(1000);
//   for (pos = 115; pos >= 85; pos-= 1) {
//     myservo.write(pos);
//     // Serial.println(pos);
//     delay(15);
//   }
//   // delay(1000);
//   // for (pos = 85; pos >= 55; pos-= 1) {
//   //   myservo.write(pos);
//   //   // Serial.println(pos);
//   //   delay(15);
//   // }
//   // delay(1000);
//   // for (pos = 55; pos >= 25; pos-= 1) {
//   //   myservo.write(pos);
//   //   // Serial.println(pos);
//   //   delay(15);
//   // }

// }

// contact hitter
void contact_hitter() {
  for (pos_c = 180; pos_c >= 90; pos_c-= 1) {
    myservo_c.write(pos_c);
    // Serial.println(pos);
    // delay(7.5);
  }
  delay(1000);
  for (pos_c = 90; pos_c <= 180; pos_c+= 1) {
    myservo_c.write(pos_c);
    // Serial.println(pos);
    // delay(7.5);
  }


}

void celebration() {
  for (pos_c = 180; pos_c >= 0; pos_c -= 1) {
    myservo_c.write(pos_c);
    // Serial.println(pos);
    delay(2.75);
  }
 
  for (pos_c = 0; pos_c <= 180; pos_c += 1) {
    myservo_c.write(pos_c);
    // Serial.println(pos);
    delay(2.75);

  }


}

