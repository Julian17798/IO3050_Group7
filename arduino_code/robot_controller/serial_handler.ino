#include <SerialCommands.h>

// Serial Commands setup
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

// Serial commands and their input string triggers
SerialCommand cmd_test_("!test", cmd_test);
SerialCommand cmd_argument_test_("!argtest", cmd_argument_test);

/*This function sets up all of our custom serial commands.*/
void setupSerialCommands() { 
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&cmd_test_);
  serial_commands_.AddCommand(&cmd_argument_test_);
}

/*This function reads the serial input and calls the relevant commands.*/
void handleSerial() {
  serial_commands_.ReadSerial();
}

/*This is the default handler, and gets called when no other command matches.*/
void cmd_unrecognized(SerialCommands* sender, const char* cmd) {  
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

/*Serial command test function.*/
void cmd_test(SerialCommands* sender) {
  sender->GetSerial()->println("Test command fired!");
}

/*Serial command argument test function. Takes one int argument.*/
void cmd_argument_test(SerialCommands* sender) {
      
  char* arg_str = sender->Next();
  
  if (arg_str == NULL) {
    sender->GetSerial()->println("ERROR NO_ARGUMENT");
    return;
  }
  else if (!is_int(arg_str)) {
    sender->GetSerial()->println("ERROR INVALID_ARGUMENT");
    return;
  }  

  int arg = atoi(arg_str);
  sender->GetSerial()->print("Argument test command fired! arg: ");
  sender->GetSerial()->println(arg);
}

/*Checks whether the input string is an integer number.*/
bool is_int(String str) {
  
  // Loop over the characters and check if all characters are digits.
  for (int i = 0; i < str.length(); i++) {
    if (!isdigit(str.charAt(i)) && str.charAt(0) != '-') { return false; }
  }
  return true;
}
