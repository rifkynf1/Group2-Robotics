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

SoftwareSerial espSerial(8, 12);

const int OBSTACLE_THRESHOLD = 20;
const unsigned long SEND_INTERVAL = 1000;

int currentDistance = 0;
int leftDistance = 0;
int rightDistance = 0;

Servo sensorServo;
String currentTurn = "FORWARD";
unsigned long lastSendTime = 0;

int getDistance();
int lookRight();
int lookLeft();
void setDirectionForward();
void setDirectionBackward();
void setDirectionLeft();
void setDirectionRight();
void runMotors(unsigned long duration_ms);
void stopRobot();
void sendStatusToESP32();
void readESP32Replies();

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(L_DIR, OUTPUT);
  pinMode(R_DIR, OUTPUT);
  pinMode(L_STEP, OUTPUT);
  pinMode(R_STEP, OUTPUT);

  sensorServo.attach(SERVO_PIN);
  sensorServo.write(90);
  delay(500);

  Serial.println("Arduino Uno Obstacle Avoidance Robot Started");
  Serial.println("ESP32 serial link started on SoftwareSerial (RX=8, TX=12)");
}

void loop() {
  currentDistance = getDistance();

  if (currentDistance > 0 && currentDistance <= OBSTACLE_THRESHOLD) {
    stopRobot();
    currentTurn = "STOP";
    sendStatusToESP32();
    Serial.println("Obstacle detected! Stopping.");
    delay(300);

    setDirectionBackward();
    currentTurn = "BACKWARD";
    sendStatusToESP32();
    Serial.println("Moving backward...");
    runMotors(400);

    stopRobot();
    currentTurn = "STOP";
    sendStatusToESP32();
    delay(300);

    rightDistance = lookRight();
    delay(250);

    leftDistance = lookLeft();
    delay(250);

    sensorServo.write(90);
    delay(250);

    if (rightDistance > leftDistance && rightDistance > OBSTACLE_THRESHOLD) {
      setDirectionRight();
      currentTurn = "RIGHT";
      sendStatusToESP32();
      Serial.println("Path clear: turning right");
      runMotors(500);
    } else if (leftDistance > rightDistance && leftDistance > OBSTACLE_THRESHOLD) {
      setDirectionLeft();
      currentTurn = "LEFT";
      sendStatusToESP32();
      Serial.println("Path clear: turning left");
      runMotors(500);
    } else {
      setDirectionRight();
      currentTurn = "RIGHT";
      sendStatusToESP32();
      Serial.println("Blocked: turning around");
      runMotors(1000);
    }

    stopRobot();
    currentTurn = "STOP";
    sendStatusToESP32();
    delay(200);
  } else {
    setDirectionForward();
    currentTurn = "FORWARD";
    runMotors(20);
  }

  if (millis() - lastSendTime >= SEND_INTERVAL) {
    sendStatusToESP32();
  }

  readESP32Replies();
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 100;

  int distance = (int)(duration * 0.034 / 2.0);
  return distance;
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

void runMotors(unsigned long duration_ms) {
  unsigned long start = millis();
  while (millis() - start < duration_ms) {
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

  String message = "DIST=" + String(currentDistance) +
                   ",MODE=AUTO,TURN=" + currentTurn;

  espSerial.println(message);
  Serial.print("Sent to ESP32: ");
  Serial.println(message);
}

void readESP32Replies() {
  if (espSerial.available()) {
    String response = espSerial.readStringUntil('\n');
    response.trim();

    if (response.length() > 0) {
      Serial.print("ESP32 Response: ");
      Serial.println(response);
    }
  }
}