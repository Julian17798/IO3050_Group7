#ifndef mpu_reader_h
#define mpu_reader_h

#include <Arduino.h>
#include <MPU9250.h>

/*MPU Reader class.
  Initialize it in the main program. Make sure to run mpuSetup() in Setup(). Use updateAngle() to get the Y angle of the MPU.*/
class MPUReader {

  private:
    int _ledPin;
    float _currentAngle;
    MPU9250 _mpu;

    void calibrate(void);

  public:
    MPUReader(int ledPin);
    void mpuSetup(int address);
    float updateAngle(void);
    bool mpuSetupComplete;

};

#endif
