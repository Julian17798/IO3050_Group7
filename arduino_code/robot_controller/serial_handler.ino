// Serial Commands setup
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
MotorController *motorController;

// Serial commands and their input string triggers
SerialCommand cmdSetTimedSpeed_("!timedSpeed", cmdSetTimedSpeed);
SerialCommand cmdSetUntimedSpeed_("!untimedSpeed", cmdSetUntimedSpeed);
SerialCommand cmdStop_("!stop", cmdStop);
SerialCommand cmdFlipMotor_("!flip", cmdFlipMotor);

/*Sets up all of our custom serial commands.*/
void setupSerialCommands(MotorController *mCtrl) { 
  motorController = mCtrl;
  
  serial_commands_.SetDefaultHandler(cmdUnrecognized);
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

/*Serial command that turns on the motors at given speeds for a given amount of time.*/
void cmdSetTimedSpeed(SerialCommands* sender) {

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

  sender->GetSerial()->print(F("Set timed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->print(spd2);
  sender->GetSerial()->print(F(", duration: "));
  sender->GetSerial()->println(duration);

  // Tell the motorcontroller to turn on the motors for a given amount of time.
  motorController->setMotorsTimed(spd1, spd2, duration);  
}

void cmdSetUntimedSpeed(SerialCommands* sender) {

  // Get and validate arguments.
  char* spd1Str = sender->Next();
  if (!validateIntInput(sender, spd1Str)) { return; }

  char* spd2Str = sender->Next();
  if (!validateIntInput(sender, spd2Str)) { return; }

  // Turn string arguments into ints.
  int spd1 = atoi(spd1Str);
  int spd2 = atoi(spd2Str);

  sender->GetSerial()->print(F("Set untimed speed. Motor 1 spd: "));
  sender->GetSerial()->print(spd1);
  sender->GetSerial()->print(F(", Motor 2 spd: "));
  sender->GetSerial()->println(spd2);

  motorController->setMotorsUntimed(spd1, spd2);
}

void cmdStop(SerialCommands* sender) {

  // Stop motors and timer
  motorController->setMotorsUntimed(0, 0);

  sender->GetSerial()->println(F("Stopped motors."));
}

void cmdFlipMotor(SerialCommands* sender) {
  char* mStr = sender->Next();
  if (mStr == NULL) {
    sender->GetSerial()->println(F("ERROR MISSING_ARGUMENT"));
    return;
  }
  else if (!isInt(mStr)) {
    sender->GetSerial()->println(F("ERROR INVALID_ARGUMENT"));
    return;
  }

  int m = atoi(mStr);

  if (m != 1 && m != 2) {
    sender->GetSerial()->println(F("ERROR INVALID_NUMBER"));
    return;
  }

  motorController->flipMotor(m);

  sender->GetSerial()->print(F("Flipped motor "));
  sender->GetSerial()->print(m);
  sender->GetSerial()->println(F("."));
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

bool validateIntInput(SerialCommands* sender, char* strInput) {
  if (strInput == NULL) {
    sender->GetSerial()->println(F("ERROR MISSING_ARGUMENT"));
    return false;
  }
  else if (!isInt(strInput)) {
    sender->GetSerial()->println(F("ERROR INVALID_ARGUMENT"));
    return false;
  }
  return true;
}
