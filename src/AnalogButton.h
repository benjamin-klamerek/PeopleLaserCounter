#ifndef ANALOGBUTTON_H
#define ANALOGBUTTON_H

#include <Arduino.h>

class AnalogButton
{
private:
  uint8_t pin;
  int threshold;
  unsigned long debounceTime;
  unsigned long lastChangeTime;
  bool lastStableState;
  bool currentState;
  bool pressedEvent;

public:
  AnalogButton(uint8_t pin, int threshold = 350);

  void setDebounceTime(unsigned long debounce);
  void loop();
  bool isPressed();
  bool getState();
};

#endif
