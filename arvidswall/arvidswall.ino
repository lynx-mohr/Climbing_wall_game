#include "pitches.h"  // Include the pitches library
#include <EEPROM.h>   // Include the EEPROM library

const int ledButtonArray[] = {A4, A5, A3};  // Pins for LEDs
const int ledStarsArray[] = {3, 6, 9, 10}; 
const int ledEightArray[] = {3, 5, 6, 7, 8, 9, 10, 12};
const int ledAllArray[] = {3, 5, 6, 7, 8, 9, 10, 12, A4, A5, A3};
const int buttonPins[] = {4, 0, 13}; // Pins for buttons
const int piezoPin = 11;  // Pin for piezo buzzer
const int microphonePin = A0;
byte lightSensor = A1;
byte potPin = A2;
byte switchPin = 2;

int initialPitch = 500;
int pitchIncrement = 50;

bool buttonState[3] = {HIGH, HIGH, HIGH};  // Initial state of buttons

// Current pitch for the piezo buzzer
int currentPitch = initialPitch;
int lightSensorValue = 0; 


//Deklarera en variabel som håller potentiometervärde
int potVal = 0;

//Deklarera en variabel som är en mappad potVal
int mappedPotVal = 0;


int activeLED = -1;            // Index of the currently active LED (-1 means none)
int previousActiveLED = -1; // Initialize to an invalid value
unsigned long startTime = 0;   // Time when an LED is lit
const unsigned long timeLimit = 6000;  // Time limit in milliseconds (6 seconds)
bool gameInProgress = false;   // Flag to track if a game is in progress
bool gameLost = false;         // Flag to track if the game was lost
int gameOverFlashCount = 0;    // Counter for the game over flash sequence
bool melodyPlayed = false;     // Flag to track if melody was played
int highScore = 0;
int currentStreak = 0;  // Current streak of correct button presses
bool victoriousMelodyPlayed = false;         // High score

int tempo = 200;
// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;


// Melody notes for Super Mario theme
int melody[] = {

  // Super Mario Bros theme
  // Score available at https://musescore.com/user/2123/scores/2145
  // Theme by Koji Kondo
  
  
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, //1
  NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, 
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

};

// Melody notes for Game Over from Super Mario
int gameOverMelody[] = {
  NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, //45
  NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
  NOTE_G4,-12, NOTE_F4, -12,
  NOTE_G4,1,   
};


// Melody notes for Victorious Melody
// Melody notes for Victorious Melody :Seven Nation Army
int victoriousMelody[] = {
  NOTE_A4, -2, NOTE_A4, 4, NOTE_C5,4,  NOTE_A4, 4,
  NOTE_G4, 4, NOTE_F4, -2, NOTE_E4, -2, 
  
  NOTE_A3, -2, NOTE_A3,4, NOTE_C4,4, NOTE_A3,4,
  NOTE_G3, 4, NOTE_F3, 4, NOTE_G3, 4, NOTE_F3,4, NOTE_E3,-2,

    NOTE_A5, -4, NOTE_A5, 8, NOTE_C6,8,  NOTE_A5, 8,
  NOTE_G5, 8, NOTE_F5, 2, NOTE_E5, -4, 
  
  NOTE_A6, -4, NOTE_A6,8, NOTE_C7,8, NOTE_A6,8,
  NOTE_G6, 8, NOTE_F6, 8, NOTE_G6, 8, NOTE_F6,8, NOTE_E6, 8,

  NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8, 
    NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8, 
    NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6, 8,  NOTE_F6, 8,
    NOTE_E6, 8,  NOTE_F6, 8,   NOTE_E6,  1,
};

 int gameOverNotes = sizeof(gameOverMelody) / sizeof(gameOverMelody[0]) / 2;
int victoriousMelodyNotes = sizeof(victoriousMelody) / sizeof(victoriousMelody  [0])/2;
int melodyNotes = sizeof(melody) / sizeof(melody  [0])/2;



void setup() {

  Serial.begin(9600); 
  
    for (int i = 0; i < 3; i++) {
    pinMode(ledButtonArray[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
    }
    for (int i = 0; i < 11; i++){
    pinMode(ledAllArray[i], OUTPUT);
    digitalWrite(ledAllArray[i], LOW);  // Turn off all LEDs initially
  }

pinMode(lightSensor, INPUT);
pinMode(potPin, INPUT);

  randomSeed(analogRead(0));  // Seed the random number generator

int storedHighScore;
  EEPROM.get(0, storedHighScore);
  highScore = storedHighScore;

}


void loop() {

//läs ljussensor. Om nästan mörkt, pulserar PWM-LEDs
lightSensorValue = analogRead(lightSensor);

if(lightSensorValue <= 300){

pulsatingStarsEffect(3, 6, 9, 10, 5000, 255);  // Example usage with pins 3, 6, 9, 10, pulse duration of 5 seconds, and maximum brightness of 255

}

while(digitalRead(switchPin) == 0){

potentiometerMusic();

//for(int i = 0; i < 12; i++){
//digitalWrite(ledAllArray[i], LOW);

//}
}


Serial.print("Current Streak: ");
Serial.print(currentStreak);
Serial.print("  Highscore: ");
Serial.println(highScore);


  if (!gameInProgress) {
    waitForGameStart();
  }
  
  unsigned long currentTime = millis();

if (gameInProgress) {
  if (activeLED != -1) {
    if (currentTime - startTime >= timeLimit) {

      gameOver();
    }
  }
  
  for (int i = 0; i < 3; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
       
    // Check if the button is pressed for the first time
    if (!gameInProgress) {
      startNewGame();
      break;
    }

      if (i == activeLED) {
        // Turn off the LED and play the beep with the current pitch
        digitalWrite(ledButtonArray[i], LOW);
        playBeep(currentPitch);

        // Increment the pitch for the next correct button press
        currentPitch += pitchIncrement;

       

        digitalWrite(ledButtonArray[i], LOW);  // Turn off the pressed LED
           currentStreak++;
        activeLED = -1;  // Reset activeLED
     
        gameInProgress = false;  // Reset game in progress

   delay(30);
       
      }
    }
    }
  }
}

void waitForGameStart() {
  if (gameLost) {
    gameLost = false;
    gameOverFlashCount = 0;
    turnOffAllLEDs();
    currentPitch = 500;
  

  }

  //melodyPlayed = false;  // Reset the melodyPlayed flag
 
  for (int i = 0; i < 3; i++) {
    if (digitalRead(buttonPins[i]) == LOW) { //always start game by pressing the white button
      startNewGame();
      melodyPlayed = true; // Reset melodyPlayed flag
      victoriousMelodyPlayed = false; // Reset victoriousMelodyPlayed flag
      break;
    }
  }
}

void startNewGame() {
 

//activeLED = -1;  // Reset activeLED before starting a new game
for(int i=0; i<8; i++){

  digitalWrite(ledEightArray[i], HIGH);
}

   if (!melodyPlayed) {
    playSuperMarioMelody();
    melodyPlayed = true; // Set melody played flag
  }

turnOffAllLEDs();
  startTime = millis();  // Record the start time
  gameInProgress = true;  // Set game in progress


// Generate a new random index for the active LED, different from the previous one
  int randomIndex;
  do {
    randomIndex = random(3);
  } while (randomIndex == previousActiveLED);

  activeLED = randomIndex;
  digitalWrite(ledButtonArray[randomIndex], HIGH);  // Turn on the random LED

  // Update the previous active LED index
  previousActiveLED = activeLED;

}

void gameOver() {

   if (gameInProgress) {
 if (currentStreak > highScore) {
      highScore = currentStreak;
    
     EEPROM.put(0, highScore); // Store new high score in EEPROM

      if (!victoriousMelodyPlayed) {
    playVictoriousMelody();     // Play victorious melody
    victoriousMelodyPlayed = true; // Set melody played flag
  }
         
    }
  }
  if (!gameLost) {
    gameLost = true;
    
    playGameOverMelody(); // Play "Game Over" melody before flashing
    for(int i=0; i<8; i++){

  digitalWrite(ledEightArray[i], LOW);
}
    flashGameLost();
 
  
  }
  
  currentStreak = 0; // Reset the current streak after game over
  melodyPlayed = false;

gameInProgress = false;
}

void flashGameLost() {
  for (int i = 0; i < 5; i++) {
    flashAllLEDs();
    delay(100);
    turnOffAllLEDs();
    delay(100);
  }
}

void flashAllLEDs() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledButtonArray[i], HIGH);
  }
}


void turnOffAllLEDs() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledButtonArray[i], LOW);
  }
}


void playVictoriousMelody(){

//int noteDuration = 200;     // Adjust this value as needed
  int blinkDuration = 20;     // Adjust this value for LED blinking speed


 // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < victoriousMelodyNotes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = victoriousMelody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }
    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(piezoPin, victoriousMelody[thisNote], noteDuration * 0.9);

   for (int ledIndex = 0; ledIndex < 11; ledIndex++) {
      digitalWrite(ledAllArray[ledIndex], HIGH);  // Turn on the LED
      delay(blinkDuration);  // Keep the LED on for the note duration
      digitalWrite(ledAllArray[ledIndex], LOW);   // Turn off the LED
    }
    
    // Wait for the specief duration before playing the next note.
    delay(noteDuration - 3 * blinkDuration);

    // stop the waveform generation before the next note.
    noTone(piezoPin);
  }
 }


void playGameOverMelody(){

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < gameOverNotes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = gameOverMelody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(piezoPin, gameOverMelody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(piezoPin);
  }
 }

void playSuperMarioMelody(){

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < melodyNotes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(piezoPin, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(piezoPin);
  }
 }
 
void pulsatingStarsEffect(int ledPin1, int ledPin2, int ledPin3, int ledPin4, int pulseDuration, int maxBrightness) {
  unsigned long startTime = millis();
  
  while (millis() - startTime < pulseDuration) {
    unsigned long elapsedTime = millis() - startTime;
    
    // Calculate the brightness based on a sine wave to achieve pulsating effect
    int brightness1 = (int)(sin(elapsedTime * 0.001) * maxBrightness + maxBrightness);
    int brightness2 = (int)(sin(elapsedTime * 0.001 + PI / 2) * maxBrightness + maxBrightness);
    int brightness3 = (int)(sin(elapsedTime * 0.001 + PI) * maxBrightness + maxBrightness);
    int brightness4 = (int)(sin(elapsedTime * 0.001 + 3 * PI / 2) * maxBrightness + maxBrightness);
    
    analogWrite(ledPin1, brightness1);
    analogWrite(ledPin2, brightness2);
    analogWrite(ledPin3, brightness3);
    analogWrite(ledPin4, brightness4);
  }
  
  // Turn off all LEDs at the end
  analogWrite(ledPin1, 0);
  analogWrite(ledPin2, 0);
  analogWrite(ledPin3, 0);
  analogWrite(ledPin4, 0);
}

// Function to play a beep on the piezo buzzer with the specified pitch
void playBeep(int pitch) {
  tone(piezoPin, pitch, 100); // Play the beep for 100 ms (adjust as needed)
  delay(100); // Add a short delay before stopping the tone
  noTone(piezoPin);
}


void potentiometerMusic(){

potVal = analogRead(A2);
  mappedPotVal = map(potVal, 0, 1023, 1, 10);

  int pitch = map(potVal, 0, 1023, initialPitch, initialPitch + pitchIncrement * 5);

  for (int i = 0; i < 11; i++) {
    digitalWrite(ledAllArray[i], i < mappedPotVal ? HIGH : LOW);
  }

  if (mappedPotVal >= 0) {
    tone(piezoPin, pitch, 700);
  } else {
    noTone(piezoPin);
  }

}

