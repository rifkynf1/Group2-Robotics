ESP32 MQTT IoT Control — Group 2

Simple IoT project using ESP32, MQTT, and WiFi to control an LED remotely.

📌 Project Overview

This project demonstrates basic Internet of Things (IoT) communication where an ESP32 connects to WiFi and an MQTT broker to receive commands and control an LED.

The ESP32 subscribes to an MQTT topic and turns the LED ON or OFF based on received messages.

⚙️ Technologies Used
ESP32
Arduino Framework
MQTT Protocol
PubSubClient Library
Wokwi Simulator
HiveMQ Public Broker
🌐 MQTT Configuration

Broker

broker.hivemq.com

Topics

Subscribe : grup2/control
Publish   : grup2/status
💡 Features

✅ WiFi Auto Connection
✅ MQTT Auto Reconnect
✅ LED Remote Control
✅ Serial Debug Monitoring
✅ Simulation Ready (Wokwi)

🔌 Hardware
ESP32 Dev Module
Onboard LED (GPIO 2)

No external components required.

▶️ How to Run
1. Build Firmware

Using PlatformIO:

PlatformIO: Build
2. Run Simulator (Wokwi)

Make sure wokwi.toml contains:

[wokwi]
version = 1

firmware = ".pio/build/esp32dev/firmware.bin"
elf = ".pio/build/esp32dev/firmware.elf"

Then:

Run Simulation
3. Control LED via MQTT

Open MQTT Web Client:

https://www.hivemq.com/demos/websocket-client/

Connect to:

broker.hivemq.com

Subscribe:

grup2/status

Publish command:

Topic: grup2/control
Message: ON

➡ LED turns ON

Message: OFF

➡ LED turns OFF

📊 Serial Monitor Output

Example:

===== ESP32 GRUP 2 START =====
WiFi Connected
MQTT CONNECTED
Subscribed to grup2/control
LED ON
🎯 Learning Objectives
Understand MQTT communication
Connect ESP32 to cloud broker
Implement IoT device control
Practice embedded networking