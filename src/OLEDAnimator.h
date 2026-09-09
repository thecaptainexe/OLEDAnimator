#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <pgmspace.h>

enum FaceState {
  HAPPY,
  SURPRISED,
  BLINK,
  SLEEP,
  SAD,
  LOOK_LEFT,
  LOOK_RIGHT
};

enum TransitionType {
  TRANSITION_NONE,
  SLIDE_LEFT,
  SLIDE_RIGHT,
  WIPE
};

class OLEDAnimator {
public:
  OLEDAnimator(uint8_t width = 128, uint8_t height = 64);

  bool begin(uint8_t i2c_addr = 0x3C, TwoWire *wire = &Wire);
  void update();

  void express(FaceState state);

  void showBootAnimation(const char* title, uint16_t durationMs = 2000);
  void drawSpinner(uint8_t centerX, uint8_t centerY, uint8_t radius);
  void drawProgressBar(uint8_t percent, const char* label = nullptr);

  void playSprite(const uint8_t* const frames[],
                  uint8_t totalFrames,
                  uint8_t width,
                  uint8_t height,
                  uint8_t fps = 15);

  void transitionTo(void (*drawFunc)(Adafruit_SSD1306&),
                    TransitionType type = SLIDE_LEFT);

  Adafruit_SSD1306& display();

private:
  Adafruit_SSD1306 _display;
  uint8_t _width;
  uint8_t _height;

  uint8_t _fps;
  uint32_t _frameIntervalMs;
  uint32_t _lastFrameMs;

  FaceState _currentFace;
  bool _faceActive;

  bool _bootActive;
  const char* _bootTitle;
  uint16_t _bootDurationMs;
  uint32_t _bootStartMs;

  bool _spriteActive;
  const uint8_t* const* _spriteFrames;
  uint8_t _spriteTotalFrames;
  uint8_t _spriteWidth;
  uint8_t _spriteHeight;
  uint8_t _spriteFps;
  uint32_t _spriteFrameIntervalMs;
  uint32_t _spriteLastFrameMs;
  uint8_t _spriteCurrentFrame;

  bool _transitionActive;
  TransitionType _transitionType;
  void (*_transitionDrawFunc)(Adafruit_SSD1306&);
  uint32_t _transitionStartMs;
  uint16_t _transitionDurationMs;

  void render();
  void renderFace();
  void renderBoot();
  void renderSprite();
  void renderTransition();

  void drawFaceEyesMouth(FaceState state, uint32_t now);
};
