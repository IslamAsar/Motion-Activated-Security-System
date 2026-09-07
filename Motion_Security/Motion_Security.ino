#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

// ---------------- Pin assignments ----------------
#define PIR_PIN 2
#define BUTTON_PIN 3
#define BUZZER_PIN 8
#define SERVO_PIN 10
#define GREEN_LED 11
#define RED_LED 12
#define LDR_PIN A0

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- System state and variables ----------------
enum SystemState { DISARMED, ARMED };
volatile SystemState currentState = DISARMED;

volatile unsigned long lastButtonPress = 0;
volatile bool stateChanged = false;

int motionCount = 0;
Servo lockServo;

// Stores the previous PIR value so we can detect a rising edge (new motion event)
bool lastPirState = LOW;

// ---------------- Button interrupt service routine ----------------
void buttonISR() {
  unsigned long currentMicros = micros();
  if (currentMicros - lastButtonPress >= 50000) {
    currentState = (currentState == DISARMED) ? ARMED : DISARMED;
    stateChanged = true;
    lastButtonPress = currentMicros;
  }
}

// ---------------- Display functions ----------------
void updateDisplay(String status, int light, bool night) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("System: ");
  display.println(status);

  display.setCursor(0, 16);
  display.print("Motions: ");
  display.println(motionCount);

  display.setCursor(0, 32);
  display.print("Light: ");
  display.print(light);
  display.println("%");

  display.setCursor(0, 48);
  if (night) {
    display.println("NIGHT MODE: ON");
  } else {
    display.println("NIGHT MODE: OFF");
  }

  display.display();
}

void displayIntruderAlert() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("INTRUDER!");
  display.display();
}

void setup() {
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  lockServo.attach(SERVO_PIN);
  lockServo.write(0);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  int lightPercent = map(ldrValue, 0, 1023, 100, 0);
  bool isNightMode = (lightPercent < 20);

  // Read the current PIR sensor state
  bool currentPirState = digitalRead(PIR_PIN);

  // Trigger only when motion is detected as a new rising edge: HIGH after LOW
  bool pirTriggered = (currentPirState == HIGH && lastPirState == LOW);

  if (stateChanged) {
    if (currentState == ARMED) {
      motionCount = 0;
    }
    stateChanged = false;
  }

  // ---------------- System behavior ----------------
  if (currentState == ARMED) {
    digitalWrite(GREEN_LED, LOW);

    if (pirTriggered) {
      // --- Alarm state ---
      digitalWrite(RED_LED, HIGH);
      lockServo.write(90);
      tone(BUZZER_PIN, 1000);

      Serial.print("[ALERT] Motion @");
      Serial.print(millis());
      Serial.print("ms | Light:");
      Serial.print(lightPercent);
      Serial.println("%");

      displayIntruderAlert();
      motionCount++; // Count each detected motion event only once

      delay(3000); // Show the alert and keep the alarm active for 3 seconds

      // Stop the alarm
      digitalWrite(RED_LED, LOW);
      noTone(BUZZER_PIN);
      lockServo.write(0);
    }

    // Update the display with the armed status and motion count
    updateDisplay("ARMED", lightPercent, isNightMode);

  } else {
    // --- Disarmed state ---
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    lockServo.write(0);
    noTone(BUZZER_PIN);

    if (isNightMode && pirTriggered) {
      tone(BUZZER_PIN, 1500, 100);
      delay(150);
      tone(BUZZER_PIN, 1500, 100);
    }

    updateDisplay("DISARMED", lightPercent, isNightMode);
  }

  // Save the current PIR reading for comparison on the next loop
  lastPirState = currentPirState;

  delay(50); // Small delay to stabilize the loop
}