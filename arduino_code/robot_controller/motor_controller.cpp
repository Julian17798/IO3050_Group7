#include "motor_controller.h"
#include <CytronMotorDriver.h>

#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

/*Constructor.*/
MotorController::MotorController(CytronMD *motor1, CytronMD *motor2, int flipMotor, int minSignal) {
  _motor1 = motor1;
  _motor2 = motor2;
  _timerActive = false;
  _flipM1 = false;
  _flipM2 = false;
  _timerTarget = 0;
  
  _lastSpd1 = 0;
  _lastSpd2 = 0;
  _lastManualSpd1 = 0;
  _lastManualSpd2 = 0;

  this->flipMotor(flipMotor);
  this->minSignal = minSignal;
}

/*Handles timed motor commands.*/
void MotorController::handleMotors() {

  // Check whether a timer is active and see if we've reached the timer target.
  if (_timerActive && millis() >= _timerTarget) {
    setMotorSpeeds(0, 0, true);
    _timerActive = false;
  }

  if (_lastManualSpd1 != _lastSpd1 || _lastManualSpd2 != _lastSpd2) setMotorSpeeds(_lastManualSpd1, _lastManualSpd2, true);
}

/*Sets the speeds of the motors to the given inputs.*/
void MotorController::setMotorSpeeds(int spd1, int spd2, bool manual) {
  if (spd1 > -minSignal && spd1 < minSignal) spd1 = 0;
  if (spd2 > -minSignal && spd2 < minSignal) spd2 = 0;

  // Constrain the input to -255 - 255.
  spd1 = constrain(spd1, -255, 255);
  spd2 = constrain(spd2, -255, 255);

  // Flip the input if necessary.
  if (_flipM1) spd1 *= -1;
  if (_flipM2) spd2 *= -1;
  
  // Set the motor speeds.
  _motor1->setSpeed(spd1);
  _motor2->setSpeed(spd2);

  _lastSpd1 = spd1;
  _lastSpd2 = spd2;

  if (!manual) return;

  _lastManualSpd1 = spd1;
  _lastManualSpd2 = spd2;
}

/*Activates the timer for a given duration.*/
void MotorController::activateTimer(int duration) {
  _timerTarget = millis() + duration;
  _timerActive = true;
}

/*Activates the motors for a given duration at given speeds.*/
void MotorController::setMotorsTimed(int spd1, int spd2, int duration, bool manual) {
  setMotorSpeeds(spd1, spd2, manual);
  activateTimer(duration);
}

/*Activates the motors at given speeds indefinitely.*/
void MotorController::setMotorsUntimed(int spd1, int spd2, bool manual) {
  setMotorSpeeds(spd1, spd2, manual);
  if (manual) _timerActive = false;
}

/*Flips future motor speeds for a given motor.*/
void MotorController::flipMotor(byte motor) {
  // Check if the bit for 1 is on -> 00000001
  if (CHECK_BIT(motor, 0)) _flipM1 = !_flipM1;
  // Check if the bit for 2 is on -> 00000010
  if (CHECK_BIT(motor, 1)) _flipM2 = !_flipM2;
}
