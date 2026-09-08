#pragma once

#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include "HardwareComponent.h"

// LED output used for the safe and intrusion status indicators.
class LedActuator : public Actuator {
private:
  uint8_t pin;

public:
  explicit LedActuator(uint8_t ledPin);
  void begin() override;
  void activate() override;
  void deactivate() override;
};

// Buzzer supports both the continuous alarm tone and a timed courtesy beep.
class Buzzer : public Actuator {
private:
  uint8_t pin;
  uint8_t courtesyStep;
  unsigned long courtesyStartedAt;

public:
  explicit Buzzer(uint8_t buzzerPin);
  void begin() override;
  void activate() override;
  void deactivate() override;
  void courtesyBeep(unsigned long now);
  void update(unsigned long now);
};

// Servo output representing the unlocked (0 degrees) and locked (90 degrees) states.
class ServoLock : public Actuator {
private:
  uint8_t pin;
  Servo servo;

public:
  explicit ServoLock(uint8_t servoPin);
  void begin() override;
  void activate() override;
  void deactivate() override;
};

// SSD1306 dashboard responsible for status and intrusion-alert rendering.
class OledDisplay : public HardwareComponent {
private:
  Adafruit_SSD1306 display;

public:
  OledDisplay();
  void begin() override;
  void updateStatus(const String& status, int light, bool night, int motions);
  void showAlert();
};