#ifndef sequencer_h
#define sequencer_h

#include <Arduino.h>

/*Sequencer class.
Allows for the sequencing of int values and passes them to a given function.*/
class Sequencer {

  private:
    unsigned long _lastSignalTime;
    void (*func)(uint8_t);
    uint8_t _sequenceSize;
    uint8_t *_outputs;
    uint16_t *_pauses;
    uint8_t _currentIndex;
    bool _playing;

  public:
    Sequencer(void (*function)(uint8_t), uint8_t sequenceSize, uint8_t *outputs, uint16_t *pauses);
    void updateSequence();
    void startSequence();
    void stopSequence();

};

#endif
