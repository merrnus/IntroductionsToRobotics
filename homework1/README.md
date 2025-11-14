# IntroductionsToRobotics

# Homework #1 - RGB LED Color Control with Potentiometers

## Description
This project controls an RGB LED using two potentiometers (rotary knobs). By turning the knobs, you can control the Red and Green color channels independently, creating various color combinations including red, green, and yellow/orange tones.

**Note:** This implementation uses 2 potentiometers instead of 3 due to hardware availability. The blue channel is not connected.

## Components Used
- 1x Arduino UNO (Robotlinking UNO R3)
- 1x RGB LED (Common Cathode)
- 2x Potentiometers (10kΩ)
- 2x Resistors (1kΩ - Brown, Black, Red, Gold)
- 1x Breadboard
- Jumper wires (Male-to-Male)

## Circuit Setup

### Connections:
**RGB LED:**
- Red leg → 1kΩ resistor → Arduino Pin 9
- Long leg (Common Cathode) → GND
- Green leg → 1kΩ resistor → Arduino Pin 10
- Blue leg → Not connected

**Potentiometer 1 (Red Control):**
- Left pin → GND
- Middle pin → Arduino A0
- Right pin → 5V

**Potentiometer 2 (Green Control):**
- Left pin → GND
- Middle pin → Arduino A1
- Right pin → 5V

### Circuit Photo:
[Circuit Setup]( )

## How It Works

The potentiometers act as variable voltage dividers. When you turn a knob:
1. The Arduino reads an analog value (0-1023) from pins A0 and A1
2. The code maps this value to LED brightness (0-255)
3. PWM (Pulse Width Modulation) on pins 9 and 10 controls the LED intensity
4. Mixing red and green at different intensities creates various colors

**Color Results:**
- Red knob only → Pure red
- Green knob only → Light green
- Both knobs → Yellow/orange (red + green mix)

## Code Explanation

The Arduino sketch performs these steps:
1. **Setup:** Configures pins 9 and 10 as outputs for LED control
2. **Loop:** 
   - Reads potentiometer values using `analogRead()`
   - Maps values from 0-1023 range to 0-255 using `map()` function
   - Sends PWM signals to LED using `analogWrite()`
   - Updates continuously with small delay

## Challenges & Learning

**Challenge:** Only had 2 potentiometers available instead of 3.

**Solution:** Focused on Red and Green channels, which still demonstrates the core concept of analog input controlling PWM output. This limitation actually simplified the learning process while maintaining the fundamental principles.

**Key Learnings:**
- Understanding analog vs digital signals
- PWM for controlling LED brightness
- Mapping value ranges
- Basic circuit construction with breadboards

## Video Demo
[Watch the project in action]( )


## Future Improvements
- Add third potentiometer for full RGB control
- Implement preset color buttons
- Add LCD display to show current RGB values
- Create color mixing animations

## Resources
Used Claude AI for learning Arduino basics and circuit debugging.


**Course:** Introduction to Robotics  
**Date:** 06/11/2025
**Student:** Muhammet Atamuradov
