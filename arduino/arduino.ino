#ifndef DEVICE_ID
#define DEVICE_ID 0 // fallback ID
#endif

char buffer[64];

const int ledPin = LED_BUILTIN;
bool ledState = false;
String incomingCommand = "";

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  Serial.begin(9600);
  while (!Serial); // Wait for serial port to connect (for some boards)

  sprintf(buffer, "ID:%d", DEVICE_ID);
  Serial.println(buffer);
}

void loop() {
  // Check if data is available
  while (Serial.available() > 0) {
    char received = Serial.read();

    // If newline, process command
    if (received == '\n') {
      incomingCommand.trim(); // Remove any extra whitespace/newlines

      // if (incomingCommand.equalsIgnoreCase("TOGGLE")) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      Serial.print("LED is now ");
      Serial.println(ledState ? "ON" : "OFF");
      // } else {
      //   Serial.println("Unknown command.");
      // }

      incomingCommand = ""; // Reset buffer
    } else {
      incomingCommand += received;
    }
    Serial.println("test\n");
  }
}

