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

## Circuit Setup

### Connections:

**Pedestrian LEDs:**
- Red LED: Long leg → 1kΩ resistor → Pin 13, Short leg → GND
- Green LED: Long leg → 1kΩ resistor → Pin 12, Short leg → GND

**Car LEDs:**
- Red LED: Long leg → 1kΩ resistor → Pin 11, Short leg → GND
- Yellow LED: Long leg → 1kΩ resistor → Pin 10, Short leg → GND
- Green LED: Long leg → 1kΩ resistor → Pin 9, Short leg → GND

**Push Button:**
- One side → Pin 2
- Same side → 10kΩ resistor → 5V
- Other side → GND

**Buzzer:**
- Positive (+) → Pin 8
- Negative (-) → GND

**7-Segment Display (Common Anode):**
- Segment A → 220Ω resistor → Pin 7
- Segment B → 220Ω resistor → Pin 6
- Segment C → 220Ω resistor → Pin 5
- Segment D → 220Ω resistor → Pin 4
- Segment E → 220Ω resistor → Pin 3
- Segment F → 220Ω resistor → Pin A0
- Segment G → 220Ω resistor → Pin A1
- Common pin → 5V

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

## Resources
Used Claude AI for understanding state machines, interrupts, and millis() timing.

Atamuradov Muhammet - 361          
November 2024
