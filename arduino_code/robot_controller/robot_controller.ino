#include <CytronMotorDriver.h>
#include <SerialCommands.h>
#include <CircularBuffer.h>
#include "pid.h"
#include "mpu_reader.h"
#include "motor_controller.h"

/*This project requires the SerialCommands library by Pedro Tiago Pereira.*/

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
PIDController pid(0, 10, 2, 0.17); // <target, kp, ki, kd>
PIDController targetPid(0, 0.001, 0, 0.0001);

long mileage = 0;

// Initialize Circular Buffer
#define bufferSize 1
CircularBuffer<int, bufferSize> angleBuffer;
CircularBuffer<int, 50> mileageBuffer;

bool balanceMode = true;

float target = 0.0;
float offset = 0.0;

void setup() {
  Serial.begin(38400);
  Wire.begin();
  delay(2000);

  // Setup the MPU reader.
  mpu.mpuSetup(0x68);

  delay(2000);

  // Setup our custom serial commands.
  setupSerialCommands();
  
  delay(2000);

  motorController.setMotorsUntimed(100, 100);
  delay(1000);
  motorController.setMotorsUntimed(0, 0);

  for (int i = 0; i < angleBuffer.size(); i++){
    angleBuffer.push(0.0);
  }

  for (int i = 0; i < mileageBuffer.size(); i++){
    angleBuffer.push(0);
  }

  unsigned long startTime = millis();
  while (startTime + 5000 > millis()){
    int angle = mpu.updateAngle();
    angleBuffer.push(angle);    
    Serial.println(bufferAvgAngle());
  }

  target = bufferAvgAngle();

  pid.printValues = false;

  Serial.println(F("*START*"));
}

void loop() {  
  handleSerial();
  
  if (!balanceMode) {
    motorController.handleMotors();
  } 
  else {
    int angle = mpu.updateAngle();
    angleBuffer.push(angle);    
    int pidResult = (int) pid.runCycle(bufferAvgAngle());
    mileageBuffer.push(constrain(pidResult, -255, 255));
  
    motorController.setMotorsUntimed(pidResult, pidResult);
    
    offset += targetPid.runCycle(-mileageAvg());
    float targetVal = target + offset;
    pid.targetValue = constrain(targetVal, target - 20, target + 20);
  
    Serial.print(bufferAvgAngle());
    Serial.print(F("\t"));
    Serial.print(pidResult);
    Serial.print(F("\t"));
    Serial.print(-mileageAvg());
    Serial.print(F("\t"));
    Serial.println(pid.targetValue);
    
  
    delay(20);
  }
}

/*Returns the average of the buffer.*/
float bufferAvgAngle(){
  float sum = 0;
  int bSize = angleBuffer.size();
  for (int i = 0; i < bSize; i++){
    sum += angleBuffer[i];
  }
  return sum / bSize / 100;
}

int mileageAvg(){
  int sum = 0;
  int bSize = mileageBuffer.size();
  for (int i = 0; i < bSize; i++) {
    sum += mileageBuffer[i];
  }
  return sum / bSize;
}
