#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_2     true

#define TURN_SPEED_SLOW 200
#define FORWARD_SPEED_SLOW 200
#define TURN_SPEED_FAST 255
#define FORWARD_SPEED_FAST 255
#define LEFT_FACTOR 1

#define ENA 6
#define ENB 5
#define IN1 11
#define IN2 12
#define IN3 7
#define IN4 8

#define DIRECTIONPIN 13
#define BUTTONPIN 2
#define RIGHTBUMPER 3
#define LEFTBUMPER 4

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     13  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define LEFTNOTINSYNC 0
#define RIGHTNOTINSYNC 1

#define IRIN_RIGHT A4
#define IRIN_MIDDLE_LEFT A2
#define IRIN_MIDDLE_RIGHT A0
#define IRIN_MIDDLE A1
#define IRIN_LEFT A3

#define THRESHHOLD 700

#define LEFT_SIDE 1

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt
#include <Servo.h>
//#include <NewPing.h>

// Create the servo object
// Servo myservo;
Servo myservo;
Servo myservo_c;

int forward_speed = FORWARD_SPEED_SLOW;
int turn_speed = TURN_SPEED_SLOW;

int lookingforT = 0;
int leftTseen = 0;
int rightTseen = 0;

int done_turning = 0;

int bumper_pressed = 0;

int pos_c;
int pos;

typedef enum {
  ORIENTATING, LEAVING_START, LEAVING_LEFTTURN, LEAVING_RIGHTTURN, 
  DETECTING_FIRSTTURN, LINEFOLLOW_LEFT, LINEFOLLOW_RIGHT, LINEFOLLOW_FORWARD, 
  T_LEFTTURN, T_RIGHTTURN, OFF_LINE, TURN_TO_CONTACT, BUMPERING, BUMPERING_FORWARD, TO_GOAL, STOP, TEST, ORIENTATING_NEW
} States_t;
States_t state;

void setleftmotorspeed(int speed) {
  if (speed > 255) {
    speed = 255;
  }
  if (speed < -255) {
    speed = -255;
  }
  if (speed == 0) {
    digitalWrite(IN1, 0);
    digitalWrite(IN2, 0);
    analogWrite(ENA, 0);
  }
  else {
    digitalWrite(IN1, LEFTNOTINSYNC^(speed > 0));
    digitalWrite(IN2, LEFTNOTINSYNC^(speed < 0));
    analogWrite(ENA, abs(speed));
  }
}

void setrightmotorspeed(int speed) {
  if (speed > 255) {
    speed = 255;
  }
  if (speed < -255) {
    speed = -255;
  }
  if (speed == 0) {
    digitalWrite(IN3, 0);
    digitalWrite(IN4, 0);
    analogWrite(ENB, 0);
  }
  else {
    digitalWrite(IN3, RIGHTNOTINSYNC^(speed > 0));
    digitalWrite(IN4, RIGHTNOTINSYNC^(speed < 0));
    analogWrite(ENB, abs(speed));
  }
}

int lineseen(int pin) {
  return analogRead(pin) > THRESHHOLD;
}

void checkultrasonic() {
  // unsigned int dist = sonar.ping();
  // dist = sonar.convert_cm(dist);
  // if (dist > 105) {
  //   state = ORIENTATING_NEW;
  // }
}

void trans_leaving_start() {
  trans_forward();
  state = LEAVING_START;
}

void trans_firstturn(){
  state = DETECTING_FIRSTTURN;
}

void start_lookforT() {
  lookingforT = 1;
}


void turn() {
   setleftmotorspeed(-255);
   setrightmotorspeed(-255);
  state = TURN_TO_CONTACT;
}

void checkforbumper() {
  if (digitalRead(RIGHTBUMPER) || digitalRead(LEFTBUMPER)) {
    state = STOP;
    setleftmotorspeed(0);
    setrightmotorspeed(0);
  }
}

void bumpered() {
  bumper_pressed = 0;
}

void bothpressed() {
  trans_stop();
  if (!done_turning) {
   turn_speed = TURN_SPEED_SLOW;
   forward_speed = FORWARD_SPEED_SLOW;
   contact_hitter();
   turn();
   //ITimer2.setInterval(2000, turn, 2001);
  }
  else {
    move_ball();
    celebration();
    delay(400);
    celebration();
    delay(400);
    celebration();
  }
}

void trans_forward() {
    state = LINEFOLLOW_FORWARD;
    setleftmotorspeed(forward_speed*LEFT_FACTOR);
    setrightmotorspeed(forward_speed);
}

void trans_stop() {
    state = STOP;
    setleftmotorspeed(0);
    setrightmotorspeed(0);
}


void trans_right() {
    state = LINEFOLLOW_RIGHT;
    setleftmotorspeed(turn_speed);
    setrightmotorspeed(-turn_speed);
}

void trans_left() {
    state = LINEFOLLOW_LEFT;
    setleftmotorspeed(-turn_speed);
    setrightmotorspeed(turn_speed);
}

void trans_offline() {
  lookingforT = 0;
  forward_speed = FORWARD_SPEED_FAST;
  turn_speed = TURN_SPEED_FAST;
  trans_forward();
  state = OFF_LINE;
}

void trans_togoal() {
  forward_speed = FORWARD_SPEED_FAST;
  trans_forward();
  state = TO_GOAL;
  done_turning = 1;
}

void check_bumper() {
  if (digitalRead(RIGHTBUMPER) || digitalRead(LEFTBUMPER)) {
    state = BUMPERING_FORWARD;
    setleftmotorspeed(255);
    setrightmotorspeed(255);
  }
  // if (digitalRead(RIGHTBUMPER)) {
  //   if (bumper_pressed==1) {
  //     bothpressed();
  //   }
  //   else {
  //     setrightmotorspeed(-255);
  //     setleftmotorspeed(255);
  //     state = BUMPERING;
  //     bumper_pressed = 2;
  //     ITimer2.setInterval(200, bumpered, 201);
  //   }
  // }
  // if (digitalRead(LEFTBUMPER) && state != TURN_TO_CONTACT && state != STOP) {
  //   if (bumper_pressed==2) {
  //     bothpressed();
  //   }
  //   else {
  //     setrightmotorspeed(255);
  //     setleftmotorspeed(-255);
  //     state = BUMPERING;
  //     bumper_pressed = 1;
  //     ITimer2.setInterval(200, bumpered, 201);
  //   }
  // }
}

void check_T() {
  if (lookingforT) {
    if (lineseen(IRIN_LEFT)) {
      setleftmotorspeed(0);
      setrightmotorspeed(turn_speed);
      state = T_LEFTTURN;
    }
    else if (lineseen(IRIN_RIGHT)) {
      setleftmotorspeed(turn_speed);
      setrightmotorspeed(0);
      state = T_RIGHTTURN;
    }
  }
}

// void servoturn(int start, int end, float d, Servo myservo_c) {
//   if (start > 180) {
//     start = 180;
//   }
//   if (start < 0) {
//     start = 0;
//   }
//   if (end > 180) {
//     end = 180;
//   }
//   if (end < 0) {
//     end = 0;
//   }
//   if (start > end) {
//     for (int pos_c = start; pos_c >= end; pos_c-= 1) {
//       myservo_c.write(pos_c);
//       delay(d);
//     }
//   }
//   else {
//     for (int pos_c = start; pos_c <= end; pos_c+= 1) {
//       myservo_c.write(pos_c);
//       delay(d);
//     }
//   }
// }

// void contact_hitter() {
//   servoturn(180, 90, 7.5, servo_flag);
//   delay(1000);
//   servoturn(90, 180, 7.5, servo_flag);
// }

// void celebration() {
//   servoturn(180, 0, 2.75, servo_flag);
//   servoturn(0, 180, 2.75, servo_flag);
// }

// void move_ball() {
//   for (int i = 0; i <3; i++) {
//     servoturn(90+30*i*(2*digitalRead(DIRECTIONPIN)-1), 90+30*(i+1)*(2*digitalRead(DIRECTIONPIN)-1), 15, servo_balldrop);
//     delay(1000);
//   }
// }

void contact_hitter() {
  for (pos_c = 180; pos_c >= 90; pos_c-= 1) {
    myservo_c.write(pos_c);
    // Serial.println(pos);
    delay(7.5);
  }
  delay(1000);
  for (pos_c = 90; pos_c <= 180; pos_c+= 1) {
    myservo_c.write(pos_c);
    // Serial.println(pos);
    delay(7.5);
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

void move_ball() {
  if (digitalRead(DIRECTIONPIN)) {
    for (pos = 90; pos >= 60; pos-= 1) {
      myservo.write(pos);
      // Serial.println(pos);
      delay(15);
    }
    delay(1000);
    for (pos = 60; pos >= 30; pos-= 1) {
      myservo.write(pos);
      // Serial.println(pos);
      delay(15);
    }
    delay(1000);
    for (pos = 30; pos >= 0; pos-= 1) {
      myservo.write(pos);
      // Serial.println(pos);
      delay(15);
    }
    delay(1000);
  }
  else {
    for (pos = 90; pos <= 120; pos+= 1) {
      myservo.write(pos);
      // Serial.println(pos);
      delay(15);
    }
    delay(1000);
    for (pos = 120; pos <= 150; pos+= 1) {
      myservo.write(pos);
      // Serial.println(pos);
      delay(15);
    }
    delay(1000);
    for (pos = 150; pos <= 180; pos+= 1) {
      myservo.write(pos);
      // Serial.println(pos);
      delay(15);
    }
  }
}

void setup() {
  ITimer2.init();
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(DIRECTIONPIN, INPUT);
  pinMode(BUTTONPIN, INPUT);
  pinMode(RIGHTBUMPER, INPUT);
  pinMode(LEFTBUMPER, INPUT);

  myservo.attach(9);
  myservo_c.attach(10);

  trans_stop();

  myservo.write(90);
  myservo_c.write(0);
  delay(2000);

  //state = TEST;
  //setleftmotorspeed(150);
  //setrightmotorspeed(150);

  state = ORIENTATING;
  if (!DIRECTIONPIN) {
    setleftmotorspeed(150);
    setrightmotorspeed(-150);
  }
  if (DIRECTIONPIN) {
    setleftmotorspeed(-150);
    setrightmotorspeed(150);
  }
  //trans_leaving_start();
}

void loop() {
  switch (state) {
    case TEST:
      check_bumper();
    case ORIENTATING:
      if (digitalRead(BUTTONPIN)) {
        trans_stop();
        ITimer2.setInterval(500, trans_leaving_start, 501);
      }
      break;
    case ORIENTATING_NEW:
      ITimer2.setInterval(2, checkultrasonic, 3);
      delay(400);
      trans_leaving_start();
      break;
    case LEAVING_START:
      if (lineseen(IRIN_LEFT)) {
        setleftmotorspeed(0);
        state = LEAVING_LEFTTURN;
      }
      if (lineseen(IRIN_RIGHT)) {
        setrightmotorspeed(0);
        state = LEAVING_RIGHTTURN;
      }
      break;
    case LEAVING_LEFTTURN:
      if (lineseen(IRIN_RIGHT)) {
        trans_forward();
        state = STOP;
        ITimer2.setInterval(500, trans_firstturn, 501);
      }
      break;
    case LEAVING_RIGHTTURN:
      if (lineseen(IRIN_LEFT)) {
        trans_forward();
        state = STOP;
        ITimer2.setInterval(500, trans_firstturn, 501);
      }
      break;
    case DETECTING_FIRSTTURN:
      if (!digitalRead(DIRECTIONPIN) && (lineseen(IRIN_MIDDLE_LEFT) || lineseen(IRIN_LEFT))) {
        trans_left();
        ITimer2.setInterval(500,start_lookforT, 501);
      }
      if (digitalRead(DIRECTIONPIN) && (lineseen(IRIN_MIDDLE_RIGHT) || lineseen(IRIN_RIGHT))) {
        trans_right();
        ITimer2.setInterval(500, start_lookforT, 501);
      }
      break;
    case LINEFOLLOW_FORWARD:
      check_bumper();
      if (lineseen(IRIN_MIDDLE_RIGHT)) {
        trans_right();
      }
      if (lineseen(IRIN_MIDDLE_LEFT)) {
        trans_left();
      }
      check_T();
      break;
    case LINEFOLLOW_LEFT:
      check_bumper();
      if (lineseen(IRIN_MIDDLE)) {
        trans_forward();
      }
      check_T();
      break;
    case LINEFOLLOW_RIGHT:
      check_bumper();
      if (lineseen(IRIN_MIDDLE)) {
        trans_forward();
      }
      check_T();
      break;
    case T_LEFTTURN:
      if (lineseen(IRIN_RIGHT)) {
        leftTseen = 0;
        rightTseen = 0;
        trans_forward();
        state = STOP;
        ITimer2.setInterval(1000, trans_offline, 1001);
      }
      break;
    case T_RIGHTTURN:
      if (lineseen(IRIN_LEFT)) {
        leftTseen = 0;
        rightTseen = 0;
        trans_forward();
        state = STOP;
        ITimer2.setInterval(1000, trans_offline, 1001);
      }
      break;
    case OFF_LINE:
      if (lineseen(IRIN_RIGHT)) {
        if (leftTseen) {
          leftTseen = 0;
          state = STOP;
          if (!digitalRead(DIRECTIONPIN)) {
            ITimer2.setInterval(400, trans_right, 401);
          }
          if (digitalRead(DIRECTIONPIN)) {
            ITimer2.setInterval(400, trans_left, 401);
          }
        }
        else {
          rightTseen = 1;
        }
      }
      if (lineseen(IRIN_LEFT)) {
        if (rightTseen) {
          rightTseen = 0;
          state = STOP;
          if (!digitalRead(DIRECTIONPIN)) {
            ITimer2.setInterval(400, trans_right, 401);
          }
          if (digitalRead(DIRECTIONPIN)) {
            ITimer2.setInterval(400, trans_left, 401);
          }
        }
        else {
          leftTseen = 1;
        }
      }
      break;
    // case BUMPERING:
    //   if (!digitalRead(RIGHTBUMPER) && !digitalRead(LEFTBUMPER)) {
    //     turn_speed = TURN_SPEED_FAST;
    //     forward_speed = FORWARD_SPEED_FAST;
    //     trans_forward();
    //     state = BUMPERING_FORWARD;
    //   }
    //   check_bumper();
    //   break;
    case BUMPERING_FORWARD:
      if (digitalRead(RIGHTBUMPER) && digitalRead(LEFTBUMPER)) {
        bothpressed();
      }
      break;
    case TURN_TO_CONTACT:
      delay(110);
      if (!digitalRead(DIRECTIONPIN)) {
        setleftmotorspeed(-255);
        setrightmotorspeed(255);
      }
      if (digitalRead(DIRECTIONPIN)) {
        setleftmotorspeed(255);
        setrightmotorspeed(-255);
      }
      delay(830);
      trans_stop();
      ITimer2.setInterval(500, trans_togoal, 501);
      break;
    case TO_GOAL:
      check_bumper();
      break;
  }
}
