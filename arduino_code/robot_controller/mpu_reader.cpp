#include <Arduino.h>
#include <MPU9250.h>
#include "mpu_reader.h"

/*Constructor.*/
MPUReader::MPUReader(): _mpu() {
  _lastUpdateTime = 0;
}

/*MPU setup method. Connects with the MPU at a given address.*/
void MPUReader::mpuSetup(int address) {

  if (!_mpu.setup(address)) {
    while (1) {
      Serial.println(F("MPU connection failed. Please check your connection with `connection_check` example."));
      delay(5000);
    }
  }
}

/*Updates and returns the current angle of the MPU.*/
int MPUReader::updateAngle() {

  unsigned long curTime = millis();
  float deltaTime = curTime - _lastUpdateTime;
  _lastUpdateTime = curTime;

  _mpu.update();

  float gyroY = -_mpu.getGyroY();
  float accAngle = atan2(_mpu.getAccX(), _mpu.getAccZ()) * RAD_TO_DEG;
  float angle = 0.99 * ((float) _currentAngle / 100 + gyroY * deltaTime / 1000) + 0.01 * (accAngle);
  _currentAngle = (int) (angle * 100);

  return _currentAngle;
}
