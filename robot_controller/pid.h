class PIDController {
  /*PID controller class.
  Initialize it in the main program and run cycles in a loop of some sort.*/
  
  private:
    float totalError = 0, previousError = 0;
    unsigned long lastUpdateTime;

  public:
    float kp, ki, kd;
    float targetValue;    
    bool printValues = false;
  
    PIDController(float target = 0, float kp = 1, float ki = 0, float kd = 0){
      /*Constructor. Initializes a PID object with its constants and a target value.*/
      
      targetValue = target;
      
      this->kp = kp;
      this->ki = ki;
      this->kd = kd;

      lastUpdateTime = millis();
    }

    float RunCycle(float currentValue){
      /*This method updates the PID controller and runs one cycle. The input for this method is the variable that needs to
      approach the target value. It returns the result of the PID cycle.*/

      // Calculate the time difference between the current update and the last update.
      float deltaTime = millis() - lastUpdateTime;
      lastUpdateTime = millis();

      // Calculate the proportional error, the difference between the current error and the previous error and update the total error.
      float error = targetValue - currentValue;
      float deltaError = error - previousError;
      totalError += error * deltaTime;

      // Standard PID calculations.
      float feedP = error * kp;
      float feedI = totalError * ki;
      float feedD = deltaError / deltaTime * kd;

      // Replace the previous error with the current error.
      previousError = error;

      // Print the results of the PID calculations if printValues == true.
      if (printValues) {        
        Serial.print(feedP); Serial.print(F("\t"));
        Serial.print(feedI); Serial.print(F("\t"));
        Serial.println(feedD);
      }

      // Return the result of the PID cycle.
      return feedP + feedI + feedD;
    }
  
};
