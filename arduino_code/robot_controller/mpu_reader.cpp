#include <Arduino.h>
#include <MPU9250.h>
#include "mpu_reader.h"

/*Constructor.*/
MPUReader::MPUReader(int ledPin): _mpu() {
  _ledPin = ledPin;
}

/*MPU setup method. Connects with the MPU at a given address.*/
void MPUReader::mpuSetup(int address) {

  if (!_mpu.setup(address)) {
    while (1) {
      Serial.println(F("MPU connection failed. Please check your connection with `connection_check` example."));
      delay(5000);
    }
  }

  calibrate();
}

/*Handles the calibration of the MPU.*/
void MPUReader::calibrate() {
  //Calibration acivation
    pinMode(_ledPin, OUTPUT);//Lampje voor testen
    Serial.println(F("Accel cal starts in 5s"));
    Serial.println(F("Leave the device on a flat plane"));
    _mpu.verbose(true);
    delay(5000);
    _mpu.calibrateAccelGyro();

    Serial.println(F("Mag cal starts in 5s"));
    Serial.println(F("Wave device in a figure eight until done"));
    digitalWrite(_ledPin, HIGH);//Warns user to wave device in figure 8
    delay(5000);
    _mpu.calibrateMag();

    Serial.println(F("<cal parameters>"));
    Serial.println(F("accel bias [g]: "));
    Serial.print(_mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(F(", "));
    Serial.print(_mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(F(", "));
    Serial.print(_mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.println();
    Serial.println(F("gyro bias [deg/s]: "));
    Serial.print(_mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(F(", "));
    Serial.print(_mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(F(", "));
    Serial.print(_mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.println();
    Serial.println(F("mag bias [mG]: "));
    Serial.print(_mpu.getMagBiasX());
    Serial.print(F(", "));
    Serial.print(_mpu.getMagBiasY());
    Serial.print(F(", "));
    Serial.print(_mpu.getMagBiasZ());
    Serial.println();
    Serial.println(F("mag scale []: "));
    Serial.print(_mpu.getMagScaleX());
    Serial.print(F(", "));
    Serial.print(_mpu.getMagScaleY());
    Serial.print(F(", "));
    Serial.print(_mpu.getMagScaleZ());
    Serial.println();
    
    _mpu.verbose(false);
    digitalWrite(_ledPin, LOW);
    Serial.println(F("Cal finished"));
}

/*Updates and returns the current angle of the MPU.*/
int MPUReader::updateAngle() {

  if (_mpu.update()) {
      static uint32_t prev_ms = millis();
      if (millis() > prev_ms + 25) {
        _currentAngle = (int) (_mpu.getPitch() * 100);
          prev_ms = millis();
      }
  }
  return _currentAngle;
}
