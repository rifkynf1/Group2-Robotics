#include <Arduino.h> 
#include <Servo.h>
#include <SoftwareSerial.h>

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int SERVO_PIN = 11;
const int ENA = 5;
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 7;
const int ENB = 6;

// SoftwareSerial for ESP32 link (RX = 8, TX = 12)
SoftwareSerial espSerial(8, 12);

const int OBSTACLE_THRESHOLD = 20;
int currentDistance = 0;
int leftDistance = 0;
int rightDistance = 0;
int motorSpeed = 150;

Servo sensorServo;
String currentTurn = "FORWARD";

int getDistance();
int lookRight();
int lookLeft();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopRobot();
void sendStatusToESP32();

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000; // Send status every 1 second

void setup() {
  Serial.begin(9600); 
  espSerial.begin(9600);
  
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

  Serial.println("Arduino Uno Obstacle Avoidance Robot Started");
  Serial.println("SoftwareSerial Link to ESP32 Started (RX=8, TX=12)");
}

void loop() {
  currentDistance = getDistance();

  if (currentDistance <= OBSTACLE_THRESHOLD && currentDistance > 0) {
    stopRobot();
    currentTurn = "STOP";
    sendStatusToESP32();
    Serial.println("Obstacle Detected! Stopping.");
    delay(300); 
    
    Serial.println("Moving Backward...");
    moveBackward();
    currentTurn = "BACKWARD";
    sendStatusToESP32();
    delay(400); 
    stopRobot();
    currentTurn = "STOP";
    sendStatusToESP32();
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
      currentTurn = "RIGHT";
      sendStatusToESP32();
      delay(500);
    } else if (leftDistance > rightDistance && leftDistance > OBSTACLE_THRESHOLD) {
      Serial.println("Path Clear: Turning Left");
      turnLeft();
      currentTurn = "LEFT";
      sendStatusToESP32();
      delay(500);
    } else {
      Serial.println("Blocked: Turning Around");
      turnRight();
      currentTurn = "RIGHT";
      sendStatusToESP32();
      delay(1000); 
    }
    
    stopRobot();
    currentTurn = "STOP";
    sendStatusToESP32();
    delay(200);

  } else {
    moveForward();
    currentTurn = "FORWARD";
  }
  
  // Periodic status update to ESP32
  if (millis() - lastSendTime >= sendInterval) {
    sendStatusToESP32();
  }

  // Read ACK/Response from ESP32
  if (espSerial.available()) {
    String response = espSerial.readStringUntil('\n');
    response.trim();
    Serial.print("ESP32 Response: ");
    Serial.println(response);
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
  if (duration == 0) return 100;
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

void sendStatusToESP32() {
  lastSendTime = millis();
  String message = "DIST=" + String(currentDistance) + ",MODE=AUTO,TURN=" + currentTurn;
  espSerial.println(message);
  Serial.print("Sent to ESP32: ");
  Serial.println(message);
}
