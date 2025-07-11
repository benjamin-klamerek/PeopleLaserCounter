#include "AnalogButton.h"

AnalogButton::AnalogButton(uint8_t pin, int threshold)
{
  this->pin = pin;
  this->threshold = threshold;
  this->debounceTime = 50;  // Valeur par défaut en ms
  this->lastChangeTime = 0;
  this->lastStableState = false;
  this->currentState = false;
  this->pressedEvent = false;
}

void AnalogButton::setDebounceTime(unsigned long debounce)
{
  this->debounceTime = debounce;
}

void AnalogButton::loop()
{
  int analogValue = analogRead(pin);
  bool newState = analogValue < threshold;  // Pressé si en dessous du seuil

  if (newState != currentState)
  {
    lastChangeTime = millis();
    currentState = newState;
  }

  if ((millis() - lastChangeTime) >= debounceTime)
  {
    if (currentState != lastStableState)
    {
      lastStableState = currentState;
      if (lastStableState == true)
      {
        pressedEvent = true;
      }
    }
  }
}

bool AnalogButton::isPressed()
{
  if (pressedEvent)
  {
    pressedEvent = false;
    return true;
  }
  return false;
}

bool AnalogButton::getState()
{
  return lastStableState;
}
