# Group2 Robotics - Communication Testing

## Overview
This project focuses on UART serial communication between Arduino Uno and ESP32 for robotics and IoT integration.

The Arduino Uno sends robot status data to the ESP32 using UART communication.  
The ESP32 receives the message, validates the format, and displays the communication log through the Serial Monitor.

---

## Task Objectives

- UART/serial communication between Arduino Uno and ESP32
- Test communication message format
- Debug using Serial Monitor/log
- Ensure stable communication between both boards

---

## Hardware Used

- Arduino Uno
- ESP32
- Jumper wires
- UART serial connection

---

## UART Wiring

| Arduino Uno | ESP32 |
|-------------|--------|
| TX          | RX (GPIO16) |
| RX          | TX (GPIO17) |
| GND         | GND |

---

## Message Format

Example communication message:

```text
DIST=18,MODE=AUTO,TURN=LEFT

description
| Parameter | Description                           |
| --------- | ------------------------------------- |
| DIST      | Distance value from ultrasonic sensor |
| MODE      | Robot operating mode                  |
| TURN      | Robot movement direction              |


message example
DIST=10,MODE=AUTO,TURN=STOP
DIST=18,MODE=AUTO,TURN=LEFT
DIST=35,MODE=AUTO,TURN=FORWARD

ESP32 Receiver Features
Receive UART serial data from Arduino Uno
Validate message format
Display communication logs
Send ACK response back to Arduino Uno


Serial Monitor Example
Arduino Uno
Sent to ESP32: DIST=18,MODE=AUTO,TURN=LEFT
ESP32 Response: ACK


ESP32
ESP32 UART Communication Started
Received from Arduino Uno: DIST=18,MODE=AUTO,TURN=LEFT
Message format valid


Communication Testing Result

The UART communication between Arduino Uno and ESP32 works successfully.

The ESP32 is able to:

receive serial data correctly
validate the message format
display debugging logs
send acknowledgment responses

Communication remains stable during repeated testing.


Project Structure
Group2-Robotics/
│
├── docs/
│   ├── communication-testing.md
│   └── uno-sender-code.md
│
├── src/
│   └── main.cpp
│
├── diagram.json
├── platformio.ini
├── wokwi.toml
└── README.md
