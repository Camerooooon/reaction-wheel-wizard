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

#define INVERT_PITCH 1
#define INVERT_ROLL 0

#define MAX_ROLL_SHUTOFF_DEG 45
#define MAX_PITCH_SHUTOFF_DEG 45

#define PITCH_DEADZONE 3.0
#define ROLL_DEADZONE 3.0

VescUart uart;
SoftwareSerial vesc_ser = SoftwareSerial(VESC_RX_PIN, VESC_TX_PIN);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

int max_angle_fault = 0;

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

  sensors_event_t orientationData, angVelocityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);

  // For our setup 
  Serial.printf("pos: %f %f %f\n", orientationData.orientation.x, orientationData.orientation.y, orientationData.orientation.z);
  Serial.printf("vel: %f %f %f\n", angVelocityData.gyro.x, angVelocityData.gyro.y, angVelocityData.gyro.z);

  float roll = orientationData.orientation.z;
  float d_roll_dt = angVelocityData.gyro.z * RAD_TO_DEG;

  float pitch = orientationData.orientation.y;
  float d_pitch_dt = angVelocityData.gyro.y * RAD_TO_DEG;

  float roll_error = DESIRED_ROLL - roll;
  float pitch_error = DESIRED_PITCH - pitch;

  float roll_command = roll_error * KP_ROLL - d_roll_dt * KD_ROLL;
  float pitch_command = pitch_error * KP_PITCH - d_pitch_dt * KD_PITCH;

  if (INVERT_PITCH) {
    pitch_command = -pitch_command;
  }

  if (INVERT_ROLL) {
    roll_command = -roll_command;
  }

  if (abs(orientationData.orientation.z) > MAX_ROLL_SHUTOFF_DEG || abs(orientationData.orientation.y) > MAX_PITCH_SHUTOFF_DEG) {
    max_angle_fault = 1;
  }

  Serial.printf("Status: ");

  if (abs(orientationData.orientation.z) < ROLL_DEADZONE) {
    roll_command = 0;
    Serial.printf("roll_deadzone ");
  }

  if (abs(orientationData.orientation.z) < PITCH_DEADZONE) {
    pitch_command = 0;
    Serial.printf("pitch_deadzone ");
  }

  if (max_angle_fault) {
    Serial.printf("max_angle_fault ");
  }

  Serial.println();

  uart.sendKeepalive();

  if (!max_angle_fault) {
    uart.setCurrent(roll_command, ROLL_CAN_ID);
    uart.setCurrent(pitch_command, PITCH_CAN_ID);
  } else {
    uart.setCurrent(0, ROLL_CAN_ID);
    uart.setCurrent(0, PITCH_CAN_ID);
  }

  delay(1000 / UPDATES_PER_SECOND);
}
