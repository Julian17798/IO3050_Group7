#include <SerialCommands.h>
#include "arm.h"
#include "sequencer.h"

#define servo1Pin 2
#define servo2Pin 4
#define servo3Pin 7

const uint8_t defaultAngles[3] = {30, 60, 127};

const uint8_t sigs0[4] = {40, 50, 40, 30};
const uint16_t pause0[3] = {2000, 2000, 2000};

Sequencer seq0(&s0Input, 4, sigs0, pause0);

const uint8_t sigs1[4] = {200, 150, 200, 100};
const uint16_t pause1[3] = {2000, 2000, 2000};

Sequencer seq1(&s1Input, 4, sigs1, pause1);

const uint8_t sigs2[4] = {200, 150, 200, 100};
const uint16_t pause2[3] = {2000, 2000, 2000};

Sequencer seq2(&s2Input, 4, sigs2, pause2);

Servo servo1, servo2, servo3;
ArmController arm(&servo1, &servo2, &servo3);

void setup() {
  Serial.begin(38400);

  setupSerialCommands();

  arm.setupServos(servo1Pin, servo2Pin, servo3Pin, defaultAngles);

  delay(1000);

  seq0.startSequence();
  seq1.startSequence();
  seq2.startSequence();
}

void loop() {
  handleSerial();
  seq0.updateSequence();
  seq1.updateSequence();
  seq2.updateSequence();
  arm.updateArm();
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
