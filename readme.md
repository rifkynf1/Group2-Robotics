Group 2 Robotics IoT Project

Simple IoT project using ESP32, WiFi, and MQTT protocol to demonstrate basic communication between device and cloud broker.

About This Project

This project was developed as part of the Group 2 robotics progress.
The ESP32 connects to WiFi and communicates using MQTT to send and receive messages.

The device subscribes to a control topic and responds based on incoming commands.

Features
-  ESP32 WiFi connection
-  MQTT communication using public broker
-  Subscribe & publish MQTT topic
-  Simple device control simulation
-  Tested using Wokwi simulator
Technologies Used
-  ESP32
-  Arduino Framework
-  PlatformIO
-  MQTT (HiveMQ Broker)
-  Wokwi Simulator

MQTT Topics
Subscribe : grup2/control
Publish   : grup2/status

Example command:

ON
OFF

How to Run
1.Clone this repository
2.Open project using VS Code + PlatformIO
3.Build the project
4.Run simulation using Wokwi
5.Open MQTT client and publish message to topic

Notes:

This project focuses on learning IoT communication concepts and basic ESP32 integration with MQTT.
