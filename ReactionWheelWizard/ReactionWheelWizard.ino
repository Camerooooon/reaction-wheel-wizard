#include <VescUart.h>
#include <SoftwareSerial.h>
#include <Adafruit_BNO055.h>

#define VESC_BAUD_RATE 115200
#define VESC_RX_PIN 1
#define VESC_TX_PIN 0

#define UPDATES_PER_SECOND 30 

VescUart uart;
SoftwareSerial vesc_ser = SoftwareSerial(VESC_RX_PIN, VESC_TX_PIN);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

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
    Serial.printf("Motor Temp: %d\n", uart.data.tempMotor);
  } else {
    Serial.printf("Failed to get RPM data ):<\n");
  }

  if (uart.getFWversion()) {
    Serial.printf("FW VER: %d\n", uart.fw_version);
  } else {
    Serial.printf("Could not get fw version\n");
  }

  sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  Serial.printf("pos: %d %d %d", orientationData.orientation.x, orientationData.orientation.y, orientationData.orientation.z);

  uart.sendKeepalive();
  uart.setCurrent(4);
  delay(1000 / UPDATES_PER_SECOND);
}
