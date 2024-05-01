#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// These constants are used by the PCA9685 driver.
// You won't need to change these values
const float MIN_PULSE_WIDTH = 650;
const float MAX_PULSE_WIDTH = 2350;
const float FREQUENCY = 50;

const float MOVE_SETTLE_DELAY = 300;

// The smoothening factor for the arm's movement. 
// Should be greater than 0 and less than 1.
// Higher number means less smooth.
// Change this number depending on how slow/fast you want the arm's movement to be.
// Values between 0.04 and 0.1 should work well.
float INTERPOLATION_FACTOR = 0.07;

// These pin numbers are for the pins on the PCA9685 motor driver, NOT the Arduino.
const int BASE_SERVO_PIN = 0;
const int BASE_ARM_SERVO_PIN = 1;
const int ARM_ARM_SERVO_PIN = 2;
const int GRIPPER_PITCH_SERVO_PIN = 3;
const int GRIPPER_GRAB_SERVO_PIN = 4;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Stores the servo angles for hovering above each square of the board
// in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const int hoverAngles[64][4] = {
  {84.0, 36.0, 8.0, 94.0}, {80.0, 47.0, 28.0, 100.0}, {76.0, 56.0, 39.0, 92.0}, {71.0, 64.0, 58.0, 98.0}, {68.0, 74.0, 76.0, 105.0}, {64.0, 82.0, 89.0, 115.0}, {61.0, 89.0, 100.0, 107.0}, {50.0, 99.0, 116.0, 117.0},         // A1 to A8
  {87.0, 36.0, 8.0, 88.0},{87.0, 48.0, 32.0, 100.0},{84.0, 56.0, 46.0, 92.0},{84.0, 68.0, 66.0, 104.0},{77.0, 79.0, 89.0, 117.0},{75.0, 87.0, 102.0, 120.0},{70.0, 97.0, 111.0, 114.0},{60.0, 107.0, 123.0, 124.0},             // B1 to B8
  {97.0, 42.0, 15.0, 89.0},{95.0, 51.0, 35.0, 100.0},{94.0, 57.0, 45.0, 92.0},{90.0, 71.0, 74.0, 108.0},{88.0, 79.0, 89.0, 114.0},{86.0, 90.0, 104.0, 120.0},{81.0, 100.0, 117.0, 117.0},{75.0, 113.0, 130.0, 124.0},           // C1 to C8
  {100.0, 42.0, 14.0, 88.0},{103.0, 53.0, 36.0, 94.0},{103.0, 57.0, 45.0, 92.0},{104.0, 71.0, 74.0, 108.0},{100.0, 81.0, 87.0, 114.0},{100.0, 89.0, 104.0, 116.0},{94.0, 103.0, 116.0, 117.0},{92.0, 114.0, 132.0, 124.0},      // D1 to D8
  {110.0, 40.0, 10.0, 89.0},{109.0, 49.0, 31.0, 94.0},{109.0, 57.0, 44.0, 91.0},{107.0, 71.0, 73.0, 107.0},{108.0, 82.0, 87.0, 113.0},{111.0, 91.0, 104.0, 116.0},{112.0, 101.0, 117.0, 117.0},{113.0, 118.0, 136.0, 124.0},    // E1 to E8
  {103.0, 35.0, 35.0, 110.0},{115.0, 54.0, 31.0, 95.0},{118.0, 57.0, 44.0, 91.0},{119.0, 70.0, 73.0, 107.0},{117.0, 79.0, 84.0, 114.0},{125.0, 89.0, 102.0, 115.0},{126.0, 101.0, 117.0, 118.0},{130.0, 114.0, 130.0, 124.0},   // F1 to F8
  {110.0, 35.0, 35.0, 110.0},{124.0, 39.0, 15.0, 77.0},{127.0, 53.0, 38.0, 91.0},{125.0, 67.0, 65.0, 108.0},{128.0, 77.0, 81.0, 114.0},{137.0, 86.0, 97.0, 115.0},{139.0, 98.0, 112.0, 114.0},{143.0, 105.0, 122.0, 118.0},     // G1 to G8
  {127.0, 38.0, 14.0, 109.0},{127.0, 41.0, 8.0, 84.0},{134.0, 54.0, 32.0, 83.0},{134.0, 64.0, 58.0, 102.0},{140.0, 73.0, 73.0, 114.0},{142.0, 81.0, 87.0, 107.0},{147.0, 92.0, 104.0, 117.0},{161.0, 98.0, 115.0, 118.0},       // H1 to H8
};

// Stores the servo angles for grabbing the piece on each square of the
// board in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const int grabbingAngles[64][4] = {
  {83.00, 32.00, 8.00, 120.00},{78.00, 37.00, 17.00, 110.00},{79.00, 44.00, 30.00, 107.00},{71.00, 54.00, 58.00, 125.00},{69.00, 61.00, 71.00, 126.00},{64.00, 68.00, 89.00, 141.00},{58.00, 76.00, 114.00, 157.00},{49.00, 79.00, 120.00, 161.00},           // A1 to A8
  {89.00, 31.00, 8.00, 116.00},{88.00, 39.00, 21.00, 110.00},{83.00, 49.00, 43.00, 119.00},{83.00, 58.00, 66.00, 131.00},{78.00, 67.00, 88.00, 144.00},{74.00, 70.00, 96.00, 141.00},{68.00, 79.00, 117.00, 157.00},{60.00, 82.00, 126.00, 160.00},           // B1 to B8 
  {96.00, 31.00, 8.00, 111.00},{91.00, 41.00, 25.00, 110.00},{94.00, 50.00, 47.00, 120.00},{93.00, 61.00, 70.00, 130.00},{91.00, 68.00, 91.00, 144.00},{87.00, 73.00, 105.00, 144.00},{82.00, 79.00, 120.00, 157.00},{76.00, 87.00, 132.00, 160.00},          // C1 to C8
  {99.00, 32.00, 8.00, 111.00},{102.00, 41.00, 26.00, 110.00},{102.00, 51.00, 50.00, 120.00},{101.00, 61.00, 71.00, 131.00},{99.00, 68.00, 92.00, 144.00},{100.00, 73.00, 105.00, 144.00},{98.00, 81.00, 125.00, 161.00},{91.00, 88.00, 137.00, 165.00},      // D1 to D8 
  {106.00, 32.00, 8.00, 111.00},{108.00, 41.00, 26.00, 110.00},{110.00, 51.00, 50.00, 120.00},{112.00, 61.00, 71.00, 131.00},{111.00, 68.00, 92.00, 144.00},{112.00, 73.00, 105.00, 144.00},{114.00, 81.00, 125.00, 161.00},{116.00, 88.00, 137.00, 165.00},  // E1 to E8 
  {116.00, 32.00, 8.00, 111.00},{113.00, 39.00, 23.00, 110.00},{117.00, 51.00, 46.00, 120.00},{115.00, 57.00, 64.00, 122.00},{122.00, 67.00, 89.00, 144.00},{124.00, 72.00, 103.00, 144.00},{126.00, 81.00, 120.00, 157.00},{132.00, 87.00, 133.00, 159.00},  // F1 to F8
  {122.00, 32.00, 8.00, 116.00},{120.00, 38.00, 21.00, 110.00},{121.00, 49.00, 45.00, 120.00},{124.00, 57.00, 61.00, 122.00},{130.00, 65.00, 86.00, 144.00},{131.00, 70.00, 97.00, 144.00},{139.00, 79.00, 117.00, 157.00},{146.00, 82.00, 126.00, 160.00},   // G1 to G8
  {127.00, 32.00, 8.00, 121.00},{126.00, 35.00, 13.00, 107.00},{133.00, 44.00, 30.00, 112.00},{134.00, 53.00, 56.00, 122.00},{138.00, 63.00, 80.00, 141.00},{141.00, 68.00, 91.00, 144.00},{149.00, 76.00, 114.00, 157.00},{155.00, 79.00, 119.00, 160.00},  // H1 to H8
};

// The angles for the position where captured pieces should be
// dropped in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const float captureHoverAngles[4] = {0.0, 0.0, 0.0, 0.0};
const float captureLowerAngles[4] = {0.0, 0.0, 0.0, 0.0};

// The angles the arm resets to after each move
const float resetAngles[4] = {108.0, 132.0, 165.0, 178.0};

// The gripper servo angles when the gripper is open and closed
const int gripperOpenAngle = 100;
const int gripperClosedAngle = 77;

// The previous destination angles of the servos
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
// The index of the current destination in either the hoverAngles array or the grabbingAngles array
int currentDestination = 0;
/**
 * The current state -
 * 0 means hover over the target
 * 1 means lower the arm at the target
 * 2 means toggle the gripper state (open -> close or closed -> open)
 * 3 means hover over the target again
*/
int currentState = 0;

// Boolean to keep track of whether the arm was notified of the move
bool engineNotified = false;


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
    engineNotified = false;

    String move = Serial.readString();
    move.trim();

    baseServoAngleCached = baseServoAngle;
    baseArmServoAngleCached = baseArmServoAngle;
    armArmServoAngleCached = armArmServoAngle;
    gripperPitchServoAngleCached = gripperPitchServoAngle;

    if (move == "q") {
      // Reset the arm's position
      baseServoAngle = resetAngles[0];
      baseArmServoAngle = resetAngles[1];
      armArmServoAngle = resetAngles[2];
      gripperPitchServoAngle = resetAngles[3];

      openGripper();
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

      currentDestination = 0;
      currentState = 0;

      baseServoAngle = hoverAngles[destinations[0]][0];
      baseArmServoAngle = hoverAngles[destinations[0]][1];
      armArmServoAngle = hoverAngles[destinations[0]][2];
      gripperPitchServoAngle = hoverAngles[destinations[0]][3];
    }
  }

  if (transitionComplete()) {
    // Update current state and current destination
    if (currentState >= 3) {
      currentState = 0;
      currentDestination += 1;
    }
    else {
      currentState += 1;
    }

    int idx = destinations[currentDestination];
    // If we go through all destinations, we reset the arm position
    if (
      currentDestination > 3
      || (currentDestination > 1 && idx == -1)
    ) {
      currentDestination = 4;
      currentState = 3;

      baseServoAngle = resetAngles[0];
      baseArmServoAngle = resetAngles[1];
      armArmServoAngle = resetAngles[2];
      gripperPitchServoAngle = resetAngles[3];

      if (!engineNotified) {
        Serial.println("move done");
        engineNotified = true;
      }
    }

    // Update destination angles/take new action 
    // according to new updated state and destination
    if (currentState == 2) {
      if (gripperClosed) openGripper();
      else closeGripper();
    }
    else if (currentDestination < 4) {
      // If index is -1 when current destination is 1, we have to hover over
      // the capture position
      if (currentDestination == 1 && idx == -1) {
        // Update the previous destination angles of the servos
        baseServoAngleCached = baseServoAngle;
        baseArmServoAngleCached = baseArmServoAngle;
        armArmServoAngleCached = armArmServoAngle;
        gripperPitchServoAngleCached = gripperPitchServoAngle;

        if (currentState == 0 || currentState == 3) {
          baseServoAngle = captureHoverAngles[0];
          baseArmServoAngle = captureHoverAngles[1];
          armArmServoAngle = captureHoverAngles[2];
          gripperPitchServoAngle = captureHoverAngles[3];
        }
        if (currentState == 1) {
          baseServoAngle = captureLowerAngles[0];
          baseArmServoAngle = captureLowerAngles[1];
          armArmServoAngle = captureLowerAngles[2];
          gripperPitchServoAngle = captureLowerAngles[3];
        }
      }
      else {
        // If we get a valid square index, we have to move to that square
        if (idx >= 0 && idx <= 63) {
          // Update the previous destination angles of the servos
          baseServoAngleCached = baseServoAngle;
          baseArmServoAngleCached = baseArmServoAngle;
          armArmServoAngleCached = armArmServoAngle;
          gripperPitchServoAngleCached = gripperPitchServoAngle;

          if (currentState == 0 || currentState == 3) {
            baseServoAngle = hoverAngles[idx][0];
            baseArmServoAngle = hoverAngles[idx][1];
            armArmServoAngle = hoverAngles[idx][2];
            gripperPitchServoAngle = hoverAngles[idx][3];
          }
          if (currentState == 1) {
            baseServoAngle = grabbingAngles[idx][0];
            baseArmServoAngle = grabbingAngles[idx][1];
            armArmServoAngle = grabbingAngles[idx][2];
            gripperPitchServoAngle = grabbingAngles[idx][3];
          }
        }
      }
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

  Serial.print(baseServoAngle);
  Serial.print(",");
  Serial.print(baseServoAnglePrev);
  Serial.print(",");
  Serial.print(baseServoAngleCached);
  Serial.print(",   ");

  Serial.print(baseArmServoAngleSmoothed);
  Serial.print(",");

  Serial.print(baseArmServoAngle);
  Serial.print(",");
  Serial.print(baseArmServoAnglePrev);
  Serial.print(",");
  Serial.print(baseArmServoAngleCached);
  Serial.print(",   ");

  Serial.print(armArmServoAngleSmoothed);
  Serial.print(",");

  Serial.print(armArmServoAngle);
  Serial.print(",");
  Serial.print(armArmServoAnglePrev);
  Serial.print(",");
  Serial.print(armArmServoAngleCached);
  Serial.print(",   ");

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
    // Serial.println("Increasing base servo angle");
    if (baseServoAnglePrev < (baseServoAngle + baseServoAngleCached) / 2) {
      baseServoAngleSmoothed = baseServoAnglePrev + abs(baseServoAnglePrev - baseServoAngleCached) * INTERPOLATION_FACTOR;

      if (baseServoAngleSmoothed == baseServoAnglePrev) {
        baseServoAngleSmoothed = baseServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      baseServoAngleSmoothed = baseServoAnglePrev + abs(baseServoAngle - baseServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else if (baseServoAngle < baseServoAngleCached) {
    // Serial.println("Decreasing base servo angle");
    if (baseServoAnglePrev > (baseServoAngle + baseServoAngleCached) / 2) {
      baseServoAngleSmoothed = baseServoAnglePrev - abs(baseServoAngleCached - baseServoAnglePrev) * INTERPOLATION_FACTOR;

      if (baseServoAngleSmoothed == baseServoAnglePrev) {
        baseServoAngleSmoothed = baseServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      baseServoAngleSmoothed = baseServoAnglePrev - abs(baseServoAnglePrev - baseServoAngle) * INTERPOLATION_FACTOR;
    }
  }

  // Calculate the smoothened baseArmServoAngle
  if (baseArmServoAngle > baseArmServoAngleCached) {
    if (baseArmServoAnglePrev < (baseArmServoAngle + baseArmServoAngleCached) / 2) {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev + abs(baseArmServoAnglePrev - baseArmServoAngleCached) * INTERPOLATION_FACTOR;

      if (baseArmServoAngleSmoothed == baseArmServoAnglePrev) {
        baseArmServoAngleSmoothed = baseArmServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev + abs(baseArmServoAngle - baseArmServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else if (baseArmServoAngle < baseArmServoAngleCached) {
    if (baseArmServoAnglePrev > (baseArmServoAngle + baseArmServoAngleCached) / 2) {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev - abs(baseArmServoAngleCached - baseArmServoAnglePrev) * INTERPOLATION_FACTOR;

      if (baseArmServoAngleSmoothed == baseArmServoAnglePrev) {
        baseArmServoAngleSmoothed = baseArmServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev - abs(baseArmServoAnglePrev - baseArmServoAngle) * INTERPOLATION_FACTOR;
    }
  }

  // Calculate the smoothed armArmServoAngle
  if (armArmServoAngle > armArmServoAngleCached) {
    if (armArmServoAnglePrev < (armArmServoAngle + armArmServoAngleCached) / 2) {
      armArmServoAngleSmoothed = armArmServoAnglePrev + abs(armArmServoAnglePrev - armArmServoAngleCached) * INTERPOLATION_FACTOR;

      if (armArmServoAngleSmoothed == armArmServoAnglePrev) {
        armArmServoAngleSmoothed = armArmServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      armArmServoAngleSmoothed = armArmServoAnglePrev + abs(armArmServoAngle - armArmServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else if (armArmServoAngle < armArmServoAngleCached) {
    if (armArmServoAnglePrev > (armArmServoAngle + armArmServoAngleCached) / 2) {
      armArmServoAngleSmoothed = armArmServoAnglePrev - abs(armArmServoAngleCached - armArmServoAnglePrev) * INTERPOLATION_FACTOR;

      if (armArmServoAngleSmoothed == armArmServoAnglePrev) {
        armArmServoAngleSmoothed = armArmServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      armArmServoAngleSmoothed = armArmServoAnglePrev - abs(armArmServoAnglePrev - armArmServoAngle) * INTERPOLATION_FACTOR;
    }
  }

  // Calculate the smoothed gripperPitchServoAngle
  if (gripperPitchServoAngle > gripperPitchServoAngleCached) {
    if (gripperPitchServoAnglePrev < (gripperPitchServoAngle + gripperPitchServoAngleCached) / 2) {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev + abs(gripperPitchServoAnglePrev - gripperPitchServoAngleCached) * INTERPOLATION_FACTOR;

      if (gripperPitchServoAngleSmoothed == gripperPitchServoAnglePrev) {
        gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev + INTERPOLATION_FACTOR;
      }
    }
    else {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev + abs(gripperPitchServoAngle - gripperPitchServoAnglePrev) * INTERPOLATION_FACTOR;
    }
  }
  else if (gripperPitchServoAngle < gripperPitchServoAngleCached) {
    if (gripperPitchServoAnglePrev > (gripperPitchServoAngle + gripperPitchServoAngleCached) / 2) {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev - abs(gripperPitchServoAngleCached - gripperPitchServoAnglePrev) * INTERPOLATION_FACTOR;

      if (gripperPitchServoAngleSmoothed == gripperPitchServoAnglePrev) {
        gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev - INTERPOLATION_FACTOR;
      }
    }
    else {
      gripperPitchServoAngleSmoothed = gripperPitchServoAnglePrev - abs(gripperPitchServoAnglePrev - gripperPitchServoAngle) * INTERPOLATION_FACTOR;
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


void openGripper() {
  float pulseWidth;
  pulseWidth = map(gripperOpenAngle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
  pwm.setPWM(GRIPPER_GRAB_SERVO_PIN, 0, pulseWidth);
  gripperClosed = false;
  delay(MOVE_SETTLE_DELAY);
}

void closeGripper() {
  float pulseWidth;
  pulseWidth = map(gripperClosedAngle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulseWidth = int(float(pulseWidth) / 1000000 * FREQUENCY * 4096);
  pwm.setPWM(GRIPPER_GRAB_SERVO_PIN, 0, pulseWidth);
  gripperClosed = true;
  delay(MOVE_SETTLE_DELAY);
}