#include <CytronMotorDriver.h>
#include <SerialCommands.h>
#include <CircularBuffer.h>
#include <Servo.h>
#include "pid.h"
#include "mpu_reader.h"
#include "motor_controller.h"

/*This project requires the SerialCommands library by Pedro Tiago Pereira and the CircularBuffer library by AgileWare.*/

// Define motor pins.
#define pwm1a 3
#define pwm1b 5
#define pwm2a 6
#define pwm2b 11

// Configure the motor drivers.
CytronMD motor1(PWM_PWM, pwm1a, pwm1b);
CytronMD motor2(PWM_PWM, pwm2a, pwm2b);

// Initialize motor controller and pass pointers to the motor drivers.
MotorController motorController(&motor1, &motor2, 2, 50);

// Initialize MPU Reader.
MPUReader mpu;

// Initialize PID Controller.
PIDController pid(0, 10, 12, 0.03); // <target, kp, ki, kd>
PIDController targetPid(0, 0.000015, 0, 0.00001);

// Initialize Circular Buffers.
#define angleBufferSize 3
CircularBuffer<int, angleBufferSize> angleBuffer;
#define mileageBufferSize 50
CircularBuffer<int, mileageBufferSize> mileageBuffer;

// Define servo pins.
#define servo1Pin 2
#define servo2Pin 4
#define servo3Pin 7

// Initialize Servo objects.
Servo servo1;
Servo servo2;
Servo servo3;

#define motorUpdateInterval 20

bool balanceMode = true;

float target = 0.0;
float offset = 0.0;

unsigned long lastMotorUpdateTime = 0;

void setup() {
  Serial.begin(38400);
  Wire.begin();
  delay(2000);

  // Attach servo pins.
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);

  testServos();

  // Setup the MPU reader.
  mpu.mpuSetup(0x68);

  delay(2000);

  // Setup custom serial commands.
  setupSerialCommands();

  delay(2000);

  // Turn on the motors for one second.
//  motorController.setMotorsUntimed(100, 100, false);
//
//  delay(1000);
//
//  motorController.setMotorsUntimed(0, 0, false);

  // Fill the buffers with 0.
  fillBuffer(angleBuffer, 0, true);
  fillBuffer(mileageBuffer, 0, true);

  // Calibrate the angle for 5000 ms.
  calibrateAngle(5000);

  Serial.println(F("*START*"));
  pid.pidMod = -1;
}

void loop() {
  handleSerial();
  handleMotors();
}

void handleMotors() {

  // Exit the function if not enough time since the last motor update has passed.
  if (millis() < lastMotorUpdateTime + motorUpdateInterval) return;

  // Set the last update time to now.
  lastMotorUpdateTime = millis();

  // Execute instructions given through serial if balance mode is off.
  if (!balanceMode) {
    motorController.handleMotors();
  }
  // Or balance the robot.
  else {
    // Get the angle and do PID calculations.
    int angle = mpu.updateAngle();
    angleBuffer.push(angle);
    int pidResult = (int) pid.runCycle(bufferAverage(angleBuffer) / 100);

    // Check whether the robot is balanced.
    bool balanced = pidResult > -motorController.minSignal && pidResult < motorController.minSignal;

    // Add mileage to the mileage buffer.
    int mileageInput = balanced ? 0 : pidResult;
    mileageBuffer.push(constrain(mileageInput, -255, 255));

    // Do PD calculations for the dynamic setpoint and set the offset.
    offset += targetPid.runCycle(-bufferSum(mileageBuffer));
    offset = constrain(offset, -20, 20);
    offset = 0;
    pid.targetValue = target + offset;

    Serial.print(bufferAverage(angleBuffer) / 100);
    Serial.print(F("\t"));
    Serial.print(pidResult);
    Serial.print(F("\t"));
    //    Serial.print(-mileageSum());
    //    Serial.print(F("\t"));
    Serial.println(pid.targetValue);

    // Control the motors using the PID result.
    if (!balanced) motorController.setMotorsUntimed(pidResult, pidResult, false);
    // Or execute serial instructions if the robot is balanced.
    else motorController.handleMotors();

  }
}

/*Fills a given int buffer with the given int number.*/
template<size_t S>
void fillBuffer(CircularBuffer<int, S> &cb, int filler, bool completeFill) {

  // Fill the buffer until it's full.
  if (!completeFill) {
    while (!cb.isFull()) cb.push(filler);
  }
  // Or completely fill the buffer with the given filler.
  else {
    for (int i = 0; i < cb.size(); i++) cb.push(filler);
  }
}

/*Returns the sum of an int buffer.*/
template<size_t S>
int bufferSum(CircularBuffer<int, S> &cb) {

  // Loop over all elements in the buffer and add them together.
  int sum = 0;
  for (int i = 0; i < cb.size(); i++) sum += cb[i];
  return sum;
}

/*Returns the float average of an int buffer.*/
template<size_t S>
float bufferAverage(CircularBuffer<int, S> &cb) {

  // Get the sum of the buffer and divide by its size.
  int sum = bufferSum(cb);
  return (float) sum / cb.size();
}

/*Calibrates the mpu angle for a given amount of time.*/
void calibrateAngle(int calibrationTime) {

  // Recalibrate the angle for the given amount of time.
  unsigned long startTime = millis();
  while (startTime + calibrationTime > millis()) {
    int angle = mpu.updateAngle();
    angleBuffer.push(angle);
  }

  // Assign the new target value and reset the offset to 0.0.
  target = bufferAverage(angleBuffer) / 100;
  offset = 0.0;
}

void testServos() {
  delay(2000);
  Serial.println("Moving servo1");
  servo1.write(10);
  delay(2000);
  Serial.println("Moving servo2");
  servo2.write(10);
  delay(2000);
  Serial.println("Moving servo3");
  servo3.write(10);
  delay(2000);
  Serial.println("Moving servo1");
  servo1.write(200);
  delay(2000);
  Serial.println("Moving servo2");
  servo2.write(200);
  delay(2000);
  Serial.println("Moving servo3");
  servo3.write(200);
  delay(2000);
//  servo1.detach();
//  servo2.detach();
//  servo3.detach();
}
