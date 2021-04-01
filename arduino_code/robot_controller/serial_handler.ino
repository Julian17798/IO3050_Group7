// Serial Commands setup
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
extern MotorController motorController;
extern PIDController pid, targetPid;
extern ArmController arm;
extern Sequencer seq0, seq1, seq2;
extern CircularBuffer<int, angleBufferSize> angleBuffer;
extern CircularBuffer<int, mileageBufferSize> mileageBuffer;

#define invArg F("ERROR INVALID_ARGUMENT")
#define invNum F("ERROR INVALID_NUMBER")
#define misArg F("ERROR MISSING_ARGUMENT")

// Serial commands and their input string triggers
SerialCommand cmdSwitchMode_("sm", cmdSwitchMode);
SerialCommand cmdSetTimedSpeed_("ts", cmdSetTimedSpeed);
SerialCommand cmdSetUntimedSpeed_("us", cmdSetUntimedSpeed);
SerialCommand cmdStop_("st", cmdStop);
SerialCommand cmdFlipMotor_("fl", cmdFlipMotor);
SerialCommand cmdModifyPidConsts_("pid", cmdModifyPidConsts);
SerialCommand cmdSetTarget_("tg", cmdSetTarget);
SerialCommand cmdFlipPid_("fp", cmdFlipPid);
SerialCommand cmdGetPid_("gp", cmdGetPid);
SerialCommand cmdServoTarget_("sv", cmdServoTarget);
SerialCommand cmdServoTargets_("s3", cmdServoTargets);
SerialCommand cmdStartSequence_("sq", cmdStartSequence);
SerialCommand cmdReset_("rs", cmdReset);

/*Sets up all of our custom serial commands.*/
void setupSerialCommands() {
  serial_commands_.SetDefaultHandler(cmdUnrecognized);
  serial_commands_.AddCommand(&cmdSwitchMode_);
  serial_commands_.AddCommand(&cmdSetTimedSpeed_);
  serial_commands_.AddCommand(&cmdSetUntimedSpeed_);
  serial_commands_.AddCommand(&cmdStop_);
  serial_commands_.AddCommand(&cmdFlipMotor_);
  serial_commands_.AddCommand(&cmdModifyPidConsts_);
  serial_commands_.AddCommand(&cmdSetTarget_);
  serial_commands_.AddCommand(&cmdFlipPid_);
  serial_commands_.AddCommand(&cmdGetPid_);
  serial_commands_.AddCommand(&cmdServoTarget_);
  serial_commands_.AddCommand(&cmdServoTargets_);
  serial_commands_.AddCommand(&cmdStartSequence_);
  serial_commands_.AddCommand(&cmdReset_);
}

/*Reads the serial input and calls the relevant commands.*/
void handleSerial() {
  serial_commands_.ReadSerial();
}

/*Default handler. Gets called when no other command matches.*/
void cmdUnrecognized(SerialCommands* sender, const char* cmd) {
  sender->GetSerial()->print(F("Unrecognized command ["));
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println(F("]"));
}

/*Toggles the mode of the robot.*/
void cmdSwitchMode(SerialCommands* sender) {
  balanceMode = !balanceMode;

  motorController.setMotorsUntimed(0, 0, true);

  if (balanceMode) sender->GetSerial()->println(F("Balance on"));
  else sender->GetSerial()->println(F("Balance off"));
}

/*Serial command that turns on the motors at given speeds for a given amount of time.*/
void cmdSetTimedSpeed(SerialCommands* sender) {

  // Get and validate arguments.
  char* spd1Str = sender->Next();
  if (!validateIntInput(sender, spd1Str)) return;

  char* spd2Str = sender->Next();
  if (!validateIntInput(sender, spd2Str)) return;

  char* timeStr = sender->Next();
  if (!validateIntInput(sender, timeStr)) return;

  // Turn string arguments into ints.
  int spd1 = atoi(spd1Str);
  int spd2 = atoi(spd2Str);
  int duration = atoi(timeStr);

  // Activate the motors at the given speeds for a given amount of time.
  motorController.setMotorsTimed(spd1, spd2, duration, true);

  sender->GetSerial()->print(F("Set timed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->print(spd2);
  sender->GetSerial()->print(F(", duration: "));
  sender->GetSerial()->println(duration);
}

/*Serial command that turns on the motors at given speeds.*/
void cmdSetUntimedSpeed(SerialCommands* sender) {

  // Get and validate arguments.
  char* spd1Str = sender->Next();
  if (!validateIntInput(sender, spd1Str)) return;

  char* spd2Str = sender->Next();
  if (!validateIntInput(sender, spd2Str)) return;

  // Turn string arguments into ints.
  int spd1 = atoi(spd1Str);
  int spd2 = atoi(spd2Str);

  // Activate the motors at the given speeds.
  motorController.setMotorsUntimed(spd1, spd2, true);

  sender->GetSerial()->print(F("Set untimed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->println(spd2);
}

/*Serial command that stops the motors.*/
void cmdStop(SerialCommands* sender) {
  
  // Stop motors and timer
  motorController.setMotorsUntimed(0, 0, true);

  sender->GetSerial()->println(F("Stopped motors"));
}

/*Serial command that flips the input for a given motor in future commands.*/
void cmdFlipMotor(SerialCommands* sender) {

  // Get and validate the argument.
  char* mStr = sender->Next();
  if (!validateIntInput(sender, mStr)) return;

  // Turn string argument into int
  int m = atoi(mStr);

  // Check whether the int value is valid for this command.
  if (m != 1 && m != 2 && m != 3) {
    sender->GetSerial()->println(invNum);
    return;
  }

  // Flip future motor inputs.
  motorController.flipMotor(m);

  if (m != 3) {
    sender->GetSerial()->print(F("Flipped motor "));
    sender->GetSerial()->println(m);
  } else {
    sender->GetSerial()->println(F("Flipped motors"));
  }
}

/*Modifies the constants of the PID controller when given a string constant name and a float value.*/
void cmdModifyPidConsts(SerialCommands* sender) {

  // Get arguments.
  char* cStr = sender->Next();
  char* modStr = sender->Next();

  // Validate float input.
  if (!validateFloatInput(sender, modStr)) return;
  float arg = atof(modStr);

  // Modify the right constant.
  if (strcmp("kp", cStr) == 0) {
    pid.modifyConstants(arg, pid.ki, pid.kd);
  } else if (strcmp("ki", cStr) == 0) {
    pid.modifyConstants(pid.kp, arg, pid.kd);
  } else if (strcmp("kd", cStr) == 0) {
    pid.modifyConstants(pid.kp, pid.ki, arg);
  } else {
    sender->GetSerial()->println(invArg);
  }
}

/*Modifies the target value of the PID controller.*/
void cmdSetTarget(SerialCommands* sender) {

  // Get and validate the argument.
  char* targetStr = sender->Next();
  if (!validateFloatInput(sender, targetStr)) return;

  // Set the target value of the pid.
  float input = atof(targetStr);
  pid.targetValue = atof(targetStr);

  sender->GetSerial()->print(F("Changed target to "));
  sender->GetSerial()->println(input);
}

/*Flips PID signal.*/
void cmdFlipPid(SerialCommands* sender) {

  pid.pidMod *= -1;

  sender->GetSerial()->print(F("Flipped PID"));
}

/*Prints current PID constants.*/
void cmdGetPid(SerialCommands* sender) {

  sender->GetSerial()->print(F("kp = "));
  sender->GetSerial()->print(pid.kp);
  sender->GetSerial()->print(F(", ki = "));
  sender->GetSerial()->print(pid.ki);
  sender->GetSerial()->print(F(", kd = "));
  sender->GetSerial()->println(pid.kd);
}

/*Sets the target signal for a given servo number.*/
void cmdServoTarget(SerialCommands* sender) {
  
  // Get and validate arguments.
  char* servoStr = sender->Next();
  if (!validateIntInput(sender, servoStr)) return;

  char* targetStr = sender->Next();
  if (!validateIntInput(sender, targetStr)) return;

  int servo = atoi(servoStr);
  if (servo < 0 || servo > 2) {
    sender->GetSerial()->println(F("INVALID SERVO"));
    return;
  }  
  
  int target = atoi(targetStr);
  if (target < 0 || target > 255) {
    sender->GetSerial()->println(F("INVALID SIGNAL"));
    return;
  }

  Serial.print(F("Setting servo "));
  Serial.print(servo);
  Serial.print(F(" to "));
  Serial.println(target);
  
  arm.setTarget(servo, target);
}

/*Sets the target signal for all servos to three given ints.*/
void cmdServoTargets(SerialCommands* sender) {

  // Get and validate arguments
  char* target1Str = sender->Next();
  if (!validateIntInput(sender, target1Str)) return;

  char* target2Str = sender->Next();
  if (!validateIntInput(sender, target2Str)) return;

  char* target3Str = sender->Next();
  if (!validateIntInput(sender, target3Str)) return;

  int target1 = atoi(target1Str);
  if (target1 < 0 || target1 > 255) {
    sender->GetSerial()->println(F("INVALID SIGNAL"));
    return;
  }
  
  int target2 = atoi(target2Str);
  if (target2 < 0 || target2 > 255) {
    sender->GetSerial()->println(F("INVALID SIGNAL"));
    return;
  }
  
  int target3 = atoi(target3Str);
  if (target3 < 0 || target3 > 255) {
    sender->GetSerial()->println(F("INVALID SIGNAL"));
    return;
  }

  Serial.print(F("Setting servos to "));
  Serial.print(target1);
  Serial.print(F(", "));
  Serial.print(target2);
  Serial.print(F(", "));
  Serial.println(target3);
  
  arm.setTargets(target1, target2, target3);  
}

/*Starts the sequences.*/
void cmdStartSequence(SerialCommands* sender) {
  seq0.startSequence();
  seq1.startSequence();
  seq2.startSequence();
}

/*Resets the pids, the motors and the angle.*/
void cmdReset(SerialCommands* sender) {

  // Reset the pid controllers.
  pid.reset();
  targetPid.reset();

  // Turn off the motors.
  motorController.setMotorsUntimed(0, 0, true);

  // Recalibrate the angle.
  calibrateAngle(5000);

  // Refill the buffers with 0.
  fillBuffer(angleBuffer, 0, true);
  fillBuffer(mileageBuffer, 0, true);
}

/*Checks whether the input string is an integer number.*/
bool isInt(String str) {

  // Check if the first character is either a digit or a '-'.
  if (!isdigit(str.charAt(0)) && str.charAt(0) != '-') return false;

  // Loop over the rest of the characters and check if they are digits.
  for (int i = 1; i < str.length(); i++) {
    if (!isdigit(str.charAt(i))) return false;
  }

  // Return true if the input string is a valid int.
  return true;
}

/*Checks whether the input string is a valid command int argument.*/
bool validateIntInput(SerialCommands* sender, char* strInput) {

  // Check if the string is empty.
  if (strInput == NULL) {
    sender->GetSerial()->println(misArg);
    return false;
  }
  // Check if the string is a valid int.
  else if (!isInt(strInput)) {
    sender->GetSerial()->println(invArg);
    return false;
  }
  return true;
}

/*Checks whether the input string is a float number.*/
bool isFloat(String str) {
  bool dotEncountered = false;

  // Check if the first character is a digit, a '-' or a '.'.
  if (!isdigit(str.charAt(0)) && str.charAt(0) != '-') {
    if (str.charAt(0) == '.') {
      dotEncountered = true;
    } else {
      return false;
    }
  }

  // Loop over the rest of the characters to check whether they are digits or a dot.
  // If more than 1 '.' is encountered, return false.
  for (int i = 1; i < str.length(); i++) {
    if (str.charAt(i) == '.') {
      if (dotEncountered) return false;
      else dotEncountered = true;
    }
    else if (!isdigit(str.charAt(i))) return false;
  }

  // Return true if the input string is a valid float.
  return true;
}

/*Checks whether the input string is a valid command float argument.*/
bool validateFloatInput(SerialCommands* sender, char* strInput) {

  // Check if the string is empty.
  if (strInput == NULL) {
    sender->GetSerial()->println(misArg);
    return false;
  }
  // Check if the string is a valid int.
  else if (!isFloat(strInput)) {
    sender->GetSerial()->println(invArg);
    return false;
  }
  return true;
}
