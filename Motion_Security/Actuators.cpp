#include "Actuators.h"

// ---------------- LedActuator ----------------
LedActuator::LedActuator(uint8_t ledPin) : pin(ledPin) {}

void LedActuator::begin() {
  pinMode(pin, OUTPUT);
  deactivate();
}

void LedActuator::activate() {
  digitalWrite(pin, HIGH);
}

void LedActuator::deactivate() {
  digitalWrite(pin, LOW);
}

// ---------------- Buzzer ----------------
Buzzer::Buzzer(uint8_t buzzerPin)
    : pin(buzzerPin), courtesyStep(0), courtesyStartedAt(0) {}

void Buzzer::begin() {
  pinMode(pin, OUTPUT);
  deactivate();
}

void Buzzer::activate() {
  tone(pin, 1000);
}

void Buzzer::deactivate() {
  noTone(pin);
  courtesyStep = 0;
}

void Buzzer::courtesyBeep(unsigned long now) {
  // Start only once; update() advances the two-tone sequence on later loops.
  if (courtesyStep == 0) {
    courtesyStep = 1;
    courtesyStartedAt = now;
    tone(pin, 1500);
  }
}

void Buzzer::update(unsigned long now) {
  // Each step is timed without delay(), keeping button and PIR handling responsive.
  if (courtesyStep == 1 && now - courtesyStartedAt >= 100UL) {
    noTone(pin);
    courtesyStep = 2;
    courtesyStartedAt = now;
  } else if (courtesyStep == 2 && now - courtesyStartedAt >= 50UL) {
    tone(pin, 1500);
    courtesyStep = 3;
    courtesyStartedAt = now;
  } else if (courtesyStep == 3 && now - courtesyStartedAt >= 100UL) {
    deactivate();
  }
}

// ---------------- ServoLock ----------------
ServoLock::ServoLock(uint8_t servoPin) : pin(servoPin) {}

void ServoLock::begin() {
  servo.attach(pin);
  deactivate();
}

void ServoLock::activate() {
  servo.write(90);
}

void ServoLock::deactivate() {
  servo.write(0);
}

// ---------------- OledDisplay ----------------
OledDisplay::OledDisplay()
    : display(128, 64, &Wire, -1) {}

void OledDisplay::begin() {
  // Halt here if the display is not connected; the dashboard is required by the system.
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {}
  }
  display.clearDisplay();
  display.display();
}

void OledDisplay::updateStatus(const String& status, int light, bool night,
                               int motions) {
  // Redraw the compact telemetry dashboard once per main-loop iteration.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("System: "));
  display.println(status);
  display.setCursor(0, 16);
  display.print(F("Motions: "));
  display.println(motions);
  display.setCursor(0, 32);
  display.print(F("Light: "));
  display.print(light);
  display.println(F("%"));
  display.setCursor(0, 48);
  display.println(night ? F("NIGHT MODE: ON") : F("NIGHT MODE: OFF"));
  display.display();
}

void OledDisplay::showAlert() {
  // The alert screen temporarily replaces the normal telemetry dashboard.
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println(F("INTRUDER!"));
  display.display();
}