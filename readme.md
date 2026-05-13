# Arduino Sensing (Obstacle Avoidance)

This module handles the eyes of our robot using an HC-SR04 ultrasonic sensor and a servo motor. We have implemented the logic to scan the environment and make basic turning decisions.

## Features
*   **Distance Measurement:** Continuously calculates the distance using the ultrasonic sensor.
*   **Environment Scanning:** When an obstacle is detected within our threshold (20 cm), the servo rotates to check the left (180°) and right (0°) sides.
*   **Decision Making:** Compares the distances and decides the best path (`LEFT`, `RIGHT`, `BACKWARD`, or `FORWARD`).

## Variables for Integration
The `printStatus()` function outputs our current status via UART in the format agreed upon: `DIST=...,MODE=...,TURN=...`. 
The `Integration` and `Motor` teams can parse this serial output or directly use the `turnCommand` variable when merging the code.

## Pin Configuration (Wokwi Testbed)
*   **TRIG_PIN:** 9
*   **ECHO_PIN:** 10
*   **SERVO_PIN:** 11