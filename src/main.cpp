#include <Arduino.h>

void setup() {
  Serial.begin(115200);

  // Wait for serial to initialize
  while (!Serial) {
    delay(10);
  }

  // Print "Hello, World!" to the serial port
  Serial.println("Hello, World!");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello, World2!");
  delay(1000);
}
