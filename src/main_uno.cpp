#include <Arduino.h> 
#include <Servo.h>
#include <SoftwareSerial.h>

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int SERVO_PIN = 11;

const int L_DIR = 2;
const int R_DIR = 4; 
const int L_STEP = 5;
const int R_STEP = 6;

// SoftwareSerial for ESP32 link (RX = 8, TX = 12)
SoftwareSerial espSerial(8, 12);

const int OBSTACLE_THRESHOLD = 20;
int currentDistance = 0;
int leftDistance = 0;
int rightDistance = 0;

Servo sensorServo;
String currentTurn = "FORWARD";

int getDistance();
int lookRight();
int lookLeft();
void setDirectionForward();
void setDirectionBackward();
void setDirectionLeft();
void setDirectionRight();
void runMotors(int duration_ms);
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

  // Initialize Stepper Pins
  pinMode(L_DIR, OUTPUT);
  pinMode(R_DIR, OUTPUT);
  pinMode(L_STEP, OUTPUT);
  pinMode(R_STEP, OUTPUT);

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
    setDirectionBackward();
    currentTurn = "BACKWARD";
    sendStatusToESP32();
    runMotors(400); // Replaces delay(400) to keep motors pulsing
    
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
      setDirectionRight();
      currentTurn = "RIGHT";
      sendStatusToESP32();
      runMotors(500); 
    } else if (leftDistance > rightDistance && leftDistance > OBSTACLE_THRESHOLD) {
      Serial.println("Path Clear: Turning Left");
      setDirectionLeft();
      currentTurn = "LEFT";
      sendStatusToESP32();
      runMotors(500); 
    } else {
      Serial.println("Blocked: Turning Around");
      setDirectionRight();
      currentTurn = "RIGHT";
      sendStatusToESP32();
      runMotors(1000); 
    }
    
    stopRobot();
    currentTurn = "STOP";
    sendStatusToESP32();
    delay(200);

  } else {
    // Normal Operation
    setDirectionForward();
    currentTurn = "FORWARD";
    runMotors(20);
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

// Note: If motors face opposite directions on a chassis, 
// one side must usually spin "backward" to push the robot forward.
void setDirectionForward() {
  digitalWrite(L_DIR, HIGH);
  digitalWrite(R_DIR, LOW); 
}

void setDirectionBackward() {
  digitalWrite(L_DIR, LOW);
  digitalWrite(R_DIR, HIGH);
}

void setDirectionLeft() {
  digitalWrite(L_DIR, LOW);
  digitalWrite(R_DIR, LOW);
}

void setDirectionRight() {
  digitalWrite(L_DIR, HIGH);
  digitalWrite(R_DIR, HIGH);
}

void runMotors(int duration_ms) {
  unsigned long start = millis();
  while(millis() - start < duration_ms) {
    digitalWrite(L_STEP, HIGH);
    digitalWrite(R_STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(L_STEP, LOW);
    digitalWrite(R_STEP, LOW);
    delayMicroseconds(1000);
  }
}

void stopRobot() {
  digitalWrite(L_STEP, LOW);
  digitalWrite(R_STEP, LOW);
}

void sendStatusToESP32() {
  lastSendTime = millis();
  String message = "DIST=" + String(currentDistance) + ",MODE=AUTO,TURN=" + currentTurn;
  espSerial.println(message);
  Serial.print("Sent to ESP32: ");
  Serial.println(message);
}