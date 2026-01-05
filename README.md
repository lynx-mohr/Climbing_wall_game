# 🧗‍♂️ Interactive Climbing Wall Game `v2.0`

> **Tinker, Brewer, Maker, Scout**

Welcome to the **Interactive Climbing Wall Game**! This project transforms a standard climbing wall into a high-tech arcade experience. It challenges climbers with physical activity, reaction speed, and memory training through integrated RGB buttons, custom soundscapes, and intense bonus rounds.

---

## ✨ Features

### 📶 Difficulty Levels

| Level | Description | Timing |
| --- | --- | --- |
| **NOOB** | Perfect for beginners and kids | Generous |
| **HARD** | Requires agility and planning | Fast |
| **HACKER** | Elite level tempo | Extreme |

### ⚡ Dynamic Bonus Rounds

*Triggered every 30 points to keep the adrenaline pumping!*

* **🧠 Memory Bonus:** A sequence flashes on the wall—repeat it correctly under pressure while the *Jaws* theme speeds up!
* **☢️ Turbo Reactor:** A rapid-fire reaction test. "Stop the reactor" by hitting buttons in an increasingly fast succession.
* **💰 Coin Bonanza:** A random arcade-style event. Collect as many "coins" (extra points) as possible before the timer expires.

### 🎶 Atmosphere & Easter Eggs

* **Iconic Soundtracks:** High-quality 8-bit renditions of *Super Mario*, *Harry Potter*, and *Interstellar*.
* **🎄 Christmas Mode:** Toggle "Jingle Bells" with a scrolling "GOD JUL" (Swedish for Merry Christmas) display.
* **🌌 Starry Night:** When the room goes dark, an LDR sensor activates an automatic "pulsating stars" light show.
* **🏆 Permanent Highscores:** Your records are safe! Highscores are saved to the Arduino's EEPROM.

---

## 🛠 Hardware Requirements

### Core Components

* **Microcontroller:** Arduino Uno (or compatible)
* **Display:** TM1637 4-digit 7-segment display
* **Inputs:** 3x Large Arcade buttons with integrated LEDs
* **Audio:** 1x Piezo buzzer

### Sensors & Controls

* **LDR:** For ambient light detection
* **Potentiometer:** For the interactive music mode
* **Toggle Switch:** To switch between game/utility modes
* **LEDs:** Extra strips or diodes for "Star" effects

---

## 🚀 Installation & Setup

1. **Libraries:** Install the `TM1637Display` library via the Arduino Library Manager.
2. **Files:** Ensure the `pitches.h` file is located in the same directory as the `.ino` sketch.
3. **Wiring:** Connect the hardware according to the following pinout:

| Component | Arduino Pin |
| --- | --- |
| **Buttons (RED, BLUE, WHITE)** | Pins 4, 0, 13 |
| **Display CLK** | Pin 1 |
| **Display DIO** | Pin 12 |
| **Buzzer (Piezo)** | Pin A0 |

> [!WARNING]
> Since the display uses **Pin 1 (TX)**, you may need to disconnect the CLK wire while uploading code to the Arduino.

---

## 🕹 How to Play

### 1. The Menu

* **Select Difficulty:** Hold the **White Button** for 4 seconds. When the countdown ends, pick:
* **White** -> NOOB
* **Red** -> HARD
* **Blue** -> HACKER


* **Special Themes:** From the main menu, hold **Red** for *Harry Potter* or **Blue** for *Interstellar*.

### 2. The Game

* **Start:** Tap the **White Button** once.
* **The Climb:** Hit the button that lights up before the time runs out.
* **Audio Feedback:** Listen closely! The faster the "ticking" sound, the less time you have left!

---

du att jag gör något mer?** Jag kan till exempel lägga till en sektion för "Future Improvements" om du har planer på att bygga ut väggen ännu mer senare!