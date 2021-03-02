#include <CytronMotorDriver.h>
#include <SerialCommands.h>
#include "pid.h"
#include "mpu_reader.h"
#include "motor_controller.h"

// Define motor pins.
#define pwm1a 3
#define pwm1b 9
#define pwm2a 10
#define pwm2b 11

// Configure the motor drivers.
CytronMD motor1(PWM_PWM, pwm1a, pwm1b);
CytronMD motor2(PWM_PWM, pwm2a, pwm2b);

// Initialize motor controller and pass pointers to the motor drivers.
MotorController motorController(&motor1, &motor2, 2);

// Initialize MPU Reader
#define ledPin 7
MPUReader mpu(ledPin);

// Initialize PID Controller.
PIDController pid(0, 91.9, 420.4, 5.0, 244.8);

bool balanceMode = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  // Setup the MPU reader.
  mpu.mpuSetup(0x68);

  // Setup our custom serial commands and pass a pointer to the motor controller (required to allow the serial commands to access the controller from another ino file).
  setupSerialCommands(&motorController, &pid);
  
  delay(1000);

  motorController.setMotorsUntimed(100, 100);
  delay(1000);
  motorController.setMotorsUntimed(0, 0);

  Serial.println(F("*** START ***"));
}

void loop() {  
  handleSerial();
  
  if (!balanceMode) {
    motorController.handleMotors();
  } 
  else {
    float angle = mpu.updateAngle();
    int pidResult = (int) pid.runCycle(angle);
  
    motorController.setMotorsUntimed(pidResult, pidResult);
  
    Serial.print(angle);
    Serial.print(F("\t"));
    Serial.println(pidResult);
  
    delay(20);
  }
}
