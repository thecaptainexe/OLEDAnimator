#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OLEDAnimator.h>

OLEDAnimator animator;

uint32_t lastProgressMs = 0;
uint8_t progress = 0;

void setup() {
  if (!animator.begin(0x3C, &Wire)) {
    while (true) { }
  }

  animator.showBootAnimation("CaptainEXE Boot", 3000);
}

void loop() {
  uint32_t now = millis();

  if (now - lastProgressMs >= 200) {
    lastProgressMs = now;
    progress += 5;
    if (progress > 100) progress = 0;
  }

  Adafruit_SSD1306 &disp = animator.display();
  disp.clearDisplay();

  animator.drawSpinner(64, 24, 10);
  animator.drawProgressBar(progress, "Loading");

  disp.display();

  animator.update();
}
