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
  {103.00,128.00,153.00,172.00},{83.00,43.00,10.00,100.00},{79.00,55.00,34.00,114.00},{74.00,69.00,56.00,131.00},{69.00,79.00,73.00,137.00},{69.00,85.00,86.00,141.00},{61.00,93.00,107.00,152.00},{51.00,103.00,117.00,152.00},
  {87.00,44.00,11.00,118.00},{88.00,48.00,17.00,103.00},{88.00,61.00,44.00,122.00},{86.00,74.00,63.00,128.00},{81.00,80.00,76.00,137.00},{76.00,91.00,93.00,141.00},{74.00,100.00,113.00,154.00},{64.00,111.00,120.00,145.00},
  {94.00,44.00,11.00,106.00},{95.00,49.00,21.00,104.00},{93.00,63.00,44.00,115.00},{96.00,74.00,64.00,125.00},{90.00,81.00,81.00,135.00},{87.00,98.00,100.00,144.00},{83.00,103.00,124.00,167.00},{77.00,111.00,126.00,144.00},
  {101.00,45.00,11.00,110.00},{102.00,50.00,20.00,100.00},{102.00,61.00,44.00,113.00},{100.00,74.00,63.00,118.00},{105.00,86.00,80.00,130.00},{99.00,97.00,100.00,139.00},{102.00,109.00,122.00,157.00},{96.00,123.00,131.00,149.00},
  {113.00,46.00,11.00,105.00},{112.00,50.00,20.00,103.00},{113.00,66.00,44.00,116.00},{109.00,59.00,64.00,129.00},{114.00,86.00,80.00,131.00},{112.00,99.00,100.00,142.00},{115.00,106.00,114.00,143.00},{117.00,121.00,128.00,141.00},
  {120.00,45.00,10.00,116.00},{120.00,52.00,20.00,104.00},{122.00,63.00,44.00,115.00},{123.00,71.00,56.00,117.00},{122.00,85.00,76.00,131.00},{125.00,94.00,97.00,138.00},{129.00,101.00,106.00,139.00},{134.00,111.00,120.00,135.00},
  {121.00,45.00,10.00,120.00},{127.00,47.00,17.00,103.00},{128.00,60.00,33.00,103.00},{128.00,71.00,56.00,123.00},{135.00,77.00,72.00,126.00},{136.00,88.00,83.00,130.00},{141.00,100.00,113.00,154.00},{148.00,103.00,119.00,149.00},
  {133.00,44.00,10.00,126.00},{131.00,45.00,10.00,103.00},{136.00,49.00,17.00,89.00},{140.00,63.00,45.00,110.00},{141.00,69.00,52.00,104.00},{146.00,81.00,73.00,121.00},{149.00,88.00,86.00,125.00},{151.00,90.00,92.00,133.00}
};

// Stores the servo angles for grabbing the piece on each square of the
// board in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const int grabbingAngles[64][4] = {
  {104.00,128.00,154.00,172.00},{83.00,33.00,10.00,103.00},{78.00,45.00,34.00,117.00},{74.00,55.00,56.00,134.00},{70.00,63.00,74.00,146.00},{69.00,69.00,86.00,150.00},{61.00,76.00,102.00,155.00},{55.00,80.00,114.00,159.00},
  {86.00,33.00,11.00,123.00},{86.00,37.00,17.00,109.00},{88.00,49.00,44.00,130.00},{83.00,59.00,64.00,135.00},{79.00,64.00,76.00,143.00},{76.00,72.00,93.00,150.00},{70.00,78.00,113.00,167.00},{65.00,84.00,124.00,163.00},
  {93.00,32.00,11.00,119.00},{93.00,39.00,20.00,109.00},{93.00,50.00,44.00,122.00},{91.00,59.00,64.00,132.00},{88.00,68.00,81.00,143.00},{91.00,75.00,100.00,155.00},{83.00,80.00,121.00,175.00},{81.00,90.00,136.00,175.00},
  {100.00,34.00,11.00,117.00},{102.00,39.00,20.00,106.00},{102.00,51.00,44.00,116.00},{101.00,59.00,64.00,131.00},{105.00,67.00,80.00,138.00},{99.00,74.00,100.00,154.00},{97.00,81.00,122.00,169.00},{96.00,87.00,130.00,159.00},
  {113.00,35.00,11.00,116.00},{112.00,39.00,20.00,106.00},{113.00,51.00,44.00,121.00},{109.00,61.00,67.00,142.00},{114.00,68.00,80.00,139.00},{112.00,74.00,100.00,154.00},{113.00,81.00,118.00,162.00},{115.00,87.00,131.00,162.00},
  {114.00,33.00,10.00,119.00},{116.00,38.00,20.00,111.00},{117.00,49.00,44.00,122.00},{118.00,56.00,56.00,123.00},{120.00,66.00,76.00,135.00},{122.00,72.00,97.00,151.00},{129.00,79.00,113.00,154.00},{134.00,83.00,125.00,155.00},
  {126.00,33.00,10.00,126.00},{122.00,37.00,18.00,111.00},{124.00,45.00,33.00,111.00},{127.00,56.00,56.00,126.00},{130.00,63.00,72.00,132.00},{136.00,68.00,86.00,136.00},{141.00,79.00,113.00,164.00},{147.00,81.00,119.00,160.00},
  {125.00,32.00,10.00,132.00},{129.00,34.00,10.00,109.00},{131.00,37.00,18.00,92.00},{136.00,50.00,45.00,115.00},{140.00,57.00,58.00,118.00},{142.00,64.00,75.00,130.00},{149.00,70.00,86.00,133.00},{148.00,71.00,92.00,140.00}
};

// The angles for the position where captured pieces should be
// dropped in the format - {base angle, base-arm angle, arm-arm angle, gripper pitch}
const float captureHoverAngles[4] = {60.0, 74.0, 70.0, 105.0};
const float captureLowerAngles[4] = {60.0, 61.0, 65.0, 126.0};

// The angles the arm resets to after each move
const float resetAngles[4] = {108.0, 132.0, 159.0, 178.0};

// The gripper servo angles when the gripper is open and closed
const int gripperOpenAngle = 100;
const int gripperClosedAngle = 60;

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

          Serial.println("\n\nNew destination angles -------------------");
          Serial.print(baseServoAngle); Serial.print(",\t"); Serial.print(baseArmServoAngle); Serial.print(",\t"); Serial.print(armArmServoAngle); Serial.print(",\t"); Serial.print(gripperPitchServoAngle); Serial.println("---------------------------------\n\n");

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
  // Serial.print(",");

  // Serial.print(baseServoAngle);
  // Serial.print(",");
  // Serial.print(baseServoAnglePrev);
  // Serial.print(",");
  // Serial.print(baseServoAngleCached);
  Serial.print(",   ");

  Serial.print(baseArmServoAngleSmoothed);
  // Serial.print(",");

  // Serial.print(baseArmServoAngle);
  // Serial.print(",");
  // Serial.print(baseArmServoAnglePrev);
  // Serial.print(",");
  // Serial.print(baseArmServoAngleCached);
  Serial.print(",   ");

  Serial.print(armArmServoAngleSmoothed);
  // Serial.print(",");

  // Serial.print(armArmServoAngle);
  // Serial.print(",");
  // Serial.print(armArmServoAnglePrev);
  // Serial.print(",");
  // Serial.print(armArmServoAngleCached);
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
      baseArmServoAngleSmoothed = baseArmServoAnglePrev + abs(baseArmServoAnglePrev - baseArmServoAngleCached) * (INTERPOLATION_FACTOR / 2);

      if (baseArmServoAngleSmoothed == baseArmServoAnglePrev) {
        baseArmServoAngleSmoothed = baseArmServoAnglePrev + (INTERPOLATION_FACTOR / 2);
      }
    }
    else {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev + abs(baseArmServoAngle - baseArmServoAnglePrev) * (INTERPOLATION_FACTOR / 2);
    }
  }
  else if (baseArmServoAngle < baseArmServoAngleCached) {
    if (baseArmServoAnglePrev > (baseArmServoAngle + baseArmServoAngleCached) / 2) {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev - abs(baseArmServoAngleCached - baseArmServoAnglePrev) * (INTERPOLATION_FACTOR / 2);

      if (baseArmServoAngleSmoothed == baseArmServoAnglePrev) {
        baseArmServoAngleSmoothed = baseArmServoAnglePrev - (INTERPOLATION_FACTOR / 2);
      }
    }
    else {
      baseArmServoAngleSmoothed = baseArmServoAnglePrev - abs(baseArmServoAnglePrev - baseArmServoAngle) * (INTERPOLATION_FACTOR / 2);
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