#include <VescUart.h>
#include <SoftwareSerial.h>

VescUart uart;
SoftwareSerial vesc_ser = SoftwareSerial(0, 1);

void setup() {
  Serial.begin(9600);
  vesc_ser.begin(19200);

  while (!Serial) {;}
  while (!vesc_ser) {;}

  uart.setSerialPort(&vesc_ser);
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
