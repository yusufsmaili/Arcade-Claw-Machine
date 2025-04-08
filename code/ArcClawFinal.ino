#include <Servo.h>

// Motor Driver Pins (L298N) for X and Y motors (shared driver)
const int motorX_IN1 = 4; 
const int motorX_IN2 = 5; 
const int motorY_IN3 = 10; 
const int motorY_IN4 = 11; 
const int enableX = 6;    
const int enableY = 12;   

// Motor Driver Pins for Z-axis
const int motorZ_IN1 = 22;
const int motorZ_IN2 = 23; 
const int enableZ = 24;   

// Servo Pin for Claw
const int clawServoPin = 51;
Servo clawServo;

// Joystick Pins
const int joyX = A0;
const int joyY = A1;
const int joyButton = 50;

// Speed Limits
const int speedMin = 100;
const int speedMax = 255;

// Servo Angles
const int clawOpen = 90;
const int clawClose = 20;

// Joystick Deadzone
const int deadZone = 50;

void setup() 
{
  pinMode(motorX_IN1, OUTPUT);
  pinMode(motorX_IN2, OUTPUT);
  pinMode(motorY_IN3, OUTPUT);
  pinMode(motorY_IN4, OUTPUT);
  pinMode(enableX, OUTPUT);
  pinMode(enableY, OUTPUT);

  pinMode(motorZ_IN1, OUTPUT);
  pinMode(motorZ_IN2, OUTPUT);
  pinMode(enableZ, OUTPUT);
  pinMode(joyButton, INPUT_PULLUP);

  clawServo.attach(clawServoPin);
  clawServo.write(clawOpen);

  Serial.begin(9600);
}

void moveMotor(int enablePin, int IN1, int IN2, int speed, bool forward) 
{
  analogWrite(enablePin, speed);
  digitalWrite(IN1, forward ? HIGH : LOW);
  digitalWrite(IN2, forward ? LOW : HIGH);
}

void stopMotor(int enablePin, int IN1, int IN2) 
{
  analogWrite(enablePin, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void controlClaw(bool close) 
{
  clawServo.write(close ? clawClose : clawOpen);
  delay(500); 
}

void loop() 
{
  int xValue = analogRead(joyX);
  int yValue = analogRead(joyY);
  bool buttonPressed = digitalRead(joyButton) == LOW;

  // Dynamically adjust speed
  int speedX = map(abs(xValue - 512), 0, 512, speedMin, speedMax);
  int speedY = map(abs(yValue - 512), 0, 512, speedMin, speedMax);

  // X-axis control with deadzone
  if (xValue > 512 + deadZone) 
  {
    moveMotor(enableX, motorX_IN1, motorX_IN2, speedX, true);
  } 
  else if (xValue < 512 - deadZone) 
  {
    moveMotor(enableX, motorX_IN1, motorX_IN2, speedX, false);
  } 
  else 
  {
    stopMotor(enableX, motorX_IN1, motorX_IN2);
  }

  // Y-axis control with deadzone
  if (yValue > 512 + deadZone) 
  {
    moveMotor(enableY, motorY_IN3, motorY_IN4, speedY, true);
  } 
  else if (yValue < 512 - deadZone) 
  {
    moveMotor(enableY, motorY_IN3, motorY_IN4, speedY, false);
  } 
  else 
  {
    stopMotor(enableY, motorY_IN3, motorY_IN4);
  }

  // Z-axis and claw control with button debounce
  static bool lastButtonState = HIGH;
  if (buttonPressed && lastButtonState == HIGH) 
  {
    // Move Z Down
    moveMotor(enableZ, motorZ_IN1, motorZ_IN2, speedMax, false);
    delay(2000);
    stopMotor(enableZ, motorZ_IN1, motorZ_IN2);

    // Close Claw
    controlClaw(true);

    // Move Z Up
    moveMotor(enableZ, motorZ_IN1, motorZ_IN2, speedMax, true);
    delay(2000);
    stopMotor(enableZ, motorZ_IN1, motorZ_IN2);

    // Open Claw
    controlClaw(false);
  }
  lastButtonState = buttonPressed;

  delay(50); // Short delay for responsiveness
}
