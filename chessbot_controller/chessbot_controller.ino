#include <Servo.h>
#include "math.h"

const int servo1pin = 9;
const float l1 = 400; // Length of first arm
const float l2 = 400; // Length of second arm
const float z_b = 150;  // Height of arm base from ground
const float z_g = 150;  // Height of gripper base from ground

Servo servo1;
int servo1pos = 0;

float x, y, z; // Cartesian coordinates
float theta_bg, theta_ba, theta_aa; // Angles of robot arms 

float *getAnglesFromCoords(float x, float y, float z, float angles[]) {
  angles[0] = atan(y/x);
  
  float num = sq(l1) + sq(x) + sq(y) + sq(z_b - z_g) - sq(l2);
  float den = 2 * l1 * sqrt(sq(x) + sq(y) + sq(z_b - z_g));
  angles[1] = acos(num/den);

  num = sq(l1) + sq(l2) - sq(x) - sq(y) - sq(z_b - z_g);
  den = 2*l1*l2;

  angles[2] = acos(num/den);
}

int rotateClockwise(Servo motor, int prevAngle, int angle) {
  Serial.print("Rotating by angle ");
  Serial.println(angle);
  motor.write((prevAngle + angle) % 180);
  return (prevAngle + angle) % 180;
}

void setup() {
  Serial.begin(9600);
  servo1.attach(servo1pin);
}

void loop() {
  servo1pos = rotateClockwise(servo1, servo1pos, 60);
  Serial.println(servo1pos);
  delay(1000);
}
