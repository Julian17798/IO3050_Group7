// Serial Commands setup
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
MotorController *mainMotorController;

// Serial commands and their input string triggers
SerialCommand cmdSwitchMode_("!switchMode", cmdSwitchMode);
SerialCommand cmdSetTimedSpeed_("!timedSpeed", cmdSetTimedSpeed);
SerialCommand cmdSetUntimedSpeed_("!untimedSpeed", cmdSetUntimedSpeed);
SerialCommand cmdStop_("!stop", cmdStop);
SerialCommand cmdFlipMotor_("!flip", cmdFlipMotor);

/*Sets up all of our custom serial commands.*/
void setupSerialCommands(MotorController *mCtrl) { 
  mainMotorController = mCtrl;
  
  serial_commands_.SetDefaultHandler(cmdUnrecognized);
  serial_commands_.AddCommand(&cmdSwitchMode_);
  serial_commands_.AddCommand(&cmdSetTimedSpeed_);
  serial_commands_.AddCommand(&cmdSetUntimedSpeed_);
  serial_commands_.AddCommand(&cmdStop_);
  serial_commands_.AddCommand(&cmdFlipMotor_);
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

  if (balanceMode) {
    sender->GetSerial()->println(F("Balance mode on."));
  } else {
    sender->GetSerial()->println(F("Balance mode off"));
  }
}

/*Serial command that turns on the motors at given speeds for a given amount of time.*/
void cmdSetTimedSpeed(SerialCommands* sender) {
  if (balanceMode) { return; }

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
  mainMotorController->setMotorsTimed(spd1, spd2, duration);  

  sender->GetSerial()->print(F("Set timed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->print(spd2);
  sender->GetSerial()->print(F(", duration: "));
  sender->GetSerial()->println(duration);
}

/*Serial command that turns on the motors at given speeds.*/
void cmdSetUntimedSpeed(SerialCommands* sender) {
  if (balanceMode) { return; }

  // Get and validate arguments.
  char* spd1Str = sender->Next();
  if (!validateIntInput(sender, spd1Str)) { return; }

  char* spd2Str = sender->Next();
  if (!validateIntInput(sender, spd2Str)) { return; }

  // Turn string arguments into ints.
  int spd1 = atoi(spd1Str);
  int spd2 = atoi(spd2Str);

  // Activate the motors at the given speeds.
  mainMotorController->setMotorsUntimed(spd1, spd2);

  sender->GetSerial()->print(F("Set untimed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->println(spd2);
}

/*Serial command that stops the motors.*/
void cmdStop(SerialCommands* sender) {
  if (balanceMode) { return; }

  // Stop motors and timer
  mainMotorController->setMotorsUntimed(0, 0);

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
    sender->GetSerial()->println(F("ERROR INVALID_NUMBER"));
    return;
  }

  // Flip future motor inputs.
  mainMotorController->flipMotor(m);

  if (m != 3) {
    sender->GetSerial()->print(F("Flipped motor "));
    sender->GetSerial()->println(m);
  } else {
    sender->GetSerial()->println(F("Flipped both motors"));
  }
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

/*Checks whether the input string is a valid command argument.*/
bool validateIntInput(SerialCommands* sender, char* strInput) {

  // Check if the string is empty.
  if (strInput == NULL) {
    sender->GetSerial()->println(F("ERROR MISSING_ARGUMENT"));
    return false;
  }
  // Check if the string is a valid int.
  else if (!isInt(strInput)) {
    sender->GetSerial()->println(F("ERROR INVALID_ARGUMENT"));
    return false;
  }
  return true;
}
