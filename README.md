# Climbing Wall Game

**Tinker, Brewer, Maker, Scout**

A fun interactive Arduino game inspired by classic reaction and timing games, featuring LEDs, buttons, and music. Perfect for makers who love electronics, games, and a bit of challenge!

---

## **Hardware Required**

* **Arduino Uno**
* **Piezo Buzzer** (for melodies and beeps)
* **LEDs**
* **LED-arcade-buttons

  * Button LEDs (`ledButtonArray`)
  * Star LEDs (`ledStarsArray`)
  * Wall LEDs (`ledEightArray`)
* **Buttons** (`buttonPins`)
* **Light Sensor** (`A1`)
* **Potentiometer** (`A2`)
* **Switch** (`D2`)

---

## **Game Description**

1. **Start the Game**

   * Press any button to start.
   * A random LED lights up — this is the target button.
   * The **Super Mario melody** plays at the start.

2. **Play the Game**

   * Press the correct button before time runs out (6 seconds).

   * Correct button:

     * LED turns off
     * Piezo buzzer beeps with increasing pitch
     * Score streak increases
     * Next LED lights up randomly

   * Wrong button or timeout:

     * Game over sequence triggered
     * **Game Over melody** plays
     * LEDs flash to signal failure

3. **High Score**

   * If you beat the previous high score:

     * EEPROM saves the new high score
     * **Victorious melody** plays

4. **Special Features**

   * **Light Sensor Effect**: If it's dark, star LEDs pulsate for visual effect.
   * **Potentiometer Music**: Hold the switch to play tones with the potentiometer and see LED feedback.

---

## **Melodies**

* **Super Mario Theme** → starts the game
* **Game Over Melody** → triggered on losing
* **Victorious Melody** → plays on high score

---

## **EEPROM Saving**

High scores are saved in EEPROM so they persist even after powering off the Arduino.

---

## **Notes**

* The game is perfect for makers who enjoy combining electronics, music, and gameplay.
* LEDs provide visual feedback, piezo buzzer provides audio feedback, and buttons test your reaction skills.
* Potentiometer and light sensor add extra interactivity for creative experiments.

---

If you want, I can also **make a shorter, punchy “one-paragraph version”** for the top of your GitHub README that immediately grabs attention — perfect for visitors who just glance at the repo.

Do you want me to do that too?
