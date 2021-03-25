#include <SerialCommands.h>
#include "arm.h"

#define servo1Pin 2
#define servo2Pin 4
#define servo3Pin 7

uint8_t defaultAngles[3] = {30, 60, 127};

Servo servo1, servo2, servo3;
ArmController arm(&servo1, &servo2, &servo3);

void setup() {
  Serial.begin(38400);

  setupSerialCommands();

  arm.setupServos(servo1Pin, servo2Pin, servo3Pin, false);

  delay(1000);
  for (uint8_t i = 0; i < 3; i++) {
    arm.setTarget(i, defaultAngles[i]);
  }
}

void loop() {
  handleSerial();
  arm.updateArm();
}
