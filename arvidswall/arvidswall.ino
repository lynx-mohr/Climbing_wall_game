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

// --- GLOBALA VARIABLER & INSTÄLLNINGAR ---
int currentStreak = 0;
int activeLED = -1;
int previousActiveLED = -1;
unsigned long startTime = 0;
bool gameInProgress = false;
int lives = 3;
unsigned long lastTick = 0;
int lightSensorValue = 0;

unsigned long whiteButtonPressStart = 0;
bool isPressingWhite = false;
unsigned long blueButtonPressStart = 0;
bool isPressingBlue = false;
unsigned long redButtonPressStart = 0;
bool isPressingRed = false;
int moveHistory[5] = {-1, -1, -1, -1, -1}; // Sparar de senaste 5 knapp-indexen
int historyIndex = 0;

// Svårighetsgrader
int difficulty = 0; 
unsigned long currentTimeLimit = 8000;
int highScores[] = {0, 0, 0}; 

int tempo = 240;
int wholenote = (60000 * 4) / tempo;

// --- SEGMENT-TEXTER ---
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t godJulText[] = { 0x00, 0x00, 0x00, 0x00, 0x3d, 0x5c, 0x5e, 0x00, 0x1e, 0x1c, 0x38, 0x00, 0x00, 0x38, 0x5c, 0x1c, 0x04, 0x6d, 0x00, 0x00, 0x5c, 0x58, 0x74, 0x00, 0x00, 0x77, 0x50, 0x1c, 0x04, 0x5e, 0x00, 0x00, 0x00, 0x00 };
const uint8_t getReadyText[] = { 0x00, 0x00, 0x00, 0x00, 0x3d, 0x79, 0x07, 0x00, 0x50, 0x79, 0x77, 0x5e, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00 };
const uint8_t gameOverText[] = { 0x00, 0x00, 0x00, 0x00, 0x3d, 0x77, 0x37, 0x79, 0x00, 0x3f, 0x1c, 0x79, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00 };

// --- MELODIER ---
int jingleMelody[] = { NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G5 };
int jingleDurations[] = { 8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 4, 4 };
int jingleNotes = sizeof(jingleMelody) / sizeof(jingleMelody[0]);

const uint16_t interstellarMelody[] = { NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_A5, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_E5, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_D6, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_D6, NOTE_E5, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_A5, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_A5, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, NOTE_E5, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_E6, NOTE_D6, NOTE_E6, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_G6, NOTE_F6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_C7, NOTE_B6, NOTE_A6, NOTE_B6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_E6, NOTE_B6, NOTE_G6, NOTE_A6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_D7, NOTE_E6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_D7, NOTE_E6, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_A6, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_A6, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6, NOTE_E6, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_C7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_E6, NOTE_E6, NOTE_E6, NOTE_E6, NOTE_E6 };
int interstellarTotalNotes = 198;

int marioMelody[] = { NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4, NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8, REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4 };
int gameOverMelody[] = { NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8, NOTE_G4,-12, NOTE_F4, -12, NOTE_G4,1 };
int victoriousMelody[] = { NOTE_A4, -2, NOTE_A4, 4, NOTE_C5,4, NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, -2, NOTE_E4, -2, NOTE_A3, -2, NOTE_A3,4, NOTE_C4,4, NOTE_A3,4, NOTE_G3, 4, NOTE_F3, 4, NOTE_G3, 4, NOTE_F3,4, NOTE_E3,-2 };

int marioNotes = sizeof(marioMelody) / sizeof(marioMelody[0]) / 2;
int gameOverNotes = sizeof(gameOverMelody) / sizeof(gameOverMelody[0]) / 2;
int victoriousMelodyNotes = sizeof(victoriousMelody) / sizeof(victoriousMelody[0])/2;

// --- HJÄLPFUNKTIONER ---

void turnOffAllLEDs() { for (int i = 0; i < 11; i++) digitalWrite(ledAllArray[i], LOW); }

void playBeep(int freq) {
  tone(piezoPin, freq, 200);
  delay(250);
  noTone(piezoPin);
}

void scrollText(const char* text) {
  display.clear();
  if (strcmp(text, "NOOB") == 0) {
    uint8_t t[] = {0x54, 0x5c, 0x5c, 0x7c}; // nOOb
    display.setSegments(t);
  } else if (strcmp(text, "HARD") == 0) {
    uint8_t t[] = {0x76, 0x77, 0x50, 0x5e}; // HArd
    display.setSegments(t);
  } else if (strcmp(text, "HACKER") == 0) {
    uint8_t t[] = {0x76, 0x77, 0x39, 0x79}; // HACE
    display.setSegments(t);
  }
  delay(1000);
}

void playSuperMarioMelody() {
  int noteIndex = 0;
  int textStep = 0;
  int totalNotes = marioNotes * 2; 
  int totalFrames = sizeof(getReadyText) - 3; // Hur många steg vi kan ta innan texten tar slut

  while (noteIndex < totalNotes) {
    int dur = 150;
    // Spela nästa ton
    if (noteIndex < totalNotes) {
      int div = marioMelody[noteIndex + 1];
      dur = (div > 0) ? (wholenote / div) : (wholenote / abs(div) * 1.5);
      tone(piezoPin, marioMelody[noteIndex], dur * 0.9); 
      noteIndex += 2;
    }

    // Visa texten, men använd % för att börja om från början om texten tar slut
    display.setSegments(getReadyText + (textStep % totalFrames)); 
    textStep++; 

    delay(dur); 
    noTone(piezoPin);
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

void playOneUpSound() {
  int n[] = {NOTE_E6, NOTE_G6, NOTE_E7, NOTE_C7, NOTE_D7, NOTE_G7};
  for(int i=0; i<6; i++) { tone(piezoPin, n[i], 125); delay(130); }
  noTone(piezoPin);
}

void playCoinSound() { 
  tone(piezoPin, NOTE_B5, 100); delay(100); 
  tone(piezoPin, NOTE_E6, 850); delay(800); 
  noTone(piezoPin); 
}

void playFireBallSound() { 
  for(int j=0; j<3; j++) { 
    tone(piezoPin, NOTE_G4, 35); delay(35); 
    tone(piezoPin, NOTE_G5, 35); delay(35); 
    tone(piezoPin, NOTE_G6, 35); delay(35); 
    noTone(piezoPin); delay(100); 
  } 
}

void flashGameLost() { for (int i = 0; i < 3; i++) { for (int j = 0; j < 3; j++) digitalWrite(ledButtonArray[j], HIGH); delay(100); turnOffAllLEDs(); delay(100); } }

void pickNextLED() {
  int randomIndex;
  do { randomIndex = random(3); } while (randomIndex == previousActiveLED);
  turnOffAllLEDs();
  activeLED = randomIndex;
  // Spara det nya valet i historiken (skiftar ut det äldsta)
  for(int i = 0; i < 4; i++) moveHistory[i] = moveHistory[i+1];
  moveHistory[4] = activeLED;

  digitalWrite(ledButtonArray[activeLED], HIGH);
  previousActiveLED = activeLED;
  startTime = millis();
  lastTick = millis();
}

void startNewGame() {
  gameInProgress = true;
  currentStreak = 0;
  lives = 3;
  display.showNumberDec(0);
  for(int i=0; i<8; i++) digitalWrite(ledEightArray[i], HIGH);
  playSuperMarioMelody();
  for(int i=0; i<8; i++) digitalWrite(ledEightArray[i], LOW);
  pickNextLED(); 
}

void triggerCoinBonanza() {
  int iterations = random(6, 11); 
  int bonanzaButton = buttonPins[activeLED]; 
  int bonanzaLED = ledButtonArray[activeLED]; 
  unsigned long bonanzaTimeLimit = 1200; // 1.2 sekunder på sig per peng
  
  // 1. START-HINT
  display.setSegments(allON); 
  tone(piezoPin, NOTE_B5, 100); delay(100); 
  tone(piezoPin, NOTE_E6, 500); 
  delay(500); 
  noTone(piezoPin);

  for (int i = 0; i < iterations; i++) {
    digitalWrite(bonanzaLED, HIGH);
    unsigned long bonanzaStart = millis();
    bool hit = false;
    
    // 2. VÄNTA PÅ TRYCK ELLER TIMEOUT
    while(millis() - bonanzaStart < bonanzaTimeLimit) {
      if(digitalRead(bonanzaButton) == LOW) {
        hit = true;
        break;
      }
    }

    if (hit) {
      // COIN-LJUD VID TRÄFF
      tone(piezoPin, NOTE_B5, 100); delay(100);
      tone(piezoPin, NOTE_E6, 500);
      
      currentStreak++; 
      display.showNumberDec(currentStreak);
      digitalWrite(bonanzaLED, LOW); 
      
      while(digitalRead(bonanzaButton) == LOW) delay(5);
      delay(150); // Lite snabbare tempo mellan pengarna
      noTone(piezoPin); 
    } 
    else {
      // MISSADE EN PENG I BONANZAN
      digitalWrite(bonanzaLED, LOW);
      playBeep(200); // Ett lite "ledset" ljud för att man missade bonusen
      delay(300);
      break; // Avbryt hela Bonanzan och gå tillbaka till vanliga spelet
    }
  }
  
  display.clear(); 
  display.showNumberDec(currentStreak); 
  delay(300);
  startTime = millis(); // Nollställ speltimern så man inte dör direkt när man kommer ut ur Bonanzan
}

void handleOneUp() {
  playOneUpSound();
  if (lives < 3) lives++;
}

// --- NY FUNKTION: TURBO REACTOR BONUS ---
void triggerTurboReactor() {
  turnOffAllLEDs();
  display.clear();
  uint8_t turboText[] = {0x78, 0x1c, 0x50, 0x5c}; // "tUrb"
  display.setSegments(turboText);

  // Siren och varningsljus
  for(int i=0; i<6; i++) {
    for(int j=0; j<6; j++) { 
      digitalWrite(ledStarsArray[j], HIGH);
      tone(piezoPin, 600 + (j*100), 50);
      delay(50);
      digitalWrite(ledStarsArray[j], LOW);
    }
  }

  int reactorHits = 0;
  float speedFactor = 2000; // Starttid i millisekunder

  while(reactorHits < 10) {
    int r = random(3);
    digitalWrite(ledButtonArray[r], HIGH);
    tone(piezoPin, 800, 50);
    
    unsigned long startHit = millis();
    bool hit = false;
    
    while(millis() - startHit < speedFactor) {
      if(digitalRead(buttonPins[r]) == LOW) {
        hit = true;
        break;
      }
    }

    if(hit) {
      reactorHits++;
      tone(piezoPin, 1000 + (reactorHits * 100), 100);
      digitalWrite(ledButtonArray[r], LOW);
      speedFactor *= 0.85; // Blir 15% snabbare för varje träff!
      delay(150);
    } else {
      // Miss - avbryt bonus (men inget förlorat liv)
      tone(piezoPin, 150, 500);
      delay(500);
      return;
    }
  }

  // Vinst!
  currentStreak += 15;
  display.showNumberDec(currentStreak);
  for(int i=0; i<5; i++) {
    for(int j=0; j<11; j++) digitalWrite(ledAllArray[j], HIGH);
    tone(piezoPin, 1000, 100); delay(100);
    turnOffAllLEDs(); delay(100);
  }
  playBonusWinFanfare();
  startTime = millis();
}

// --- UPPDATERAD TRAPPSTEGS-LOGIK ---
void handleCorrectHit() {
  tone(piezoPin, NOTE_C6, 40); 
  currentStreak++;
  display.showNumberDec(currentStreak);

  // Var 30:e poäng slumpar vi nu mellan Memory och Turbo
  if (currentStreak > 0 && currentStreak % 30 == 0) {
    if(random(0, 2) == 0) triggerMemoryBonus();
    else triggerTurboReactor();
  } 

  //7 % chans för CoinBonanza
  else if (random(0, 100) < 7) {
    triggerCoinBonanza();
  }
  
  pickNextLED();
}

void gameOver() {
  turnOffAllLEDs();
  if (currentStreak > highScores[difficulty]) {
    highScores[difficulty] = currentStreak; 
    EEPROM.put(difficulty * 4, currentStreak);
    playVictoriousMelody(); 
  }
  display.clear();
  int textStep = 0, noteIndex = 0;
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
    textStep++; delay(dur); noTone(piezoPin);
  }
  flashGameLost();
  display.showNumberDec(highScores[difficulty]);
  gameInProgress = false;
}

void handleMiss() {
  lives--;
  turnOffAllLEDs();
  if (lives > 0) {
    int lostLifeMelody[] = { NOTE_B4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4 };
    for (int i = 0; i < 6; i++) { tone(piezoPin, lostLifeMelody[i], 150); delay(200); noTone(piezoPin); }
    uint8_t livesText[] = { 0x38, 0x00, 0x00, display.encodeDigit(lives) };
    display.setSegments(livesText);
    delay(1500);
    display.showNumberDec(currentStreak);
    pickNextLED();
  } else { gameOver(); }
}

void runChristmasMode() {
  display.clear(); while(digitalRead(buttonPins[2]) == LOW) delay(10);
  int currentNote = 0, textStep = 0;
  unsigned long lastNoteTime = 0; int currentNoteDuration = 0;
  while(true) {
    if (digitalRead(buttonPins[2]) == LOW) { noTone(piezoPin); delay(500); break; }
    if (millis() - lastNoteTime >= (unsigned long)currentNoteDuration) {
      noTone(piezoPin);
      int noteDur = 1200 / jingleDurations[currentNote];
      currentNoteDuration = noteDur * 1.3;
      tone(piezoPin, jingleMelody[currentNote], noteDur);
      lastNoteTime = millis();
      currentNote = (currentNote + 1) % jingleNotes;
      display.setSegments(godJulText + textStep);
      textStep = (textStep + 1) % (sizeof(godJulText) - 3);
    }
  }
}

void runHarryPotterMode() {
  display.clear(); while(digitalRead(buttonPins[0]) == LOW) delay(10);
  const uint8_t hp1[] = {0x76, 0x77, 0x50, 0x50, 0x6e, 0x00, 0x73, 0x5c, 0x07, 0x07, 0x79, 0x50, 0x00, 0x04, 0x6d, 0x00, 0x76, 0x79, 0x50, 0x79, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp2[] = {0x50, 0x5c, 0x54, 0x00, 0x38, 0x04, 0x70, 0x79, 0x6d, 0x00, 0x73, 0x04, 0x79, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp3[] = {0x6d, 0x79, 0x79, 0x00, 0x07, 0x76, 0x79, 0x00, 0x3d, 0x5c, 0x38, 0x5e, 0x00, 0x6d, 0x54, 0x04, 0x07, 0x39, 0x76, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp4[] = {0x5e, 0x5c, 0x00, 0x54, 0x5c, 0x07, 0x00, 0x3d, 0x5c, 0x00, 0x07, 0x5c, 0x00, 0x6d, 0x38, 0x79, 0x79, 0x73, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp5[] = {0x73, 0x1c, 0x50, 0x79, 0x00, 0x54, 0x77, 0x3d, 0x04, 0x39, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp6[] = {0x07, 0x76, 0x79, 0x00, 0x5e, 0x77, 0x50, 0x70, 0x00, 0x38, 0x5c, 0x50, 0x5e, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp7[] = {0x38, 0x5c, 0x1c, 0x04, 0x6d, 0x00, 0x04, 0x6d, 0x00, 0x77, 0x00, 0x76, 0x79, 0x50, 0x5c, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp8[] = {0x77, 0x50, 0x1c, 0x04, 0x5e, 0x00, 0x04, 0x6d, 0x00, 0x77, 0x00, 0x76, 0x79, 0x50, 0x5c, 0x00, 0x00, 0x00, 0x00};
  const uint8_t hp9[] = {0x76, 0x5c, 0x3d, 0x1c, 0x77, 0x50, 0x07, 0x6d, 0x00, 0x04, 0x6d, 0x00, 0x7c, 0x79, 0x6d, 0x07, 0x00, 0x00, 0x00, 0x00};
  int choice = random(0, 9); const uint8_t* currentText; int textLen;
  switch(choice) { case 0: currentText = hp1; textLen = sizeof(hp1); break; case 1: currentText = hp2; textLen = sizeof(hp2); break; case 2: currentText = hp3; textLen = sizeof(hp3); break; case 3: currentText = hp4; textLen = sizeof(hp4); break; case 4: currentText = hp5; textLen = sizeof(hp5); break; case 5: currentText = hp6; textLen = sizeof(hp6); break; case 6: currentText = hp7; textLen = sizeof(hp7); break; case 7: currentText = hp8; textLen = sizeof(hp8); break; case 8: currentText = hp9; textLen = sizeof(hp9); break; }
  int hpMelody[] = { REST, NOTE_D4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_D5, NOTE_C5, NOTE_A4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_GS4, NOTE_D4, NOTE_D4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_D5, NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_B4, NOTE_DS5, NOTE_D5, NOTE_CS5, NOTE_CS4, NOTE_B4, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_AS4, NOTE_D5, NOTE_AS4, NOTE_DS5, NOTE_D5, NOTE_CS5, NOTE_A4, NOTE_AS4, NOTE_D5, NOTE_CS5, NOTE_CS4, NOTE_D4, NOTE_D5, REST, NOTE_AS4, NOTE_D5, NOTE_AS4, NOTE_D5, NOTE_AS4, NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_B4, NOTE_DS5, NOTE_D5, NOTE_CS5, NOTE_CS4, NOTE_AS4, NOTE_G4 };
  int hpDurs[] = { 2, 4, 4, 8, 4, 2, 4, 2, 2, 4, 8, 4, 2, 4, 1, 4, 4, 8, 4, 2, 4, 2, 4, 2, 4, 4, 8, 4, 2, 4, 1, 4, 2, 4, 2, 4, 2, 4, 2, 4, 4, 8, 4, 2, 4, 1, 4, 4, 2, 4, 2, 4, 2, 4, 2, 4, 4, 8, 4, 2, 4, 1 };
  int textStep = 0;
  for (int note = 0; note < 62; note++) {
    if (digitalRead(buttonPins[0]) == LOW) break;
    int duration = 900 / hpDurs[note];
    display.setSegments(currentText + textStep); textStep++; if (textStep >= textLen - 3) textStep = 0;
    turnOffAllLEDs(); digitalWrite(ledButtonArray[0], HIGH); if (note % 4 == 0) digitalWrite(ledStarsArray[random(6)], HIGH);
    tone(piezoPin, hpMelody[note], duration * 0.8); delay(duration * 0.7);
    turnOffAllLEDs(); delay(duration * 0.4); noTone(piezoPin);
  }
}

void runInterstellarMode() {
  display.clear(); while(digitalRead(buttonPins[1]) == LOW) delay(10);
  const uint8_t stayText[] = {0x6d, 0x07, 0x77, 0x6e}; 
  display.setSegments(stayText);
  for (int thisNote = 0; thisNote < interstellarTotalNotes; thisNote++) {
    if (digitalRead(buttonPins[1]) == LOW) break;
    int noteDuration = (thisNote < 192) ? 150 : 600;
    turnOffAllLEDs();
    if (thisNote < 192) digitalWrite(ledButtonArray[1], HIGH);
    else { for (int i = 0; i < 11; i++) digitalWrite(ledAllArray[i], HIGH); }
    tone(piezoPin, interstellarMelody[thisNote], noteDuration * 0.8);
    delay(noteDuration * 0.8); turnOffAllLEDs(); delay(noteDuration * 0.2); noTone(piezoPin);
  }
}

void selectDifficulty() {
  // 1. Vänta tills spelaren har släppt vita knappen helt
  while(digitalRead(buttonPins[2]) == LOW) {
    delay(10); 
  }
  
  bool selecting = true;
  display.clear();
  uint8_t selText[] = {0x6d, 0x79, 0x38, 0x00}; // "SEL"
  display.setSegments(selText);
  delay(200); 

  while(selecting) {
    // 2. BLINK-LOGIK (10 gånger i sekunden)
    bool blinkState = (millis() / 100) % 2; 
    
    for(int i=0; i<3; i++) {
      digitalWrite(ledButtonArray[i], blinkState ? HIGH : LOW);
    }

    // 3. LYSSNA PÅ VAL (Ny ordning enligt önskemål)
    
    // VIT KNAPP (buttonPins[2]) = NOOB
    if (digitalRead(buttonPins[2]) == LOW) { 
      difficulty = 0; 
      currentTimeLimit = 8000;
      scrollText("NOOB");
      playBeep(440);
      selecting = false;
    }
    // RÖD KNAPP (buttonPins[0]) = HARD
    else if (digitalRead(buttonPins[0]) == LOW) { 
      difficulty = 1; 
      currentTimeLimit = 3000;
      scrollText("HARD");
      playBeep(660);
      selecting = false;
    }
    // BLÅ KNAPP (buttonPins[1]) = HACKER (Interstellar-knappen)
    else if (digitalRead(buttonPins[1]) == LOW) { 
      difficulty = 2; 
      currentTimeLimit = 1500;
      scrollText("HACKER");
      tone(piezoPin, 100, 1000); 
      delay(1000);
      selecting = false;
    }
  } 
  
  turnOffAllLEDs();
  display.showNumberDec(highScores[difficulty]);
  
  // 1. Vänta tills ALLA knappar är släppta
  while(digitalRead(buttonPins[0]) == LOW || digitalRead(buttonPins[1]) == LOW || digitalRead(buttonPins[2]) == LOW) {
    delay(10);
  }

  // 2. RENSA TIDTAGNINGEN (Detta tar bort 2:an!)
  isPressingWhite = false;
  whiteButtonPressStart = millis(); // Sätt starttiden till NU istället för förr
  
  delay(500);
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
  for(int i=0; i<3; i++) {
    int stored;
    EEPROM.get(i * 4, stored);
    highScores[i] = (stored == -1 || stored > 1000) ? 0 : stored;
  }
  
  display.setBrightness(5);
  display.showNumberDec(highScores[difficulty]);

  delay(500); // Vänta en halvsekund så Pin 13 hinner lugna ner sig
  isPressingWhite = false;
  whiteButtonPressStart = 0;
  display.showNumberDec(highScores[difficulty]);

}

// --- LOOP ---
void loop() {
  lightSensorValue = analogRead(lightSensor);

  // 1. Stjärneffekt i mörker
  if (lightSensorValue > 300 && !gameInProgress) {
    pulsatingStarsEffect(3, 5, 6, A3, 10, 11, 5000, 255);
    return;
  }

  // 2. Potentiometer-musik
  if (digitalRead(switchPin) == LOW && !gameInProgress) {
    potentiometerMusic();
    return;
  }

  // 3. MENY-LÄGE (Om spelet inte körs)
  if (!gameInProgress) {
    int pulseValue = (sin(millis() * 0.003) * 127) + 128; 
    analogWrite(9, pulseValue); 

    // BLÅ KNAPP
    if (digitalRead(buttonPins[1]) == LOW) {
      if (!isPressingBlue) { blueButtonPressStart = millis(); isPressingBlue = true; } 
      unsigned long elapsed = millis() - blueButtonPressStart;
      if (elapsed > 1000 && elapsed <= 4000) display.showNumberDec(4 - (elapsed / 1000));
      if (elapsed > 4000) { runInterstellarMode(); isPressingBlue = false; blueButtonPressStart = 0; }
    } else if (isPressingBlue) {
      isPressingBlue = false; blueButtonPressStart = 0; 
      display.showNumberDec(highScores[difficulty]); 
    }

    // RÖD KNAPP
    if (digitalRead(buttonPins[0]) == LOW) {
      if (!isPressingRed) { redButtonPressStart = millis(); isPressingRed = true; } 
      unsigned long elapsed = millis() - redButtonPressStart;
      if (elapsed > 1000 && elapsed <= 4000) display.showNumberDec(4 - (elapsed / 1000));
      if (elapsed > 4000) { runHarryPotterMode(); isPressingRed = false; redButtonPressStart = 0; }
    } else if (isPressingRed) {
      isPressingRed = false; redButtonPressStart = 0; 
      display.showNumberDec(highScores[difficulty]); 
    }

    // VIT KNAPP
    if (digitalRead(buttonPins[2]) == LOW) {
      if (!isPressingWhite) { whiteButtonPressStart = millis(); isPressingWhite = true; } 
      unsigned long elapsed = millis() - whiteButtonPressStart;
      if (elapsed > 1000 && elapsed < 2000) display.showNumberDec(3);
      else if (elapsed >= 2000 && elapsed < 3000) display.showNumberDec(2);
      else if (elapsed >= 3000 && elapsed < 4000) display.showNumberDec(1);
      
      if (elapsed > 4000) { 
        selectDifficulty(); 
        isPressingWhite = false; 
        whiteButtonPressStart = 0; 
      }
    } else if (isPressingWhite) {
      unsigned long totalTime = millis() - whiteButtonPressStart;
      isPressingWhite = false; whiteButtonPressStart = 0;
      if (totalTime < 1000) { 
        analogWrite(9, 0); 
        startNewGame(); 
      } else { 
        display.showNumberDec(highScores[difficulty]); 
      }
    }
  } 
  // 4. SPEL-LÄGE (Om gameInProgress == true)
  else {
    unsigned long elapsed = millis() - startTime;
    if (elapsed >= currentTimeLimit) { 
      handleMiss();
    } else {
      unsigned long tickInterval = map(elapsed, 0, currentTimeLimit, 1000, 150);
      if (millis() - lastTick >= tickInterval) {
        tone(piezoPin, NOTE_C5, 30);
        lastTick = millis();
      }
    }
    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPins[i]) == LOW && i == activeLED) handleCorrectHit();
    }
  }
}

void triggerMemoryBonus() {
  if (moveHistory[0] == -1) return;

  turnOffAllLEDs();
  display.clear();
  
  // 1. STARTA MED MORSE-KOD
  playMorseMemory();
  
  uint8_t memText[] = {0x37, 0x79, 0x54, 0x5c}; // "nEno"
  display.setSegments(memText);
  delay(500);

  // 2. VISA SEKVENSRN
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledButtonArray[moveHistory[i]], HIGH);
    tone(piezoPin, 440 + (moveHistory[i] * 100), 200);
    delay(400);
    digitalWrite(ledButtonArray[moveHistory[i]], LOW);
    delay(100);
  }

  // 3. VÄNTA PÅ SVAR + JAWS-TEMA
  unsigned long bonusStart = millis();
  int correctAnswers = 0;
  display.showNumberDec(20);

  while (millis() - bonusStart < 10000) { 
    unsigned long elapsed = millis() - bonusStart;
    
    // JAWS-LOGIK: Spela "Du-dum" med ökande hastighet
    // Vi använder modulo för att skapa en puls
    int jawsSpeed = map(elapsed, 0, 10000, 800, 200); // Går snabbare och snabbare
    if (elapsed % jawsSpeed < 50) {
      tone(piezoPin, (elapsed % (jawsSpeed*2) < jawsSpeed) ? 165 : 175, 100); // Växlar mellan E och F
    }

    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        if (i == moveHistory[correctAnswers]) {
          tone(piezoPin, 880, 100);
          digitalWrite(ledButtonArray[i], HIGH);
          delay(200);
          digitalWrite(ledButtonArray[i], LOW);
          correctAnswers++;
          
          if (correctAnswers == 5) {
            playBonusWinFanfare();
            currentStreak += 20;
            display.showNumberDec(currentStreak);
            delay(1000);
            startTime = millis(); 
            return; 
          }
          while(digitalRead(buttonPins[i]) == LOW); 
        } else {
          handleMiss();
          return;
        }
      }
    }
  }
  handleMiss(); // Tiden ute
}


void playMorseMemory() {
  // Morse för "MEMORY": -- . -- --- .-. -.--
  // Vi förenklar det till korta och långa pip
  int pattern[] = {1,1, 0, 0, 1,1, 0,0,0, 1,0,1, 0,1,1,0,1,1}; // 1=lång, 0=kort
  for(int i=0; i<18; i++) {
    int duration = (pattern[i] == 1) ? 300 : 100;
    tone(piezoPin, 600, duration);
    delay(duration + 50);
  }
}

void playBonusWinFanfare() {
  // En snabb, stigande fanfar
  int notes[] = {NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_G5, NOTE_C6};
  int durations[] = {100, 100, 100, 300, 100, 500};
  
  for (int i = 0; i < 6; i++) {
    // Tänd alla lampor i takt med sista tonen
    if (i == 3) {
      for(int j = 0; j < 11; j++) digitalWrite(ledAllArray[j], HIGH);
    }
    tone(piezoPin, notes[i], durations[i]);
    delay(durations[i] + 20);
    noTone(piezoPin);
  }
  delay(200);
  turnOffAllLEDs();
}
