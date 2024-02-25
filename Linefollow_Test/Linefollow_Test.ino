#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_1     true

#define ENA 6
#define ENB 5
#define IN1 10
#define IN2 11
#define IN3 12
#define IN4 13

#define SPEEDPIN A0
#define BUTTONPIN 3
#define RIGHTBUMPER 8
#define LEFTBUMPER 9

#define LEFTNOTINSYNC 1
#define RIGHTNOTINSYNC 0

#define IRIN_RIGHT A4
#define IRIN_MIDDLE A5
#define IRIN_LEFT A3

#define LEFT_THRESH 500
#define MIDDLE_THRESH 500
#define RIGHT_THRESH 500

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt

int inp;
int outp;

typedef enum {
  LEFT, RIGHT, FORWARD, STOP
} States_t;
States_t state;

void setup() {
  ITimer1.init();
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(SPEEDPIN, INPUT);
  pinMode(BUTTONPIN, INPUT);
  pinMode(RIGHTBUMPER, INPUT);
  pinMode(LEFTBUMPER, INPUT);

  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(11, 0);

  Serial.begin(9600);
  while (!Serial);

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

void trans_forward() {
  state = FORWARD;
  inp = analogRead(SPEEDPIN);
  outp = map(inp, 0, 1024, 0, 255);
  setleftmotorspeed(outp);
  setrightmotorspeed(outp);
}

void trans_right() {
  state = RIGHT;
  inp = analogRead(SPEEDPIN);
  outp = map(inp, 0, 1024, -255, 255);
  setleftmotorspeed(-outp);
  setrightmotorspeed(outp);
}

void trans_left() {
  state = LEFT;
  inp = analogRead(SPEEDPIN);
  outp = map(inp, 0, 1024, -255, 255);
  setleftmotorspeed(outp);
  setrightmotorspeed(-outp);
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
  }
}
