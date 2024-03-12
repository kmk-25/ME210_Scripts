#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_2     true

//Defining speed as a value from 1-255 to slow down the robot if needed
#define TURN_SPEED_SLOW 255
#define FORWARD_SPEED_SLOW 255
#define TURN_SPEED_FAST 255
#define FORWARD_SPEED_FAST 255

//This was for when the left motor was running faster than the right:
//setting it to some value <1 slows it down compared to the right motor
//to ensure the robot travels straight
#define LEFT_FACTOR 1

//Motor control pins
#define ENA 6
#define ENB 5
#define IN1 11
#define IN2 12
#define IN3 7
#define IN4 8

//Sensing pins; note that buttonpin was repurposed for communication
//with the ultrasonic subsystem
#define DIRECTIONPIN 13
#define BUTTONPIN 2
#define RIGHTBUMPER 3
#define LEFTBUMPER 4

//Reverses motor direction; used to compensate for motor power being wired
//differently from build to build
#define LEFTNOTINSYNC 0
#define RIGHTNOTINSYNC 1

//Tape sensing input pins
#define IRIN_RIGHT A4
#define IRIN_MIDDLE_LEFT A2
#define IRIN_MIDDLE_RIGHT A0
#define IRIN_MIDDLE A1
#define IRIN_LEFT A3

//minimum analog read value above which tape is sensed
#define THRESHHOLD 700

#include <TimerInterrupt.h>         //https://github.com/khoih-prog/TimerInterrupt
#include <Servo.h>

// Create the servo object
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

//No comments for the next few functions; either self-explanatory,
//or explained in the state machine control code

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
  //Reorients after hitting the contact zone
  if (!done_turning) {
   turn_speed = TURN_SPEED_SLOW;
   forward_speed = FORWARD_SPEED_SLOW;
   contact_hitter();
   turn();
  }
  else {
    //Dispenses balls after contact switches pressed at the end
    move_ball(1);

    //This part was added last-minute before the competition
    //It turns the robot around, then dispenses balls from the opposite direciton
    myservo.write(90);
    setleftmotorspeed(255);
    setrightmotorspeed(-255);
    delay(1700);
    trans_stop();
    move_ball(-1);
    delay(400);

    //Ending celebration
    celebration();
    delay(400);
    celebration();
    delay(400);
    celebration();
    delay(400);
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

//If either contact sensor is pressed, the robot moves forward
//until the other sensor is pressed
void check_bumper() {
  if (digitalRead(RIGHTBUMPER) || digitalRead(LEFTBUMPER)) {
    state = BUMPERING_FORWARD;
    setleftmotorspeed(255);
    setrightmotorspeed(255);
  }
}

//If either sensor on the far left/right detects a line, the robot
//turns until the other far sensor does too, then records that as a T
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

//This is implementation of slowing down turning with a servo motor
//by writing in incremental steps with a time delay between each
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

void contact_hitter() {
  slowturn(0, 180, myservo_c, 2.75);
}

void celebration() {
  slowturn(180, 0, myservo_c, 2.75);
  slowturn(0, 180, myservo_c, 2.75);
}

//The variable reversed is used to ensure the balls
//are dispensing in different directions before and after
//the robot turns around at the end
void move_ball(int reversed) {
  int direction = reversed*(-digitalRead(DIRECTIONPIN)*2+1);
  for (int curhole = 0; curhole < 3; curhole++){
    slowturn(90+direction*30*curhole, 90+direction*30*(curhole+1), myservo, 15);
  delay(100);
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

  //Code to test contact switches

  //state = TEST;
  //setleftmotorspeed(150);
  //setrightmotorspeed(150);

  //The robot starts spinning, then stops when it detects
  //a signal communicating that it is oriented correctly
  state = ORIENTATING;
  if (!digitalRead(DIRECTIONPIN)) {
    setleftmotorspeed(150);
    setrightmotorspeed(-150);
  }
  if (digitalRead(DIRECTIONPIN)) {
    setleftmotorspeed(-150);
    setrightmotorspeed(150);
  }

  //When switching to the ultrasonic sensor, an initialization delay
  //meant the robot would skip directly to moving forward if faced
  //towards the empty space initially. This corrects that.
  if (digitalRead(BUTTONPIN)) {
    delay(400);
  }

  //Uncomment to test without initial orientation
  //trans_leaving_start();
}

void loop() {
  switch (state) {
    //Test state to check contact switches
    case TEST:
      check_bumper();

    //Spinning at the start to find the correct direction. Changes to finding
    //the edge when oriented correctly
    case ORIENTATING:
      if (digitalRead(BUTTONPIN)) {
        trans_stop();
        ITimer2.setInterval(100, trans_leaving_start, 101);
      }
      break;
    
    //The next three states allow the robot to find the edge of the start zone,
    //reorient to face the line, then travel forward to start line following after a delay
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

    //After leaving the start zone, the robot travels forward until it detects a line on the
    //side it is turning towards, then turns in that direction to start line following. This
    //ensures it can line up properly with the line regardless of starting position
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

    //Basic line following states
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

    //Once the robot detects a T with a far right/left sensor,
    //the following states orient the robot to align with the T
    //before it starts moving forward to find the next line.

    //This is the same algorithm used to leave the starting block
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

    //This waits until both far sensors have detected a line,
    //before transitioning back to line following while turning
    //in the correct direction. This makes the transition from the
    //T to the line connecting to the contact zone
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

    //Waiting for both contact switches once one pressed
    case BUMPERING_FORWARD:
      if (digitalRead(RIGHTBUMPER) && digitalRead(LEFTBUMPER)) {
        bothpressed();
      }
      break;

    //This is a hardcoded algorithm to turn from the contact zone to
    //the ball drop area. The delays are blocking to ensure no other
    //state transitions can interrupt the turn.
    case TURN_TO_CONTACT:
      delay(120);
      if (!digitalRead(DIRECTIONPIN)) {
        setleftmotorspeed(-255);
        setrightmotorspeed(255);
      }
      if (digitalRead(DIRECTIONPIN)) {
        setleftmotorspeed(255);
        setrightmotorspeed(-255);
      }
      delay(900);
      trans_togoal();
      break;

    //Moves straight forward until the robot reaches the ball drop station
    case TO_GOAL:
      check_bumper();
      break;
  }
}
