#include "OLEDAnimator.h"

OLEDAnimator::OLEDAnimator(uint8_t width, uint8_t height)
  : _display(width, height, &Wire, -1),
    _width(width),
    _height(height),
    _fps(30),
    _frameIntervalMs(1000 / 30),
    _lastFrameMs(0),
    _currentFace(HAPPY),
    _faceActive(false),
    _bootActive(false),
    _bootTitle(nullptr),
    _bootDurationMs(0),
    _bootStartMs(0),
    _spriteActive(false),
    _spriteFrames(nullptr),
    _spriteTotalFrames(0),
    _spriteWidth(0),
    _spriteHeight(0),
    _spriteFps(15),
    _spriteFrameIntervalMs(1000 / 15),
    _spriteLastFrameMs(0),
    _spriteCurrentFrame(0),
    _transitionActive(false),
    _transitionType(TRANSITION_NONE),
    _transitionDrawFunc(nullptr),
    _transitionStartMs(0),
    _transitionDurationMs(400) {
}

bool OLEDAnimator::begin(uint8_t i2c_addr, TwoWire *wire) {
  _display.setI2CAddress(i2c_addr);
  _display.begin(SSD1306_SWITCHCAPVCC, i2c_addr);
  _display.clearDisplay();
  _display.display();
  _lastFrameMs = millis();
  return true;
}

Adafruit_SSD1306& OLEDAnimator::display() {
  return _display;
}

void OLEDAnimator::update() {
  uint32_t now = millis();
  if (now - _lastFrameMs >= _frameIntervalMs) {
    _lastFrameMs = now;
    render();
  }

  if (_spriteActive && _spriteFrames != nullptr && _spriteTotalFrames > 0) {
    if (now - _spriteLastFrameMs >= _spriteFrameIntervalMs) {
      _spriteLastFrameMs = now;
      _spriteCurrentFrame = (_spriteCurrentFrame + 1) % _spriteTotalFrames;
    }
  }
}

void OLEDAnimator::express(FaceState state) {
  _currentFace = state;
  _faceActive = true;
}

void OLEDAnimator::showBootAnimation(const char* title, uint16_t durationMs) {
  _bootTitle = title;
  _bootDurationMs = durationMs;
  _bootStartMs = millis();
  _bootActive = true;
}

void OLEDAnimator::drawSpinner(uint8_t centerX, uint8_t centerY, uint8_t radius) {
  uint32_t now = millis();
  float angle = (now % 1000) / 1000.0f * TWO_PI;
  uint8_t x = centerX + radius * cos(angle);
  uint8_t y = centerY + radius * sin(angle);

  _display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);
  _display.fillCircle(x, y, 2, SSD1306_WHITE);
}

void OLEDAnimator::drawProgressBar(uint8_t percent, const char* label) {
  uint8_t barX = 10;
  uint8_t barY = _height - 16;
  uint8_t barW = _width - 20;
  uint8_t barH = 8;

  _display.drawRect(barX, barY, barW, barH, SSD1306_WHITE);

  if (percent > 100) percent = 100;
  uint8_t fillW = (barW * percent) / 100;
  _display.fillRect(barX + 1, barY + 1, fillW - 2, barH - 2, SSD1306_WHITE);

  if (label != nullptr) {
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(barX, barY - 10);
    _display.print(label);
  }
}

void OLEDAnimator::playSprite(const uint8_t* const frames[],
                              uint8_t totalFrames,
                              uint8_t width,
                              uint8_t height,
                              uint8_t fps) {
  _spriteFrames = frames;
  _spriteTotalFrames = totalFrames;
  _spriteWidth = width;
  _spriteHeight = height;
  _spriteFps = fps;
  _spriteFrameIntervalMs = 1000 / _spriteFps;
  _spriteLastFrameMs = millis();
  _spriteCurrentFrame = 0;
  _spriteActive = true;
}

void OLEDAnimator::transitionTo(void (*drawFunc)(Adafruit_SSD1306&),
                                TransitionType type) {
  _transitionDrawFunc = drawFunc;
  _transitionType = type;
  _transitionStartMs = millis();
  _transitionActive = true;
}

void OLEDAnimator::render() {
  _display.clearDisplay();

  if (_bootActive) {
    renderBoot();
  } else if (_faceActive) {
    renderFace();
  }

  if (_spriteActive) {
    renderSprite();
  }

  if (_transitionActive) {
    renderTransition();
  }

  _display.display();
}

void OLEDAnimator::renderFace() {
  uint32_t now = millis();
  drawFaceEyesMouth(_currentFace, now);
}

void OLEDAnimator::drawFaceEyesMouth(FaceState state, uint32_t now) {
  uint8_t eyeW = 24;
  uint8_t eyeH = 16;
  uint8_t eyeRadius = 6;
  uint8_t leftEyeX = 20;
  uint8_t rightEyeX = _width - 20 - eyeW;
  uint8_t eyeY = 16;

  bool blink = false;
  if (state == BLINK) {
    uint32_t phase = (now / 150) % 4;
    blink = (phase == 0);
  }

  if (!blink && state != SLEEP) {
    _display.drawRoundRect(leftEyeX, eyeY, eyeW, eyeH, eyeRadius, SSD1306_WHITE);
    _display.drawRoundRect(rightEyeX, eyeY, eyeW, eyeH, eyeRadius, SSD1306_WHITE);

    uint8_t pupilOffsetX = 0;
    if (state == LOOK_LEFT) pupilOffsetX = -3;
    else if (state == LOOK_RIGHT) pupilOffsetX = 3;

    uint8_t pupilY = eyeY + eyeH / 2;
    uint8_t pupilLX = leftEyeX + eyeW / 2 + pupilOffsetX;
    uint8_t pupilRX = rightEyeX + eyeW / 2 + pupilOffsetX;

    _display.fillCircle(pupilLX, pupilY, 3, SSD1306_WHITE);
    _display.fillCircle(pupilRX, pupilY, 3, SSD1306_WHITE);
  } else {
    _display.drawLine(leftEyeX, eyeY + eyeH / 2,
                      leftEyeX + eyeW, eyeY + eyeH / 2, SSD1306_WHITE);
    _display.drawLine(rightEyeX, eyeY + eyeH / 2,
                      rightEyeX + eyeW, eyeY + eyeH / 2, SSD1306_WHITE);
  }

  uint8_t mouthW = 50;
  uint8_t mouthH = 16;
  uint8_t mouthX = (_width - mouthW) / 2;
  uint8_t mouthY = 40;

  if (state == HAPPY) {
    _display.drawRoundRect(mouthX, mouthY, mouthW, mouthH, 8, SSD1306_WHITE);
  } else if (state == SURPRISED) {
    _display.drawCircle(_width / 2, mouthY + mouthH / 2, 6, SSD1306_WHITE);
  } else if (state == SAD) {
    _display.drawLine(mouthX, mouthY + mouthH,
                      mouthX + mouthW, mouthY + mouthH - 4, SSD1306_WHITE);
  } else if (state == SLEEP) {
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(_width - 20, 8);
    _display.print("Z");
  } else {
    _display.drawLine(mouthX, mouthY + mouthH / 2,
                      mouthX + mouthW, mouthY + mouthH / 2, SSD1306_WHITE);
  }
}

void OLEDAnimator::renderBoot() {
  uint32_t now = millis();
  uint32_t elapsed = now - _bootStartMs;
  if (elapsed >= _bootDurationMs) {
    _bootActive = false;
    return;
  }

  _display.setTextSize(1);
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(8, 8);
  if (_bootTitle != nullptr) {
    _display.print(_bootTitle);
  } else {
    _display.print("Booting...");
  }

  uint8_t percent = (elapsed * 100) / _bootDurationMs;
  drawProgressBar(percent, nullptr);
}

void OLEDAnimator::renderSprite() {
  if (_spriteFrames == nullptr || _spriteTotalFrames == 0) return;

  const void* framePtr = (const void*)pgm_read_ptr(&_spriteFrames[_spriteCurrentFrame]);
  int16_t x = (_width - _spriteWidth) / 2;
  int16_t y = (_height - _spriteHeight) / 2;

  _display.drawBitmap(x, y,
                      (const uint8_t*)framePtr,
                      _spriteWidth, _spriteHeight,
                      SSD1306_WHITE);
}

void OLEDAnimator::renderTransition() {
  uint32_t now = millis();
  uint32_t elapsed = now - _transitionStartMs;
  if (elapsed >= _transitionDurationMs) {
    _transitionActive = false;
    if (_transitionDrawFunc) {
      _transitionDrawFunc(_display);
    }
    return;
  }

  uint8_t progress = (elapsed * 100) / _transitionDurationMs;

  if (_transitionType == WIPE) {
    uint8_t h = (_height * progress) / 100;
    _display.fillRect(0, 0, _width, h, SSD1306_WHITE);
  } else if (_transitionType == SLIDE_LEFT || _transitionType == SLIDE_RIGHT) {
    int16_t offset = (_width * progress) / 100;
    if (_transitionType == SLIDE_LEFT) offset = -offset;
    _display.setCursor(0 + offset, _height / 2 - 4);
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.print("Transition...");
  }
}
