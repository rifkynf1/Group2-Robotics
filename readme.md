# Group 2 Robotics - Integrated IoT & Sensing Project

This repository contains the integrated codebases for the Group 2 Robotics project. The project is split into two co-existing systems that communicate over UART:

1. **ESP32 IoT Controller:** Handles WiFi connection and MQTT communication with a cloud broker (HiveMQ).
2. **Arduino Uno Sensing Robot:** Handles environment scanning (ultrasonic HC-SR04 + servo motor), obstacle avoidance, and motor controls.

---

## Project Architecture & UART Link

The Arduino Uno scans the environment. When it makes movement decisions or reads sensor values, it transmits the data to the ESP32 via SoftwareSerial. The ESP32 receives, validates, and forwards this data to the MQTT broker.

### UART Wiring (Physical / Wokwi)
- **Arduino Uno TX (Pin 12)** -> **ESP32 RX (GPIO 16)**
- **Arduino Uno RX (Pin 8)** <- **ESP32 TX (GPIO 17)**
- **GND** -> Shared Ground between both microcontrollers

### Data Format
The status message sent from the Arduino Uno to the ESP32 follows this format:
```text
DIST=18,MODE=AUTO,TURN=LEFT
```
- **DIST:** Current distance value from the ultrasonic sensor.
- **MODE:** Current mode of the robot (e.g., `AUTO`).
- **TURN:** Robot movement direction (`FORWARD`, `BACKWARD`, `LEFT`, `RIGHT`, `STOP`).

---

## Features

### ESP32 IoT Module (`src/main_esp32.cpp`)
- Connects to WiFi (`Wokwi-GUEST` in simulation).
- Connects to public MQTT Broker (`broker.hivemq.com`).
- Subscribes to control topic: `grup2/device1/control` (supports `ON` and `OFF` to control built-in LED).
- Publishes to status topic: `grup2/device1/status`.
- Receives UART data from Arduino Uno, validates the message format, responds with `ACK` or `ERROR`, and forwards the valid status to the MQTT status topic.

### Arduino Sensing Module (`src/main_uno.cpp`)
- Continuous distance monitoring with HC-SR04 ultrasonic sensor.
- 180-degree scanning with a micro servo motor when obstacles are detected within 20cm.
- Movement control logic based on open paths (controlling ENA, ENB, IN1, IN2, IN3, IN4).
- Sends periodic and event-driven status updates to the ESP32 via SoftwareSerial.

---

## How to Run & Build

This project is configured as a multi-environment PlatformIO project.

### 1. Build the Code
Open the project using VS Code with the PlatformIO extension:
- **Build ESP32:** Run target `esp32dev` (`pio run -e esp32dev`)
- **Build Arduino Uno:** Run target `uno` (`pio run -e uno`)

### 2. Wokwi Simulation
Because Wokwi does not support simulating two microcontrollers in a single project, separate simulation configurations are provided:

#### To Simulate ESP32 (Default)
The files `diagram.json` and `wokwi.toml` at the root are configured for the ESP32.
1. Build the `esp32dev` environment.
2. Click the Wokwi start button/icon in VS Code to run.

#### To Simulate Arduino Uno
1. Build the `uno` environment.
2. Copy `diagram_uno.json` and `wokwi_uno.toml` to overwrite `diagram.json` and `wokwi.toml` at the root:
   - **Windows PowerShell:**
     ```powershell
     Copy-Item diagram_uno.json diagram.json -Force
     Copy-Item wokwi_uno.toml wokwi.toml -Force
     ```
3. Click the Wokwi start button/icon in VS Code to run.
