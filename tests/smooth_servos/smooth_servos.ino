#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const float MIN_PULSE_WIDTH = 650;
const float MAX_PULSE_WIDTH = 2350;
const float FREQUENCY = 50;

const float MOVE_SETTLE_DELAY = 300;
// The factor for smoothening the arm's movement. Should be greater than 0 and less than 1.
// Higher number means less smooth.
const float INTERPOLATION_FACTOR = 0.02;

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

// Keep track of the servo angles for the previous square
float baseServoAngleCached = 108;
float baseArmServoAngleCached = 132;
float armArmServoAngleCached = 165;
float gripperPitchServoAngleCached = 165;

// The current angles of the servos
float baseServoAngle = 108;
float baseArmServoAngle = 132;
float armArmServoAngle = 165;
float gripperPitchServoAngle = 165;

// The angles of the servos at the previous time step
float baseServoAnglePrev = 108;
float baseArmServoAnglePrev = 132;
float armArmServoAnglePrev = 165;
float gripperPitchServoAnglePrev = 165;

// The angles of the servos at the next time step
float baseServoAngleSmoothed = 108;
float baseArmServoAngleSmoothed = 132;
float armArmServoAngleSmoothed = 165;
float gripperPitchServoAngleSmoothed = 165;

void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);

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
}

void loop() {
  if (Serial.available()) {
    String move = Serial.readString();
    move.trim();

    baseServoAngleCached = baseServoAngle;
    baseArmServoAngleCached = baseArmServoAngle;
    armArmServoAngleCached = armArmServoAngle;
    gripperPitchServoAngleCached = gripperPitchServoAngle;

    if (move == "q") {
      Serial.println("Resetting arm position");
      
      baseServoAngle = 108;
      baseArmServoAngle = 132;
      armArmServoAngle = 165;
      gripperPitchServoAngle = 165;
    }
    else {
      Serial.print("Moving to "); Serial.println(move);
      int idx = getSquareIndex(move);
      
      baseServoAngle = hoverAngles[idx][0];
      baseArmServoAngle = hoverAngles[idx][1];
      armArmServoAngle = hoverAngles[idx][2];
      gripperPitchServoAngle = hoverAngles[idx][3];
    }
  }

  baseServoAngleSmoothed = (baseServoAngle * INTERPOLATION_FACTOR) + (baseServoAnglePrev * (1-INTERPOLATION_FACTOR));
  baseArmServoAngleSmoothed = (baseArmServoAngle * INTERPOLATION_FACTOR) + (baseArmServoAnglePrev * (1-INTERPOLATION_FACTOR));
  armArmServoAngleSmoothed = (armArmServoAngle * INTERPOLATION_FACTOR) + (armArmServoAnglePrev * (1-INTERPOLATION_FACTOR));
  gripperPitchServoAngleSmoothed = (gripperPitchServoAngle * INTERPOLATION_FACTOR) + (gripperPitchServoAnglePrev * (1-INTERPOLATION_FACTOR));

  baseServoAnglePrev = baseServoAngleSmoothed;
  baseArmServoAnglePrev = baseArmServoAngleSmoothed;
  armArmServoAnglePrev = armArmServoAngleSmoothed;
  gripperPitchServoAnglePrev = gripperPitchServoAngleSmoothed;

  float pulseWidth = map(baseServoAngleSmoothed, 0.0, 180.0, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
  pwm.setPWM(BASE_SERVO_PIN, 0, pulseWidth);

  pulseWidth = map(baseArmServoAngleSmoothed, 0.0, 180.0, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
  pwm.setPWM(BASE_ARM_SERVO_PIN, 0, pulseWidth);

  pulseWidth = map(armArmServoAngleSmoothed, 0.0, 180.0, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
  pwm.setPWM(ARM_ARM_SERVO_PIN, 0, pulseWidth);

  pulseWidth = map(gripperPitchServoAngleSmoothed, 0.0, 180.0, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
  pwm.setPWM(GRIPPER_PITCH_SERVO_PIN, 0, pulseWidth);

  Serial.print(baseServoAngleSmoothed);
  Serial.print(",");
  Serial.print(baseArmServoAngleSmoothed);
  Serial.print(",");
  Serial.print(armArmServoAngleSmoothed);
  Serial.print(",");
  Serial.println(gripperPitchServoAngleSmoothed);

  delay(5);  // loop 200 times a second
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