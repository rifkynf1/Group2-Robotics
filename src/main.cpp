#include <Arduino.h> 
#include <Servo.h>

// Pin Definitions 
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int SERVO_PIN = 11;

// Thresholds & Variables 
const int OBSTACLE_THRESHOLD = 20;
int currentDistance = 0;
int leftDistance = 0;
int rightDistance = 0;

// Integration Variables 
String turnCommand = "FORWARD"; 
String currentMode = "AUTO";

Servo sensorServo;

// FUNCTION PROTOTYPES
// for compiler to recognize functions before they are defined
int getDistance();
int lookRight();
int lookLeft();
void printStatus();

void setup() {
  Serial.begin(9600); 
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  sensorServo.attach(SERVO_PIN);
  sensorServo.write(90); 
  delay(500);
}

void loop() {
  currentDistance = getDistance();
  
  if (currentDistance <= OBSTACLE_THRESHOLD) {
    turnCommand = "STOP";
    printStatus(); 
    
    rightDistance = lookRight();
    delay(300);
    
    leftDistance = lookLeft();
    delay(300);
    
    sensorServo.write(90);
    delay(300);
    
    if (rightDistance > leftDistance && rightDistance > OBSTACLE_THRESHOLD) {
      turnCommand = "RIGHT";
    } else if (leftDistance > rightDistance && leftDistance > OBSTACLE_THRESHOLD) {
      turnCommand = "LEFT";
    } else {
      turnCommand = "BACKWARD"; 
    }
  } else {
    turnCommand = "FORWARD";
  }
  
  printStatus();
  delay(200); 
}

// Helper Functions 
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

int lookRight() {
  sensorServo.write(0);
  delay(500); 
  return getDistance();
}

int lookLeft() {
  sensorServo.write(180);
  delay(500); 
  return getDistance();
}

void printStatus() {
  Serial.print("DIST=");
  Serial.print(currentDistance);
  Serial.print(",MODE=");
  Serial.print(currentMode);
  Serial.print(",TURN=");
  Serial.println(turnCommand);
}