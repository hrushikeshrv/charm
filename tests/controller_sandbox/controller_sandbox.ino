#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const int MIN_PULSE_WIDTH = 650;
const int MAX_PULSE_WIDTH = 2350;
const float FREQUENCY = 50;

const float MOVE_SETTLE_DELAY = 300;
const float SERVO_ROTATION_DELAY = 1500;  // The time in ms that the arm should take to move from one position to another

const int BASE_SERVO_PIN = 0;
const int BASE_ARM_SERVO_PIN = 1;
const int ARM_ARM_SERVO_PIN = 2;
const int GRIPPER_PITCH_SERVO_PIN = 3;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Stores the servo angles for hovering above each square of the board
// in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const int hoverAngles[64][4] = {
  {84, 36, 8, 94}, {80, 47, 28, 100}, {76, 56, 39, 92}, {71, 64, 58, 98}, {68, 74, 76, 105}, {64, 82, 89, 115}, {61, 89, 100, 107}, {50, 99, 116, 117},         // A1 to A8
  {87, 36, 8, 88},{87, 48, 32, 100},{84, 56, 46, 92},{84, 68, 66, 104},{77, 79, 89, 117},{75, 87, 102, 120},{70, 97, 111, 114},{60, 107, 123, 124},             // B1 to B8
  {97, 42, 15, 89},{95, 51, 35, 100},{94, 57, 45, 92},{90, 71, 74, 108},{88, 79, 89, 114},{86, 90, 104, 120},{81, 100, 117, 117},{75, 113, 130, 124},           // C1 to C8
  {100, 42, 14, 88},{103, 53, 36, 94},{103, 57, 45, 92},{104, 71, 74, 108},{100, 81, 87, 114},{100, 89, 104, 116},{94, 103, 116, 117},{92, 114, 132, 124},      // D1 to D8
  {110, 40, 10, 89},{109, 49, 31, 94},{109, 57, 44, 91},{107, 71, 73, 107},{108, 82, 87, 113},{111, 91, 104, 116},{112, 101, 117, 117},{113, 118, 136, 124},    // E1 to E8
  {103, 35, 35, 110},{115, 54, 31, 95},{118, 57, 44, 91},{119, 70, 73, 107},{117, 79, 84, 114},{125, 89, 102, 115},{126, 101, 117, 118},{130, 114, 130, 124},   // F1 to F8
  {110, 35, 35, 110},{124, 39, 15, 77},{127, 53, 38, 91},{125, 67, 65, 108},{128, 77, 81, 114},{137, 86, 97, 115},{139, 98, 112, 114},{143, 105, 122, 118},     // G1 to G8
  {127, 38, 14, 109},{127, 41, 8, 84},{134, 54, 32, 83},{134, 64, 58, 102},{140, 73, 73, 114},{142, 81, 87, 107},{147, 92, 104, 117},{161, 98, 115, 118},       // H1 to H8
};

// Stores the servo angles for grabbing the piece on each square of the
// board in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const int grabbingAngles[64][4] = {
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
};

// The angles for the position where captured pieces should be
// dropped in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const int captureAngles[4] = {0, 0, 0, 0};

// The angles the arm resets to after each move
const int resetAngles[4] = {108, 132, 165, 165};

float baseServoAngle = 108;
float baseArmServoAngle = 132;
float armArmServoAngle = 165;
float gripperPitchServoAngle = 165;

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  
  resetArmPosition();
  setArmPosition(0, 0, 0, 0);
  resetArmPosition();
}

void loop() {
  if (Serial.available()) {
    String move = Serial.readString();
    move.trim();
    
    if (move == "q") {
      resetArmPosition();
    } 
    else {
      Serial.print("Moving to "); Serial.println(move);
      int idx = getSquareIndex(move);
      setArmPosition(hoverAngles[idx][0], hoverAngles[idx][1], hoverAngles[idx][2], hoverAngles[idx][3]);
    }
  }
}

/*
  Moves a piece from the passed start square to the end
  square. If a piece is present on the end square, captures
  it first.
*/
void makeMove(String start, String end, bool capture) {
  int (*startHoverPos)[4] = &hoverAngles[getSquareIndex(start)];
  int (*startGrabPos)[4] = &grabbingAngles[getSquareIndex(start)];
  int (*endHoverPos)[4] = &hoverAngles[getSquareIndex(end)];
  int (*endGrabPos)[4] = &grabbingAngles[getSquareIndex(end)];

  if (capture) {
    setArmPosition(endHoverPos[0], endHoverPos[1], endHoverPos[2], endHoverPos[3]);
    // delay(MOVE_SETTLE_DELAY);
    setArmPosition(endGrabPos[0], endGrabPos[1], endGrabPos[2], endGrabPos[3]);
    // delay(MOVE_SETTLE_DELAY);
    closeGripper();
    delay(MOVE_SETTLE_DELAY);
    setArmPosition(captureAngles[0], captureAngles[1], captureAngles[2], captureAngles[3]);
    delay(MOVE_SETTLE_DELAY);
    openGripper();
  }

  setArmPosition(startHoverPos[0], startHoverPos[1], startHoverPos[2], startHoverPos[3]);
  // delay(MOVE_SETTLE_DELAY);
  setArmPosition(startGrabPos[0], startGrabPos[1], startGrabPos[2], startGrabPos[3]);
  // delay(MOVE_SETTLE_DELAY);
  closeGripper();
  delay(MOVE_SETTLE_DELAY);

  setArmPosition(endHoverPos[0], endHoverPos[1], endHoverPos[2], endHoverPos[3]);
  // delay(MOVE_SETTLE_DELAY);
  setArmPosition(endGrabPos[0], endGrabPos[1], endGrabPos[2], endGrabPos[3]);
  delay(MOVE_SETTLE_DELAY);
  openGripper();
  delay(MOVE_SETTLE_DELAY);
}

/*
  Sets the arm's position by accepting end position angles 
  for all servos and rotating all servos at once instead of one
  servo at a time
*/
void setArmPosition(int baseAngle, int baseArmAngle, int armArmAngle, int gripperPitchAngle) {
  // The number of times we will have to update the servo position given, how long a 
  // move should take, and the frequency of updates
  float numUpdates = (SERVO_ROTATION_DELAY / 1000.0) * FREQUENCY * 4;
  float baseDelta = ((float) (baseAngle - baseServoAngle)) / numUpdates;
  float baseArmDelta = ((float) (baseArmAngle - baseArmServoAngle)) / numUpdates;
  float armArmDelta = ((float) (armArmAngle - armArmServoAngle)) / numUpdates;
  float gripperPitchDelta = ((float) (gripperPitchAngle - gripperPitchServoAngle)) / numUpdates;
  Serial.print("Before moving. Chose number of updates "); Serial.print(numUpdates); Serial.print(" and deltas: "); Serial.print(baseDelta); Serial.print(", "); Serial.print(baseArmDelta); Serial.print(", "); Serial.print(armArmDelta); Serial.print(", "); Serial.println(gripperPitchDelta);
  Serial.print("Current servo angles: ");Serial.print(baseServoAngle); Serial.print(", "); Serial.print(baseArmServoAngle); Serial.print(", "); Serial.print(armArmServoAngle); Serial.print(", "); Serial.println(gripperPitchServoAngle);
  Serial.print("Destination angles: "); Serial.print(baseAngle); Serial.print(", "); Serial.print(baseArmAngle); Serial.print(", "); Serial.print(armArmAngle); Serial.print(", "); Serial.println(gripperPitchAngle);

  for (int i = 0; i < numUpdates; i++) {
    baseServoAngle += baseDelta;
    baseArmServoAngle += baseArmDelta;
    armArmServoAngle += armArmDelta;
    gripperPitchServoAngle += gripperPitchDelta;

    int pulseWidth;
    pulseWidth = map(baseServoAngle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
    pwm.setPWM(BASE_SERVO_PIN, 0, pulseWidth);

    pulseWidth = map(baseArmServoAngle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
    pwm.setPWM(BASE_ARM_SERVO_PIN, 0, pulseWidth);

    pulseWidth = map(armArmServoAngle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
    pwm.setPWM(ARM_ARM_SERVO_PIN, 0, pulseWidth);

    pulseWidth = map(gripperPitchServoAngle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
    pwm.setPWM(GRIPPER_PITCH_SERVO_PIN, 0, pulseWidth);

    delayMicroseconds((SERVO_ROTATION_DELAY * 1000) / (numUpdates * 8));
  }
  Serial.println("After moving");
  Serial.print("Current servo angles: ");Serial.print(baseServoAngle); Serial.print(", "); Serial.print(baseArmServoAngle); Serial.print(", "); Serial.print(armArmServoAngle); Serial.print(", "); Serial.println(gripperPitchServoAngle);
  Serial.println("");
}

/*
  Closes the gripper
*/
void closeGripper() {}

/*
  Opens the gripper
*/
void openGripper() {}


/* 
  Move all servos to initial positions before the arm shuts down.
  Make sure this function is called every time before the arm shuts 
  down, or the servos behave unreliably when the arm wakes up.
*/
void resetArmPosition() {
  Serial.println("Resetting arm position");
  setArmPosition(resetAngles[0], resetAngles[1], resetAngles[2], resetAngles[3]);
}

/*
  Returns the index into the square array for a 
  particular square as a string
*/
int getSquareIndex(String square) {
  switch (square[0]) {
    case 'A':
    case 'a':
      return (int) square[1] - 49;
    case 'B':
    case 'b':
      return 8 + (int) square[1] - 49;
    case 'C':
    case 'c':
      return 16 + (int) square[1] - 49;
    case 'D': 
    case 'd':
      return 24 + (int) square[1] - 49;
    case 'E':
    case 'e':
      return 32 + (int) square[1] - 49;
    case 'F':
    case 'f':
      return 40 + (int) square[1] - 49;
    case 'G':
    case 'g':
      return 48 + (int) square[1] - 49;
    case 'H':
    case 'h':
      return 56 + (int) square[1] - 49;
  }
}

/*
  Utility function to return the largest element in an array
*/
int get_max(int *l, int size) {
  int largest = l[0];
  for (int i = 0; i < size; i++) {
    if (l[i] > largest) {
      largest = l[i];
    }
  }
  return largest;
}