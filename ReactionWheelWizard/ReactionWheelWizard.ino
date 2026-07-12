#include <VescUart.h>
#include <SoftwareSerial.h>

#define VESC_BAUD_RATE 115200
#define VESC_RX_PIN 1
#define VESC_TX_PIN 0

#define UPDATES_PER_SECOND 30 

VescUart uart;
SoftwareSerial vesc_ser = SoftwareSerial(VESC_RX_PIN, VESC_TX_PIN);

void setup() {
  Serial.begin(9600);
  vesc_ser.begin(VESC_BAUD_RATE);

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
  } else {
    Serial.printf("Could not get fw version\n");
  }

  uart.sendKeepalive();
  uart.setCurrent(4);
  delay(1000 / UPDATES_PER_SECOND);
}
