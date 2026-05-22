#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial unoSerial(2);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* topic_control = "grup2/device1/control";
const char* topic_status  = "grup2/device1/status";

unsigned long lastReconnectAttempt = 0;

String fit16(const String& text) {
  if (text.length() <= 16) return text;
  return text.substring(0, 16);
}

void showLCD(const String& line1, const String& line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(fit16(line1));
  lcd.setCursor(0, 1);
  lcd.print(fit16(line2));
}

void showWaitingState() {
  showLCD("MQTT CONNECTED!", "Waiting 4 Data..");
}

void showMessagePreview(const String& title, String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(fit16(title));
  lcd.setCursor(0, 1);
  lcd.print(fit16(msg));
}

bool isValidRobotMessage(const String& message) {
  return message.indexOf("DIST=") >= 0 &&
         message.indexOf("MODE=") >= 0 &&
         message.indexOf("TURN=") >= 0;
}

void setup_wifi() {
  showLCD("WiFi Connecting.", "");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  showLCD("WIFI CONNECTED!", "");
  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT RX [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (message == "ON") {
    showLCD("MQTT: RECV CMD", "TURN LED ON");
    client.publish(topic_status, "CMD:ON");
    delay(1000);
    showWaitingState();
  } else if (message == "OFF") {
    showLCD("MQTT: RECV CMD", "TURN LED OFF");
    client.publish(topic_status, "CMD:OFF");
    delay(1000);
    showWaitingState();
  }
}

void reconnect() {
  if (client.connected()) return;
  if (millis() - lastReconnectAttempt < 5000) return;

  lastReconnectAttempt = millis();

  showLCD("MQTT Connecting.", "");
  String clientId = "ESP32-Grup2-" + String(random(0xffff), HEX);

  if (client.connect(clientId.c_str())) {
    client.subscribe(topic_control);
    client.publish(topic_status, "Device Online");

    Serial.println("MQTT connected.");
    showWaitingState();
  } else {
    Serial.print("MQTT failed, rc=");
    Serial.println(client.state());
    showLCD("MQTT RETRYING..", "");
  }
}

void processIncomingData(String message, bool fromUno) {
  message.trim();
  if (message.length() == 0) return;

  Serial.print(fromUno ? "UNO -> ESP32: " : "TERM -> ESP32: ");
  Serial.println(message);

  if (isValidRobotMessage(message)) {
    if (fromUno) {
      unoSerial.println("ACK");
    }

    bool ok = false;
    if (client.connected()) {
      ok = client.publish(topic_status, message.c_str());
    }

    if (ok) {
      showMessagePreview("MQTT: SENT!", message);
      Serial.println("Published to MQTT.");
    } else {
      showMessagePreview("MQTT OFFLINE", message);
      Serial.println("Publish skipped/offline.");
    }

    delay(1200);

    if (client.connected()) {
      showWaitingState();
    } else {
      showLCD("MQTT RETRYING..", "Waiting 4 Data..");
    }
  } else {
    if (fromUno) {
      unoSerial.println("ERROR");
    }

    showLCD("UART ERROR", "Invalid Format");
    Serial.println("Invalid input format.");
    delay(1200);

    if (client.connected()) {
      showWaitingState();
    } else {
      showLCD("MQTT RETRYING..", "Waiting 4 Data..");
    }
  }
}

void setup() {
  Serial.begin(115200);
  unoSerial.begin(9600, SERIAL_8N1, 16, 17);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  randomSeed(micros());

  showLCD("Booting ESP32...", "");
  delay(1000);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("Type DIST=15,MODE=AUTO,TURN=LEFT then press Enter");
  reconnect();
}

void loop() {
  if (unoSerial.available()) {
    String msg = unoSerial.readStringUntil('\n');
    processIncomingData(msg, true);
  }

  if (Serial.available()) {
    String msg = Serial.readString();
    msg.trim();
    processIncomingData(msg, false);
  }

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}