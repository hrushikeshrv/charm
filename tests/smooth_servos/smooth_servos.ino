#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const float MIN_PULSE_WIDTH = 650;
const float MAX_PULSE_WIDTH = 2350;
const float FREQUENCY = 50;

const float MOVE_SETTLE_DELAY = 300;
// The smoothening factor for the arm's movement. Should be greater than 0 and less than 1.
// Higher number means less smooth.
float INTERPOLATION_FACTOR = 0.04;

const int BASE_SERVO_PIN = 0;
const int BASE_ARM_SERVO_PIN = 1;
const int ARM_ARM_SERVO_PIN = 2;
const int GRIPPER_PITCH_SERVO_PIN = 3;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Stores the servo angles for hovering above each square of the board
// in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const float hoverAngles[64][4] = {
  {84.0, 36.0, 8.0, 94.0}, {80.0, 47.0, 28.0, 100.0}, {76.0, 56.0, 39.0, 92.0}, {71.0, 64.0, 58.0, 98.0}, {68.0, 74.0, 76.0, 105.0}, {64.0, 82.0, 89.0, 115.0}, {61.0, 89.0, 100.0, 107.0}, {50.0, 99.0, 116.0, 117.0},         // A1 to A8
  {87.0, 36.0, 8.0, 88.0},{87.0, 48.0, 32.0, 100.0},{84.0, 56.0, 46.0, 92.0},{84.0, 68.0, 66.0, 104.0},{77.0, 79.0, 89.0, 117.0},{75.0, 87.0, 102.0, 120.0},{70.0, 97.0, 111.0, 114.0},{60.0, 107.0, 123.0, 124.0},             // B1 to B8
  {97.0, 42.0, 15.0, 89.0},{95.0, 51.0, 35.0, 100.0},{94.0, 57.0, 45.0, 92.0},{90.0, 71.0, 74.0, 108.0},{88.0, 79.0, 89.0, 114.0},{86.0, 90.0, 104.0, 120.0},{81.0, 100.0, 117.0, 117.0},{75.0, 113.0, 130.0, 124.0},           // C1 to C8
  {100.0, 42.0, 14.0, 88.0},{103.0, 53.0, 36.0, 94.0},{103.0, 57.0, 45.0, 92.0},{104.0, 71.0, 74.0, 108.0},{100.0, 81.0, 87.0, 114.0},{100.0, 89.0, 104.0, 116.0},{94.0, 103.0, 116.0, 117.0},{92.0, 114.0, 132.0, 124.0},      // D1 to D8
  {110, 40, 10, 89},{109, 49, 31, 94},{109, 57, 44, 91},{107, 71, 73, 107},{108, 82, 87, 113},{111, 91, 104, 116},{112, 101, 117, 117},{113, 118, 136, 124},    // E1 to E8
  {103, 35, 35, 110},{115, 54, 31, 95},{118, 57, 44, 91},{119, 70, 73, 107},{117, 79, 84, 114},{125, 89, 102, 115},{126, 101, 117, 118},{130, 114, 130, 124},   // F1 to F8
  {110, 35, 35, 110},{124, 39, 15, 77},{127, 53, 38, 91},{125, 67, 65, 108},{128, 77, 81, 114},{137, 86, 97, 115},{139, 98, 112, 114},{143, 105, 122, 118},     // G1 to G8
  {127, 38, 14, 109},{127, 41, 8, 84},{134, 54, 32, 83},{134, 64, 58, 102},{140, 73, 73, 114},{142, 81, 87, 107},{147, 92, 104, 117},{161, 98, 115, 118},       // H1 to H8
};

// Stores the servo angles for grabbing the piece on each square of the
// board in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const float grabbingAngles[64][4] = {
  {83.00, 32.00, 8.00, 120.00},{78.00, 37.00, 17.00, 110.00},{79.00, 44.00, 30.00, 107.00},{71.00, 54.00, 58.00, 125.00},{69.00, 61.00, 71.00, 126.00},{64.00, 68.00, 89.00, 141.00},{58.00, 76.00, 114.00, 157.00},{49.00, 79.00, 120.00, 161.00},   // A1 to A8
  {89.00, 31.00, 8.00, 116.00},{88.00, 39.00, 21.00, 110.00},{83.00, 49.00, 43.00, 119.00},{83.00, 58.00, 66.00, 131.00},{78.00, 67.00, 88.00, 144.00},{74.00, 70.00, 96.00, 141.00},{68.00, 79.00, 117.00, 157.00},{60.00, 82.00, 126.00, 160.00},   // B1 to B8 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
  {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 
};

// The angles for the position where captured pieces should be
// dropped in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const float captureAngles[4] = {0, 0, 0, 0};

// The angles the arm resets to after each move
const float resetAngles[4] = {108, 132, 165, 178};

// The angles of the servos for the previous destination
float baseServoAngleCached = resetAngles[0];
float baseArmServoAngleCached = resetAngles[1];
float armArmServoAngleCached = resetAngles[2];
float gripperPitchServoAngleCached = resetAngles[3];

// The current destination angles of the servos
float baseServoAngle = resetAngles[0];
float baseArmServoAngle = resetAngles[1];
float armArmServoAngle = resetAngles[2];
float gripperPitchServoAngle = resetAngles[3];

// The angles of the servos at the previous time step
float baseServoAnglePrev = resetAngles[0];
float baseArmServoAnglePrev = resetAngles[1];
float armArmServoAnglePrev = resetAngles[2];
float gripperPitchServoAnglePrev = resetAngles[3];

// The angles of the servos at the next time step
float baseServoAngleSmoothed = resetAngles[0];
float baseArmServoAngleSmoothed = resetAngles[1];
float armArmServoAngleSmoothed = resetAngles[2];
float gripperPitchServoAngleSmoothed = resetAngles[3];

// True if the gripper is currently closed, i.e. the arm is holding a piece
bool gripperClosed = false;
// The indices of the squares to visit sequentially
int destinations[4] = {-1, -1, -1, -1};
// The index of the current destination
int currentDestination = 0;
/**
 * The current state -
 * 0 means hover over the target
 * 1 means lower the arm at the target
 * 2 means toggle the gripper state (open -> close or close -> open)
 * 3 means hover over the target again
*/
int currentState = 0;


void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);

  float pulseWidth;
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
      bool capture = move.substring(2, 3) == "x";
      String start = move.substring(0, 2);
      String end = move.substring(3);
      int startIdx = getSquareIndex(start);
      int endIdx = getSquareIndex(end);

      if (capture) {
        destinations[0] = endIdx;
        destinations[1] = -1;
        destinations[2] = startIdx;
        destinations[3] = endIdx;
      }
      else {
        destinations[0] = startIdx;
        destinations[1] = endIdx;
        destinations[2] = -1;
        destinations[3] = -1;
      }

      currentDestination = -1;
      currentState = -1;
    }
  }

  if (transitionComplete()) {
    if (currentState >= 3) {
      currentState = 0;
      currentDestination += 1;
    }
    else {
      currentState += 1;
    }
  }

  updateSmoothedAngles();

  baseServoAnglePrev = baseServoAngleSmoothed;
  baseArmServoAnglePrev = baseArmServoAngleSmoothed;
  armArmServoAnglePrev = armArmServoAngleSmoothed;
  gripperPitchServoAnglePrev = gripperPitchServoAngleSmoothed;

  // Calculate PWM pulse width from smoothened angles
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

/**
 * Returns the index into the square array for a 
 * particular square as a string
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

/**
 * Updates all servo angles for the next time step 
 * based on the start angles, current angles, destination angles,
 * and the interpolation factor.
*/
void updateSmoothedAngles() {
  // Calculate the smoothened baseServoAngle
  if (baseServoAngle > baseServoAngleCached) {
    if (baseServoAnglePrev < (baseServoAngle + baseServoAngleCached) / 2) {
      baseServoAngleSmoothed = baseServoAnglePrev + (baseServoAnglePrev - baseServoAngleCached) * INTERPOLATION_FACTOR;

      if (baseServoAngleSmoothed == baseServoAnglePrev) {
        baseServoAngleSmoothed = baseServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      baseServoAngleSmoothed = baseServoAnglePrev + (baseServoAngle - baseServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else {
    if (baseServoAnglePrev > (baseServoAngle + baseServoAngleCached) / 2) {
      baseServoAngleSmoothed = baseServoAnglePrev - (baseServoAngleCached - baseServoAnglePrev) * INTERPOLATION_FACTOR;

      if (baseServoAngleSmoothed == baseServoAnglePrev) {
        baseServoAngleSmoothed = baseServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      baseServoAngleSmoothed = baseServoAnglePrev - (baseServoAnglePrev - baseServoAngle) * INTERPOLATION_FACTOR;
    }
  }

  // Calculate the smoothened baseArmServoAngle
  if (baseArmServoAngle > baseArmServoAngleCached) {
    if (baseArmServoAnglePrev < (baseArmServoAngle + baseArmServoAngleCached) / 2) {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev + (baseArmServoAnglePrev - baseArmServoAngleCached) * INTERPOLATION_FACTOR;

      if (baseArmServoAngleSmoothed == baseArmServoAnglePrev) {
        baseArmServoAngleSmoothed = baseArmServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev + (baseArmServoAngle - baseArmServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else {
    if (baseArmServoAnglePrev > (baseArmServoAngle + baseArmServoAngleCached) / 2) {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev - (baseArmServoAngleCached - baseArmServoAnglePrev) * INTERPOLATION_FACTOR;

      if (baseArmServoAngleSmoothed == baseArmServoAnglePrev) {
        baseArmServoAngleSmoothed = baseArmServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev - (baseArmServoAnglePrev - baseArmServoAngle) * INTERPOLATION_FACTOR;
    }
  }

  // Calculate the smoothed armArmServoAngle
  if (armArmServoAngle > armArmServoAngleCached) {
    if (armArmServoAnglePrev < (armArmServoAngle + armArmServoAngleCached) / 2) {
      armArmServoAngleSmoothed = armArmServoAnglePrev + (armArmServoAnglePrev - armArmServoAngleCached) * INTERPOLATION_FACTOR;

      if (armArmServoAngleSmoothed == armArmServoAnglePrev) {
        armArmServoAngleSmoothed = armArmServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      armArmServoAngleSmoothed = armArmServoAnglePrev + (armArmServoAngle - armArmServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else {
    if (armArmServoAnglePrev > (armArmServoAngle + armArmServoAngleCached) / 2) {
      armArmServoAngleSmoothed = armArmServoAnglePrev - (armArmServoAngleCached - armArmServoAnglePrev) * INTERPOLATION_FACTOR;

      if (armArmServoAngleSmoothed == armArmServoAnglePrev) {
        armArmServoAngleSmoothed = armArmServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      armArmServoAngleSmoothed = armArmServoAnglePrev - (armArmServoAnglePrev - armArmServoAngle) * INTERPOLATION_FACTOR;
    }
  }

  // Calculate the smoothed gripperPitchServoAngle
  if (gripperPitchServoAngle > gripperPitchServoAngleCached) {
    if (gripperPitchServoAnglePrev < (gripperPitchServoAngle + gripperPitchServoAngleCached) / 2) {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev + (gripperPitchServoAnglePrev - gripperPitchServoAngleCached) * INTERPOLATION_FACTOR;

      if (gripperPitchServoAngleSmoothed == gripperPitchServoAnglePrev) {
        gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev + (gripperPitchServoAngle - gripperPitchServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else {
    if (gripperPitchServoAnglePrev > (gripperPitchServoAngle + gripperPitchServoAngleCached) / 2) {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev - (gripperPitchServoAngleCached - gripperPitchServoAnglePrev) * INTERPOLATION_FACTOR;

      if (gripperPitchServoAngleSmoothed == gripperPitchServoAnglePrev) {
        gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev - (gripperPitchServoAnglePrev - gripperPitchServoAngle) * INTERPOLATION_FACTOR;
    }
  }
}


/**
 * Returns true if the arm has finished moving to its 
 * destination and we can update the state to start the
 * next step of the move
*/
bool transitionComplete() {
  bool baseDone = false;
  bool baseArmDone = false;
  bool armArmDone = false;
  bool gripperDone = false;

  if (abs(baseServoAngle - baseServoAnglePrev) < 0.1) baseDone = true;
  if (abs(baseArmServoAngle - baseArmServoAnglePrev) < 0.1) baseArmDone = true;
  if (abs(armArmServoAngle - armArmServoAnglePrev) < 0.1) armArmDone = true;
  if (abs(gripperPitchServoAngle - gripperPitchServoAnglePrev) < 0.1) gripperDone = true;

  return baseDone && baseArmDone && armArmDone && gripperDone;
}