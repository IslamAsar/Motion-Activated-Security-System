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
ButtonSensor* ButtonSensor::_instance = nullptr;

ButtonSensor::ButtonSensor(uint8_t buttonPin)
  : _pin(buttonPin), _pressed(false), _lastInterruptAt(0) {
  _instance = this;
}

void ButtonSensor::begin() {
  pinMode(_pin, INPUT_PULLUP);
  _pressed = false;
  _lastInterruptAt = millis();
  attachInterrupt(digitalPinToInterrupt(_pin), ButtonSensor::handleInterrupt, FALLING);
}

int ButtonSensor::readValue() {
  return digitalRead(_pin);
}

bool ButtonSensor::wasPressed() {
  noInterrupts();
  bool pressed = _pressed;
  _pressed = false;
  interrupts();
  return pressed;
}

void ButtonSensor::handleInterrupt() {
  if (_instance == nullptr) {
    return;
  }

  unsigned long now = millis();
  if (now - _instance->_lastInterruptAt >= 50UL) {
    _instance->_pressed = true;
    _instance->_lastInterruptAt = now;
  }
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