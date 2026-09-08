#include "Sensors.h"

// ---------------- MotionSensor ----------------
MotionSensor::MotionSensor(uint8_t sensorPin) : pin(sensorPin) {}

void MotionSensor::begin() {
  pinMode(pin, INPUT);
}

int MotionSensor::readValue() {
  return digitalRead(pin);
}

bool MotionSensor::isMotionDetected() {
  return readValue() == HIGH;
}

// ---------------- ButtonSensor ----------------
ButtonSensor::ButtonSensor(uint8_t buttonPin)
    : pin(buttonPin), previousState(HIGH), lastChangeAt(0) {}

void ButtonSensor::begin() {
  pinMode(pin, INPUT_PULLUP);
  previousState = digitalRead(pin);
}

int ButtonSensor::readValue() {
  return digitalRead(pin);
}

bool ButtonSensor::wasPressed() {
  bool currentState = readValue();
  unsigned long now = millis();
  // Accept only a HIGH-to-LOW transition after the 50 ms debounce interval.
  bool pressed = previousState == HIGH && currentState == LOW &&
                 now - lastChangeAt >= 50UL;
  if (currentState != previousState) {
    lastChangeAt = now;
    previousState = currentState;
  }
  return pressed;
}

// ---------------- LightSensor ----------------
LightSensor::LightSensor(uint8_t sensorPin) : pin(sensorPin) {}

void LightSensor::begin() {}

int LightSensor::readValue() {
  return analogRead(pin);
}

int LightSensor::readPercent() {
  // The voltage-divider reading is inverted: lower ADC values mean brighter light.
  return map(readValue(), 0, 1023, 100, 0);
}