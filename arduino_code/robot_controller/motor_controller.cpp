#include "motor_controller.h"
#include <CytronMotorDriver.h>

/*Constructor.*/
MotorController::MotorController(CytronMD *motor1, CytronMD *motor2, int flipMotor) {
  _motor1 = motor1;
  _motor2 = motor2;
  _timerActive = false;
  _flipM1 = false;
  _flipM2 = false;
  _timerTarget = 0;

  this->flipMotor(flipMotor);
      
}

/*Handles timed motor commands.*/
void MotorController::handleMotors() {

  // Check whether a timer is active and see if we've reached the timer target.
  if (_timerActive && millis() >= _timerTarget) {
    setMotorSpeeds(0, 0);
    _timerActive = false;
  }
}

/*Sets the speeds of the motors to the given inputs.*/
void MotorController::setMotorSpeeds(int spd1, int spd2) {

  // Constrain the input to -255 - 255.
  spd1 = constrain(spd1, -255, 255);
  spd2 = constrain(spd2, -255, 255);

  // Flip the input if necessary.
  if (_flipM1) { spd1 *= -1; }
  if (_flipM2) { spd2 *= -1; }
  
  // Set the motor speeds.
  _motor1->setSpeed(spd1);
  _motor2->setSpeed(spd2);
}

/*Activates the timer for a given duration.*/
void MotorController::activateTimer(int duration) {
  _timerTarget = millis() + duration;
  _timerActive = true;
}

/*Activates the motors for a given duration at given speeds.*/
void MotorController::setMotorsTimed(int spd1, int spd2, int duration) {
  setMotorSpeeds(spd1, spd2);
  activateTimer(duration);
}

/*Activates the motors at given speeds indefinitely.*/
void MotorController::setMotorsUntimed(int spd1, int spd2) {
  setMotorSpeeds(spd1, spd2);
  _timerActive = false;
}

/*Flips future motor speeds for a given motor.*/
void MotorController::flipMotor(int motor) {
  switch (motor) {
    
    case 1:
      _flipM1 = !_flipM1;
      break;
      
    case 2:
      _flipM2 = !_flipM2;
      break;

    case 3:
      _flipM1 = !_flipM1;
      _flipM2 = !_flipM2;
      break;
      
    default:
      break;
      
  }
}
