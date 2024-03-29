#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_2     true

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt


#define IRIN_RIGHT A4
#define IRIN_MIDDLE_LEFT A2
#define IRIN_MIDDLE_RIGHT A0
#define IRIN_MIDDLE A1
#define IRIN_LEFT A3

void setup() {
  // put your setup code here, to run once:
  ITimer2.init();
  pinMode(IRIN_LEFT, INPUT);
  pinMode(IRIN_RIGHT, INPUT);
  pinMode(IRIN_MIDDLE, INPUT);

  Serial.begin(9600);
  while (!Serial);
  Serial.println(1);
  ITimer2.setInterval(500, printir);
}

void printir() {
  Serial.println(analogRead(IRIN_LEFT));
}
void loop() {

}
