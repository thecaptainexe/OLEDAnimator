# OLEDAnimator

![Arduino Library](https://img.shields.io/badge/Arduino-OLEDAnimator-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Arduino-orange.svg)

A lightweight, non-blocking OLED animation and UI library for Arduino, built for 128x64 SSD1306/SH1106 displays using the Adafruit GFX and Adafruit SSD1306 libraries.

OLEDAnimator brings your OLED to life with:

- Expressive robot faces
- Boot animations and loaders
- Spinners and progress bars
- Simple screen transitions
- Bitmap sprite playback from PROGMEM

---

## Features

- **Non-blocking engine** using `millis()` and a target FPS (default 30 FPS).
- **Expressive faces**: `HAPPY`, `SURPRISED`, `BLINK`, `SLEEP`, `SAD`, `LOOK_LEFT`, `LOOK_RIGHT`.
- **Loaders & widgets**: boot animation, spinner, progress bar.
- **Sprite player**: plays PROGMEM bitmap frames at a chosen FPS.
- **Screen transitions**: basic slide and wipe modes.
- **Designed for low-memory boards** like ATmega328P (Uno/Nano).

---

## Hardware Wiring (I2C)

Typical Arduino + 128x64 I2C OLED (SSD1306):

| OLED Pin | Arduino Uno/Nano |
|----------|------------------|
| VCC      | 5V (or 3.3V)     |
| GND      | GND              |
| SCL      | A5               |
| SDA      | A4               |

On other boards, use the dedicated SDA/SCL pins.

---

## Installation

### Install via Library Manager

1. Open **Arduino IDE**.
2. Go to **Sketch → Include Library → Manage Libraries…**
3. Search for **OLEDAnimator** (once published).
4. Click **Install**.

### Install via ZIP Import

1. Download or clone the repository.
2. Zip the `OLED Animator` folder.
3. In Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library…**
4. Select the zipped folder.

---

## Quickstart Example

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OLEDAnimator.h>

OLEDAnimator animator;

void setup() {
  if (!animator.begin(0x3C, &Wire)) {
    while (true) { }
  }

  animator.express(HAPPY);
}

void loop() {
  animator.update();
}
```

---

## API Reference

### Class: `OLEDAnimator`

#### Core Methods

- **`bool begin(uint8_t i2c_addr = 0x3C, TwoWire *wire = &Wire);`**  
  Initializes the display and internal timing engine.

- **`void update();`**  
  Non-blocking tick function. Call once per `loop()`.

---

### Expressive Faces

```cpp
enum FaceState {
  HAPPY,
  SURPRISED,
  BLINK,
  SLEEP,
  SAD,
  LOOK_LEFT,
  LOOK_RIGHT
};
```

- **`void express(FaceState state);`**  
  Sets the current face expression.

---

### Loaders & Widgets

- **`void showBootAnimation(const char* title, uint16_t durationMs = 2000);`**  
  Displays a timed boot animation with a progress bar.

- **`void drawSpinner(uint8_t centerX, uint8_t centerY, uint8_t radius);`**  
  Draws a rotating spinner.

- **`void drawProgressBar(uint8_t percent, const char* label = nullptr);`**  
  Draws a horizontal progress bar.

---

### Sprite Player

```cpp
void playSprite(
  const uint8_t* const frames[],
  uint8_t totalFrames,
  uint8_t width,
  uint8_t height,
  uint8_t fps = 15
);
```

- Plays PROGMEM bitmap frames at a chosen FPS.
- Frames are drawn centered on the display.

---

### Screen Transitions

```cpp
enum TransitionType {
  TRANSITION_NONE,
  SLIDE_LEFT,
  SLIDE_RIGHT,
  WIPE
};
```

- **`void transitionTo(void (*drawFunc)(Adafruit_SSD1306&), TransitionType type = SLIDE_LEFT);`**  
  Performs a simple transition, then calls the provided draw function.

---

## Examples Included

### 01 — Expressive Faces  
Cycles through `HAPPY`, `SURPRISED`, `BLINK`, `SLEEP` using non-blocking timers.

### 02 — Loaders & Widgets  
Demonstrates boot animation, spinner, and progress bar.

### 03 — Custom Sprites  
Shows how to store and animate a 4-frame 32×32 sprite in PROGMEM.

---

## Dependencies

- **Adafruit GFX Library**  
- **Adafruit SSD1306**

---

## Notes

- No `delay()` is used anywhere in the animation engine.
- All timing is based on `millis()` for smooth non-blocking behavior.
- Designed to be compatible with Arduino Library Manager requirements.

---

## Author

**CaptainEXE**  (thecaptainexe)
