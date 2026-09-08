#include "Actuators.h"

// ---------------- LedActuator ----------------
LedActuator::LedActuator(uint8_t ledPin) : _pin(ledPin) {}

void LedActuator::begin() {
  pinMode(_pin, OUTPUT);
  deactivate();
}

void LedActuator::activate() {
  digitalWrite(_pin, HIGH);
}

void LedActuator::deactivate() {
  digitalWrite(_pin, LOW);
}

// ---------------- Buzzer ----------------
Buzzer::Buzzer(uint8_t buzzerPin)
  : _pin(buzzerPin), _courtesyStep(0), _courtesyStartedAt(0) {}

void Buzzer::begin() {
  pinMode(_pin, OUTPUT);
  deactivate();
}

void Buzzer::activate() {
  tone(_pin, 1000);
}

void Buzzer::deactivate() {
  noTone(_pin);
  _courtesyStep = 0;
}

void Buzzer::courtesyBeep(unsigned long now) {
  // Start only once; update() advances the two-tone sequence on later loops.
  if (_courtesyStep == 0) {
    _courtesyStep = 1;
    _courtesyStartedAt = now;
    tone(_pin, 1500);
  }
}

void Buzzer::update(unsigned long now) {
  // Each step is timed without delay(), keeping button and PIR handling responsive.
  if (_courtesyStep == 1 && now - _courtesyStartedAt >= 100UL) {
    noTone(_pin);
    _courtesyStep = 2;
    _courtesyStartedAt = now;
  } else if (_courtesyStep == 2 && now - _courtesyStartedAt >= 50UL) {
    tone(_pin, 1500);
    _courtesyStep = 3;
    _courtesyStartedAt = now;
  } else if (_courtesyStep == 3 && now - _courtesyStartedAt >= 100UL) {
    deactivate();
  }
}

// ---------------- ServoLock ----------------
ServoLock::ServoLock(uint8_t servoPin) : _pin(servoPin) {}

void ServoLock::begin() {
  _servo.attach(_pin);
  deactivate();
}

void ServoLock::activate() {
  _servo.write(90);
}

void ServoLock::deactivate() {
  _servo.write(0);
}

// ---------------- OledDisplay ----------------
OledDisplay::OledDisplay()
  : _display(128, 64, &Wire, -1) {}

void OledDisplay::begin() {
  // Halt here if the display is not connected; the dashboard is required by the system.
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {}
  }
  _display.clearDisplay();
  _display.display();
}

void OledDisplay::updateStatus(const String& status, int light, bool night,
                               int motions) {
  // Redraw the compact telemetry dashboard once per main-loop iteration.
  _display.clearDisplay();
  _display.setTextSize(1);
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(0, 0);
  _display.print(F("System: "));
  _display.println(status);
  _display.setCursor(0, 16);
  _display.print(F("Motions: "));
  _display.println(motions);
  _display.setCursor(0, 32);
  _display.print(F("Light: "));
  _display.print(light);
  _display.println(F("%"));
  _display.setCursor(0, 48);
  _display.println(night ? F("NIGHT MODE: ON") : F("NIGHT MODE: OFF"));
  _display.display();
}

void OledDisplay::showAlert() {
  // The alert screen temporarily replaces the normal telemetry dashboard.
  _display.clearDisplay();
  _display.setTextSize(2);
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(10, 25);
  _display.println(F("INTRUDER!"));
  _display.display();
}