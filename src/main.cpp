#include <Arduino.h>
#include <RCSwitch.h>
#include <ezButton.h>
#include <arduino-timer.h>
#include "AnalogButton.h"

#define COUNTER_LIMIT 1000
#define LASER_THRESHOLD 450
#define INVERT_PIN 6
#define PIN_RC 8
#define NB_BITS_TO_SEND 16
#define GLOBAL_REFRESH_RATE 15000     // ms
#define BUTTON_SHIFT_REFRESH_RATE 500 // ms
#define BUTTON_RESET_LONG_PRESS_TIME 3000 // ms

RCSwitch mySwitch = RCSwitch();

ezButton buttonAddOne(2);
ezButton buttonRemoveOne(3);
ezButton buttonReset(4);

AnalogButton laser1(A0, LASER_THRESHOLD);
AnalogButton laser2(A1, LASER_THRESHOLD);

// To be able to send 0, we use 1 as the first value (which is in fact 0)
int counter = 1;

//Too handle reset button long press
// We need to wait 3 seconds before resetting the counter
unsigned long resetPressStartTime = 0;
bool resetActionDone = true;

// Create a timer for periodic sending
// This timer will send the counter value every GLOBAL_REFRESH_RATE milliseconds
auto timerPeriodicSending = timer_create_default();

// Delay buttons sending to avoid flooding counter
auto timerButton = timer_create_default();

bool sendCounterValues(void *)
{
  mySwitch.send(counter, NB_BITS_TO_SEND);
  Serial.println("Counter SENT !");
  return true;
}

void scheduleNextSend()
{
  timerButton.cancel();
  timerButton.in(BUTTON_SHIFT_REFRESH_RATE, sendCounterValues);
}

void setup()
{
  mySwitch.enableTransmit(PIN_RC);
  mySwitch.setPulseLength(350);
  mySwitch.setProtocol(1);
  mySwitch.setRepeatTransmit(10);
  Serial.begin(9600);

  buttonAddOne.setDebounceTime(50);
  buttonRemoveOne.setDebounceTime(50);
  buttonReset.setDebounceTime(50);

  laser1.setDebounceTime(20);
  laser2.setDebounceTime(20);

  pinMode(INVERT_PIN, INPUT_PULLUP);

  timerPeriodicSending.every(GLOBAL_REFRESH_RATE, sendCounterValues);

  Serial.println("Arduino ready");
}

void increaseCounter()
{
  if (counter < COUNTER_LIMIT)
  {
    counter++;
    Serial.print("Counter increased : ");
    Serial.println(counter);
    scheduleNextSend();
  }
}

void decreaseCounter()
{
  if (counter > 1)
  {
    counter--;
    Serial.print("Counter decreased : ");
    Serial.println(counter);
    scheduleNextSend();
  }
}

void resetCounter()
{
  counter = 1;
  Serial.println("Counter reset to 0");
  sendCounterValues(nullptr);
}

void loopButtons()
{
  buttonAddOne.loop();
  buttonRemoveOne.loop();
  buttonReset.loop();
  if (buttonAddOne.isReleased())
  {
    increaseCounter();
  }
  else if (buttonRemoveOne.isReleased())
  {
    decreaseCounter();
  }

  if (buttonReset.isPressed())
  {
    resetPressStartTime = millis();
    resetActionDone = false;
  }

  if (buttonReset.getState() == LOW)
  {
    if (!resetActionDone && (millis() - resetPressStartTime >= BUTTON_RESET_LONG_PRESS_TIME))
    {
      resetCounter();
      resetActionDone = true;
    }
  }

  if (buttonReset.isReleased())
  {
    resetPressStartTime = 0;
    resetActionDone = false;
  }
}

void loopLasers()
{
  laser1.loop();
  laser2.loop();

  bool invert = digitalRead(INVERT_PIN);

  if (laser1.isPressed())
  {
    if (invert)
    {
      Serial.println("Laser 1 : +1");
      increaseCounter();
    }
    else
    {
      Serial.println("Laser 1 : -1");
      decreaseCounter();
    }
  }

  if (laser2.isPressed())
  {
    if (invert)
    {
      Serial.println("Laser 2 : -1");
      decreaseCounter();
    }
    else
    {
      Serial.println("Laser 2 : +1");
      increaseCounter();
    }
  }
}

void loop()
{
  timerPeriodicSending.tick();
  timerButton.tick();
  loopButtons();
  loopLasers();
}
