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

    void setMotorSpeeds(int spd1, int spd2);
    void activateTimer(int duration);

  public:
    MotorController(CytronMD *motor1, CytronMD *motor2, int flipMotor);
    void handleMotors(void);
    void setMotorsTimed(int spd1, int spd2, int duration);
    void setMotorsUntimed(int spd1, int spd2);
    void flipMotor(byte motor);

};

#endif
