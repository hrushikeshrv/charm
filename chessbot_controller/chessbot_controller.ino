#include <Servo.h>
#include "math.h"

const int BASE_SERVO_PIN = 9;  // Base servo
const int ARM1_SERVO_PIN = 10; // Arm 1 servo
const int ARM2_SERVO_PIN = 11; // Arm 2 servo

const float l1 = 400; // Length of first arm
const float l2 = 400; // Length of second arm


Servo base_servo;
int base_servo_pos = 0;
Servo arm1_servo;
int arm1_servo_pos = 0;
Servo arm2_servo;
int arm2_servo_pos = 0;

float x, y, z; // Cartesian coordinates
float theta_bg, theta_ba, theta_aa; // Angles of robot arms 

float *getAnglesFromCoords(float x, float y, float z, float angles[]) {
  angles[0] = atan(y/x);  // theta_bg
  
  float num = sq(l1) + sq(x) + sq(y) + sq(z) - sq(l2);
  float den = 2 * l1 * sqrt(sq(x) + sq(y) + sq(z));
  angles[1] = acos(num/den) + atan(z/(sq(x) + sq(y)));  // theta_ba

  num = sq(l1) + sq(l2) - sq(x) - sq(y) - sq(z);
  den = 2*l1*l2;

  angles[2] = acos(num/den);  // theta_aa
}

int rotateServo(Servo motor, int prevAngle, int angle) {
  /*
  Rotates the given servo by angle in the clockwise direction.
  Also accepts the current position of the servo and returns
  the new position of the servo.

  Servo angle is limited between 0 and 180.
  */
  motor.write((prevAngle + angle) % 181);
  return (prevAngle + angle) % 181;
}

void setup() {
  Serial.begin(9600);
  base_servo.attach(BASE_SERVO_PIN);
  arm1_servo.attach(ARM1_SERVO_PIN);
  arm2_servo.attach(ARM2_SERVO_PIN);
}

void loop() {
  while (Serial.available() == 0) {}
  int angle = Serial.readString().toInt();
  base_servo_pos = rotateServo(base_servo, base_servo_pos, angle);
  Serial.println(base_servo_pos);
  delay(1000);
}
