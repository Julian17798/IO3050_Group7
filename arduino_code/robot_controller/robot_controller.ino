#include <CytronMotorDriver.h>
#include "pid.h"
#include "mpu_reader.h"

#define pwm1a 3
#define pwm1b 9
#define pwm2a 10
#define pwm2b 11

// Configure the motor driver.
CytronMD motor1(PWM_PWM, pwm1a, pwm1b);
CytronMD motor2(PWM_PWM, pwm2a, pwm2b);

// Initialize MPU Reader
MPUReader testMpu;

// Initialize PID Controller.
PIDController testPid(0, 1, 1, 0);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  testMpu.mpuSetup();
  delay(1000);
}

void loop() {  
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

  float angle = testMpu.updateAngle();
  Serial.print(angle);
  Serial.print("\t");
  Serial.println(testPid.runCycle(angle));
}
