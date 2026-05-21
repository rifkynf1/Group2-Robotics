#include <WiFi.h>
#include <PubSubClient.h>

/*
========================================
   GROUP 2 ROBOTICS
   ESP32 MQTT IOT CONTROLLER
========================================
*/

// ================= WIFI CONFIG =================

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ================= MQTT CONFIG =================

const char* mqtt_server = "broker.hivemq.com";

#define LED 2

WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial unoSerial(2); // UART2 (RX=GPIO16, TX=GPIO17)

// MQTT Topics
const char* topic_control = "grup2/device1/control";
const char* topic_status  = "grup2/device1/status";

// ================= WIFI CONNECT =================

void setup_wifi() {

  Serial.println();
  Serial.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT CALLBACK =================

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.println("\n📩 MQTT Message Received");

  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Topic   : ");
  Serial.println(topic);

  Serial.print("Message : ");
  Serial.println(message);

  // ===== LED CONTROL =====

  if (message == "ON") {

    digitalWrite(LED, HIGH);
    Serial.println("💡 LED ON");

    client.publish(topic_status, "LED ON");
  }

  else if (message == "OFF") {

    digitalWrite(LED, LOW);
    Serial.println("💡 LED OFF");

    client.publish(topic_status, "LED OFF");
  }
}

// ================= MQTT RECONNECT =================

void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    String clientId = "ESP32-Grup2-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println("✅ MQTT Connected");

      client.subscribe(topic_control);
      client.publish(topic_status, "Device Online");

      Serial.println("Subscribed to control topic");

    } else {

      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5 sec");

      delay(5000);
    }
  }
}

// ================= SETUP =================

void setup() {

  Serial.begin(115200);
  delay(1500);

  Serial.println("\n=================================");
  Serial.println(" GROUP 2 ROBOTICS STARTING...");
  Serial.println(" ESP32 MQTT DEVICE ONLINE");
  Serial.println("=================================");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Initialize ESP32 UART2 (RX=GPIO16, TX=GPIO17) to communicate with Uno
  unoSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("ESP32 UART Link with Uno Started (RX=16, TX=17)");
}

// ================= LOOP =================

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Read UART from Arduino Uno
  if (unoSerial.available()) {
    String message = unoSerial.readStringUntil('\n');
    message.trim();

    Serial.print("Received from Arduino Uno: ");
    Serial.println(message);

    if (message.indexOf("DIST=") >= 0 &&
        message.indexOf("MODE=") >= 0 &&
        message.indexOf("TURN=") >= 0) {
      Serial.println("✅ Message format valid");
      unoSerial.println("ACK");
      
      // Forward the status message to MQTT status topic
      client.publish(topic_status, message.c_str());
      Serial.println("Forwarded message to MQTT topic");
    } else {
      Serial.println("❌ Message format invalid");
      unoSerial.println("ERROR");
    }
  }
}
