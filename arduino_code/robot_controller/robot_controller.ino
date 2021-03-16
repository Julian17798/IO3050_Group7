#include <CytronMotorDriver.h>
#include <SerialCommands.h>
#include <CircularBuffer.h>
#include "pid.h"
#include "mpu_reader.h"
#include "motor_controller.h"

/*This project requires the SerialCommands library by Pedro Tiago Pereira and the CircularBuffer library by AgileWare.*/

// Define motor pins.
#define pwm1a 3
#define pwm1b 9
#define pwm2a 10
#define pwm2b 11

// Configure the motor drivers.
CytronMD motor1(PWM_PWM, pwm1a, pwm1b);
CytronMD motor2(PWM_PWM, pwm2a, pwm2b);

// Initialize motor controller and pass pointers to the motor drivers.
MotorController motorController(&motor1, &motor2, 2, 50);

// Initialize MPU Reader
MPUReader mpu;

// Initialize PID Controller.
PIDController pid(0, 10, 3, 0.17); // <target, kp, ki, kd>
PIDController targetPid(0, 0.000015, 0, 0.00001);

// Initialize Circular Buffers
#define angleBufferSize 3
CircularBuffer<int, angleBufferSize> angleBuffer;
#define mileageBufferSize 50
CircularBuffer<int, mileageBufferSize> mileageBuffer;

#define motorUpdateInterval 20

bool balanceMode = true;

float target = 0.0;
float offset = 0.0;

unsigned long lastMotorUpdateTime = 0;

void setup() {
  Serial.begin(38400);
  Wire.begin();
  delay(2000);

  // Setup the MPU reader.
  mpu.mpuSetup(0x68);

  delay(2000);

  // Setup custom serial commands.
  setupSerialCommands();

  delay(2000);

  // Turn on the motors for one second.
  motorController.setMotorsUntimed(100, 100, false);

  delay(1000);

  motorController.setMotorsUntimed(0, 0, false);

  // Fill the buffers with 0.
  fillBuffer(angleBuffer, 0, true);
  fillBuffer(mileageBuffer, 0, true);

  // Calibrate the angle for 5000 ms.
  calibrateAngle(5000);

  Serial.println(F("*START*"));
}

void loop() {
  handleSerial();
  handleMotors();
}

void handleMotors() {
  if (millis() < lastMotorUpdateTime + motorUpdateInterval) return;

  if (!balanceMode) {
    motorController.handleMotors();
  }
  else {
    int angle = mpu.updateAngle();
    angleBuffer.push(angle);
    int pidResult = (int) pid.runCycle(bufferAverage(angleBuffer) / 100);
    
    bool balanced = pidResult > -motorController.minSignal && pidResult < motorController.minSignal;
    
    int mileageInput = balanced? 0 : pidResult;    

    mileageBuffer.push(constrain(mileageInput, -255, 255));

    offset += targetPid.runCycle(-bufferSum(mileageBuffer));
    offset = constrain(offset, -20, 20);
    pid.targetValue = target + offset;

    Serial.print(bufferAverage(angleBuffer) / 100);
    Serial.print(F("\t"));
    //    Serial.print(pidResult);
    //    Serial.print(F("\t"));
    //    Serial.print(-mileageSum());
    //    Serial.print(F("\t"));
    Serial.println(pid.targetValue);

    if (!balanced) motorController.setMotorsUntimed(pidResult, pidResult, false);
    else motorController.handleMotors();

  }
  lastMotorUpdateTime = millis();
}

/*Fills a given int buffer with the given int number.*/
template<size_t S>
void fillBuffer(CircularBuffer<int, S> &cb, int filler, bool completeFill) {
  if (!completeFill) {
    while (!cb.isFull()) {
      cb.push(filler);
    }
  }
  else {
    for (int i = 0; i < cb.size(); i++) {
      cb.push(filler);
    }
  }
}

/*Returns the sum of an int buffer.*/
template<size_t S>
int bufferSum(CircularBuffer<int, S> &cb) {
  int sum = 0;
  for (int i = 0; i < cb.size(); i++) {
    sum += cb[i];
  }
  return sum;
}

/*Returns the float average of an int buffer.*/
template<size_t S>
float bufferAverage(CircularBuffer<int, S> &cb) {
  int sum = bufferSum(cb);
  return (float) sum / cb.size();
}

/*Calibrates the mpu angle for a given amount of time.*/
void calibrateAngle(int calibrationTime) {
  unsigned long startTime = millis();
  while (startTime + calibrationTime > millis()) {
    int angle = mpu.updateAngle();
    angleBuffer.push(angle);
  }

  target = bufferAverage(angleBuffer) / 100;
  offset = 0.0;
}
