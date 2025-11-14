// RGB LED Control with 2 Potentiometers
// Controls Red and Green colors

// Define pins
const int redPin = 9;      // Red LED connected to pin 9
const int greenPin = 10;   // Green LED connected to pin 10
const int potRed = A0;     // Potentiometer for Red connected to A0
const int potGreen = A1;   // Potentiometer for Green connected to A1

void setup() {
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  // Read potentiometer values (0-1023)
  int redValue = analogRead(potRed);
  int greenValue = analogRead(potGreen);
  
  // Map potentiometer values to LED brightness (0-255)
  int redBrightness = map(redValue, 0, 1023, 0, 255);
  int greenBrightness = map(greenValue, 0, 1023, 0, 255);
  
  // Set LED brightness
  analogWrite(redPin, redBrightness);
  analogWrite(greenPin, greenBrightness);
  
  Serial.print("Red: ");
  Serial.print(redBrightness);
  Serial.print(" | Green: ");
  Serial.println(greenBrightness);
  
  delay(10);
}
