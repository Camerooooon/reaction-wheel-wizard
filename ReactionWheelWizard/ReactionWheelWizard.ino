#include <VescUart.h>

VescUart uart;

void setup() {
  Serial.begin(9600);

  while (!Serial) {;}
}

void loop() {
  Serial.println("Hello world!");

  if (uart.getVescValues()) {
    Serial.printf("RPM: %d\n", uart.data.rpm);
  } else {
    Serial.printf("Failed to get RPM data ):<\n");
  }

  if (uart.getFWversion()) {
    Serial.printf("FW VER: %d\n", uart.fw_version);
  }
  delay(1000);
}
