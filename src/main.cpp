#include <Arduino.h> 
#include <Servo.h>

// --- SENSOR & SERVO PINS ---
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int SERVO_PIN = 11;

// --- MOTOR DRIVER PINS (L298N) ---
// Based on your course slide pinout
const int ENA = 5;
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 7;
const int ENB = 6;

// --- THRESHOLDS & VARIABLES ---
const int OBSTACLE_THRESHOLD = 20; // cm
int currentDistance = 0;
int leftDistance = 0;
int rightDistance = 0;
int motorSpeed = 150; // Speed range: 0-255

Servo sensorServo;

// --- FUNCTION PROTOTYPES ---
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
  
  // Sensor Setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Servo Setup
  sensorServo.attach(SERVO_PIN);
  sensorServo.write(90); 
  delay(500);

  // Motor Setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void loop() {
  currentDistance = getDistance();
  
  // 2. TAMBAH OBSTACLE AVOIDANCE
  if (currentDistance <= OBSTACLE_THRESHOLD && currentDistance > 0) {
    // a. Stop
    stopRobot();
    Serial.println("Obstacle Detected! Stopping.");
    delay(300); 
    
    // b. Mundur (Move Backward)
    Serial.println("Moving Backward...");
    moveBackward();
    delay(400); // Mundur for 0.4 seconds
    stopRobot();
    delay(300);
    
    // c. Scan Kiri/Kanan (Servo Scanning)
    rightDistance = lookRight();
    delay(300);
    
    leftDistance = lookLeft();
    delay(300);
    
    // Return servo to center
    sensorServo.write(90);
    delay(300);
    
    // d. Pilih jalan aman (Choose safe path)
    if (rightDistance > leftDistance && rightDistance > OBSTACLE_THRESHOLD) {
      Serial.println("Path Clear: Turning Right");
      turnRight();
      delay(500); // Time needed to rotate right
    } else if (leftDistance > rightDistance && leftDistance > OBSTACLE_THRESHOLD) {
      Serial.println("Path Clear: Turning Left");
      turnLeft();
      delay(500); // Time needed to rotate left
    } else {
      // If both sides are blocked, turn around
      Serial.println("Blocked: Turning Around");
      turnRight();
      delay(1000); 
    }
    
    stopRobot();
    delay(200);

  } else {
    // 1. ROBOT BASIC (Forward by default if clear)
    moveForward();
  }
  
  delay(50); // Small delay for stability
}

// ==========================================
//           HELPER FUNCTIONS
// ==========================================

// --- ULTRASONIC MEASUREMENT ---
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return 100; // If no echo, assume path is clear (100cm)
  int distance = duration * 0.034 / 2;
  return distance;
}

// --- 3. SERVO SCANNING ---
int lookRight() {
  sensorServo.write(0); // Look 0 degrees
  delay(500);
  int distance = getDistance();
  return distance;
}

int lookLeft() {
  sensorServo.write(180); // Look 180 degrees
  delay(500);
  int distance = getDistance();
  return distance;
}

// --- 1. BASIC MOTOR CONTROLS ---
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
  digitalWrite(IN2, HIGH); // Left wheels backward
  digitalWrite(IN3, HIGH); // Right wheels forward
  digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, HIGH); // Left wheels forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  // Right wheels backward
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