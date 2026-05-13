#include <Arduino.h>

HardwareSerial unoSerial(2);

void setup() {
  Serial.begin(115200);

  // ESP32 UART2
  // RX = GPIO16, TX = GPIO17
  unoSerial.begin(9600, SERIAL_8N1, 16, 17);

  Serial.println("ESP32 UART Communication Started");
}

void loop() {
  if (unoSerial.available()) {
    String message = unoSerial.readStringUntil('\n');
    message.trim();

    Serial.print("Received from Arduino Uno: ");
    Serial.println(message);

    if (message.indexOf("DIST=") >= 0 &&
        message.indexOf("MODE=") >= 0 &&
        message.indexOf("TURN=") >= 0) {
      Serial.println("Message format valid");
      unoSerial.println("ACK");
    } else {
      Serial.println("Message format invalid");
      unoSerial.println("ERROR");
    }
  }
}
