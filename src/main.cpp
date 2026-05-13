#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT Broker
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// Connect WiFi
void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Connect MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    if (client.connect("ESP32RobotClient")) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());

      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  Serial2.begin(9600);

  delay(1000);

  Serial.println("ESP32 ROBOT IOT START");

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Simulated data from Arduino Uno
String robotData = "";

if (Serial2.available()) {
  robotData = Serial2.readStringUntil('\n');

  Serial.println("Received from Uno:");
  Serial.println(robotData);
}

  // Variables
  String distance = "";
  String mode = "";
  String turn = "";

  // Parsing
  int dIndex = robotData.indexOf("DIST=");
  int mIndex = robotData.indexOf("MODE=");
  int tIndex = robotData.indexOf("TURN=");

  if (dIndex >= 0 && mIndex >= 0 && tIndex >= 0) {

    distance =
      robotData.substring(dIndex + 5,
      robotData.indexOf(",", dIndex));

    mode =
      robotData.substring(mIndex + 5,
      robotData.indexOf(",", mIndex));

    turn =
      robotData.substring(tIndex + 5);

    // Publish parsed data
    client.publish(
      "faridrobot/distance",
      distance.c_str()
    );

    client.publish(
      "faridrobot/mode",
      mode.c_str()
    );

    client.publish(
      "faridrobot/turn",
      turn.c_str()
    );

    Serial.println("Parsed MQTT Publish:");
    Serial.println(distance);
    Serial.println(mode);
    Serial.println(turn);
  }

  delay(5000);
}