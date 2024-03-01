#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_2     true

#define TURN_SPEED_SLOW 150
#define FORWARD_SPEED_SLOW 150
#define TURN_SPEED_FAST 100
#define FORWARD_SPEED_FAST 100
#define LEFT_FACTOR 1

#define ENA 5
#define ENB 6
#define IN1 8
#define IN2 9
#define IN3 12
#define IN4 13

#define DIRECTIONPIN 1
#define BUTTONPIN 4
#define RIGHTBUMPER 2
#define LEFTBUMPER 3

#define LEFTNOTINSYNC 0
#define RIGHTNOTINSYNC 0

#define IRIN_RIGHT A0
#define IRIN_MIDDLE_LEFT A3
#define IRIN_MIDDLE_RIGHT A4
#define IRIN_MIDDLE A2
#define IRIN_LEFT A1

#define THRESHHOLD 500

#define LEFT_SIDE 1

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt

int forward_speed = FORWARD_SPEED_SLOW;
int turn_speed = TURN_SPEED_SLOW;

int lookingforT = 0;
int leftTseen = 0
int rightTseen = 0;

int done_turning = 0;

int bumper_pressed = 0;

typedef enum {
  ORIENTATING, LEAVING_START, LEAVING_LEFTTURN, LEAVING_RIGHTTURN, 
  DETECTING_FIRSTTURN, LINEFOLLOW_LEFT, LINEFOLLOW_RIGHT, LINEFOLLOW_FORWARD, 
  OFF_LINE, TURN_TO_CONTACT, BUMPERING, BUMPERING_FORWARD, TO_GOAL, STOP
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
  return analogRead(pin) > THRESHHOLD
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
  if (digitalRead(DIRECTIONPIN)) {
   setleftmotorspeed(-220);
   setrightmotorspeed(0);
  }
  else {
   setleftmotorspeed(0);
   setrightmotorspeed(-220);
  }
  state = TURN_TO_CONTACT;
}

void checkforbumper() {
  if (digitalRead(RIGHTBUMPER) || digitalRead(LEFTBUMPER)) {
    state = STOP;
    setleftmotorspeed(0);
    setrightmotorspeed(0);
  }
}
void bothpressed() {
  trans_stop();
  if (!done_turning) {
    turn_speed = TURN_SPEED_SLOW;
    forward_speed = FORWARD_SPEED_SLOW;
    ITimer2.setInterval(2000, turn, 2001);
  }
}

void trans_forward() {
    state = FORWARD;
    setleftmotorspeed(forward_speed*LEFT_FACTOR);
    setrightmotorspeed(forward_speed);
}

void trans_stop() {
    state = STOP;
    setleftmotorspeed(0);
    setrightmotorspeed(0);
}


void trans_right() {
    state = RIGHT;
    setleftmotorspeed(turn_speed);
    setrightmotorspeed(-turn_speed);
}

void trans_left() {
    state = LEFT;
    setleftmotorspeed(-turn_speed);
    setrightmotorspeed(turn_speed);
}

void trans_offline() {
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
  if (digitalRead(RIGHTBUMPER)) {
    if (bumper_pressed==1) {
      bothpressed();
    }
    else {
      setrightmotorspeed(-255);
      setleftmotorspeed(255);
      state = BUMPERING;
      bumper_pressed = 2;
      ITimer2.setInterval(200, bumpered, 201);
    }
  }
  if (digitalRead(LEFTBUMPER) && state != TURN && state != STOP) {
    if (bumper_pressed==2) {
      bothpressed();
    }
    else {
      setrightmotorspeed(255);
      setleftmotorspeed(-255);
      state = BUMPERING;
      bumper_pressed = 1;
      ITimer2.setInterval(200, bumpered, 201);
    }
  }
}

void check_T() {
  if (lookingforT) {
    if (lineseen(IRIN_LEFT)) {
      setleftmotorspeed(0);
      setrightmotorspeed(turn_speed);
      state = T_LEFTTURN
    }
    if (lineseen(IRIN_RIGHT)) {
      setleftmotorspeed(turn_speed);
      setrightmotorspeed(0);
      state = T_RIGHTTURN
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

  delay(1000);
  state = SPINNING;
  //setleftmotorspeed(-255);
  //setrightmotorspeed(255);
  trans_forward();
}

void loop() {
  switch (state) {
    case ORIENTATING:
      if (digitalRead(BUTTONPIN)) {
        trans_stop();
        ITimer2.setInterval(500, trans_offline, 501);
      }
      break;
    case LEAVING_START:
      if (lineseen(IRIN_LEFT)) {
        setleftmotorspeed(0);
        state = LEAVING_LEFTTURN
      }
      if (lineseen(IRIN_RIGHT)) {
        setrightmotorspeed(0);
        state = LEAVING_RIGHTTURN
      }
      break;
    case LEAVING_LEFTTURN:
      if (lineseen(IRIN_RIGHT)) {
        trans_forward();
        state = STOP;
        ITimer2.setInterval(trans_firstturn, 500, 501);
      }
      break;
    case LEAVING_RIGHTTURN:
      if (lineseen(IRIN_LEFT)) {
        trans_forward();
        state = STOP;
        ITimer2.setInterval(trans_firstturn, 500, 501);
      }
      break;
    case DETECTING_FIRSTTURN:
      if (digitalRead(DIRECTIONPIN) && (lineseen(IRIN_MIDDLE_LEFT) || lineseen(IRIN_LEFT))) {
        trans_left();
        ITimer2.setInterval(lookforT, 500, 501)
      }
      if (!digitalRead(DIRECTIONPIN) && (lineseen(IRIN_MIDDLE_RIGHT) || lineseen(IRIN_RIGHT))) {
        trans_right();
        ITimer2.setInterval(lookforT, 500, 501)
      }
    case FORWARD:
      check_T();
      check_bumper();
      if (analogRead(IRIN_RIGHT) > RIGHT_THRESH) {
        trans_right();
      }
      if (analogRead(IRIN_LEFT) > LEFT_THRESH) {
        trans_left();
      }
      break;
    case LEFT:
      check_T();
      check_bumper();
      if (lookingforT) {
        if (lineseen(IRIN_LEFT)) {
          setleftmotorspeed(0);
          setrightmotorspeed(turn_speed);
          state = T_LEFTTURN
        }
        if (lineseen(IRIN_RIGHT)) {
          setleftmotorspeed(turn_speed);
          setrightmotorspeed(0);
          state = T_RIGHTTURN
        }
      }
      if (analogRead(IRIN_MIDDLE) > MIDDLE_THRESH) {
        trans_forward();
      }
      break;
    case RIGHT:
      check_T();
      check_bumper();
      if (analogRead(IRIN_MIDDLE) > MIDDLE_THRESH) {
        trans_forward();
      }
      break;
    case T_LEFTTURN:
      if (lineseen(IRIN_RIGHT)) {
        leftTseen = 0
        rightTseen = 0;
        trans_forward();
        state = STOP;
        ITimer2.setInterval(trans_offline, 500, 501);
      }
      break;
    case T_RIGHTTURN:
      if (lineseen(IRIN_LEFT)) {
        leftTseen = 0
        rightTseen = 0;
        trans_forward();
        state = STOP;
        ITimer2.setInterval(trans_offline, 500, 501);
      }
      break;
    case OFF_LINE:
      if (lineseen(IRIN_RIGHT)) {
        if (leftTseen) {
          leftTseen = 0;
          state = STOP;
          if (digitalRead(DIRECTIONPIN)) {
            ITimer2.setInterval(200, trans_right, 201)
          }
          else{
            ITimer2.setInterval(200, trans_left, 201)
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
          if (digitalRead(DIRECTIONPIN)) {
            ITimer2.setInterval(200, trans_right, 201)
          }
          else{
            ITimer2.setInterval(200, trans_left, 201)
          }
        }
        else {
          leftTseen = 1;
        }
      }
      break;
    case BUMPERING:
      if (!digitalRead(RIGHTBUMPER) && !digitalRead(LEFTBUMPER)) {
        turn_speed = TURN_SPEED_FAST;
        forward_speed = FORWARD_SPEED_FAST;
        trans_forward();
        state = BUMPERING_FORWARD;
      }
      break;
    case TURN_TO_CONTACT:
      delay(840);
      trans_stop();
      ITimer2.setInterval(500, trans_togoal, 501);
      break;
  }
}
