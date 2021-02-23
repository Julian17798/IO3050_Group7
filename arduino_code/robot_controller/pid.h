#ifndef pid_h
#define pid_h

#include <Arduino.h>

class PIDController {
  /*PID controller class.
  Initialize it in the main program and run cycles in a loop of some sort.*/
  
  private:
    float _totalError, _previousError;
    unsigned long _lastUpdateTime;

  public:
    float kp, ki, kd;
    float targetValue;    
    bool printValues;
  
    PIDController(float target = 0, float kp = 1, float ki = 0, float kd = 0);
    float runCycle(float currentValue);
  
};

#endif
