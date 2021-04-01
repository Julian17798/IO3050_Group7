#include "sequencer.h"

/*Constructor. Takes a pointer to a function, the length of the sequence, and arrays containing
  output values and pause lengths in ms.*/
Sequencer::Sequencer(void (*function)(uint8_t), uint8_t sequenceSize, uint8_t *outputs, uint16_t *pauses) {
  func = function;
  _sequenceSize = sequenceSize;
  _outputs = outputs;
  _pauses = pauses;
}

/*Takes care of playing the sequence.*/
void Sequencer::updateSequence() {

  // Exit out of the method if the sequence is not playing.
  if (!_playing) return;

  // Check whether it is time to change the output. Exit if not.
  if (millis() < _lastSignalTime + * (_pauses + _currentIndex)) return;

  // Increment the index and execute the function with the item in the array with corresponding index.
  _currentIndex++;
  func(*(_outputs + _currentIndex));
  _lastSignalTime = millis();

  // Set _playing to false if the sequence is over.
  if (_currentIndex != _sequenceSize - 1) return;

  _playing = false;
}

/*Starts the playback of a sequence.*/
void Sequencer::startSequence() {

  // Exit the method if the sequence is already playing.
  if (_playing) return;

  // Execute the function with the first element from the output array.
  func(*_outputs);
  _currentIndex = 0;
  _playing = true;
  _lastSignalTime = millis();

  Serial.print(F("Starting sequence with "));
  Serial.println(*_outputs);
  Serial.print(F("Wait for "));
  Serial.println(*_pauses);
}
