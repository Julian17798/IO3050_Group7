#ifndef mpu_reader_h
#define mpu_reader_h

#include <Arduino.h>
#include <MPU9250.h>

/*MPU Reader class.
  Initialize it in the main program. Make sure to run mpuSetup() in Setup(). Use updateAngle() to get the Y angle of the MPU.*/
class MPUReader {

  private:
    unsigned long _lastUpdateTime;
    float _gyroAngle, _currentAngle;
    MPU9250 _mpu;

  public:
    MPUReader(void);
    void mpuSetup(int address);
    float updateAngle(void);

};

#endif
