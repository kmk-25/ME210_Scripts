#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_1     true
#define USE_TIMER_2     true

#define ENA 6
#define ENB 5
#define IN1 11
#define IN2 12
#define IN3 7
#define IN4 8

#define SPEEDPIN A5
#define BUTTONPIN 2
#define RIGHTBUMPER 3
#define LEFTBUMPER 4

#define LEFTNOTINSYNC 0
#define RIGHTNOTINSYNC 1

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt

int inp;
int outp;
int bouncing = 0;
int bouncing2 = 0;
int pushed = 0;
int pushed2 = 0;
int direction = 0;

void debounce() {
  bouncing = 0;
}

void debounce2() {
  bouncing2 = 0;
}

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

  Serial.begin(9600);
  while (!Serial);
  delay(1000);
  setleftmotorspeed(255);
  setrightmotorspeed(255);
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

void loop() {

  //inp = analogRead(SPEEDPIN);
  //outp = map(inp, 0, 1024, 0, 255);
  pushed2 = 1;
  outp = 255;
  if (digitalRead(RIGHTBUMPER) && digitalRead(LEFTBUMPER) && !pushed) {
    pushed = 1;
    bouncing = 1;
    ITimer2.setInterval(1500, debounce, 1501);
    setleftmotorspeed(-outp*pushed2);
    setrightmotorspeed(-outp*pushed2);
  }
  if (!digitalRead(RIGHTBUMPER) && !digitalRead(LEFTBUMPER) && !bouncing && pushed) {
    pushed = 0;
    setleftmotorspeed(outp*pushed2);
    setrightmotorspeed(outp*pushed2);
  }
  // if (digitalRead(BUTTONPIN) && !bouncing2) {
  //   pushed2 = 1^pushed2;
  //   setleftmotorspeed(outp*pushed2);
  //   setrightmotorspeed(outp*pushed2);
  //   bouncing2 = 1;
  //   ITimer1.setInterval(500, debounce2, 501);
  // }
}
