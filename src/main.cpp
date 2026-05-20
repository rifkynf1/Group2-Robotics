#include <Arduino.h> 
#include <Servo.h>

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int SERVO_PIN = 11;
const int ENA = 5;
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 7;
const int ENB = 6;

const int OBSTACLE_THRESHOLD = 20;
int currentDistance = 0;
int leftDistance = 0;
int rightDistance = 0;
int motorSpeed = 150;

Servo sensorServo;

int getDistance();
int lookRight();
int lookLeft();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopRobot();

void setup() {
  Serial.begin(9600); 
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  sensorServo.attach(SERVO_PIN);
  sensorServo.write(90); 
  delay(500);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void loop() {
  currentDistance = getDistance();

  if (currentDistance <= OBSTACLE_THRESHOLD && currentDistance > 0) {
    stopRobot();
    Serial.println("Obstacle Detected! Stopping.");
    delay(300); 
    
    Serial.println("Moving Backward...");
    moveBackward();
    delay(400); 
    stopRobot();
    delay(300);
    
    rightDistance = lookRight();
    delay(300);
    
    leftDistance = lookLeft();
    delay(300);
    
    sensorServo.write(90);
    delay(300);
    
    if (rightDistance > leftDistance && rightDistance > OBSTACLE_THRESHOLD) {
      Serial.println("Path Clear: Turning Right");
      turnRight();
      delay(500);
    } else if (leftDistance > rightDistance && leftDistance > OBSTACLE_THRESHOLD) {
      Serial.println("Path Clear: Turning Left");
      turnLeft();
      delay(500);
    } else {
      Serial.println("Blocked: Turning Around");
      turnRight();
      delay(1000); 
    }
    
    stopRobot();
    delay(200);

  } else {
    moveForward();
  }
  
  delay(50);
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 100; //
  int distance = duration * 0.034 / 2;
  return distance;
}

int lookRight() {
  sensorServo.write(0);
  delay(500);
  int distance = getDistance();
  return distance;
}

int lookLeft() {
  sensorServo.write(180);
  delay(500);
  int distance = getDistance();
  return distance;
}

void moveForward() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}