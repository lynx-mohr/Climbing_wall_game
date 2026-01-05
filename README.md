🧗‍♂️ Interactive Climbing Wall Game (v2.0)

-Tinker, Brewer, Maker, Scout-


This is an Arduino-based reaction game designed specifically for climbing walls. The game challenges the climber through physical activity, speed, and memory training using integrated RGB buttons, sound effects, and interactive bonus rounds.


✨ Features
Three Difficulty Levels:

NOOB: Generous timing for beginners.

HARD: Requires quick movements and agility on the wall.

HACKER: Extreme tempo for elite climbers.

Dynamic Bonus Rounds (Triggered every 30 points):

Memory Bonus: A sequence is displayed—the climber must repeat it under time pressure (accompanied by the "Jaws" theme!).

Turbo Reactor: A speed test where the climber must "stop a reactor" by hitting buttons in rapidly increasing succession.

Coin Bonanza: A random bonus event that lets the player collect "coins" (extra points) in a high-speed arcade style.

Atmosphere & Easter Eggs:

Iconic Soundtracks: Built-in melodies from Super Mario, Harry Potter, and Interstellar.

Christmas Mode: "Jingle Bells" with a scrolling "GOD JUL" (Merry Christmas) text.

Starry Night: An automatic light show activated via a Light Dependent Resistor (LDR) when the room goes dark.

Permanent Highscores: Uses Arduino EEPROM to save records for each difficulty level even after power-off.

🛠 Hardware Requirements
Microcontroller: Arduino Uno (or compatible).

Display: TM1637 4-digit 7-segment display.

Inputs: 3x Arcade buttons with built-in LEDs.

Audio: 1x Piezo buzzer.

Sensors: LDR (Light sensor), Potentiometer (for music mode), Toggle Switch.

LEDs: Extra LED strips or diodes for "Star" effects.

🚀 Installation & Setup
Install the TM1637Display library via the Arduino Library Manager.

Ensure the pitches.h file is in the same directory as the .ino sketch.

Wire the hardware according to the pin configuration in the code:

Buttons: Pins 4, 0, 13.

Display: CLK Pin 1, DIO Pin 12.

Buzzer: Pin A0.

🕹 How to Play
Select Difficulty: Hold the White Button for 4 seconds (a countdown will appear). Select your level: White (Noob), Red (Hard), or Blue (Hacker).

Start Game: Tap the White Button once to begin.

The Climb: Hit the button that lights up before the time runs out. The higher your streak, the faster the "ticking" sound becomes!

Special Modes: From the menu, hold the Red or Blue button to play the Harry Potter or Interstellar themes.
