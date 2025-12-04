# Homework 1 - RGB LED Control

## Description
Control an RGB LED color by adjusting three potentiometers. Each potentiometer controls one color channel (red, green, or blue), allowing you to mix any color.

## Components
- 1x Arduino UNO
- 1x RGB LED (common cathode)
- 3x Potentiometers (10kΩ)
- 3x Resistors (1kΩ)
- Breadboard and jumper wires

## Circuit Wiring

**RGB LED (4 legs):**
- Leg 1 (Red) → 1kΩ resistor → Pin 9
- Leg 2 (Long - Common Cathode) → GND
- Leg 3 (Green) → 1kΩ resistor → Pin 10
- Leg 4 (Blue) → 1kΩ resistor → Pin 11

**Potentiometer 1 (Red control):**
- Left pin → GND
- Middle pin → Pin A0
- Right pin → 5V

**Potentiometer 2 (Green control):**
- Left pin → GND
- Middle pin → Pin A1
- Right pin → 5V

**Potentiometer 3 (Blue control):**
- Left pin → GND
- Middle pin → Pin A2
- Right pin → 5V

## How it works

The Arduino reads analog values from the three potentiometers (0-1023) and maps them to PWM brightness values (0-255). By turning the knobs, you control how much red, green, and blue light the LED produces, creating different colors.

**Color mixing examples:**
- Red only → pure red
- Green only → pure green
- Blue only → pure blue
- Red + Green → yellow
- Red + Blue → magenta/purple
- Green + Blue → cyan
- All three → white
- All off → no light

The code uses PWM (Pulse Width Modulation) on pins 9, 10, and 11 to control LED brightness. Higher values mean brighter colors.

## Problems encountered

At first I only had 2 potentiometers so could only control red and green. Found the third one later and added blue control for full RGB mixing.

Had to figure out which leg of the RGB LED was which color by testing. The long leg is the common cathode that goes to ground.

## Photo
![Circuit](circuit.jpg)

## Video
[Video demo link]

## Resources
Used Claude AI for understanding PWM and analogRead/analogWrite functions.

---
**Course:** Introduction to Robotics  
**Date:** November 2024
**Student**: Atamuradov Muhammet - 361
