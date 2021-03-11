#include <Arduino.h>
#include "pid.h"

/*Constructor. Initializes a PID object with its constants and a target value.*/
PIDController::PIDController(float target, float kp, float ki, float kd) {

  _totalError = 0;
  _previousError = 0;
  _lastUpdateTime = 0;
  printValues = true;
  pidMod = 1;

  targetValue = target;
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;
}

/*Updates the PID controller and runs one cycle. The input for this method is the variable that needs to
  approach the target value. It returns the result of the PID cycle.*/
float PIDController::runCycle(float currentValue) {

  // Calculate the time difference between the current update and the last update.
  float deltaTime = _lastUpdateTime == 0 ? 0 : (float) (millis() - _lastUpdateTime) / 1000;
  _lastUpdateTime = millis();

  // Calculate the proportional error, the difference between the current error and the previous error and update the total error.
  float error = targetValue - currentValue;
  float deltaError = error - _previousError;
  _totalError += error * deltaTime;
  _totalError = constrain(_totalError, -300, 300);

  // Standard PID calculations.
  float feedP = error * kp;
  float feedI = _totalError * ki;
  float feedD = deltaTime == 0 ? 0 : deltaError / deltaTime * kd;

  // Replace the previous error with the current error.
  _previousError = error;

  // Print the results of the PID calculations if printValues == true. Might be useful for tweaking.
  if (printValues) {
    Serial.print(feedP); Serial.print(F("\t"));
    Serial.print(feedI); Serial.print(F("\t"));
    Serial.println(feedD);
  }

  // Return the result of the PID cycle.
  return (feedP + feedI + feedD) * pidMod;
}

/*Modifies the PID constants.*/
void PIDController::modifyConstants(float kp, float ki, float kd) {
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;

  Serial.print(F("PID modified: kp = "));
  Serial.print(kp);
  Serial.print(F(", ki = "));
  Serial.print(ki);
  Serial.print(F(", kd = "));
  Serial.println(kd);
}
