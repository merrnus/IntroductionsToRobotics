// Traffic Light System
// Homework 2

const int pedRed = 13;
const int pedGreen = 12;
const int carRed = 11;
const int carYellow = 10;
const int carGreen = 9;

const int buttonPin = 2;
const int buzzerPin = 8;

const int segA = 7;
const int segB = 6;
const int segC = 5;
const int segD = 4;
const int segE = 3;
const int segF = A0;
const int segG = A1;

const int STATE_IDLE = 1;
const int STATE_GREEN_WAIT = 2;
const int STATE_TRANSITION = 3;
const int STATE_PED_GO = 4;
const int STATE_WARNING = 5;

volatile int currentState = STATE_IDLE;
volatile bool buttonPressed = false;

unsigned long stateStart = 0;
unsigned long lastBlink = 0;
unsigned long lastBuzzer = 0;

bool blinkOn = false;
bool buzzerOn = false;

// patterns for digits (inverted because common anode)
const bool digits[10][7] = {
  {0,0,0,0,0,0,1},
  {1,0,0,1,1,1,1},
  {0,0,1,0,0,1,0},
  {0,0,0,0,1,1,0},
  {1,0,0,1,1,0,0},
  {0,1,0,0,1,0,0},
  {0,1,0,0,0,0,0},
  {0,0,0,1,1,1,1},
  {0,0,0,0,0,0,0},
  {0,0,0,0,1,0,0}
};

void setup() {
  pinMode(pedRed, OUTPUT);
  pinMode(pedGreen, OUTPUT);
  pinMode(carRed, OUTPUT);
  pinMode(carYellow, OUTPUT);
  pinMode(carGreen, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
  
  Serial.begin(9600);
  
  currentState = STATE_IDLE;
  stateStart = millis();
  updateLights();
}

void loop() {
  unsigned long now = millis();
  
  switch(currentState) {
    case STATE_IDLE:
      if (buttonPressed) {
        buttonPressed = false;
        changeState(STATE_GREEN_WAIT);
      }
      showNumber(0);
      break;
    
    case STATE_GREEN_WAIT:
      showCountdown(8000, now);
      if (now - stateStart >= 8000) {
        changeState(STATE_TRANSITION);
      }
      break;
      
    case STATE_TRANSITION:
      showCountdown(3000, now);
      if (now - stateStart >= 3000) {
        changeState(STATE_PED_GO);
      }
      break;
      
    case STATE_PED_GO:
      showCountdown(8000, now);
      
      if (now - lastBuzzer >= 500) {
        buzzerOn = !buzzerOn;
        digitalWrite(buzzerPin, buzzerOn);
        lastBuzzer = now;
      }
      
      if (now - stateStart >= 8000) {
        changeState(STATE_WARNING);
      }
      break;
      
    case STATE_WARNING:
      showCountdown(4000, now);
      
      if (now - lastBlink >= 250) {
        blinkOn = !blinkOn;
        digitalWrite(pedGreen, blinkOn);
        lastBlink = now;
      }
      
      if (now - lastBuzzer >= 250) {
        buzzerOn = !buzzerOn;
        digitalWrite(buzzerPin, buzzerOn);
        lastBuzzer = now;
      }
      
      if (now - stateStart >= 4000) {
        changeState(STATE_IDLE);
      }
      break;
  }
}

void buttonISR() {
  if (currentState == STATE_IDLE) {
    buttonPressed = true;
  }
}

void changeState(int newState) {
  currentState = newState;
  stateStart = millis();
  lastBlink = millis();
  lastBuzzer = millis();
  updateLights();
}

void updateLights() {
  digitalWrite(pedRed, LOW);
  digitalWrite(pedGreen, LOW);
  digitalWrite(carRed, LOW);
  digitalWrite(carYellow, LOW);
  digitalWrite(carGreen, LOW);
  digitalWrite(buzzerPin, LOW);
  
  blinkOn = false;
  buzzerOn = false;
  
  if (currentState == STATE_IDLE) {
    digitalWrite(carGreen, HIGH);
    digitalWrite(pedRed, HIGH);
  }
  else if (currentState == STATE_GREEN_WAIT) {
    digitalWrite(carGreen, HIGH);
    digitalWrite(pedRed, HIGH);
  }
  else if (currentState == STATE_TRANSITION) {
    digitalWrite(carYellow, HIGH);
    digitalWrite(pedRed, HIGH);
  }
  else if (currentState == STATE_PED_GO) {
    digitalWrite(carRed, HIGH);
    digitalWrite(pedGreen, HIGH);
  }
  else if (currentState == STATE_WARNING) {
    digitalWrite(carRed, HIGH);
  }
}

void showCountdown(unsigned long duration, unsigned long now) {
  unsigned long elapsed = now - stateStart;
  unsigned long remaining = duration - elapsed;
  int seconds = (remaining + 999) / 1000;
  
  if (seconds < 0) seconds = 0;
  if (seconds > 9) seconds = 9;
  
  showNumber(seconds);
}

void showNumber(int num) {
  if (num < 0 || num > 9) {
    digitalWrite(segA, HIGH);
    digitalWrite(segB, HIGH);
    digitalWrite(segC, HIGH);
    digitalWrite(segD, HIGH);
    digitalWrite(segE, HIGH);
    digitalWrite(segF, HIGH);
    digitalWrite(segG, HIGH);
    return;
  }
  
  digitalWrite(segA, digits[num][0]);
  digitalWrite(segB, digits[num][1]);
  digitalWrite(segC, digits[num][2]);
  digitalWrite(segD, digits[num][3]);
  digitalWrite(segE, digits[num][4]);
  digitalWrite(segF, digits[num][5]);
  digitalWrite(segG, digits[num][6]);
}
