
#include <Servo.h>

// Define motor pins
#define ENA 5
#define IN1 2
#define IN2 3
#define ENB 6
#define IN3 4
#define IN4 7
#define Z_EN 9
#define Z_IN1 8
#define Z_IN2 10

// Define pins for the joystick
const int joystickX = A0;
const int joystickY = A1;
const int buttonPin = 2;

// Movement thresholds for the joystick
const int threshold = 100;

// Servo pin
#define SERVO_PIN 11
Servo clawServo;

int servoPosition = 0;
bool returnInProgress = false;

void setup() {
  // Motor pins setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(Z_EN, OUTPUT);
  pinMode(Z_IN1, OUTPUT);
  pinMode(Z_IN2, OUTPUT);

  // Button setup
  pinMode(buttonPin, INPUT_PULLUP);

  // Servo setup
  clawServo.attach(SERVO_PIN);
  clawServo.write(servoPosition);

  Serial.begin(9600);
}

void loop() {
  int xValue = analogRead(joystickX) - 512;
  int yValue = analogRead(joystickY) - 512;
  bool buttonPressed = digitalRead(buttonPin) == LOW;

  if (!returnInProgress) {
    if (xValue > threshold) {
      moveMotor(ENA, IN1, IN2, HIGH);
    } else if (xValue < -threshold) {
      moveMotor(ENA, IN1, IN2, LOW);
    } else {
      stopMotor(ENA);
    }

    if (yValue > threshold) {
      moveMotor(ENB, IN3, IN4, HIGH);
    } else if (yValue < -threshold) {
      moveMotor(ENB, IN3, IN4, LOW);
    } else {
      stopMotor(ENB);
    }
  }

  if (buttonPressed && !returnInProgress) {
    returnInProgress = true;
    returnToOriginAndOpenClaw();
    returnInProgress = false;
  }
}

void moveMotor(int enablePin, int inPin1, int inPin2, bool direction) {
  analogWrite(enablePin, 200);
  digitalWrite(inPin1, direction ? HIGH : LOW);
  digitalWrite(inPin2, direction ? LOW : HIGH);
}

void stopMotor(int enablePin) {
  analogWrite(enablePin, 0);
}

void returnToOriginAndOpenClaw() {
  Serial.println("Returning X-axis to 0...");
  moveMotor(ENA, IN1, IN2, LOW);
  delay(1500);
  stopMotor(ENA);

  Serial.println("Returning Y-axis to 0...");
  moveMotor(ENB, IN3, IN4, LOW);
  delay(1500);
  stopMotor(ENB);

  Serial.println("Opening the claw...");
  clawServo.write(0);
  delay(500);
}
