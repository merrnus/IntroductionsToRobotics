/*
 * ENDLESS RUNNER GAME - Homework 5
 * 
 * A simple endless runner where you jump over obstacles.
 * - Player runs automatically
 * - Jump with joystick UP
 * - Avoid fire, collect cups
 * - Game speeds up over time
 * - Top 3 scores saved in EEPROM
 */

#include <LiquidCrystal.h>
#include <EEPROM.h>

// ==================== PIN TANIMLAMALARI ====================
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int joyY = A0;
const int joySW = 6;
const int buzzer = 9;

// ==================== CUSTOM CHARACTERS ====================
byte playerChar[8] = {
  B01110,
  B01110,
  B00100,
  B01110,
  B10101,
  B00100,
  B01010,
  B10001
};

byte cupChar[8] = {
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100
};

byte fireChar[8] = {
  B00100,
  B01010,
  B10101,
  B10101,
  B11111,
  B11111,
  B11111,
  B01110
};

byte groundChar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

const int PLAYER = 0;
const int CUP = 1;
const int FIRE = 2;
const int GROUND = 3;

// ==================== GAME CONSTANTS ====================
const int WORLD_SIZE = 200;
const int SCREEN_WIDTH = 16;
const int PLAYER_COL = 3;
const int JUMP_HOLD = 3;

// ==================== GAME VARIABLES ====================
// All the stuff the game needs to track
enum State {
  MENU,
  PLAYING,
  GAME_OVER
};

State state = MENU;
int menuOption = 0;

// World
uint8_t world[2][WORLD_SIZE];
int scrollPos = 0;
int worldGenPos = 16;

// Player
int playerY = 1;
bool isJumping = false;
int jumpTimer = 0;

// Game
int score = 0;
int highScores[3] = {0, 0, 0};
unsigned long gameStartTime = 0;
unsigned long lastUpdate = 0;
int updateInterval = 200;

// Input
unsigned long lastInputTime = 0;

// Fire spacing
int lastGroundFire = -10;
int lastAirFire = -10;

// Game over
bool gameOverRendered = false;

// ==================== SETUP ====================
void setup() {
  lcd.begin(16, 2);
  pinMode(joySW, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);

  lcd.createChar(PLAYER, playerChar);
  lcd.createChar(CUP, cupChar);
  lcd.createChar(FIRE, fireChar);
  lcd.createChar(GROUND, groundChar);

  randomSeed(analogRead(A1));
  loadHighScores();

  lcd.setCursor(0, 0);
  lcd.print("ENDLESS RUNNER");
  lcd.setCursor(0, 1);
  lcd.print("Press to start");
  delay(2000);

  showMenu();
}

// ==================== MAIN LOOP ====================
void loop() {
  switch(state) {
    case MENU: 
      handleMenu(); 
      break;
      
    case PLAYING:
      if(millis() - lastUpdate >= updateInterval) {
        updateGame();
        lastUpdate = millis();
      }
      break;
      
    case GAME_OVER: 
      handleGameOver(); 
      break;
  }
}

// ==================== MENU ====================
void showMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuOption == 0 ? ">START" : " START");
  lcd.setCursor(0, 1);
  lcd.print(menuOption == 1 ? ">HISCORE" : " HISCORE");
}

void handleMenu() {
  if(millis() - lastInputTime < 200) return;

  int y = 1023 - analogRead(joyY);
  bool btn = (digitalRead(joySW) == LOW);

  if(y > 700 || y < 300) {
    menuOption = 1 - menuOption;
    showMenu();
    tick();
    lastInputTime = millis();
  }

  if(btn) {
    if(menuOption == 0) startGame();
    else showHighScores();
    lastInputTime = millis();
  }
}

void showHighScores() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TOP:");
  lcd.print(highScores[0]);
  lcd.print(" 2:");
  lcd.print(highScores[1]);
  lcd.setCursor(0, 1);
  lcd.print("3rd:");
  lcd.print(highScores[2]);
  delay(3000);
  showMenu();
}

// ==================== GAME INIT ====================
void startGame() {
  for(int i = 0; i < WORLD_SIZE; i++) {
    world[0][i] = 255;
    world[1][i] = GROUND;
  }

  scrollPos = 0;
  worldGenPos = 16;

  playerY = 1;
  isJumping = false;
  jumpTimer = 0;

  score = 0;
  gameStartTime = millis();
  lastUpdate = millis();
  updateInterval = 200;

  lastGroundFire = -10;
  lastAirFire = -10;
  gameOverRendered = false;

  for(int i = 0; i < 5; i++) {
    generateWorld();
  }

  state = PLAYING;
  lcd.clear();
}

// ==================== GAME LOGIC ====================
// Where all the game mechanics happen
void updateGame() {
  int y = 1023 - analogRead(joyY);
  if(y > 700 && !isJumping && playerY == 1) {
    jump();
  }

  updatePhysics();

  scrollPos++;
  
  // CRITICAL FIX: Check world generation before overflow!
  if(scrollPos >= worldGenPos - 48) {
    generateWorld();
  }

  if(checkCollision()) {
    gameOverSound();
    state = GAME_OVER;
    return;
  }

  score = (millis() - gameStartTime) / 100;

  if(score > 100 && updateInterval > 100) {
    updateInterval = 150;
  } else if(score > 200 && updateInterval > 80) {
    updateInterval = 100;
  }

  renderGame();
}

void jump() {
  isJumping = true;
  jumpTimer = JUMP_HOLD;
  beep();
}

void updatePhysics() {
  if(isJumping) {
    if(jumpTimer > 0) {
      playerY = 0;
      jumpTimer--;
    } else {
      playerY = 1;
      isJumping = false;
    }
  }
}

bool checkCollision() {
  int worldCol = scrollPos + PLAYER_COL;
  if(worldCol >= WORLD_SIZE) return false;

  uint8_t cell = world[playerY][worldCol];

  if(cell == FIRE) return true;

  if(cell == CUP) {
    score += 10;
    world[playerY][worldCol] = 255;
    coinSound();
  }

  return false;
}

// ==================== WORLD GENERATION ====================
// Creates obstacles randomly (with spacing so they're always beatable!)
void generateWorld() {
  int start = worldGenPos;
  
  if(start + 16 >= WORLD_SIZE) {
    shiftWorldLeft();
    start = worldGenPos;
  }
  
  // Clear
  for(int i = 0; i < 16; i++) {
    int idx = start + i;
    if(idx >= WORLD_SIZE) break;
    world[0][idx] = 255;
    world[1][idx] = GROUND;
  }
  
  // Track ALL fire positions
  int firePositions[32];  // Max 16 ground + 16 air
  int fireCount = 0;
  
  // Generate ground fires
  int lastGF = -10;
  for(int i = 0; i < 16; i++) {
    if(random(100) < 8 && (i - lastGF > 4)) {
      int idx = start + i;
      if(idx < WORLD_SIZE) {
        world[1][idx] = FIRE;
        firePositions[fireCount++] = i;  // Store position
        lastGF = i;
      }
    }
  }
  
  // Generate air fires - CHECK ALL EXISTING FIRES!
  int lastAF = -10;
  for(int i = 0; i < 16; i++) {
    int idx = start + i;
    if(idx >= WORLD_SIZE) break;
    
    // Check if ANY fire is within 3 positions
    bool tooClose = false;
    for(int f = 0; f < fireCount; f++) {
      int firePos = firePositions[f];
      if(abs(i - firePos) <= 3) {  // Within 3 positions?
        tooClose = true;
        break;
      }
    }
    
    if(tooClose) {
      // Only cup allowed
      if(random(100) < 30) {
        world[0][idx] = CUP;
      }
      continue;
    }
    
    // Safe to place air fire
    int r = random(100);
    if(r < 6 && (i - lastAF > 5)) {
      world[0][idx] = FIRE;
      firePositions[fireCount++] = i;  // Add to fire list
      lastAF = i;
    } else if(r < 30) {
      world[0][idx] = CUP;
    }
  }
  
  worldGenPos += 16;
  
  Serial.print("Gen ");
  Serial.print(start);
  Serial.print(" Fires: ");
  Serial.println(fireCount);
}

// ==================== RENDERER ====================
// Display-only functions - no game logic
void renderGame() {
  // Top row
  lcd.setCursor(0, 0);
  for(int i = 0; i < 16; i++) {
    int worldCol = scrollPos + i;

    if(i == PLAYER_COL && playerY == 0) {
      lcd.write(PLAYER);
    } else if(worldCol >= 0 && worldCol < WORLD_SIZE) {
      uint8_t airCell = world[0][worldCol];
      uint8_t groundCell = world[1][worldCol];
      
      // EMERGENCY FIX: If both fire, clear air
      if(airCell == FIRE && groundCell == FIRE) {
        Serial.print("RENDER: Double fire at ");
        Serial.println(worldCol);
        world[0][worldCol] = 255;
        airCell = 255;
        tone(buzzer, 3000, 50);  // Beep to alert!
      }
      
      if(airCell != 255) {
        lcd.write(airCell);
      } else {
        lcd.print(" ");
      }
    } else {
      lcd.print(" ");
    }
  }

  // Bottom row (unchanged)
  lcd.setCursor(0, 1);
  for(int i = 0; i < 16; i++) {
    int worldCol = scrollPos + i;
    if(i == PLAYER_COL && playerY == 1) {
      lcd.write(PLAYER);
    } else if(worldCol >= 0 && worldCol < WORLD_SIZE) {
      lcd.write(world[1][worldCol]);
    } else {
      lcd.write(GROUND);
    }
  }

  // Score
  lcd.setCursor(12, 0);
  if(score < 1000) lcd.print(" ");
  if(score < 100) lcd.print(" ");
  if(score < 10) lcd.print(" ");
  lcd.print(score);
}

// ==================== WORLD RECYCLING ====================
// Moves everything back when we run out of array space
void shiftWorldLeft() {
  int shiftAmount = 100;
  
  Serial.print("Shifting world! scrollPos=");
  Serial.print(scrollPos);
  Serial.print(" worldGenPos=");
  Serial.println(worldGenPos);
  
  // Move everything left
  for(int i = 0; i < WORLD_SIZE - shiftAmount; i++) {
    world[0][i] = world[0][i + shiftAmount];
    world[1][i] = world[1][i + shiftAmount];
  }
  
  // Clear the end
  for(int i = WORLD_SIZE - shiftAmount; i < WORLD_SIZE; i++) {
    world[0][i] = 255;
    world[1][i] = GROUND;
  }
  
  // Update positions
  scrollPos -= shiftAmount;
  worldGenPos -= shiftAmount;
  
  // Update fire tracking
  if(lastGroundFire >= shiftAmount) {
    lastGroundFire -= shiftAmount;
  } else {
    lastGroundFire = -10;
  }
  
  if(lastAirFire >= shiftAmount) {
    lastAirFire -= shiftAmount;
  } else {
    lastAirFire = -10;
  }
  
  // Safety
  if(scrollPos < 0) scrollPos = 0;
  if(worldGenPos < 16) worldGenPos = 16;
}


// ==================== GAME OVER ====================
void handleGameOver() {
  static bool scoreSaved = false;
  static unsigned long gameOverStartTime = 0;
  
  if(!gameOverRendered) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GAME OVER!");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    
    if(!scoreSaved) {
      saveHighScore();
      scoreSaved = true;
    }
    
    gameOverRendered = true;
    gameOverStartTime = millis();
    lastInputTime = millis();
    return;
  }
  
  if(millis() - gameOverStartTime < 2000) {
    return;
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Press for menu  ");
  
  if(millis() - lastInputTime > 300) {
    if(digitalRead(joySW) == LOW) {
      gameOverRendered = false;
      scoreSaved = false;
      state = MENU;
      
      delay(200);
      showMenu();
      lastInputTime = millis();
    }
  }
}

// ==================== EEPROM ====================
void loadHighScores() {
  for(int i = 0; i < 3; i++) {
    int addr = i * 2;
    highScores[i] = (EEPROM.read(addr) << 8) | EEPROM.read(addr + 1);
    if(highScores[i] > 9999) highScores[i] = 0;
  }
}

void saveHighScore() {
  for(int i = 0; i < 3; i++) {
    if(score > highScores[i]) {
      for(int j = 2; j > i; j--) {
        highScores[j] = highScores[j-1];
      }
      highScores[i] = score;

      for(int k = 0; k < 3; k++) {
        int addr = k * 2;
        EEPROM.write(addr, highScores[k] >> 8);
        EEPROM.write(addr + 1, highScores[k] & 0xFF);
      }
      break;
    }
  }
}

// ==================== SOUNDS ====================
void tick() {
  tone(buzzer, 2000, 20);
}

void beep() {
  digitalWrite(buzzer, HIGH);
  delay(30);
  digitalWrite(buzzer, LOW);
}

void coinSound() {
  tone(buzzer, 1500, 80);
}

void gameOverSound() {
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(buzzer, LOW);
}
