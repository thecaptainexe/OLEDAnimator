#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OLEDAnimator.h>

OLEDAnimator animator;

FaceState states[] = { HAPPY, SURPRISED, BLINK, SLEEP };
uint8_t currentIndex = 0;

uint32_t lastChangeMs = 0;
const uint32_t changeIntervalMs = 2000;

void setup() {
  if (!animator.begin(0x3C, &Wire)) {
    while (true) { }
  }

  animator.express(states[currentIndex]);
}

void loop() {
  uint32_t now = millis();

  if (now - lastChangeMs >= changeIntervalMs) {
    lastChangeMs = now;
    currentIndex = (currentIndex + 1) % (sizeof(states) / sizeof(states[0]));
    animator.express(states[currentIndex]);
  }

  animator.update();
}
