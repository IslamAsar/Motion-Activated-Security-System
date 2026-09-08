#include "Sensors.h"

// ---------------- MotionSensor ----------------
MotionSensor::MotionSensor(uint8_t sensorPin) : _pin(sensorPin) {}

void MotionSensor::begin() {
  pinMode(_pin, INPUT);
}

int MotionSensor::readValue() {
  return digitalRead(_pin);
}

bool MotionSensor::isMotionDetected() {
  return readValue() == HIGH;
}

// ---------------- ButtonSensor ----------------
ButtonSensor::ButtonSensor(uint8_t buttonPin)
  : _pin(buttonPin), _previousState(HIGH), _lastChangeAt(0) {}

void ButtonSensor::begin() {
  pinMode(_pin, INPUT_PULLUP);
  _previousState = digitalRead(_pin);
}

int ButtonSensor::readValue() {
  return digitalRead(_pin);
}

bool ButtonSensor::wasPressed() {
  bool currentState = readValue();
  unsigned long now = millis();
  // Accept only a HIGH-to-LOW transition after the 50 ms debounce interval.
  bool pressed = _previousState == HIGH && currentState == LOW &&
                 now - _lastChangeAt >= 50UL;
  if (currentState != _previousState) {
    _lastChangeAt = now;
    _previousState = currentState;
  }
  return pressed;
}

// ---------------- LightSensor ----------------
LightSensor::LightSensor(uint8_t sensorPin) : _pin(sensorPin) {}

void LightSensor::begin() {}

int LightSensor::readValue() {
  return analogRead(_pin);
}

int LightSensor::readPercent() {
  // The voltage-divider reading is inverted: lower ADC values mean brighter light.
  return map(readValue(), 0, 1023, 100, 0);
}