# 🕹️ Arduino OLED Tetris Arcade Game

An interactive, space-optimized recreation of the classic arcade game **Tetris** designed to run smoothly on an **Arduino Uno** using a **0.96" I2C OLED Display** and a standard **Analog Joystick Module**.

---

## 🚀 Features
* **Automated Gravity Engine:** Pieces descend naturally with progressive soft-drop functions.
* **Full Bit-Packed Rotation Matrix:** Houses all 7 classic Tetromino profiles (I, J, L, O, S, T, Z) optimized for dynamic flash storage.
* **Proximity Collision Detection:** Advanced boundary checks prevent wall clipping.
* **Score & Line Tracker:** Clear entire horizontal rows to rack up points and watch your stats update live on the dashboard panel.
* **Memory-Optimized Flow:** Restructured architecture using custom function segment bridges and `PROGMEM` data flags to fit safely within the strict limitations of AVR dynamically allocated RAM.
* **Integrated 8-Bit Audio Engine:** Native Game Boy inspired tone sweeps mapped via non-blocking frequency arrays on Digital Pin 6.


---

## 🔌 Hardware Wiring Configuration

| Component | Component Pin | Arduino Pin |
| :--- | :--- | :--- |
| |Buzzer | +ve | 6 |
| **0.96" I2C OLED Display** | GND | GND |
| | VDD / VCC | 5V |
| | SCK / SCL | A5 |
| | SDA | A4 |
| **Analog Joystick Module** | GND | GND |
| | +5V / VCC | 5V |
| | VRx (X-Axis) | A0 (Horizontal Slide) |
| | VRy (Y-Axis) | A1 (Soft Drop) |
| | SW (Switch Button)| Digital Pin 3 (Block Rotate) |

---

## 🕹️ Controls Layout
* **Slide Left / Right:** Tilt the joystick knob along the X-axis (`A0`).
* **Soft Drop Speed-up:** Pull the joystick back along the Y-axis (`A1`) to engage fast-drop gravity.
* **Rotate Piece:** Click the thumbstick down firmly like a tactical button (`Pin 3`) to flip the active block 90° clockwise.
* **Restart Game:** If you trigger a stack overflow Game Over screen, click the button down once to purge the array and boot onto a fresh board.

---

## 🛠️ Required Libraries
Before uploading the sketch, ensure you install these modules via the Arduino IDE Library Manager:
1. `Adafruit_SSD1306`
2. `Adafruit_GFX_Library`
3. `Wire` (Standard Built-in Library)
