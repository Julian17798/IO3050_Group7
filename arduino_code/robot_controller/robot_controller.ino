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

// Initialize motor controller.
MotorController testMotorController(&motor1, &motor2, 2);

// Initialize MPU Reader
MPUReader testMpu;

// Initialize PID Controller.
PIDController testPid(0, 1, 1, 0);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  // Setup the MPU reader.
  testMpu.mpuSetup(0x68);

  // Setup our custom serial commands.
  setupSerialCommands(&testMotorController);
  
  delay(1000);

  Serial.println(F("*** START ***"));

  testMotorController.setMotorsTimed(100, 100, 1000);
}

void loop() {  
  handleSerial();
  testMotorController.handleMotors();
}
