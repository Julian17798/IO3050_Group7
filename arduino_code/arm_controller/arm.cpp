#include "arm.h"

/*Constructor.*/
ArmController::ArmController(Servo *servo1, Servo *servo2, Servo *servo3) {

  servos[0] = servo1;
  servos[1] = servo2;
  servos[2] = servo3;

  lastUpdateTime = 0;
  servoSpeed = 1;
  updateInterval = 20;

}

/*Attach ports and set the servos to the middle angle.*/
void ArmController::setupServos(uint8_t port1, uint8_t port2, uint8_t port3, uint8_t defaultAngles[3]) {

  // Attach given ports
  servos[0]->attach(port1);
  servos[1]->attach(port2);
  servos[2]->attach(port3);

  // Set all servos to 127 and set the min and max signals for each servo to 0 and 255 respectively.
  for (uint8_t i = 0; i < 3; i++) {
    servos[i]->write(defaultAngles[i]);
    s_current[i] = defaultAngles[i];
    s_targets[i] = defaultAngles[i];

    s_min[i] = 0;
    s_max[i] = 255;
  }
}

/*Changes the update interval.*/
void ArmController::setUpdateInterval(uint8_t value) {
  updateInterval = value;
}

/*Changes the number of units the servo signal changes per update.*/
void ArmController::setServoSpeed(uint8_t value) {
  servoSpeed = value;
}

/*Set the minimum signal for a given servo.*/
void ArmController::setMin(uint8_t servo, uint8_t value) {
  if (servo >= 3) return;
  s_min[servo] = value;
}

/*Set the maximum signal for a given servo.*/
void ArmController::setMax(uint8_t servo, uint8_t value) {
  if (servo >= 3) return;
  s_max[servo] = value;
}

/*Set the target signal for a given servo.*/
void ArmController::setTarget(uint8_t servo, uint8_t value) {
  if (servo >= 3) return;
  s_targets[servo] = constrain(value, s_min[servo], s_max[servo]);
}

/*Set the targets for all servos.*/
void ArmController::setTargets(uint8_t value1, uint8_t value2, uint8_t value3) {
  setTarget(0, value1);
  setTarget(1, value2);
  setTarget(2, value3);
}

/*Updates servo signals depending on their target signals.*/
void ArmController::updateArm() {

  // Return if it is not yet time to update.
  if (millis() < this->lastUpdateTime + this->updateInterval) return;
  this->lastUpdateTime = millis();

  // Loop over all servos.
  for (byte i = 0; i < 3; i++) {

    // Skip this iteration if the servo has already reached the target signal.
    if (s_current[i] == s_targets[i]) continue;

    // Find the difference between the target and the current signal for the servo.
    int dSignal = s_targets[i] - s_current[i];

    // Change the current signal depending on the difference between the target and the current signal.
    s_current[i] += abs(dSignal) >= servoSpeed ? (dSignal / abs(dSignal)) * servoSpeed : dSignal;

    // Write the new signal to the servo.
    servos[i]->write(s_current[i]);
  }
}
