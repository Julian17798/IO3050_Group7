/*Returns the sum of an int buffer.*/
template<size_t S>
int bufferSum(CircularBuffer<int, S> &cb) {

  // Loop over all elements in the buffer and add them together.
  int sum = 0;
  for (uint8_t i = 0; i < cb.size(); i++) sum += cb[i];
  return sum;
}

/*Returns the float average of an int buffer.*/
template<size_t S>
float bufferAverage(CircularBuffer<int, S> &cb) {

  // Get the sum of the buffer and divide by its size.
  int sum = bufferSum(cb);
  return (float) sum / cb.size();
}

/*Calibrates the mpu angle for a given amount of time.*/
void calibrateAngle(int calibrationTime) {

  // Recalibrate the angle for the given amount of time.
  unsigned long startTime = millis();
  while (startTime + calibrationTime > millis()) {
    int angle = mpu.updateAngle();
    angleBuffer.push(angle);
  }

  // Assign the new target value and reset the offset to 0.0.
  target = bufferAverage(angleBuffer) / 100;
  offset = 0.0;
}
