#ifndef motor_controller
#define motor_controller

#include <CytronMotorDriver.h>

/*Motor Controller class.
 Handles everything related to controlling the motors.*/
class MotorController {

  private:
    CytronMD *_motor1;
    CytronMD *_motor2;
    
    bool _flipM1, _flipM2;
    bool _timerActive;
    unsigned long _timerTarget;
    int _lastSpd1, _lastSpd2;
    int _lastManualSpd1, _lastManualSpd2;

    void setMotorSpeeds(int spd1, int spd2, bool manual);
    void activateTimer(int duration);

  public:
    uint8_t minSignal;
  
    MotorController(CytronMD *motor1, CytronMD *motor2, uint8_t flipMotor, uint8_t minSignal);
    void handleMotors(void);
    void setMotorsTimed(int spd1, int spd2, int duration, bool manual);
    void setMotorsUntimed(int spd1, int spd2, bool manual);
    void flipMotor(uint8_t motor);

};

#endif
