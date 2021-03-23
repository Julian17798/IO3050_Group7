// Serial Commands setup
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
extern ArmController arm;

#define invArg F("ERROR INVALID_ARGUMENT")
#define invNum F("ERROR INVALID_NUMBER")
#define misArg F("ERROR MISSING_ARGUMENT")

SerialCommand cmdServoTarget_("!servo", cmdServoTarget);

/*Sets up all of our custom serial commands.*/
void setupSerialCommands() {
  serial_commands_.SetDefaultHandler(cmdUnrecognized);
  serial_commands_.AddCommand(&cmdServoTarget_);
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

void cmdServoTarget(SerialCommands* sender) {
  
  // Get and validate arguments.
  char* servoStr = sender->Next();
  if (!validateIntInput(sender, servoStr)) return;

  char* targetStr = sender->Next();
  if (!validateIntInput(sender, targetStr)) return;

  int servo = atoi(servoStr);
  if (servo < 0 || servo > 2) {
    sender->GetSerial()->println(F("INVALID SERVO"));
  }  
  
  int target = atoi(targetStr);
  if (target < 0 || target > 255) {
    sender->GetSerial()->println(F("INVALID SIGNAL"));
  }

  Serial.print("Setting servo ");
  Serial.print(servo);
  Serial.print(" to ");
  Serial.println(target);
  arm.setTarget(servo, target);
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
