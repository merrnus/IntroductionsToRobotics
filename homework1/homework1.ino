// RGB LED Control with 3 Potentiometers

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

const int potRed = A0;
const int potGreen = A1;
const int potBlue = A2;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  int redValue = analogRead(potRed);
  int greenValue = analogRead(potGreen);
  int blueValue = analogRead(potBlue);
  
  int redBrightness = map(redValue, 0, 1023, 0, 255);
  int greenBrightness = map(greenValue, 0, 1023, 0, 255);
  int blueBrightness = map(blueValue, 0, 1023, 0, 255);
  
  analogWrite(redPin, redBrightness);
  analogWrite(greenPin, greenBrightness);
  analogWrite(bluePin, blueBrightness);
  
  Serial.print("R: ");
  Serial.print(redBrightness);
  Serial.print(" | G: ");
  Serial.print(greenBrightness);
  Serial.print(" | B: ");
  Serial.println(blueBrightness);
  
  delay(100);
}
