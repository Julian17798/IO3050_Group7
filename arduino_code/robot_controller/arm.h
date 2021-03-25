#ifndef arm_h
#define arm_h

#include <Servo.h>
#include <Arduino.h>

/*Arm Controller class.
  Handles everything related to controlling the servos in the arm.*/
class ArmController {

  private:
    unsigned long lastUpdateTime;
    Servo *servos[3];
    uint8_t s_current[3];
    uint8_t s_targets[3];
    uint8_t s_min[3];
    uint8_t s_max[3];
    uint8_t servoSpeed;
    uint8_t updateInterval;

  public:
    ArmController(Servo *servo1, Servo *servo2, Servo *servo3);
    void setupServos(uint8_t port1, uint8_t port2, uint8_t port3, bool setAngles);
    void setUpdateInterval(uint8_t value);
    void setServoSpeed(uint8_t value);
    void setMin(uint8_t servo, uint8_t value);
    void setMax(uint8_t servo, uint8_t value);
    void setTarget(uint8_t servo, uint8_t value);
    void setTargets(uint8_t value1, uint8_t value2, uint8_t value3);
    void updateArm();

};

#endif
