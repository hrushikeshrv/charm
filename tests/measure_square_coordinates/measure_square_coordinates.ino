/*
  PCA9685 PWM Servo Driver Example
  pca9685-servomotor-demo.ino
  Demonstrates use of 16 channel I2C PWM driver board with 4 servo motors
  Uses Adafruit PWM library
  Uses 4 potentiometers for input

  DroneBot Workshop 2018
  https://dronebotworkshop.com
*/

// Include Wire Library for I2C Communications
#include <Wire.h>

// Include Adafruit PWM Library
#include <Adafruit_PWMServoDriver.h>

#define MIN_PULSE_WIDTH       650.0
#define MAX_PULSE_WIDTH       2350.0
#define FREQUENCY             50.0

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Define Potentiometer Inputs

int controlA = A0;
int controlB = A1;
int controlC = A2;
int controlD = A3;

// Define Motor Outputs on PCA9685 board

int motorA = 0;
int motorB = 1;
int motorC = 2;
int motorD = 4;

void setup() 
{
  Serial.begin(115200);
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
}


void moveMotor(int controlIn, int motorOut)
{
  int potVal;
  float pulseWidth;
  
  // Read values from potentiometer
  potVal = analogRead(controlIn);
  
  // Convert to pulse width
  pulseWidth = map(potVal, 0, 1023, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulseWidth = int(float(pulseWidth) / 1000000.0 * FREQUENCY * 4096.0);
  
  //Control Motor
  pwm.setPWM(motorOut, 0, pulseWidth);

}

void loop() {

  //Control Motor A
  moveMotor(controlA, motorA);
  
  //Control Motor B
  moveMotor(controlB, motorB);
    
  //Control Motor C
  moveMotor(controlC, motorC);
  
  //Control Motor D
  moveMotor(controlD, motorD);
  
  if (Serial.available()) {
    for (int i = 14; i < 18; i++) {
      int potVal = analogRead(i);
      float servoAngle = map(potVal, 0, 1023, 0.0, 180.0);
      Serial.print(servoAngle); Serial.print(", ");
    }
    Serial.println("");
    Serial.readString();
  }
}