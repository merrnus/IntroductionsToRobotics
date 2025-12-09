// H4 - SIMON SAYS

const int dataPin = 11;
const int latchPin = 12;
const int clockPin = 8;
const int digit1Pin = 4;
const int digit2Pin = 5;
const int digit3Pin = 6;
const int digit4Pin = 7;

const int joyX = A0;
const int joyY = A1;
const int joyButton = 2;
const int pauseButton = 3;
const int buzzerPin = 9;
const int seedAnalog = A3; // for randomSeed

const byte FONT[] = {
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00000111, // 7
  B01111111, // 8
  B01101111, // 9
  B01110111, // A (10)
  B01111100, // b (11)
  B00111001, // C (12)
  B01011110, // d (13)
  B01111001, // E (14)
  B01110001, // F (15)
  B01110110, // H (16)
  B00000110, // I (17)
  B00111000, // L (18)
  B01110011, // P (19)
  B00111110, // U (20)
  B01101111, // g (21)
  B01101101, // S (22)
  B01111000, // t (23)
  B01011100, // o (24)
  B01100010, // y (25)
  B00000000  // blank (26) 
};

// 26 characters total (indexes 0..25)
const char CHARS[] = "0123456789AbCdEFHILPUgStoy ";

enum State { MENU, START_GAME, SHOW_SEQ, INPUT_PHASE, CHECK_ANSWER };
State state = MENU;
int menuOption = 0;

byte sequence[4];
byte answer[4];
int score = 0;
int highScore = 0;
int seqTime = 5000;

int cursor = 0;
bool locked = false;
unsigned long seqStart = 0;
unsigned long lastJoyRead = 0;
unsigned long btnPress = 0;
bool btnDown = false;

byte display[4] = {25, 25, 25, 25};
int activeDigit = 0;
unsigned long lastBlink = 0;
bool blinkOn = true;

int lastMenuXState = 0; // -1 left, 0 center, +1 right
int lastCursorXState = 0;
unsigned long lastPauseDebounce = 0;
const unsigned long pauseDebounceMs = 250;

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(digit1Pin, OUTPUT);
  pinMode(digit2Pin, OUTPUT);
  pinMode(digit3Pin, OUTPUT);
  pinMode(digit4Pin, OUTPUT);
  
  pinMode(joyButton, INPUT_PULLUP);
  pinMode(pauseButton, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("=== SIMON SAYS ===");
  randomSeed(analogRead(seedAnalog));
  
  showMenu();
}

void loop() {
  updateDisplay();
  readInput();
  gameLogic();
}

void updateDisplay() {
  static unsigned long lastSwitch = 0;
  if (millis() - lastSwitch >= 3) { // faster multiplexing -> smoother
    lastSwitch = millis();
    
    // turn off all digits (common-anode/common-cathode specifics: assumed active LOW)
    digitalWrite(digit1Pin, HIGH);
    digitalWrite(digit2Pin, HIGH);
    digitalWrite(digit3Pin, HIGH);
    digitalWrite(digit4Pin, HIGH);
    
    byte pattern = FONT[display[activeDigit]];
    
    // blinking for cursor during input
    if (state == INPUT_PHASE && activeDigit == cursor) {
      int blinkSpeed = locked ? 500 : 125;
      if (millis() - lastBlink >= blinkSpeed) {
        lastBlink = millis();
        blinkOn = !blinkOn;
      }
      if (!blinkOn) pattern = B11111111; // blank
    }
    
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, pattern);
    digitalWrite(latchPin, HIGH);
    
    switch(activeDigit) {
      case 0: digitalWrite(digit1Pin, LOW); break;
      case 1: digitalWrite(digit2Pin, LOW); break;
      case 2: digitalWrite(digit3Pin, LOW); break;
      case 3: digitalWrite(digit4Pin, LOW); break;
    }
    
    activeDigit = (activeDigit + 1) % 4;
  }
}

void setDisplay(byte b0, byte b1, byte b2, byte b3) {
  display[0] = b0;
  display[1] = b1;
  display[2] = b2;
  display[3] = b3;
}

void showChars(char c0, char c1, char c2, char c3) {
  display[0] = getIndex(c0);
  display[1] = getIndex(c1);
  display[2] = getIndex(c2);
  display[3] = getIndex(c3);
}

int getIndex(char c) {
  // CHARS length = 26 -> indexes 0..25
  for (int i = 0; i < 26; i++) {
    if (CHARS[i] == c) return i;
  }
  return 25; // blank
}

void showNumber(int n) {
  if (n > 9999) n = 9999;
  if (n < 0) n = 0;
  display[0] = (n / 1000) % 10;
  display[1] = (n / 100) % 10;
  display[2] = (n / 10) % 10;
  display[3] = n % 10;
}

void readInput() {
  // limit joystick reading frequency
  if (millis() - lastJoyRead < 120) return;
  lastJoyRead = millis();
  
  int x = analogRead(joyX);
  int y = analogRead(joyY);
  bool btn = (digitalRead(joyButton) == LOW);
  
  // joystick button handling (short/long press)
  if (btn && !btnDown) {
    btnDown = true;
    btnPress = millis();
    beep(30);
  } else if (!btn && btnDown) {
    unsigned long dur = millis() - btnPress;
    if (dur < 800) shortPress();
    else longPress();
    btnDown = false;
  }
  
  // MENU X handling with edge detection to avoid flicker
  if (state == MENU) {
    int xState = 0;
    if (x < 300) xState = -1;
    else if (x > 700) xState = 1;
    else xState = 0;
    
    if (xState != lastMenuXState) {
      // only react on edge from center to left/right (prevents chattering when held)
      if (lastMenuXState == 0 && xState != 0) {
        menuOption = 1 - menuOption;
        showMenu();
        tick();
      }
      lastMenuXState = xState;
    }
  }
  
  // INPUT_PHASE cursor movement (only when unlocked) - edge detection
  if (state == INPUT_PHASE && !locked) {
    int xState = 0;
    if (x < 300) xState = -1;
    else if (x > 700) xState = 1;
    else xState = 0;
    
    if (xState != lastCursorXState) {
      if (lastCursorXState == 0 && xState == -1) {
        cursor = (cursor + 3) % 4;
        tick();
      } else if (lastCursorXState == 0 && xState == 1) {
        cursor = (cursor + 1) % 4;
        tick();
      }
      lastCursorXState = xState;
    }
  }
  
  // When locked: Y changes the character at cursor (wraps over 26 symbols)
  if (state == INPUT_PHASE && locked) {
    if (y < 300) {
      // up: previous symbol
      answer[cursor] = (answer[cursor] + 25) % 26; // -1 mod 26
      display[cursor] = answer[cursor];
      tick();
    } else if (y > 700) {
      // down: next symbol
      answer[cursor] = (answer[cursor] + 1) % 26;
      display[cursor] = answer[cursor];
      tick();
    }
  }
  
  // pause button with simple debounce (go to menu)
  if (digitalRead(pauseButton) == LOW) {
    if (millis() - lastPauseDebounce > pauseDebounceMs) {
      lastPauseDebounce = millis();
      if (state != MENU) {
        showChars('P', 'A', 'U', 'S');
        waitWithUpdates(1000);
        state = MENU;
        showMenu();
      }
    }
  }
}

void shortPress() {
  if (state == MENU) {
    selectMenu();
  } else if (state == INPUT_PHASE) {
    locked = !locked;
    Serial.println(locked ? "Lock" : "Unlock");
  }
}

void longPress() {
  if (state == INPUT_PHASE) {
    Serial.println("Submit!");
    state = CHECK_ANSWER;
  }
}

void gameLogic() {
  switch(state) {
    case MENU:
      // nothing to do here (menu shown by showMenu)
      break;
      
    case START_GAME:
      for (int i = 0; i < 4; i++) {
        sequence[i] = random(0, 26); // include 0..25
      }
      setDisplay(sequence[0], sequence[1], sequence[2], sequence[3]);
      seqStart = millis();
      state = SHOW_SEQ;
      
      Serial.print("Memorize: ");
      for (int i = 0; i < 4; i++) Serial.print(CHARS[sequence[i]]);
      Serial.println();
      break;
      
    case SHOW_SEQ:
      if (millis() - seqStart >= seqTime) {
        startInputPhase();
      }
      break;
      
    case INPUT_PHASE:
      // waiting for player actions
      break;
      
    case CHECK_ANSWER:
      checkAnswer();
      break;
  }
}

void showMenu() {
  if (menuOption == 0) {
    showChars('P', 'L', 'A', '4');
  } else {
    showChars('H', '0', 'L', 'A');
  }
}

void selectMenu() {
  if (menuOption == 0) {
    Serial.println("Start!");
    score = 0;
    seqTime = 5000;
    state = START_GAME;
  } else {
    showNumber(highScore);
    waitWithUpdates(2000);
    showMenu();
  }
}

void startInputPhase() {
  Serial.println("Input!");
  for (int i = 0; i < 4; i++) answer[i] = 25; // blank
  cursor = 0;
  locked = false;

  setDisplay(25, 25, 25, 25);
  state = INPUT_PHASE;
}

void checkAnswer() {
  bool correct = true;
  for (int i = 0; i < 4; i++) {
    if (sequence[i] != answer[i]) {
      correct = false;
      break;
    }
  }
  
  if (correct) {
    Serial.println("CORRECT!");
    score++;
    successSound();
    showNumber(score);
    waitWithUpdates(1500);
    
    if (seqTime > 2000) seqTime -= 500;
    state = START_GAME;
  } else {
    Serial.println("WRONG!");
    errorSound();
    showChars('E', ' ', ' ', ' ');
    waitWithUpdates(1000);
    showNumber(score);
    waitWithUpdates(2000);
    
    if (score > highScore) highScore = score;
    
    state = MENU;
    showMenu();
  }
}

// Helper that keeps display & input active while waiting
void waitWithUpdates(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    updateDisplay();
    // Allow some input polling during waits (but throttle to not starve CPU)
    readInput();
    // small yield to avoid busy spinning too hard
    delay(1);
  }
}

void beep(int ms) {
  tone(buzzerPin, 1000, ms);
  // keep display responsive while tone plays
  waitWithUpdates(ms);
}

void tick() {
  tone(buzzerPin, 2000, 20);
  waitWithUpdates(25);
}

void successSound() {
  tone(buzzerPin, 523, 100);
  waitWithUpdates(120);
  tone(buzzerPin, 659, 100);
  waitWithUpdates(120);
  tone(buzzerPin, 784, 150);
  waitWithUpdates(160);
}

void errorSound() {
  tone(buzzerPin, 200, 200);
  waitWithUpdates(250);
  tone(buzzerPin, 150, 300);
  waitWithUpdates(320);
}
