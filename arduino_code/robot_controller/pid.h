#ifndef pid_h
#define pid_h

#include <Arduino.h>

/*PID controller class.
  Initialize it in the main program and run cycles in a loop of some sort.*/
class PIDController {

  private:
    float _totalError, _previousError;
    unsigned long _lastUpdateTime;

  public:
    float kp, ki, kd, filterCoefficient;
    float targetValue;
    float pidMod;
    bool printValues;

    PIDController(float target, float kp, float ki, float kd, float filterCoefficient);
    float runCycle(float currentValue);
    void modifyConstants(float kp, float ki, float kd);

};

#endif
