#include <VescUart.h>

VescUart uart;

void setup() {
  Serial.begin(9600);

  while (!Serial) {;}
}

void loop() {
  Serial.println("Hello world!");

  if (uart.getVescValues()) {
    Serial.printf("RPM: %d", uart.data.rpm);
  } else {
    Serial.printf("Failed to get RPM data ):<");
  }
  delay(1000);
}
