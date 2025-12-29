#include "pitches.h"
#include <EEPROM.h>
#include <TM1637Display.h>

const int ledButtonArray[] = {A4, A5, 9};
const int ledStarsArray[] = {3, 5, 6, A3, 10, 11};
const int ledEightArray[] = {3, 5, 6, 7, 8, A3, 10, 11};
const int ledAllArray[] = {3, 5, 6, 7, 8, A3, 10, 11, A4, A5, 9};
// VIKTIGT: Vita knappen är index 2 (pin 13)
const int buttonPins[] = {4, 0, 13}; 
const int piezoPin = A0;



byte lightSensor = A1;
byte potPin = A2;
byte switchPin = 2;
#define CLK 1
#define DIO 12

TM1637Display display = TM1637Display(CLK, DIO);

const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t allOFF[] = {0x00, 0x00, 0x00, 0x00};

// GOD JUL TEXT
const uint8_t godJulText[] = {
  0x00, 0x00, 0x00, 0x00,       // Start-tömning
  0x3d, 0x5c, 0x5e,             // G O D
  0x00,                         // Mellanslag
  0x1e, 0x1c, 0x38,             // J U L
  0x00, 0x00,                   // Mellanslag
  0x38, 0x5c, 0x1c, 0x04, 0x6d, // L O V I S (S ser ut som 5)
  0x00, 0x00,                   // Mellanslag
  0x5c, 0x58, 0x74,             // O C H (C=c, H=h)
  0x00, 0x00,                   // Mellanslag
  0x77, 0x50, 0x1c, 0x04, 0x5e, // A R V I D (A=A, R=r, V=u, I=i, D=d)
  0x00, 0x00, 0x00, 0x00        // Slut-tömning
};

const uint8_t getReadyText[] = {
  0x00, 0x00, 0x00, 0x00,          // Space before
  0x3d, 0x79, 0x07,                // G E T
  0x00,                            // space
  0x50, 0x79, 0x77, 0x5e, 0x6e,    // R E A D Y 
  0x00, 0x00, 0x00, 0x00           // Space after
};

const uint8_t gameOverText[] = {
  0x00, 0x00, 0x00, 0x00, 
  0x3d, 0x77, 0x37, 0x79, // G A M E
  0x00, 
  0x3f, 0x1c, 0x79, 0x50, // O V E R
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

// JINGLE BELLS NOTER & LÄNGDER
int jingleMelody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};
int jingleDurations[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 8, 16,
  8, 8, 8, 8,
  4, 4
};
int jingleNotes = sizeof(jingleMelody) / sizeof(jingleMelody[0]);

int initialPitch = 500;
int pitchIncrement = 50;
int currentPitch = initialPitch;
int lightSensorValue = 0;
int potVal = 0;
int mappedPotVal = 0;

int activeLED = -1;
int previousActiveLED = -1;
unsigned long startTime = 0;
const unsigned long timeLimit = 8000;
bool gameInProgress = false;
bool gameLost = false;
int highScore = 0;
int currentStreak = 0;
bool melodyPlayed = false;
bool victoriousMelodyPlayed = false;

// Variables 4-seconds press
unsigned long whiteButtonPressStart = 0;
bool isPressingWhite = false;

// Variables for blue button (Interstellar)
unsigned long blueButtonPressStart = 0;
bool isPressingBlue = false;

// Interstellar-melody
// 192 notes, 16 measures á 12 notes
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
  // Här slutar not 192 (NOTE_D7 ersatt med NOTE_E7 i nästa steg i din plan)
  
  // De 6 avslutande fjärdedelarna
  NOTE_E7, NOTE_E6, NOTE_E6, NOTE_E6, NOTE_E6, NOTE_E6
};

int interstellarTotalNotes = 198;



int tempo = 240;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

// Melodier
int melody[] = { NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4, NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8, REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4 };
int gameOverMelody[] = { NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8, NOTE_G4,-12, NOTE_F4, -12, NOTE_G4,1 };
int victoriousMelody[] = { NOTE_A4, -2, NOTE_A4, 4, NOTE_C5,4,  NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, -2, NOTE_E4, -2, NOTE_A3, -2, NOTE_A3,4, NOTE_C4,4, NOTE_A3,4, NOTE_G3, 4, NOTE_F3, 4, NOTE_G3, 4, NOTE_F3,4, NOTE_E3,-2, NOTE_A5, -4, NOTE_A5, 8, NOTE_C6,8,  NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 2, NOTE_E5, -4, NOTE_A6, -4, NOTE_A6,8, NOTE_C7,8, NOTE_A6,8, NOTE_G6, 8, NOTE_F6, 8, NOTE_G6, 8, NOTE_F6,8, NOTE_E6, 8, NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8, NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8, NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8, NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6,  1 };

int gameOverNotes = sizeof(gameOverMelody) / sizeof(gameOverMelody[0]) / 2;
int victoriousMelodyNotes = sizeof(victoriousMelody) / sizeof(victoriousMelody[0])/2;
int melodyNotes = sizeof(melody) / sizeof(melody[0])/2;


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

void loop() {
  lightSensorValue = analogRead(lightSensor);

  // 1. Bed time
  if (lightSensorValue <= 200 && !gameInProgress) {
    pulsatingStarsEffect(3, 5, 6, A3, 10, 11, 5000, 255);
    return;
  }

  // 2. Musik-läge
  if (digitalRead(switchPin) == LOW && !gameInProgress) {
    potentiometerMusic();
    return;
  }

  // 3. Om spelet INTE körs (Vänteläge)
  if (!gameInProgress) {
    // Pulsera vita knappen
    int pulseValue = (sin(millis() * 0.003) * 127) + 128; 
    analogWrite(9, pulseValue); 
    
    // --- HANTERING AV BLÅ KNAPP (Index 1, Pin 0) ---
    if (digitalRead(buttonPins[1]) == LOW) {
      if (!isPressingBlue) {
        blueButtonPressStart = millis();
        isPressingBlue = true;
      } 
      
      unsigned long elapsed = millis() - blueButtonPressStart;

      // Visa nedräkning på displayen (3, 2, 1)
      if (elapsed > 3000) display.showNumberDec(1);
      else if (elapsed > 2000) display.showNumberDec(2);
      else if (elapsed > 1000) display.showNumberDec(3);

      if (elapsed > 4000) {
           runInterstellarMode(); 
           isPressingBlue = false; 
           blueButtonPressStart = 0;
      }
    } else {
      if (isPressingBlue) {
        // Om man släpper för tidigt, startar inget spel (eftersom bara vit startar)
        isPressingBlue = false;
        blueButtonPressStart = 0;
        display.showNumberDec(highScore);
      }
    }
    
    // Läs av vita knappen (pin 13)
    bool whiteButtonIsDown = (digitalRead(buttonPins[2]) == LOW);

    if (whiteButtonIsDown) {
      if (!isPressingWhite) {
        whiteButtonPressStart = millis(); // Starta klockan PRECIS nu
        isPressingWhite = true;
      } 
      
      unsigned long elapsed = millis() - whiteButtonPressStart;

      // Visa nedräkning
      if (elapsed > 3000) display.showNumberDec(1);
      else if (elapsed > 2000) display.showNumberDec(2);
      else if (elapsed > 1000) display.showNumberDec(3);

      // Har vi nått 4 sekunder?
      if (elapsed > 4000) {
           runChristmasMode(); 
           isPressingWhite = false; 
           whiteButtonPressStart = 0;
           display.showNumberDec(highScore);
      }
    } 
    else {
      // Om knappen släpps upp (Release)
      if (isPressingWhite) {
        unsigned long totalPressTime = millis() - whiteButtonPressStart;
        isPressingWhite = false;
        whiteButtonPressStart = 0;

        // Starta spel BARA om trycket var kortare än 4 sekunder
        if (totalPressTime < 4000) {
           analogWrite(9, 0); 
           startNewGame();
        } else {
           display.showNumberDec(highScore);
        }
      }
    }
  } 
  // 4. Om spelet KÖRS
  else {
    unsigned long currentTime = millis();
    if (currentTime - startTime >= timeLimit) {
      gameOver();
    }
    
    // Här kollar vi ALLA knappar för träff under spelets gång
    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        if (i == activeLED) {
          handleCorrectHit();
        }
      }
    }
  }
}


// --- JUL-LÄGET ---
void runChristmasMode() {
  display.clear();
  analogWrite(9, 255);
  while(digitalRead(buttonPins[2]) == LOW) { delay(10); }

  int currentNote = 0;
  int textStep = 0;
  int totalTextFrames = sizeof(godJulText) - 3;
  unsigned long lastNoteTime = 0;
  int currentNoteDuration = 0;

  while(true) {
    // 1. KOLLA AVBROTT (Tryck på vita knappen)
    if (digitalRead(buttonPins[2]) == LOW) {
      noTone(piezoPin);
      break;
    }

    // 2. HANTERA MUSIKEN (Icke-blockerande)
    if (millis() - lastNoteTime >= (unsigned long)currentNoteDuration) {
      noTone(piezoPin); // Stoppa förra noten
      
      // Beräkna nästa not
      int noteDur = 1200 / jingleDurations[currentNote];
      currentNoteDuration = noteDur * 1.3; // Paus inräknad
      
      tone(piezoPin, jingleMelody[currentNote], noteDur);
      
      lastNoteTime = millis();
      currentNote++;
      
      // Om låten är slut, börja om
      if (currentNote >= jingleNotes) currentNote = 0;

      // 3. HANTERA TEXTEN (Flytta ett steg varje gång en not byts)
      display.setSegments(godJulText + textStep);
      textStep++;
      if (textStep >= totalTextFrames) textStep = 0;
    }
  }
  display.clear();
  noTone(piezoPin);
}

bool scrollGodJulInterruptible() {
  int totalFrames = sizeof(godJulText) - 3;
  for (int i = 0; i < totalFrames; i++) {
    display.setSegments(godJulText + i);
    for(int k=0; k<30; k++){ 
      delay(10); 
      if(digitalRead(buttonPins[2]) == LOW) return false;
    }
  }
  return true;
}

// --- HELA STARTNEWGAME OCH DYLIKT ---
void startNewGame() {
  gameInProgress = true;
  currentStreak = 0;
  currentPitch = initialPitch;
  display.showNumberDec(0);
  for(int i=0; i<8; i++) digitalWrite(ledEightArray[i], HIGH);
  playSuperMarioMelody();
  for(int i=0; i<8; i++) digitalWrite(ledEightArray[i], LOW);
  victoriousMelodyPlayed = false;
  pickNextLED(); 
}

void handleCorrectHit() {
  playBeep(currentPitch);
  currentPitch += pitchIncrement;
  currentStreak++;
  display.showNumberDec(currentStreak);
  delay(150); 
  pickNextLED();
}

void pickNextLED() {
  int randomIndex;
  do {
    randomIndex = random(3);
  } while (randomIndex == previousActiveLED);
  turnOffAllLEDs();
  activeLED = randomIndex;
  digitalWrite(ledButtonArray[activeLED], HIGH);
  previousActiveLED = activeLED;
  startTime = millis();
}

void gameOver() {
  turnOffAllLEDs();
  
  if (currentStreak > highScore) {
    highScore = currentStreak;
    EEPROM.put(0, highScore);
    playVictoriousMelody(); 
  }

  display.clear();
  
  int textStep = 0;
  // Vi räknar ut hur många steg som behövs för att visa 4 tecken i taget genom hela arrayen
  int totalSteps = sizeof(gameOverText) - 3; 
  int noteIndex = 0;
  int totalNotes = gameOverNotes * 2;

  // DENNA LOOP KÖR TILLS TEXTEN ÄR BORTA
  while (textStep < totalSteps) {
    
    int dur = 180; // Standardfart

    // Spela musik om det finns noter kvar
    if (noteIndex < totalNotes) {
      int div = gameOverMelody[noteIndex + 1];
      dur = (div > 0) ? (wholenote / div) : (wholenote / abs(div) * 1.5);
      tone(piezoPin, gameOverMelody[noteIndex], dur * 0.8);
      noteIndex += 2;
    }

    // Uppdatera displayen - vi skickar in adressen till nuvarande steg
    display.setSegments(&gameOverText[textStep], 4, 0);
    
    textStep++; // Gå till nästa steg i texten

    delay(dur);
    noTone(piezoPin);
  }

  display.clear(); // Sista säkerhetsåtgärden

  // Resten av din kod (HighScore-visning etc.)
  for(int i=0; i<8; i++) digitalWrite(ledEightArray[i], LOW);
  flashGameLost();
  display.showNumberDec(highScore);
  
  gameInProgress = false;
}

// --- DENNA VAR TRASIG I DIN KOD, NU ÄR DEN FIXAD ---
void pulsatingStarsEffect(int ledPin1, int ledPin2, int ledPin3, int ledPin4, int ledPin5, int ledPin6, int pulseDuration, int maxBrightness) {
  unsigned long pStartTime = millis();
  while (millis() - pStartTime < pulseDuration) {
    unsigned long elapsedTime = millis() - pStartTime;
    // Nu är alla rader med!
    analogWrite(ledPin1, (sin(elapsedTime * 0.002) * 127) + 128);
    analogWrite(ledPin2, (sin(elapsedTime * 0.002 + 1) * 127) + 128);
    analogWrite(ledPin3, (sin(elapsedTime * 0.002 + 2) * 127) + 128);
    analogWrite(ledPin4, (sin(elapsedTime * 0.002 + 3) * 127) + 128);
    analogWrite(ledPin5, (sin(elapsedTime * 0.002 + 4) * 127) + 128);
    analogWrite(ledPin6, (sin(elapsedTime * 0.002 + 5) * 127) + 128);
    
    // Om man trycker på någon knapp bryts loopen direkt
    for(int i=0; i<3; i++) {
      if(digitalRead(buttonPins[i]) == LOW) return; 
    }
  }
  turnOffAllLEDs();
}

// --- ÖVRIGA FUNKTIONER ---
void flashGameLost() {
  for (int i = 0; i < 3; i++) {
    flashAllLEDs();
    delay(100);
    turnOffAllLEDs();
    delay(100);
  }
}
void flashAllLEDs() {
  for (int i = 0; i < 3; i++) digitalWrite(ledButtonArray[i], HIGH);
}
void turnOffAllLEDs() {
  for (int i = 0; i < 3; i++) digitalWrite(ledButtonArray[i], LOW);
}
void playBeep(int pitch) {
  tone(piezoPin, pitch, 100);
  delay(100);
  noTone(piezoPin);
}

void playSuperMarioMelody() {
  int noteIndex = 0;
  int totalNotes = melodyNotes * 2;
  int totalFrames = sizeof(getReadyText) - 3; 
  int textStep = 0;
  bool stopLooping = false;

  display.clear();

  while (noteIndex < totalNotes || textStep < totalFrames) {
    int dur = 150; 

    // 1. Spela musik så länge det finns noter kvar
    if (noteIndex < totalNotes) {
      int div = melody[noteIndex + 1];
      dur = (div > 0) ? (wholenote / div) : (wholenote / abs(div) * 1.5);
      tone(piezoPin, melody[noteIndex], dur * 0.9);
      noteIndex += 2;

      // Om vi närmar oss slutet av låten (ca 1.5 sekund kvar), sluta börja om texten
      if (noteIndex > totalNotes - 12) { 
        stopLooping = true;
      }
    }

    // 2. Uppdatera displayen
    // Vi skickar bara segment om vi är inom arrayens gränser
    if (textStep < totalFrames) {
      display.setSegments(getReadyText + textStep);
    }

    // 3. Hantera nästa steg för texten
    textStep++;
    
    // Om vi ska loopa och nått slutet, börja om
    if (!stopLooping && textStep >= totalFrames - 4) {
      textStep = 0; 
    }

    // 4. Vänta (om musiken är slut kör vi en fast hastighet för utrullningen)
    delay(dur);
    noTone(piezoPin);

    // Säkerhetsspärr: Om texten har rullat ut helt OCH musiken är slut, bryt allt
    if (stopLooping && textStep >= totalFrames) break;
  }

  display.clear(); 
}

void playVictoriousMelody(){
  int blinkDuration = 20;
  for (int thisNote = 0; thisNote < victoriousMelodyNotes * 2; thisNote = thisNote + 2) {
    divider = victoriousMelody[thisNote + 1];
    if (divider > 0) noteDuration = (wholenote) / divider;
    else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    tone(piezoPin, victoriousMelody[thisNote], noteDuration * 0.9);
    for (int ledIndex = 0; ledIndex < 11; ledIndex++) {
      digitalWrite(ledAllArray[ledIndex], HIGH);
      delay(blinkDuration);
      digitalWrite(ledAllArray[ledIndex], LOW);
    }
    delay(noteDuration - 3 * blinkDuration);
    noTone(piezoPin);
  }
}
void potentiometerMusic(){
  potVal = analogRead(A2);
  mappedPotVal = map(potVal, 0, 1023, 1, 11);
  int pitch = map(potVal, 0, 1023, initialPitch, initialPitch + pitchIncrement * 5);
  for (int i = 0; i < 11; i++) {
    digitalWrite(ledAllArray[i], i < mappedPotVal ? HIGH : LOW);
  }
  if (mappedPotVal >= 0) tone(piezoPin, pitch, 100);
  else noTone(piezoPin);
}

void runInterstellarMode() {
  const uint8_t stayText[] = {0x6d, 0x07, 0x77, 0x6e}; // S T A Y
  display.setSegments(stayText);
  int starIndex = 0;

  // Vänta tills knappen släpps
  while(digitalRead(buttonPins[1]) == LOW) delay(10);

  bool keepPlaying = true;
  while(keepPlaying) {
    for (int thisNote = 0; thisNote < interstellarTotalNotes; thisNote++) {
      if (digitalRead(buttonPins[1]) == LOW) { keepPlaying = false; break; }

      // Bestäm tempo baserat på 100 BPM
      int noteDuration = (thisNote < 192) ? 150 : 600;

      // Ljussynk:
      if (thisNote < 192) {
        // Flytta stjärnan varje fjärdedel (var 4:e not)
        if (thisNote % 4 == 0) {
          for(int i=0; i<6; i++) digitalWrite(ledStarsArray[i], LOW);
          digitalWrite(ledStarsArray[starIndex], HIGH);
          starIndex = (starIndex + 1) % 6;
        }
      } else {
        // Finalen: flytta stjärnan för varje tungt slag
        for(int i=0; i<6; i++) digitalWrite(ledStarsArray[i], LOW);
        digitalWrite(ledStarsArray[starIndex], HIGH);
        starIndex = (starIndex + 1) % 6;
      }

      // Spela noten - lite kortare än noteDuration för att få separation
      tone(piezoPin, interstellarMelody[thisNote], noteDuration * 0.8); 
      delay(noteDuration); 
      noTone(piezoPin);
    }
    keepPlaying = false; // Spela bara igenom en gång
  }
  turnOffAllLEDs();
  display.showNumberDec(highScore);
}
