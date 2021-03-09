// Serial Commands setup
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
extern MotorController motorController;
extern PIDController pid;

#define invArg F("ERROR INVALID_ARGUMENT")
#define invNum F("ERROR INVALID_NUMBER")
#define misArg F("ERROR MISSING_ARGUMENT")

// Serial commands and their input string triggers
SerialCommand cmdSwitchMode_("!switchMode", cmdSwitchMode);
SerialCommand cmdSetTimedSpeed_("!timedSpeed", cmdSetTimedSpeed);
SerialCommand cmdSetUntimedSpeed_("!untimedSpeed", cmdSetUntimedSpeed);
SerialCommand cmdStop_("!stop", cmdStop);
SerialCommand cmdFlipMotor_("!flip", cmdFlipMotor);
SerialCommand cmdModifyPidConsts_("!pid", cmdModifyPidConsts);
SerialCommand cmdSetTarget_("!target", cmdSetTarget);
SerialCommand cmdFlipPid_("!flipPid", cmdFlipPid);
SerialCommand cmdGetPid_("!getPid", cmdGetPid);

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

  motorController.setMotorsUntimed(0, 0);

  if (balanceMode) {
    sender->GetSerial()->println(F("Balance on"));
  } else {
    sender->GetSerial()->println(F("Balance off"));
  }
}

/*Serial command that turns on the motors at given speeds for a given amount of time.*/
void cmdSetTimedSpeed(SerialCommands* sender) {
  if (!checkMode(sender, balanceMode, false)) { return; }

  // Get and validate arguments.
  char* spd1Str = sender->Next();
  if (!validateIntInput(sender, spd1Str)) { return; }

  char* spd2Str = sender->Next();
  if (!validateIntInput(sender, spd2Str)) { return; }

  char* timeStr = sender->Next();
  if (!validateIntInput(sender, timeStr)) { return; }

  // Turn string arguments into ints.
  int spd1 = atoi(spd1Str);
  int spd2 = atoi(spd2Str);
  int duration = atoi(timeStr);

  // Activate the motors at the given speeds for a given amount of time.
  motorController.setMotorsTimed(spd1, spd2, duration);  

  sender->GetSerial()->print(F("Set timed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->print(spd2);
  sender->GetSerial()->print(F(", duration: "));
  sender->GetSerial()->println(duration);
}

/*Serial command that turns on the motors at given speeds.*/
void cmdSetUntimedSpeed(SerialCommands* sender) {
  if (!checkMode(sender, balanceMode, false)) { return; }

  // Get and validate arguments.
  char* spd1Str = sender->Next();
  if (!validateIntInput(sender, spd1Str)) { return; }

  char* spd2Str = sender->Next();
  if (!validateIntInput(sender, spd2Str)) { return; }

  // Turn string arguments into ints.
  int spd1 = atoi(spd1Str);
  int spd2 = atoi(spd2Str);

  // Activate the motors at the given speeds.
  motorController.setMotorsUntimed(spd1, spd2);

  sender->GetSerial()->print(F("Set untimed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->println(spd2);
}

/*Serial command that stops the motors.*/
void cmdStop(SerialCommands* sender) {
  if (!checkMode(sender, balanceMode, false)) { return; }

  // Stop motors and timer
  motorController.setMotorsUntimed(0, 0);

  sender->GetSerial()->println(F("Stopped motors"));
}

/*Serial command that flips the input for a given motor in future commands.*/
void cmdFlipMotor(SerialCommands* sender) {

  // Get and validate the argument.
  char* mStr = sender->Next();
  if (!validateIntInput(sender, mStr)) { return; }

  // Turn string argument into
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
  if (!validateFloatInput(sender, modStr)) { return; }
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
  if (!validateFloatInput(sender, targetStr)) { return; }

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

/*Checks whether the input string is an integer number.*/
bool isInt(String str) {
  
  // Check if the first character is either a digit or a '-'.
  if (!isdigit(str.charAt(0)) && str.charAt(0) != '-') { return false; }
  
  // Loop over the rest of the characters and check if they are digits.
  for (int i = 1; i < str.length(); i++) {
    if (!isdigit(str.charAt(i))) { return false; }
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
      if (dotEncountered) { return false; }
      else { dotEncountered = true; }
    }
    else if (!isdigit(str.charAt(i))) { return false; }
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

/*Checks whether the robot mode required for the command to execute is the right one.*/
bool checkMode(SerialCommands* sender, bool mode, bool desiredMode) {
  if (mode == desiredMode) {
    return true;
  }
  else {
    sender->GetSerial()->println(F("Rejected. Wrong mode"));
    return false;
  }
}
