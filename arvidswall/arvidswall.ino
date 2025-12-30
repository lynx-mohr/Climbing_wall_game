#include "pitches.h"
#include <EEPROM.h>
#include <TM1637Display.h>

// --- PIN-KONFIGURATION ---
const int ledButtonArray[] = {A4, A5, 9};
const int ledStarsArray[] = {3, 5, 6, A3, 10, 11};
const int ledEightArray[] = {3, 5, 6, 7, 8, A3, 10, 11};
const int ledAllArray[] = {3, 5, 6, 7, 8, A3, 10, 11, A4, A5, 9};
const int buttonPins[] = {4, 0, 13}; // Röd=0, Blå=1, Vit=2
const int piezoPin = A0;

byte lightSensor = A1;
byte potPin = A2;
byte switchPin = 2;
#define CLK 1
#define DIO 12

TM1637Display display = TM1637Display(CLK, DIO);

// --- VARIABLER ---
int initialPitch = 500;
int pitchIncrement = 50;
int currentPitch = initialPitch;
int lightSensorValue = 0;
int highScore = 0;
int currentStreak = 0;
int activeLED = -1;
int previousActiveLED = -1;
unsigned long startTime = 0;
const unsigned long timeLimit = 8000;
bool gameInProgress = false;

// Variabler för långtryck (Easter Eggs)
unsigned long whiteButtonPressStart = 0;
bool isPressingWhite = false;
unsigned long blueButtonPressStart = 0;
bool isPressingBlue = false;

// Musik-inställningar
int tempo = 240;
int wholenote = (60000 * 4) / tempo;

// --- SEGMENT-TEXTER ---
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t godJulText[] = {
  0x00, 0x00, 0x00, 0x00, 0x3d, 0x5c, 0x5e, 0x00, 0x1e, 0x1c, 0x38, 0x00, 0x00,
  0x38, 0x5c, 0x1c, 0x04, 0x6d, 0x00, 0x00, 0x5c, 0x58, 0x74, 0x00, 0x00,
  0x77, 0x50, 0x1c, 0x04, 0x5e, 0x00, 0x00, 0x00, 0x00
};
const uint8_t getReadyText[] = {
  0x00, 0x00, 0x00, 0x00, 0x3d, 0x79, 0x07, 0x00, 0x50, 0x79, 0x77, 0x5e, 0x6e, 
  0x00, 0x00, 0x00, 0x00, 0x00 // Extra tomma steg för utrullning
};

const uint8_t gameOverText[] = {
  0x00, 0x00, 0x00, 0x00, 0x3d, 0x77, 0x37, 0x79, 0x00, 0x3f, 0x1c, 0x79, 0x50, 
  0x00, 0x00, 0x00, 0x00, 0x00 // Extra tomma steg för utrullning
};


// --- MELODIER ---
int jingleMelody[] = { NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G5 };
int jingleDurations[] = { 8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 4, 4 };
int jingleNotes = sizeof(jingleMelody) / sizeof(jingleMelody[0]);

const uint16_t interstellarMelody[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, 
  NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, 
  NOTE_C6, NOTE_B5, NOTE_A5, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_E5, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_G5, 
  NOTE_A5, NOTE_B5, NOTE_D6, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_D6, NOTE_E5, 
  NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_A5, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, 
  NOTE_B5, NOTE_C6, NOTE_D6, NOTE_A5, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, NOTE_E5, NOTE_C6, NOTE_B5, NOTE_C6, 
  NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_B5, NOTE_C6, NOTE_D6,
  NOTE_E6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_B5, 
  NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_E6, NOTE_D6, NOTE_E6, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, 
  NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_G6, NOTE_F6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6, 
  NOTE_C7, NOTE_B6, NOTE_A6, NOTE_B6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_E6, NOTE_B6, NOTE_G6, NOTE_A6, NOTE_G6, 
  NOTE_A6, NOTE_B6, NOTE_D7, NOTE_E6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_D7, NOTE_E6, 
  NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_A6, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6, 
  NOTE_B6, NOTE_C7, NOTE_D7, NOTE_A6, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6, NOTE_E6, NOTE_C7, NOTE_B6, NOTE_C7,
  NOTE_A6, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, 
  NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, 
  NOTE_E7, NOTE_E6, NOTE_E6, NOTE_E6, NOTE_E6, NOTE_E6
};
int interstellarTotalNotes = 198;

int marioMelody[] = { NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4, NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8, REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4 };
int gameOverMelody[] = { NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8, NOTE_G4,-12, NOTE_F4, -12, NOTE_G4,1 };
int victoriousMelody[] = { NOTE_A4, -2, NOTE_A4, 4, NOTE_C5,4, NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, -2, NOTE_E4, -2, NOTE_A3, -2, NOTE_A3,4, NOTE_C4,4, NOTE_A3,4, NOTE_G3, 4, NOTE_F3, 4, NOTE_G3, 4, NOTE_F3,4, NOTE_E3,-2 };

int marioNotes = sizeof(marioMelody) / sizeof(marioMelody[0]) / 2;
int gameOverNotes = sizeof(gameOverMelody) / sizeof(gameOverMelody[0]) / 2;
int victoriousMelodyNotes = sizeof(victoriousMelody) / sizeof(victoriousMelody[0])/2;

// --- SETUP ---
void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(ledButtonArray[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 11; i++) {
    pinMode(ledAllArray[i], OUTPUT);
    digitalWrite(ledAllArray[i], LOW);
  }
  pinMode(lightSensor, INPUT);
  pinMode(potPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);

  randomSeed(analogRead(0));
  int storedHighScore;
  EEPROM.get(0, storedHighScore);
  highScore = (storedHighScore == -1) ? 0 : storedHighScore;

  display.setBrightness(5);
  display.setSegments(allON);
  delay(1000);
  display.clear();
  display.showNumberDec(highScore);
}

// --- LOOP ---
void loop() {
  lightSensorValue = analogRead(lightSensor);

  if (lightSensorValue <= 200 && !gameInProgress) {
    pulsatingStarsEffect(3, 5, 6, A3, 10, 11, 5000, 255);
    return;
  }

  if (digitalRead(switchPin) == LOW && !gameInProgress) {
    potentiometerMusic();
    return;
  }

  if (!gameInProgress) {
    // Pulsera vita knappen
    int pulseValue = (sin(millis() * 0.003) * 127) + 128; 
    analogWrite(9, pulseValue); 

    // Blå knapp (Interstellar)
    if (digitalRead(buttonPins[1]) == LOW) {
      if (!isPressingBlue) { blueButtonPressStart = millis(); isPressingBlue = true; } 
      unsigned long elapsed = millis() - blueButtonPressStart;
      if (elapsed > 3000) display.showNumberDec(1);
      else if (elapsed > 2000) display.showNumberDec(2);
      else if (elapsed > 1000) display.showNumberDec(3);
      if (elapsed > 4000) { runInterstellarMode(); isPressingBlue = false; blueButtonPressStart = 0; }
    } else {
      if (isPressingBlue) { isPressingBlue = false; blueButtonPressStart = 0; display.showNumberDec(highScore); }
    }

    // Vit knapp (Spel eller Jul)
    if (digitalRead(buttonPins[2]) == LOW) {
      if (!isPressingWhite) { whiteButtonPressStart = millis(); isPressingWhite = true; } 
      unsigned long elapsed = millis() - whiteButtonPressStart;
      if (elapsed > 3000) display.showNumberDec(1);
      else if (elapsed > 2000) display.showNumberDec(2);
      else if (elapsed > 1000) display.showNumberDec(3);
      if (elapsed > 4000) { runChristmasMode(); isPressingWhite = false; whiteButtonPressStart = 0; }
    } else {
      if (isPressingWhite) {
        unsigned long totalPressTime = millis() - whiteButtonPressStart;
        isPressingWhite = false; whiteButtonPressStart = 0;
        if (totalPressTime < 4000) { analogWrite(9, 0); startNewGame(); }
        else { display.showNumberDec(highScore); }
      }
    }
  } else {
    // Spelet körs
    if (millis() - startTime >= timeLimit) gameOver();
    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPins[i]) == LOW && i == activeLED) handleCorrectHit();
    }
  }
}

// --- SPEL-FUNKTIONER ---
void startNewGame() {
  gameInProgress = true;
  currentStreak = 0;
  currentPitch = initialPitch;
  display.showNumberDec(0);
  for(int i=0; i<8; i++) digitalWrite(ledEightArray[i], HIGH);
  playSuperMarioMelody();
  for(int i=0; i<8; i++) digitalWrite(ledEightArray[i], LOW);
  pickNextLED(); 
}

void handleCorrectHit() {
  playBeep(currentPitch);
  currentPitch += pitchIncrement;
  currentStreak++;
  display.showNumberDec(currentStreak);
  
  // --- NY BELÖNINGSLOGIK ---
  if (currentStreak > 0) {
    // 1. Var 25:e poäng (25, 50, 75...) - 1-up!
    if (currentStreak % 25 == 0) {
      playOneUpSound();
      startTime = millis(); 
    } 
    // 2. Var 10:e poäng (10, 20, 30, 40...) - Fireball!
    else if (currentStreak % 10 == 0) {
      playFireBallSound();
      startTime = millis(); 
    } 
    // 3. Var 5:e poäng (5, 15, 35...) - Coin!
    else if (currentStreak % 5 == 0) {
      playCoinSound();
      startTime = millis(); 
    }
  }
  
  delay(150);
  pickNextLED();
}

void pickNextLED() {
  int randomIndex;
  do { randomIndex = random(3); } while (randomIndex == previousActiveLED);
  turnOffAllLEDs();
  activeLED = randomIndex;
  digitalWrite(ledButtonArray[activeLED], HIGH);
  previousActiveLED = activeLED;
  startTime = millis();
}

void gameOver() {
  turnOffAllLEDs();
  if (currentStreak > highScore) {
    highScore = currentStreak; EEPROM.put(0, highScore);
    playVictoriousMelody(); 
  }
  display.clear();
  int textStep = 0, noteIndex = 0;
  // Vi kör så länge det finns tecken i arrayen (inklusive våra nya nollor)
  int totalSteps = sizeof(gameOverText) - 3; 
  
  while (textStep < totalSteps) {
    int dur = 180;
    if (noteIndex < gameOverNotes * 2) {
      int div = gameOverMelody[noteIndex + 1];
      dur = (div > 0) ? (wholenote / div) : (wholenote / abs(div) * 1.5);
      tone(piezoPin, gameOverMelody[noteIndex], dur * 0.8);
      noteIndex += 2;
    }
    
    display.setSegments(&gameOverText[textStep], 4, 0);
    
    textStep++; 
    delay(dur); 
    noTone(piezoPin);
  }
  display.clear();
  flashGameLost();
  display.showNumberDec(highScore);
  gameInProgress = false;
}

// --- LJUDEFFEKTER (MARIO) ---
void playCoinSound() {
  for (int i = 0; i < 8; i++) digitalWrite(ledEightArray[i], HIGH);
  tone(piezoPin, NOTE_B5, 100); delay(100);
  tone(piezoPin, NOTE_E6, 850); delay(800);
  noTone(piezoPin);
  for (int i = 0; i < 8; i++) digitalWrite(ledEightArray[i], LOW);
}

void playOneUpSound() {
  for (int i = 0; i < 8; i++) digitalWrite(ledEightArray[i], HIGH);
  tone(piezoPin, NOTE_E6, 125); delay(130);
  tone(piezoPin, NOTE_G6, 125); delay(130);
  tone(piezoPin, NOTE_E7, 125); delay(130);
  tone(piezoPin, NOTE_C7, 125); delay(130);
  tone(piezoPin, NOTE_D7, 125); delay(130);
  tone(piezoPin, NOTE_G7, 125); delay(125);
  noTone(piezoPin);
  for (int i = 0; i < 8; i++) digitalWrite(ledEightArray[i], LOW);
}

void playFireBallSound() {
  for (int i = 0; i < 8; i++) digitalWrite(ledEightArray[i], HIGH);
  tone(piezoPin, NOTE_G4, 35); delay(35);
  tone(piezoPin, NOTE_G5, 35); delay(35);
  tone(piezoPin, NOTE_G6, 35); delay(35);
  noTone(piezoPin); delay(200);
  for (int i = 0; i < 8; i++) digitalWrite(ledEightArray[i], LOW);
}

// --- EASTER EGGS (JUL & INTERSTELLAR) ---
void runChristmasMode() {
  display.clear(); analogWrite(9, 255);
  while(digitalRead(buttonPins[2]) == LOW) { delay(10); }
  int currentNote = 0, textStep = 0;
  int totalTextFrames = sizeof(godJulText) - 3;
  unsigned long lastNoteTime = 0; int currentNoteDuration = 0;

  while(true) {
    if (digitalRead(buttonPins[2]) == LOW) { noTone(piezoPin); break; }
    if (millis() - lastNoteTime >= (unsigned long)currentNoteDuration) {
      noTone(piezoPin);
      int noteDur = 1200 / jingleDurations[currentNote];
      currentNoteDuration = noteDur * 1.3;
      tone(piezoPin, jingleMelody[currentNote], noteDur);
      lastNoteTime = millis();
      currentNote = (currentNote + 1) % jingleNotes;
      display.setSegments(godJulText + textStep);
      textStep = (textStep + 1) % totalTextFrames;
    }
  }
  display.clear(); noTone(piezoPin);
}

void runInterstellarMode() {
  const uint8_t stayText[] = {0x6d, 0x07, 0x77, 0x6e}; 
  display.setSegments(stayText);
  while(digitalRead(buttonPins[1]) == LOW) delay(10);
  bool keepPlaying = true;
  while(keepPlaying) {
    for (int thisNote = 0; thisNote < interstellarTotalNotes; thisNote++) {
      if (digitalRead(buttonPins[1]) == LOW) { keepPlaying = false; break; }
      int noteDuration = (thisNote < 192) ? 150 : 600;
      turnOffAllLEDs();
      if (thisNote < 192) digitalWrite(ledButtonArray[1], HIGH);
      else { for (int i = 0; i < 11; i++) digitalWrite(ledAllArray[i], HIGH); }
      tone(piezoPin, interstellarMelody[thisNote], noteDuration * 0.8);
      delay(noteDuration * 0.8); turnOffAllLEDs(); delay(noteDuration * 0.2); noTone(piezoPin);
    }
    keepPlaying = false;
  }
  turnOffAllLEDs(); display.showNumberDec(highScore);
}

// --- HJÄLPFUNKTIONER ---
void turnOffAllLEDs() { for (int i = 0; i < 11; i++) digitalWrite(ledAllArray[i], LOW); }
void playBeep(int pitch) { tone(piezoPin, pitch, 100); delay(150); noTone(piezoPin); }
void flashGameLost() { for (int i = 0; i < 3; i++) { for (int j = 0; j < 3; j++) digitalWrite(ledButtonArray[j], HIGH); delay(100); turnOffAllLEDs(); delay(100); } }

void playSuperMarioMelody() {
  int noteIndex = 0, textStep = 0;
  int totalNotes = marioNotes * 2; 
  int totalFrames = sizeof(getReadyText) - 3;
  
  while (noteIndex < totalNotes || textStep < totalFrames) {
    int dur = 150;
    if (noteIndex < totalNotes) {
      int div = marioMelody[noteIndex + 1];
      dur = (div > 0) ? (wholenote / div) : (wholenote / abs(div) * 1.5);
      tone(piezoPin, marioMelody[noteIndex], dur * 0.9); 
      noteIndex += 2;
    }
    
    display.setSegments(getReadyText + textStep);

    textStep++; 
    // Loopa texten om musiken pågår, men bara fram till de sista nollorna
    if (noteIndex < totalNotes - 12 && textStep >= totalFrames - 4) textStep = 0;
    
    delay(dur); 
    noTone(piezoPin);
    if (noteIndex >= totalNotes && textStep >= totalFrames) break;
  }
  display.clear();
}

void playVictoriousMelody(){
  for (int i = 0; i < victoriousMelodyNotes * 2; i += 2) {
    int div = victoriousMelody[i + 1];
    int dur = (div > 0) ? (wholenote / div) : (wholenote / abs(div) * 1.5);
    tone(piezoPin, victoriousMelody[i], dur * 0.9);
    digitalWrite(ledAllArray[random(11)], HIGH); delay(dur); turnOffAllLEDs();
  }
}

void pulsatingStarsEffect(int p1, int p2, int p3, int p4, int p5, int p6, int d, int m) {
  unsigned long s = millis();
  while (millis() - s < d) {
    int el = millis() - s;
    analogWrite(p1, (sin(el * 0.002) * 127) + 128); analogWrite(p2, (sin(el * 0.002 + 1) * 127) + 128);
    analogWrite(p3, (sin(el * 0.002 + 2) * 127) + 128); analogWrite(p4, (sin(el * 0.002 + 3) * 127) + 128);
    analogWrite(p5, (sin(el * 0.002 + 4) * 127) + 128); analogWrite(p6, (sin(el * 0.002 + 5) * 127) + 128);
    for(int i=0; i<3; i++) if(digitalRead(buttonPins[i]) == LOW) return;
  }
  turnOffAllLEDs();
}

void potentiometerMusic(){
  int p = analogRead(potPin);
  int m = map(p, 0, 1023, 1, 11);
  int f = map(p, 0, 1023, 500, 1000);
  for (int i = 0; i < 11; i++) digitalWrite(ledAllArray[i], i < m ? HIGH : LOW);
  tone(piezoPin, f, 100);
}
