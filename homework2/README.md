# Homework 2 - Traffic Light

## Description
Traffic light system for pedestrian crossing. When you press the button, it goes through different states with countdown and sounds.

## Components
- Arduino UNO
- 5 LEDs (red, yellow, green for cars + red, green for pedestrians)
- Push button
- Active buzzer
- 7-segment display (1 digit)
- Resistors and wires

## Wiring
- Pedestrian LEDs: pins 13 (red), 12 (green)
- Car LEDs: pins 11 (red), 10 (yellow), 9 (green)
- Button: pin 2
- Buzzer: pin 8
- Display segments: pins 7,6,5,4,3,A0,A1 (common anode to 5V)

## How it works
After pressing button:
1. Cars stay green for 8 sec (countdown)
2. Yellow light for 3 sec
3. Pedestrians can cross - green for 8 sec with slow beeps
4. Warning - blinking green for 4 sec with fast beeps
5. Back to normal (cars green)

Used state machine pattern and millis() for timing. Button uses interrupt so it responds immediately but only works when cars are green.

## Problems solved
My display was common anode not cathode so I had to figure out how to invert the segment patterns (LOW=on instead of HIGH=on). Also took time to understand the 8 second delay requirement after button press.

## Video
(will add later, it has too many storage thats why i could not add for a moment)

Atamuradov Muhammet - 361          
November 2024
