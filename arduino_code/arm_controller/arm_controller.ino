#include <SerialCommands.h>
#include "arm.h"
#include "sequencer.h"

#define servo1Pin 2
#define servo2Pin 4
#define servo3Pin 7

uint8_t defaultAngles[3] = {30, 60, 127};

uint8_t sigs[4] = {200, 150, 200, 100};
uint16_t pause[3] = {2000, 2000, 2000};

Sequencer seq(&s1Input, 4, sigs, pause);

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

  seq.startSequence();
}

void loop() {
  handleSerial();
  seq.updateSequence();
  arm.updateArm();
}

void s1Input(uint8_t sig) {
  arm.setTarget(1, sig);
}
