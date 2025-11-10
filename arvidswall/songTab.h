


//Den här funktionen spelar lilla snigel

    void playLillaSnigel(){
  int melody[] = {NOTE_G4, NOTE_G4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_D4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_C4};

int noteDurations[] = {4, 4, 4, 4, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 2}; 

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 25; thisNote++) {
  int noteDuration = 1500 / noteDurations[thisNote];
  tone(A3, melody[thisNote], noteDuration);
  int pauseBetweenNotes = noteDuration * 1.10;
  delay(pauseBetweenNotes);
  noTone(A3);
  
}
    } 

     void playBlinkaLilla(){
    
      int melody[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4};

int noteDurations[] = {4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2};  

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 42; thisNote++) {
  int noteDuration = 1000 / noteDurations[thisNote];
  tone(A3, melody[thisNote], noteDuration);
  int pauseBetweenNotes = noteDuration * 1.30;
  delay(pauseBetweenNotes);
  noTone(A3);
  }
     }

     void playBleedTheFreak(){
      
      int melody[] = {NOTE_B3, NOTE_FS4, NOTE_B4, NOTE_G3, NOTE_FS4, NOTE_B4, NOTE_A3, NOTE_B4, NOTE_E3, NOTE_B3, NOTE_FS4, NOTE_B4, NOTE_FS4, NOTE_B3, NOTE_B4, NOTE_FS4, NOTE_B3, NOTE_FS4, NOTE_B4, NOTE_G3, NOTE_FS4, NOTE_B4, NOTE_A3, NOTE_B4, NOTE_A3, NOTE_G3, NOTE_E3, NOTE_B3, NOTE_FS4, NOTE_B4, NOTE_FS4, NOTE_B3, NOTE_FS4, NOTE_B4, NOTE_G3, NOTE_FS4, NOTE_B4, NOTE_A3, NOTE_B4, NOTE_E3, NOTE_B3, NOTE_FS4, NOTE_B4, NOTE_FS4, NOTE_B3, NOTE_B4, NOTE_FS4, NOTE_B3, NOTE_FS4, NOTE_B4, NOTE_G3, NOTE_FS4, NOTE_B4, NOTE_A3, NOTE_B4, NOTE_A3, NOTE_G3, NOTE_E3};
int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1};  

// iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 58; thisNote++) {
  int noteDuration = 1000 / noteDurations[thisNote];
  tone(A3, melody[thisNote], noteDuration);
  int pauseBetweenNotes = noteDuration * 1.1;
  delay(pauseBetweenNotes);
  noTone(A3);
  }
     }
