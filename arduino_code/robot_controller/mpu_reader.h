#ifndef mpu_reader_h
#define mpu_reader_h

#include <Arduino.h>
#include <MPU9250.h>

class MPUReader {
  /*MPU Reader class.
  Initialize it in the main program. Make sure to run mpuSetup() in Setup(). Use updateAngle() to get the Y angle of the MPU.*/
  
  private:
    unsigned long _lastUpdateTime;
    float _gyroAngle, _currentAngle;
    MPU9250 _mpu;

  public:
    MPUReader(void);
    void mpuSetup(void);
    float updateAngle(void);
    
};

#endif
