#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include <NewPing.h>
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define OUTPUT_PIN 10

#define USE_TIMER_1     true

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#include <TimerInterrupt.h>

typedef enum {PINGING, SENDING, STOPPED} States_t;
States_t state;

void send() {
  digitalWrite(OUTPUT_PIN, HIGH);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  ITimer1.init();
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, LOW);
  state = PINGING;
  //ITimer1.setInterval(1000, sonarping);
}

void loop() {
  //This just pings the ultrasonic sensor once every 100 mS, then
  //sets the output pin high if the distance detected is in the desired range.
  //The desired range corresponds to the ropot facing towards the gap in the wall,
  //and the 700 mS delay gives the robot time to rotate to the line.
  if (state == PINGING) {
    unsigned int uS = sonar.ping();
    uS = sonar.convert_cm(uS);
    if (uS < 120 && uS > 105) {
      state = SENDING;
      delay(700);
      send();
    }
    delay(100);
  }
}
