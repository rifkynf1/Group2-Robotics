# Group 2 Robotics - Arduino Uno and ESP32 Obstacle Avoidance Robot

This repository contains a two-board robotics project built with PlatformIO and Wokwi. The Arduino Uno handles obstacle detection and motor movement, while the ESP32 handles WiFi, MQTT communication, and LCD status display.

## Project overview

The system is split into two main controllers:

- **Arduino Uno (`src/main_uno.cpp`)**: reads the HC-SR04 ultrasonic sensor, rotates the servo to scan for clearer directions, drives the left and right stepper motor channels through two A4988 drivers, and sends robot status messages to the ESP32.
- **ESP32 (`src/main_esp32.cpp`)**: connects to WiFi, connects to the public HiveMQ broker, receives status messages from the Uno over UART2, validates the message format, publishes valid messages to MQTT, and displays connection or status information on the I2C LCD.

The message format used between the Arduino Uno and ESP32 is:

```text
DIST=18,MODE=AUTO,TURN=LEFT
```

In this format, `DIST` is the measured distance, `MODE` is the robot mode, and `TURN` is the current movement decision such as `FORWARD`, `BACKWARD`, `LEFT`, `RIGHT`, or `STOP`.

## Folder structure

```text
GROUP2-ROBOTICS/
├── docs/
├── include/
├── lib/
├── sim_esp32/
│   ├── diagram.json
│   └── wokwi.toml
├── sim_uno/
│   ├── diagram.json
│   └── wokwi.toml
├── src/
│   ├── main_esp32.cpp
│   └── main_uno.cpp
├── test/
├── platformio.ini
└── readme.md
```

The `sim_uno` folder stores the Wokwi diagram and simulator configuration for the Arduino Uno build, and `sim_esp32` stores the Wokwi diagram and simulator configuration for the ESP32 build. The `platformio.ini` file defines separate PlatformIO environments so each board is built only with its own source file and libraries.

## Hardware and pin mapping

### Arduino Uno side

Based on the current Uno code and Wokwi diagram, the Arduino Uno uses these main pins:

- Ultrasonic sensor: `TRIG = D9`, `ECHO = D10`.
- Servo motor: `D11`.
- Left motor driver: `DIR = D2`, `STEP = D5`.
- Right motor driver: `DIR = D4`, `STEP = D6`.
- SoftwareSerial link to ESP32: `RX = D8`, `TX = D12`.

The Wokwi Uno diagram uses two A4988 drivers to control four stepper motors, where one driver handles the left side pair and the other handles the right side pair.

### ESP32 side

Based on the current ESP32 code, the ESP32 uses these interfaces:

- UART2 for communication with the Uno: `RX = GPIO16`, `TX = GPIO17`.
- I2C LCD: `SDA = GPIO21`, `SCL = GPIO22`.

### UART connection between boards

The communication link between the two boards is intended to be:

- Arduino Uno `TX` (D12) -> ESP32 `RX` (GPIO16)
- Arduino Uno `RX` (D8) <- ESP32 `TX` (GPIO17)
- Common ground between both boards

## Code behavior

### Arduino Uno logic

The Uno continuously measures front distance using the HC-SR04 sensor. If the measured distance is above the obstacle threshold, the robot keeps moving forward by setting the direction pins and pulsing both step channels.

If an obstacle is detected within 20 cm, the robot stops, moves backward briefly, scans right and left with the servo-mounted ultrasonic sensor, compares both side distances, then turns toward the clearer path. After every important movement state, the Uno sends a formatted status message such as `DIST=12,MODE=AUTO,TURN=RIGHT` to the ESP32 through `SoftwareSerial`.

### ESP32 logic

The ESP32 connects to `Wokwi-GUEST` WiFi and then connects to the public MQTT broker at `broker.hivemq.com`. After connecting, it subscribes to `grup2/device1/control` and publishes robot status to `grup2/device1/status`.

When a message arrives from the Uno over UART2, the ESP32 trims the message, checks that it contains at least `DIST=` and `MODE=`, replies with `ACK` for valid data or `ERROR` for invalid data, publishes valid data to MQTT, and updates the LCD to show the current state.

## PlatformIO configuration

This project uses two PlatformIO environments:

| Environment | Board | Framework | Source filter | Libraries |
|---|---|---|---|---|
| `esp32dev` | ESP32 Dev Module | Arduino | Includes all sources except `main_uno.cpp` | `PubSubClient`, `LiquidCrystal_I2C` |
| `uno` | Arduino Uno | Arduino | Includes all sources except `main_esp32.cpp` | `Servo` |

This setup allows both boards to live in the same repository without mixing incompatible code during compilation.

## How to run in VS Code

### Requirements

Install these tools first:

- Visual Studio Code
- PlatformIO IDE extension for VS Code
- Wokwi for VS Code extension

Open the project folder in VS Code after the extensions are installed.

### Build the Arduino Uno code

Use the Uno environment when you want to compile the obstacle avoidance controller:

```bash
pio run -e uno
```

This builds `src/main_uno.cpp` with the Arduino Uno environment and the Servo library.

### Build the ESP32 code

Use the ESP32 environment when you want to compile the IoT and LCD controller:

```bash
pio run -e esp32dev
```

This builds `src/main_esp32.cpp` with the ESP32 environment and installs `PubSubClient` plus `LiquidCrystal_I2C` through PlatformIO.

### Run the Uno simulation in Wokwi

1. Build the Uno environment first with `pio run -e uno`.
2. Open the `sim_uno` folder files in VS Code.
3. Start Wokwi using the Uno simulator configuration in `sim_uno/wokwi.toml` and `sim_uno/diagram.json`.

This simulation represents the robot controller side with the ultrasonic sensor, servo, A4988 drivers, and stepper motors.

### Run the ESP32 simulation in Wokwi

1. Build the ESP32 environment first with `pio run -e esp32dev`.
2. Open the `sim_esp32` folder files in VS Code.
3. Start Wokwi using the ESP32 simulator configuration in `sim_esp32/wokwi.toml` and `sim_esp32/diagram.json`.

This simulation represents the IoT controller side with WiFi, MQTT logic, and the LCD display.

## Notes for the current iteration

This repository is organized so the Arduino Uno and ESP32 are developed and built separately, but they are intended to work together as one integrated robot system over UART. In the current iteration, the Uno is responsible for autonomous movement and sensor-based path decisions, while the ESP32 is responsible for external connectivity and user-facing status display.

The four stepper motors are also stand-in for four DC Motors, Stepper motors are used due to DC Motors being an unavailable part in Wokwi

The code currently assumes that the communication link between the Uno and ESP32 is part of the full system design and should be treated as the normal operating path in physical hardware.