# Communication + Testing

## Objective
This section tests UART serial communication between Arduino Uno and ESP32.

## Task
- UART/serial communication between Uno and ESP32
- Test message format
- Debug using serial monitor/log
- Make sure both boards communicate stably

## Message Format
The message format used in this project is:

DIST=18,MODE=AUTO,TURN=LEFT

## Message Meaning
- DIST: distance value from the ultrasonic sensor
- MODE: robot mode
- TURN: robot movement direction

## Example Messages
DIST=18,MODE=AUTO,TURN=LEFT
DIST=35,MODE=AUTO,TURN=FORWARD
DIST=10,MODE=AUTO,TURN=STOP

## UART Wiring
Arduino Uno TX is connected to ESP32 RX.  
Arduino Uno RX is connected to ESP32 TX.  
GND must be connected between both boards.

## Testing Result
The Arduino Uno sends data to the ESP32 through UART serial communication.  
The ESP32 receives the message, checks the format, and sends an ACK response back to the Uno.

## Serial Monitor Log Example

Arduino Uno log:

Sent to ESP32: DIST=18,MODE=AUTO,TURN=LEFT
ESP32 Response: ACK

ESP32 log:

ESP32 UART Communication Started
Received from Arduino Uno: DIST=18,MODE=AUTO,TURN=LEFT
Message format valid

## Debug Checklist
- Arduino Uno sends serial data
- ESP32 receives the data
- Message format is valid
- ESP32 sends ACK response
- Serial monitor shows clear logs
- Communication remains stable during repeated testing
