#include <Servo.h>
#include "math.h"


float board[64][2];
void initializeBoardCoordinates(float armX, float armY, float squareWidth=5.0625) {
  /*
    Generates the (x, y) coordinates for all squares of the board
    given the width of one square of the board. The generated
    coordinates correspond to the center of the square.

    (armX, armY) is mapped to (0, 0), and the
    x axis is considered to be along a rank, and the y axis
    to be along a file. Coordinates to all squares are generated
    from the reference point (armX, armY);
  */
  for (int i = 0; i < 64; i++) {
    int rank = i / 8;
    int file = i % 8;
    board[i][0] = (file * square_width) - armX;
    board[i][1] = (rank * square_width) - armY;
  }
}

const int BASE_SERVO_PIN = 9;  // Base servo
const int ARM1_SERVO_PIN = 10; // Arm 1 servo
const int ARM2_SERVO_PIN = 11; // Arm 2 servo

const float L1 = 400; // Length of first arm
const float L2 = 400; // Length of second arm
const float ARM_X = 0.0;  // The distance of the arm base from the square A1, along the x axis
const float ARM_Y = 0.0;  // The distance of the arm base from the square A1, along the y axis

Servo baseServo;
Servo arm1Servo;
Servo arm2Servo;

float x, y, z; // Cartesian coordinates
float angles[3]; // Angles of robot arms 



void getAnglesFromCoords(float x, float y, float z, float (&armAngles) [3]) {
  armAngles[0] = atan(y/x);  // theta_bg
  
  float num = sq(L1) + sq(x) + sq(y) + sq(z) - sq(L2);
  float den = 2 * L1 * sqrt(sq(x) + sq(y) + sq(z));
  armAngles[1] = acos(num/den) + atan(z/(sq(x) + sq(y)));  // theta_ba

  num = sq(L1) + sq(L2) - sq(x) - sq(y) - sq(z);
  den = 2*L1*L2;

  armAngles[2] = acos(num/den);  // theta_aa
}

void rotateServo(Servo motor, int angle) {
  /*
    Rotates the given servo motor to the passed angle.
    Limits the angle between 0 and 180.
  */
  motor.write(angle % 181);
}

void navigateTo(float x, float y, float z) {
  /*
    Navigate the arm to the given coordinates.
  */
  getAnglesFromCoordinates(x, y, z, angles);
  int theta_bg = (int) angles[0] * 57296 / 1000;  // Convert to degrees
  int theta_ba = (int) angles[1] * 57296 / 1000;  // Convert to degrees
  int theta_aa = (int) angles[2] * 57296 / 1000;  // Convert to degrees

  rotateServo(baseServo, theta_bg);
  rotateServo(arm1Servo, theta_ba);
  rotateServo(arm2Servo, theta_aa);
}

void move(String start, String end, bool capture) {
  /*
    Pick up the piece at the start square and move it
    to the end square. If capture is true, pick
    up and move the end piece out of the way first.
  */
}

void setup() {
  Serial.begin(9600);
  initializeBoardCoordinates(ARM_X, ARM_Y);
  baseServo.attach(BASE_SERVO_PIN);
  arm1Servo.attach(ARM1_SERVO_PIN);
  arm2Servo.attach(ARM2_SERVO_PIN);
}

void loop() {
  if (Serial.available()) {
    String move = Serial.readString();
    Serial.println(move);
    String start = move.charAt(0) + move.charAt(1);
    String end = move.charAt(3) + move.charAt(4);
    bool capture = move.charAt(2) == 'x';
  }
}
