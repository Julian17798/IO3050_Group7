// Serial Commands setup
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
extern ArmController arm;
extern Sequencer seq0, seq1, seq2;

#define invArg F("ERROR INVALID_ARGUMENT")
#define invNum F("ERROR INVALID_NUMBER")
#define misArg F("ERROR MISSING_ARGUMENT")

SerialCommand cmdServoTarget_("!servo", cmdServoTarget);
SerialCommand cmdServoTargets_("!servos", cmdServoTargets);
SerialCommand cmdStartSequence_("!seq", cmdStartSequence);


/*Sets up all of our custom serial commands.*/
void setupSerialCommands() {
  serial_commands_.SetDefaultHandler(cmdUnrecognized);
  serial_commands_.AddCommand(&cmdServoTarget_);
  serial_commands_.AddCommand(&cmdServoTargets_);
  serial_commands_.AddCommand(&cmdStartSequence_);
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

/*Starts the sequences.*/
void cmdStartSequence(SerialCommands* sender) {
  seq0.startSequence();
  seq1.startSequence();
  seq2.startSequence();
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
    return;
  }

  int target = atoi(targetStr);
  if (target < 0 || target > 255) {
    sender->GetSerial()->println(F("INVALID SIGNAL"));
    return;
  }

  Serial.print("Setting servo ");
  Serial.print(servo);
  Serial.print(" to ");
  Serial.println(target);
  arm.setTarget(servo, target);
}

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
