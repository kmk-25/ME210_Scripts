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

#define LEFT_THRESH 500
#define MIDDLE_THRESH 500
#define RIGHT_THRESH 500

#define LEFT_SIDE 1

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt

int inp;
int outp;
int no_trans = 0;
int forward_speed = FORWARD_SPEED_SLOW;
int turn_speed = TURN_SPEED_SLOW;
int swap = 0;

int r_seen = 0;
int center_seen = 0;
int done_turning = 0;
int bumper_pressed = 0;
int turnflag = 0;
int left_start = 0;

typedef enum {
  LEFT, RIGHT, FORWARD, OFF_LINE, STOP, TURN, TO_GOAL, BUMPERING, BUMPERING_FORWARD,
  SPINNING
} States_t;
States_t state;

void turn() {
  if (digitalRead(DIRECTIONPIN)) {
   setleftmotorspeed(-220);
   setrightmotorspeed(0);
  }
  else {
   setleftmotorspeed(0);
   setrightmotorspeed(-220);
  }
  state = TURN;
}

void go() {
  forward_speed = FORWARD_SPEED_FAST;
  trans_forward();
  state = TO_GOAL;
  done_turning = 1;
}

void del() {
  no_trans = 0;
}

void bumpered() {
  bumper_pressed = 0;
}

void swit() {
  swap = 2;
}

void togoal() {
  trans_forward();
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
  if (!no_trans) {
    state = FORWARD;
    setleftmotorspeed(forward_speed*LEFT_FACTOR);
    setrightmotorspeed(forward_speed);
  }
}

void trans_stop() {
  //if (!no_trans) {
    state = STOP;
    setleftmotorspeed(0);
    setrightmotorspeed(0);
  //}
}


void trans_right() {
  if (!no_trans) {
    state = RIGHT;
    setleftmotorspeed(turn_speed);
    setrightmotorspeed(-turn_speed);
  }
}

void trans_left() {
  if (!no_trans) {
    state = LEFT;
    setleftmotorspeed(-turn_speed);
    setrightmotorspeed(turn_speed);
  }
}

void trans_offline() {
  if (!no_trans) {
    trans_forward();
    state = OFF_LINE;
    if (left_start) {
      ITimer2.setInterval(200, del, 201);
      no_trans = 1;
    }
  }
}

void loop() {
  switch (state) {
    case FORWARD:
      if (analogRead(IRIN_RIGHT) > RIGHT_THRESH) {
        trans_right();
      }
      if (analogRead(IRIN_LEFT) > LEFT_THRESH) {
        trans_left();
      }
      break;
    case LEFT:
      if (analogRead(IRIN_MIDDLE) > MIDDLE_THRESH) {
        trans_forward();
      }
      break;
    case RIGHT:
      if (analogRead(IRIN_MIDDLE) > MIDDLE_THRESH) {
        trans_forward();
      }
      break;
    case OFF_LINE:
      if (!no_trans && analogRead(IRIN_LEFT) > MIDDLE_THRESH) {
        center_seen = 1;
      }
      if (!no_trans && analogRead(IRIN_RIGHT) > RIGHT_THRESH) {
        r_seen = 1;
      }
      if (!swap && center_seen && r_seen && analogRead(IRIN_MIDDLE) < MIDDLE_THRESH && analogRead(IRIN_RIGHT) < RIGHT_THRESH) {
        ITimer2.setInterval(150, swit, 151);
        swap = 1;
      }
      if (swap == 2) {
        swap = 0;
        center_seen = 0;
        r_seen = 0;
        if (!left_start) {
          trans_forward();
          left_start = 1;
        }
        else if (digitalRead(DIRECTIONPIN)) {
          trans_right();
        }
        else {
          trans_left();
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
    case TURN:
      delay(840);
      trans_stop();
      ITimer2.setInterval(500, go, 501);
      break;

    case SPINNING:
      if (digitalRead(BUTTONPIN)) {
        trans_stop();
        ITimer2.setInterval(500, trans_offline, 501);
      }
      break;
  }
  if (digitalRead(RIGHTBUMPER) && state != TURN  && state != STOP) {
    if (bumper_pressed==1) {
      bothpressed();
    }
    else {
      setrightmotorspeed(-255);
      setleftmotorspeed(255);
      state = BUMPERING;
      bumper_pressed = 2;
      ITimer2.setInterval(400, bumpered, 401);
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
