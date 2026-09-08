#pragma once

#include "HardwareComponent.h"

// Digital PIR input used to detect a new motion event.
class MotionSensor : public Sensor {
private:
  uint8_t _pin;

public:
  explicit MotionSensor(uint8_t sensorPin);
  void begin() override;
  int readValue() override;
  bool isMotionDetected();
};

// Debounced active-low push-button input used to change system mode.
class ButtonSensor : public Sensor {
private:
  uint8_t _pin;
  bool _previousState;
  unsigned long _lastChangeAt;

public:
  explicit ButtonSensor(uint8_t buttonPin);
  void begin() override;
  int readValue() override;
  bool wasPressed();
};

// Analog LDR input converted to the application's 0-100 percent scale.
class LightSensor : public Sensor {
private:
  uint8_t _pin;

public:
  explicit LightSensor(uint8_t sensorPin);
  void begin() override;
  int readValue() override;
  int readPercent();
};