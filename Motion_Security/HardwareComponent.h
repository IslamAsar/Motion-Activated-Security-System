#pragma once

#include <Arduino.h>

// Common lifecycle interface for every hardware abstraction.
class HardwareComponent {
public:
  virtual ~HardwareComponent() {}
  virtual void begin() = 0;
};

// Sensors expose a common raw reading API for polymorphic initialization/use.
class Sensor : public HardwareComponent {
public:
  virtual int readValue() = 0;
};

// Actuators expose common safe/active commands to the application layer.
class Actuator : public HardwareComponent {
public:
  virtual void activate() = 0;
  virtual void deactivate() = 0;
};