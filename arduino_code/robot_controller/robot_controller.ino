#include <CytronMotorDriver.h>
#include <SerialCommands.h>
#include <CircularBuffer.h>
#include <Servo.h>
#include "pid.h"
#include "mpu_reader.h"
#include "motor_controller.h"
#include "arm.h"
#include "sequencer.h"

/*This project requires
  SerialCommands by Pedro Tiago Pereira,
  CircularBuffer by AgileWare,
  MPU9250 by hideakitai and
  Cytron Motor Drivers Library by Cytron Technologies Sdn Bhd.*/

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
Servo servo1, servo2, servo3;

// The default signals to the servos upon startup.
const uint8_t defaultAngles[3] = {30, 60, 127};

// Initialize an Arm Controller object and pass pointers to the servo objects.
ArmController arm(&servo1, &servo2, &servo3);

// Initialize the Sequencers.

/*How to write a sequence:
  A sequence is defined by two arrays. One is an array of unsigned 8 bit ints (or chars)
  and the other is an array of unsigned 16 bit ints. The first array with length x contains all the output signals
  in the sequence and the second array with length x - 1 contains all the "delays" between the signals in ms.*/

const uint8_t sigs0[4] = {40, 50, 40, 30};
const uint16_t pause0[3] = {2000, 2000, 2000};

/*Initializing a Sequencer object requires the two earlier mentioned arrays as well as a pointer to a function
  and an int specification of how long the sequence is. The function is required to have one single uint8_t argument and
  is called in the sequencer with an element from the output array as input.*/

Sequencer seq0(&s0Input, 4, sigs0, pause0); // <pointer to function, length of sequence, signal array, delay array>

const uint8_t sigs1[4] = {200, 150, 200, 100};
const uint16_t pause1[3] = {2000, 2000, 2000};

Sequencer seq1(&s1Input, 4, sigs1, pause1);

const uint8_t sigs2[4] = {200, 150, 200, 100};
const uint16_t pause2[3] = {2000, 2000, 2000};

Sequencer seq2(&s2Input, 4, sigs2, pause2);


#define motorUpdateInterval 20

bool balanceMode = true;

float target = 0.0, offset = 0.0;

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

  // Fill the buffers with 0.
  fillBuffer(angleBuffer, 0, true);
  fillBuffer(mileageBuffer, 0, true);

  // Calibrate the angle for 5000 ms.
  calibrateAngle(5000);

  // Setup the servos in the mechanical arm.
  arm.setupServos(servo1Pin, servo2Pin, servo3Pin, defaultAngles);

  Serial.println(F("*START*"));
  pid.pidMod = -1;

  // Uncomment this to run the sequences for the servos at startup.
  //  seq0.startSequence();
  //  seq1.startSequence();
  //  seq2.startSequence();
}

void loop() {
  handleSerial();
  seq0.updateSequence();
  seq1.updateSequence();
  seq2.updateSequence();
  arm.updateArm();
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
    bool balanced = abs(pidResult) < motorController.minSignal;

    // Add mileage to the mileage buffer.
    int mileageInput = balanced ? 0 : pidResult;
    mileageBuffer.push(constrain(mileageInput, -255, 255));

    // Do PD calculations for the dynamic setpoint and set the offset.
    offset += targetPid.runCycle(-bufferSum(mileageBuffer));
    offset = constrain(offset, -20, 20);
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

void s0Input(uint8_t sig) {
  arm.setTarget(0, sig);
}

void s1Input(uint8_t sig) {
  arm.setTarget(1, sig);
}

void s2Input(uint8_t sig) {
  arm.setTarget(2, sig);
}
