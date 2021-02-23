#include <Arduino.h>
#include <MPU9250.h>
#include "mpu_reader.h"

MPUReader::MPUReader():_mpu(){
  /*Constructor.*/

  //empty for now
}

void MPUReader::mpuSetup(){  
  if (!_mpu.setup(0x68)) {
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }
}

float MPUReader::updateAngle(){
  /*This method updates and returns the current angle of the MPU.*/
  
  // Get the time difference between now and the previous update.
  unsigned long deltaTime = millis() - _lastUpdateTime;
  _lastUpdateTime = millis();

  // Update the MPU.
  _mpu.update();

  // Calculate the angle.
  float gyroY = -_mpu.getGyroY();
  _gyroAngle += (float) gyroY * deltaTime / 1000;
  float accAngle = atan2(_mpu.getAccX(), _mpu.getAccZ()) / (2 * 3.1415) * 360;
  _currentAngle = 0.98 * (_currentAngle + (float) gyroY * deltaTime / 1000) + 0.02 * accAngle;

  return _currentAngle;
}