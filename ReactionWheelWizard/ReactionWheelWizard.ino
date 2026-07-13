#include <VescUart.h>
#include <SoftwareSerial.h>
#include <Adafruit_BNO055.h>

#define VESC_BAUD_RATE 115200
#define VESC_RX_PIN 1
#define VESC_TX_PIN 0

#define ROLL_CAN_ID 0
#define PITCH_CAN_ID 1

#define UPDATES_PER_SECOND 30 

#define DESIRED_ROLL 0
#define DESIRED_PITCH 0

#define KP_ROLL 0.2
#define KD_ROLL 0

#define KP_PITCH 0.2
#define KD_PITCH 0

#define INVERT_PITCH 0
#define INVERT_ROLL 0

VescUart uart;
SoftwareSerial vesc_ser = SoftwareSerial(VESC_RX_PIN, VESC_TX_PIN);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
  Serial.begin(9600);
  vesc_ser.begin(VESC_BAUD_RATE);

  while (!Serial) {}
  while (!vesc_ser) {
    Serial.printf("Waiting for vesc serial...");
    delay(1000);
  }

  while (!bno.begin()) {
    Serial.printf("Waiting for BNO...");
    delay(1000);
  }
  uart.setSerialPort(&vesc_ser);
}

void loop() {
  Serial.println("Hello world!");

  sensors_event_t orientationData, angVelocityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);

  // For our setup 
  Serial.printf("pos: %f %f %f\n", orientationData.orientation.x, orientationData.orientation.y, orientationData.orientation.z);
  Serial.printf("vel: %f %f %f\n", angVelocityData.gyro.x, angVelocityData.gyro.y, angVelocityData.gyro.z);

  float roll = orientationData.orientation.z;
  float d_roll_dt = angVelocityData.gyro.z * RAD_TO_DEG;

  float pitch = orientationData.orientation.x;
  float d_pitch_dt = angVelocityData.gyro.x * RAD_TO_DEG;

  float roll_error = DESIRED_ROLL - roll;
  float pitch_error = DESIRED_PITCH - pitch;

  float roll_command = roll_error * KP_ROLL - d_roll_dt * KD_ROLL;
  float pitch_command = pitch_error * KP_PITCH - d_pitch_dt * KD_PITCH;

  uart.sendKeepalive();
  // uart.setCurrent(roll_command, ROLL_CAN_ID);
  uart.setCurrent(4, PITCH_CAN_ID);
  delay(1000 / UPDATES_PER_SECOND);
}
