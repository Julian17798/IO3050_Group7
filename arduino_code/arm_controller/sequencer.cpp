#include "sequencer.h"

Sequencer::Sequencer(void (*function)(uint8_t), uint8_t sequenceSize, uint8_t *outputs, uint16_t *pauses) {
  func = function;
  _sequenceSize = sequenceSize;
  _outputs = outputs;
  _pauses = pauses;
}

void Sequencer::updateSequence() {
  if (!_playing) return;

  if (millis() < _lastSignalTime + *(_pauses + _currentIndex)) return;

  _currentIndex++;
  func(*(_outputs + _currentIndex));
  _lastSignalTime = millis();

  Serial.print(F("Sending "));
  Serial.println(*(_outputs + _currentIndex));

  if (_currentIndex != _sequenceSize - 1) {
    Serial.print(F("Wait for "));
    Serial.println(*(_pauses + _currentIndex));
  }

  if (_currentIndex != _sequenceSize - 1) return;

  _playing = false;
}

void Sequencer::startSequence() {
  if (_playing) return;
  
  func(*_outputs);
  _currentIndex = 0;
  _playing = true;
  _lastSignalTime = millis();

  Serial.print(F("Starting sequence with "));
  Serial.println(*_outputs);
  Serial.print(F("Wait for "));
  Serial.println(*_pauses);
}
