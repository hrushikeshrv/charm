#include <Servo.h>

// The (maximum) number of miliseconds a servo should take to move from
// its start position to end position (independant of the angle it needs to move)
const int ROTATION_DURATION = 1200;   // 1.2 seconds

const int BASE_SERVO_PIN = 3;       // D3
const int BASE_ARM_SERVO_PIN = 5;   // D5
const int ARM_ARM_SERVO_PIN = 6;    // D6
const int GRIPPER_PITCH_SERVO_PIN = 9;  // D9, continuous rotation servo
const int GRIPPER_SERVO_PIN = 10;   // D10, continuous rotation servo

Servo baseServo;
Servo baseArmServo;
Servo armArmServo;
Servo gripperPitchServo;
Servo gripperServo;

int baseServoAngle = 0;
int baseArmServoAngle = 0;
int armArmServoAngle = 0;
int gripperPitchServoAngle = 0;
int gripperServoAngle = 0;

// Stores the servo angles for hovering above each square of the board
// in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const int hoverAngles[64][4] = {
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
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

// The coordinates for the position where captured pieces should be
// dropped in the format -{base angle, base-arm angle, arm-arm angle, gripper pitch}
const int captureAngles[4] = {0, 0, 0, 0};


void setup() {
  Serial.begin(9600);
  baseServo.attach(BASE_SERVO_PIN);
  baseArmServo.attach(BASE_ARM_SERVO_PIN);
  armArmServo.attach(ARM_ARM_SERVO_PIN);
  gripperPitchServo.attach(GRIPPER_PITCH_SERVO_PIN);
  gripperServo.attach(GRIPPER_SERVO_PIN);

  baseServoAngle = rotateServo(baseServo, baseServoAngle, 180);
  
  armArmServoAngle = rotateServo(armArmServo, armArmServoAngle, 90);
}

void loop() {
  if (Serial.available()) {
    String move = Serial.readString();
    move.trim();
    if (move == 'q') {
      shutDownArm();
      return;
    } 
    Serial.println("Move is " + move);
    String start = move.substring(0, 2);
    String end = move.substring(3, 5);
    bool capture = move[2] == 'x';
    // baseServoAngle = rotateServo(baseServo, baseServoAngle, angle);
    // baseArmServoAngle = rotateServo(baseArmServo, baseArmServoAngle, angle);
    // armArmServoAngle = rotateServo(armArmServo, armArmServoAngle, angle);
    // gripperPitchServoAngle = rotateServo(gripperPitchServo, gripperPitchServoAngle, angle);
  }
}

int rotateServo(Servo servo, int startAngle, int endAngle) {
  int rotationDelay = ROTATION_DURATION / (startAngle - endAngle);
  if (rotationDelay < 0) rotationDelay *= -1;

  if (startAngle > endAngle) {
    for (int i = startAngle; i >= endAngle; i--) {
      servo.write(i);
      delay(rotationDelay);
    }
  }
  else {
    for (int i = startAngle; i <= endAngle; i++) {
      servo.write(i);
      delay(rotationDelay);
    }
  }
  return endAngle;
}

/*
  Sets the arm's position by accepting end position angles 
  for all servos and rotating all servos at once instead of one
  servo at a time
*/
void setArmPosition(int baseAngle, int baseArmAngle, int armArmAngle, int gripperPitchAngle) {
  int diffs[] = {
    abs(baseAngle - baseServoAngle), 
    abs(baseArmAngle - baseArmServoAngle), 
    abs(armArmAngle - armArmServoAngle), 
    abs(gripperPitchAngle - gripperPitchServoAngle)
  };
  int max_angle = get_max(diffs, 4);
  int rotationDelay = ROTATION_DURATION / max_angle;

  int baseDirection = baseAngle > baseServoAngle ? 1 : -1;
  int baseArmDirection = baseArmAngle > baseArmServoAngle ? 1 : -1;
  int armArmDireciton = armArmAngle > armArmServoAngle ? 1 : -1;
  int gripperPitchDirection = gripperPitchAngle > gripperPitchServoAngle ? 1 : -1;

  bool baseMoving = baseAngle != baseServoAngle;
  bool arm1Moving = baseArmAngle != baseArmServoAngle;
  bool arm2Moving = armArmAngle != armArmServoAngle;
  bool gripperMoving = gripperPitchAngle != gripperPitchServoAngle;
  bool movingServos = baseMoving || arm1Moving || arm2Moving || gripperMoving;

  while (movingServos) {
    if (baseAngle == baseServoAngle) baseMoving = false;
    if (baseArmAngle == baseArmServoAngle) arm1Moving = false;
    if (armArmAngle == armArmServoAngle) arm2Moving = false;
    if (gripperPitchAngle == gripperPitchServoAngle) gripperMoving = false;
    movingServos = baseMoving || arm1Moving || arm2Moving || gripperMoving;
    
    baseServoAngle += baseDirection;
    baseServo.write(baseServoAngle);

    baseArmServoAngle += baseArmDirection;
    baseArmServo.write(baseArmServoAngle);

    armArmServoAngle += armArmDirection;
    armArmServo.write(armArmServoAngle);

    gripperPitchServoAngle += gripperPitchDirection;
    gripperPitchServo.write(gripperPitchServoAngle);

    delay(rotationDelay);
  }
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
void shutDownArm() {
  rotateServo(baseServo, baseServoAngle, 180);
  rotateServo(armArmServo, armArmServoAngle, 90);
}

/*
  Returns the index into the square array for a 
  particular square as a string
*/
int getSquareIndex(String square) {
  switch (square[0]) {
    case 'A':
      return (int) square[1] - 1;
    case 'B':
      return 7 + (int) square[1];
    case 'C':
      return 15 + (int) square[1];
    case 'D': 
      return 23 + (int) square[1];
    case 'E':
      return 31 + (int) square[1];
    case 'F':
      return 39 + (int) square[1];
    case 'G':
      return 47 + (int) square[1];
    case 'H':
      return 55 + (int) square[1];
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