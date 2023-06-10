#include <Servo.h>
#include "math.h"


String squares[] = {
  "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8",
  "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8",
  "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8",
  "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8",
  "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8",
  "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
  "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8",
  "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8",
}
float boardCoordinates[64][2];
void initializeBoardCoordinates(float armX, float armY, float squareWidth=5.0625) {
  /*
    Generates the (x, y) coordinates for all squares of the boardCoordinates
    given the width of one square of the boardCoordinates. The generated
    coordinates correspond to the center of the square.

    (armX, armY) is mapped to (0, 0), and the
    x axis is considered to be along a rank, and the y axis
    to be along a file. Coordinates to all squares are generated
    from the reference point (armX, armY);
  */
  for (int i = 0; i < 64; i++) {
    int rank = i / 8;
    int file = i % 8;
    boardCoordinates[i][0] = (file * squareWidth) - armX;
    boardCoordinates[i][1] = (rank * squareWidth) - armY;
  }
}

const int BASE_SERVO_PIN = A0;          // Base servo
const int ARM1_SERVO_PIN = A1;          // Arm 1 servo
const int ARM2_SERVO_PIN = A2;          // Arm 2 servo
const int GRIPPER_SERVO_PIN = A3;       // Gripper servo

const float L1 = 400;                   // Length of first arm
const float L2 = 400;                   // Length of second arm
const float ARM_X = 0.0;                // The distance of the arm base from the square A1, along the x axis
const float ARM_Y = 0.0;                // The distance of the arm base from the square A1, along the y axis
const float HOVER_Z = 15.0;             // The z distance of gripper base such that the gripper hovers above a piece
const float PICK_UP_Z = 0;              // The z distance of the gripper base when we want to pick up a piece
const float CAPTURE_X = 30.0;           // The x coordinate where the arm should put the captured pieces
const float CAPTURE_Y = 30.0;           // The y coordinate where the arm should put the captured pieces
const float MOTOR_WRITE_DELAY = 500;    // The amount of time to wait for the servo motors to finish rotation
const float GRIPPER_TOGGLE_DELAY = 100; // The amount of time to wait for the gripper to open and close

Servo baseServo;
Servo arm1Servo;
Servo arm2Servo;
Servo gripperServo;

float x, y, z;                          // Cartesian coordinates
float angles[3];                        // Angles of robot arms 


/*
  Calculates the arm angles given the (x, y, z) coordinates
  the arm needs to go to. Stores the calculated angles in the 
  angles array
*/
void getAnglesFromCoords(float x, float y, float z, float (&armAngles) [3]) {
  armAngles[0] = atan(y/x);  // theta_bg
  
  float num = sq(L1) + sq(x) + sq(y) + sq(z) - sq(L2);
  float den = 2 * L1 * sqrt(sq(x) + sq(y) + sq(z));
  armAngles[1] = acos(num/den) + atan(z/(sq(x) + sq(y)));  // theta_ba

  num = sq(L1) + sq(L2) - sq(x) - sq(y) - sq(z);
  den = 2*L1*L2;

  armAngles[2] = acos(num/den);  // theta_aa
}


/*
  Rotates the given servo motor to the passed angle.
  Limits the angle between 0 and 180.
*/
void rotateServo(Servo motor, int angle) {
  motor.write(angle % 181);
}


/*
  Navigate the arm to the given (x, y, z) coordinates.
*/
void navigateTo(float x, float y, float z) {
  getAnglesFromCoords(x, y, z, angles);
  int theta_bg = (int) angles[0] * 57296 / 1000;  // Convert to degrees
  int theta_ba = (int) angles[1] * 57296 / 1000;  // Convert to degrees
  int theta_aa = (int) angles[2] * 57296 / 1000;  // Convert to degrees

  rotateServo(baseServo, theta_bg);
  rotateServo(arm1Servo, theta_ba);
  rotateServo(arm2Servo, theta_aa);
  delay(MOTOR_WRITE_DELAY);
}


/*
  Sends move to the connected computer. Start square name is
  sent first, then end square name.
*/
void sendMove(String start, String end) {
  Serial.print(start);
  Serial.print(end);
}

void openGripper() {}

void closeGripper() {}


/*
  Returns the index into the boardCoordinates array for a 
  given square. Returns -1 if invalid square is passed.
*/
int getSquareCoordinateIndex(String square) {
  for (int i = 0; i < 64; i++) {
    if (squares[i] == square) return i;
  }
  return -1;
}


/*
  Pick up the piece at the start square and move it
  to the end square. If capture is true, pick
  up and move the end piece out of the way first.
*/
void makeMove(String start, String end, bool capture) {
  int endCoordIndex = getSquareCoordinateIndex(end);
  int startCoordIndex = getSquareCoordinateIndex(start);
  float endX = boardCoordinates[endCoordIndex][0];
  float endY = boardCoordinates[endCoordIndex][1];
  float startX = boardCoordinates[startCoordIndex][0];
  float startY = boardCoordinates[startCoordIndex][1];
  if (capture) {
    navigateTo(endX, endY, HOVER_Z);
    openGripper();
    navigateTo(endX, endY, PICK_UP_Z);
    closeGripper();
    navigateTo(endX, endY, HOVER_Z);
    navigateTo(CAPTURE_X, CAPTURE_Y, HOVER_Z);
    openGripper();
  }
  navigateTo(startX, startY, HOVER_Z);
  openGripper();
  navigateTo(startX, startY, PICK_UP_Z);
  closeGripper();
  navigateTo(startX, startY, HOVER_Z);
  navigateTo(endX, endY, HOVER_Z);
  navigateTo(endX, endY, PICK_UP_Z);
  openGripper();
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
    String start = move.substring(0, 2);
    String end = move.substring(3, 5);
    bool capture = move[2] == 'x';
    makeMove(start, end, capture);
  }
  else {
    // Poll board to check if move was made
    // If move was made, send it to the computer via sendMove()
  }
}
