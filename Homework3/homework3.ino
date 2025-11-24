// H3 - Home Alarm System
// Pin definitions
const int trigPin = 6;
const int echoPin = 7;
const int ldrPin = A0;
const int buzzerPin = 8;
const int redLED = 9;
const int greenLED = 10;

// System variables
bool armed = false;
String password = "1234";
int sensitivity = 20;
int lightThreshold = 200;
int baselineDistance = 0;
String systemName = "Home Alarm";

// Timing
unsigned long lastAutoArmCheck = 0;  
unsigned long lastSensorCheck = 0;
unsigned long lastBuzzer = 0;
bool alarmActive = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  
  digitalWrite(greenLED, HIGH);
  
  Serial.println("================================");
  Serial.println(systemName);
  Serial.println("================================");
  
  calibrate();
  showMenu();
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    handleInput(input);
  }
  
  // Only check sensor if armed AND alarm NOT active
  if (armed && !alarmActive) {  // Added !alarmActive
    if (millis() - lastSensorCheck > 300) {
      lastSensorCheck = millis();
      checkSensor();
    } n
  }

    // Check for auto-arm every 3 seconds
  if (!armed && millis() - lastAutoArmCheck > 3000) {
    lastAutoArmCheck = millis();
    checkAutoArm();
  }
  
  // Handle alarm beeping
  if (alarmActive && millis() - lastBuzzer > 200) {
    lastBuzzer = millis();
    digitalWrite(buzzerPin, !digitalRead(buzzerPin));
  }
}

void checkAutoArm() {
  int light = analogRead(ldrPin);
  
  // Debug: show current light level
  Serial.print("Light level: ");
  Serial.println(light);
  
  if (light < lightThreshold) {
    Serial.println("\n>>> DARKNESS DETECTED <<<");
    Serial.println("Auto-arming system (Night Mode)...");
    armNow();
  }
}

void showMenu() {
  Serial.println("\n===== MENU =====");
  if (armed) {
    Serial.println("Status: ARMED");
    Serial.println("1. Disarm (password)");
  } else {
    Serial.println("Status: DISARMED");
    Serial.println("1. Arm System");
    Serial.println("2. Test Alarm");
    Serial.println("3. Settings");
  }
  Serial.println("================");
  Serial.print("Choice: ");
}

void handleInput(String input) {
  if (alarmActive) {
    if (input == password) {
      Serial.println("Correct! Disarmed.");
      stopAlarm();
    } else {
      Serial.println("Wrong password!");
    }
    return;
  }
  
  if (armed) {
    if (input == "1") {
      askPassword();
    }
  } else {
    if (input == "1") {
      armNow();
    } else if (input == "2") {
      testAlarm();
    } else if (input == "3") {
      settings();
    }
  }
}

void armNow() {
  Serial.println("\nArming in...");
  for (int i = 3; i > 0; i--) {
    Serial.println(i);
    delay(1000);
  }
  armed = true;
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, HIGH);
  Serial.println("ARMED!");

  Serial.println("Move away from sensor! (3 sec)");
  delay(3000);  // 5 second grace period
  Serial.println("Now monitoring!");

  showMenu();
}

void askPassword() {
  Serial.print("\nPassword: ");
  while (!Serial.available()) {}
  String pass = Serial.readStringUntil('\n');
  pass.trim();
  
  if (pass == password) {
    armed = false;
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    Serial.println("Disarmed!");
  } else {
    Serial.println("Wrong!");
  }
  showMenu();
}

void testAlarm() {
  Serial.println("Testing...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(redLED, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(redLED, LOW);
    delay(200);
  }
  digitalWrite(greenLED, HIGH);
  Serial.println("Done");
  showMenu();
}

void settings() {
  Serial.println("\n-- Settings --");
  Serial.println("1. Sensitivity: " + String(sensitivity));
  Serial.println("2. Light: " + String(lightThreshold));
  Serial.println("3. Password");
  Serial.println("4. Name");
  Serial.println("5. Back");
  Serial.print("Choice: ");
  
  while (!Serial.available()) {}
  String c = Serial.readStringUntil('\n');
  c.trim();
  
  if (c == "1") {
    Serial.print("New (10-50): ");
    while (!Serial.available()) {}
    sensitivity = Serial.readStringUntil('\n').toInt();
    Serial.println("Updated!");
  } else if (c == "2") {
    Serial.print("Current light: ");
    Serial.println(analogRead(ldrPin));
    Serial.print("New (0-1023): ");
    while (!Serial.available()) {}
    lightThreshold = Serial.readStringUntil('\n').toInt();
    Serial.println("Updated!");
  } else if (c == "3") {
    Serial.print("Current pass: ");
    while (!Serial.available()) {}
    String old = Serial.readStringUntil('\n');
    old.trim();
    if (old == password) {
      Serial.print("New pass: ");
      while (!Serial.available()) {}
      password = Serial.readStringUntil('\n');
      password.trim();
      Serial.println("Changed!");
    } else {
      Serial.println("Wrong!");
    }
  } else if (c == "4") {
    Serial.print("New name: ");
    while (!Serial.available()) {}
    systemName = Serial.readStringUntil('\n');
    systemName.trim();
    Serial.println("Updated!");
  }
  
  showMenu();
}

void calibrate() {
  Serial.println("\nCalibrating...");
  delay(1000);
  
  long total = 0;
  for (int i = 0; i < 10; i++) {
    total += getDistance();
    delay(100);
  }
  baselineDistance = total / 10;
  
  Serial.print("Baseline: ");
  Serial.print(baselineDistance);
  Serial.println(" cm\n");
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int dist = duration * 0.034 / 2;
  
  if (dist > 0 && dist < 400) return dist;
  return baselineDistance;
}

void checkSensor() {
  int dist = getDistance();
  
  Serial.print("Distance: ");
  Serial.print(dist);
  Serial.print(" cm | Baseline: ");
  Serial.print(baselineDistance);
  Serial.print(" | Trigger if change > ");
  Serial.println(sensitivity);
  
  // Trigger if distance CHANGES significantly
  if (abs(dist - baselineDistance) > sensitivity) {
    Serial.println("\n!!! MOVEMENT DETECTED !!!");
    Serial.print("Password: ");
    alarmActive = true;
    digitalWrite(redLED, HIGH);
  }
}

void stopAlarm() {
  alarmActive = false;
  armed = false;
  digitalWrite(buzzerPin, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);
  showMenu();
}
