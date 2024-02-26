#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_2     true

#define TURN_SPEED 190
#define FORWARD_SPEED 255

#define ENA 5
#define ENB 6
#define IN1 12
#define IN2 13
#define IN3 8
#define IN4 9

#define SPEEDPIN A0
#define BUTTONPIN 2
#define RIGHTBUMPER 4
#define LEFTBUMPER 3

#define LEFTNOTINSYNC 0
#define RIGHTNOTINSYNC 0

#define IRIN_RIGHT A1
#define IRIN_MIDDLE A3
#define IRIN_LEFT A2

#define LEFT_THRESH 500
#define MIDDLE_THRESH 500
#define RIGHT_THRESH 500

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt

int inp;
int outp;
int no_trans = 0;

typedef enum {
  LEFT, RIGHT, FORWARD, OFF_LINE, STOP, OFF_LINE_TRANS
} States_t;
States_t state;

void del() {
  no_trans = 0;
}

void setup() {
  ITimer2.init();
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
  if (!no_trans) {
    state = FORWARD;
    //inp = analogRead(SPEEDPIN);
    //outp = map(inp, 0, 1024, 0, 255);
    setleftmotorspeed(FORWARD_SPEED);
    setrightmotorspeed(FORWARD_SPEED);
  }
}

void trans_stop() {
  if (!no_trans) {
    state = STOP;
    setleftmotorspeed(0);
    setrightmotorspeed(0);
  }
}

void trans_right() {
  if (!no_trans) {
    state = RIGHT;
    //inp = analogRead(SPEEDPIN);
    //outp = map(inp, 0, 1024, -255, 255);
    setleftmotorspeed(TURN_SPEED);
    setrightmotorspeed(-TURN_SPEED);
  }
}

void trans_left() {
  if (!no_trans) {
    state = LEFT;
    //inp = analogRead(SPEEDPIN);
    //outp = map(inp, 0, 1024, -255, 255);
    setleftmotorspeed(-TURN_SPEED);
    setrightmotorspeed(TURN_SPEED);
  }
}

void trans_offline() {
  if (!no_trans) {
    trans_stop();
    state = OFF_LINE;
    ITimer2.setInterval(200, del, 201);
    no_trans = 1;
  }
}

void loop() {
  switch (state) {
    case FORWARD:
      if (analogRead(IRIN_RIGHT) > RIGHT_THRESH) {
        if (analogRead(IRIN_LEFT) > LEFT_THRESH) {
          trans_offline();
        }
        else {
          trans_right();
        }
      }
      if (analogRead(IRIN_LEFT) > LEFT_THRESH) {
        trans_left();
      }
      break;
    case LEFT:
      if (analogRead(IRIN_MIDDLE) > MIDDLE_THRESH) {
        if (analogRead(IRIN_LEFT) > LEFT_THRESH) {
          trans_offline();
        }
        else{
          trans_forward();
        }
      }
      break;
    case RIGHT:
      if (analogRead(IRIN_MIDDLE) > MIDDLE_THRESH) {
        if (analogRead(IRIN_RIGHT) > RIGHT_THRESH) {
          trans_offline();
        }
        else{
          trans_forward();
        }
      }
      break;
    case OFF_LINE_TRANS:
      break;
  }

}
