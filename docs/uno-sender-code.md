#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11); 
// RX = 10, TX = 11

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  Serial.println("Arduino Uno UART Started");
}

void loop() {
  int distance = random(5, 50);
  String mode = "AUTO";
  String turn;

  if (distance < 15) {
    turn = "STOP";
  } else if (distance < 25) {
    turn = "LEFT";
  } else {
    turn = "FORWARD";
  }

  String message = "DIST=" + String(distance) + ",MODE=" + mode + ",TURN=" + turn;

  espSerial.println(message);

  Serial.print("Sent to ESP32: ");
  Serial.println(message);

  if (espSerial.available()) {
    String response = espSerial.readStringUntil('\n');
    response.trim();

    Serial.print("ESP32 Response: ");
    Serial.println(response);
  }

  delay(1000);
}
