# Stanford ME210 Final Project Codebase

This is code developed in collaboration with Pierre Labroche. It drives a robot built for the Stanford class ME210.
### Scripts:

- IR_Test: Script for testing and calibrating IR sensors. When connected to a computer, it continuously prints the read value so the sensors can be adjusted to maximize response
- Motor_Test: When a button is pressed, it moves the motors forward until the robot hits a wall, then backs up. Used to calibrate speed and motor direction
- Servo_Test: Used to test the servo motor, and develop algorithms for both the ball release and the contact hitter
- Linefollow_Test: Initial development and testing for the line following algorithm using 3 line sensors
- Final_Script: The final, fully functional and commented script uploaded to main arduino
- Final_Ultrasonic: The final, fully functional and commented script uploaded to ultrasonic sensing arduino

Other branches:

- ServoMotors was used to develop the servo motor control scripts
- Working was the code used for the very first functioning run -- kept for posterity, but less commented and efficient than Main.
