#include "Actuators.h"
#include "Sensors.h"

// The controller has two operating modes, selected with the push button.
enum SystemState { DISARMED, ARMED };

// Hardware objects keep pin access and device-specific behavior encapsulated.
MotionSensor motionSensor(2);
ButtonSensor modeButton(3);
LightSensor lightSensor(A0);
Buzzer alarmBuzzer(8);
ServoLock lockServo(10);
LedActuator greenLed(11);
LedActuator redLed(12);
OledDisplay display;

Sensor* sensors[] = { &motionSensor, &modeButton, &lightSensor };
Actuator* actuators[] = { &alarmBuzzer, &lockServo, &greenLed, &redLed };

// These values belong to the application state, not to individual devices.
SystemState currentState = DISARMED;
bool lastPirState = LOW;
bool alarmActive = false;
unsigned long alarmStartedAt = 0;
int motionCount = 0;

// Start all alarm outputs together and keep the start time for non-blocking timing.
void startAlarm(unsigned long now, int lightPercent) {
  alarmActive = true;
  alarmStartedAt = now;
  redLed.activate();
  lockServo.activate();
  alarmBuzzer.activate();
  display.showAlert();
  motionCount++;
  Serial.print("[ALERT] Motion @");
  Serial.print(now);
  Serial.print("ms | Light:");
  Serial.print(lightPercent);
  Serial.println("%");
}

// Return every alarm output to its safe idle state.
void stopAlarm() {
  alarmActive = false;
  redLed.deactivate();
  lockServo.deactivate();
  alarmBuzzer.deactivate();
}

void setup() {
  Serial.begin(9600);

  // Use the common base-class API to initialize all sensors and actuators.
  for (Sensor* sensor : sensors) {
    sensor->begin();
  }
  for (Actuator* actuator : actuators) {
    actuator->begin();
  }
  display.begin();
  lockServo.deactivate();
  alarmBuzzer.deactivate();
}

void loop() {
  unsigned long now = millis();
  int lightPercent = lightSensor.readPercent();
  bool isNightMode = lightPercent < 20;
  bool currentPirState = motionSensor.isMotionDetected();
  // A rising edge counts one motion event, even if the PIR stays HIGH.
  bool pirTriggered = currentPirState && !lastPirState;

  if (modeButton.wasPressed()) {
    // Disarming also cancels an active alarm immediately.
    currentState = (currentState == DISARMED) ? ARMED : DISARMED;
    motionCount = (currentState == ARMED) ? 0 : motionCount;
    stopAlarm();
  }

  if (currentState == ARMED) {
    greenLed.deactivate();
    if (pirTriggered && !alarmActive) {
      startAlarm(now, lightPercent);
    }
    // millis() keeps the loop responsive while the three-second alarm is active.
    if (alarmActive && now - alarmStartedAt >= 3000UL) {
      stopAlarm();
    }
    if (!alarmActive) {
      display.updateStatus("ARMED", lightPercent, isNightMode, motionCount);
    }
  } else {
    greenLed.activate();
    redLed.deactivate();
    lockServo.deactivate();
    if (isNightMode && pirTriggered) {
      // The courtesy beep has its own non-blocking sequence in Buzzer.
      alarmBuzzer.courtesyBeep(now);
    }
    alarmBuzzer.update(now);
    display.updateStatus("DISARMED", lightPercent, isNightMode, motionCount);
  }
  lastPirState = currentPirState;
}