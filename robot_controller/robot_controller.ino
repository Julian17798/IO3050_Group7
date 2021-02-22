#include <CytronMotorDriver.h>
#include "pid.h"

// Configure the motor driver.
CytronMD motor1(PWM_PWM, 3, 9);   // PWM 1A = Pin 3, PWM 1B = Pin 9.
CytronMD motor2(PWM_PWM, 10, 11); // PWM 2A = Pin 10, PWM 2B = Pin 11.

// Initialize PID Controller
PIDController testPid(1, 1, 1, 0);

void setup() {
  Serial.begin(9600);
  while(!Serial) {}
}

void loop() {
//  testPid.RunCycle(1);
  
//  motor1.setSpeed(128);   // Motor 1 runs forward at 50% speed.
//  motor2.setSpeed(-128);  // Motor 2 runs backward at 50% speed.
//  delay(1000);
//  
//  motor1.setSpeed(255);   // Motor 1 runs forward at full speed.
//  motor2.setSpeed(-255);  // Motor 2 runs backward at full speed.
//  delay(1000);
//
//  motor1.setSpeed(0);     // Motor 1 stops.
//  motor2.setSpeed(0);     // Motor 2 stops.
//  delay(1000);
//
//  motor1.setSpeed(-128);  // Motor 1 runs backward at 50% speed.
//  motor2.setSpeed(128);   // Motor 2 runs forward at 50% speed.
//  delay(1000);
//  
//  motor1.setSpeed(-255);  // Motor 1 runs backward at full speed.
//  motor2.setSpeed(255);   // Motor 2 runs forward at full speed.
//  delay(1000);
//
//  motor1.setSpeed(0);     // Motor 1 stops.
//  motor2.setSpeed(0);     // Motor 2 stops.
//  delay(1000);
}
