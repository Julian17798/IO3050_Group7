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

//Initialize testnob
int pinSwitchStatus = 6;
int switchStatus;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  pinMode (pinSwitchStatus, INPUT);//for testnob

  testMpu.mpuSetup();
  delay(1000);
}

void loop() {  
  //simulated angle by testNob
//  switchStatus = analogRead(pinSwitchStatus);
//  Serial.println(switchStatus);
//  float angle=90*switchStatus/1023;

  
  //Calculating required power input for motor
  float angle = testMpu.updateAngle();//Retrieving robot angle
  float CurrentPIDAnswer = testPid.runCycle(angle);//Using angle to calculate PID input
  
  float PIDCorrection = 2500;//<--------------------------------------------------Addapt to change PID input power based on simulink answer
  float motorInput = (CurrentPIDAnswer / PIDCorrection);



  //Activating motor to steer wheels in position based on the motorInput and steeringInput
  int motor1speed = motorInput;// + steeringInputLeft;//Adding steeringinput to motorspeed
  int motor2speed = motorInput;// + steeringInputRight;//<------------------------------------------Maken

  motor1speed = constrain(motor1speed,-255,255);//Constraining motor input
  motor2speed = constrain(motor2speed,-255,255);

  motor1.setSpeed(motor1speed);//Sending signal to motor
  motor1.setSpeed(motor2speed);



  delay(20);//delay

}
