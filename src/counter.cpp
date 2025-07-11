#include <Arduino.h>
#include <RCSwitch.h>
#include <FastLED.h>

#define COUNTER_LIMIT 1000
#define LIMIT_PEOPLE 60
#define LED_PIN 8
#define NUM_DIGITS 3
#define LEDS_PER_DIGIT 7
#define NUM_LEDS (NUM_DIGITS * LEDS_PER_DIGIT)

CRGB leds[NUM_LEDS];

// Segment order : C, D, E, F, A, B, G
const byte digitSegments[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {1, 0, 0, 0, 0, 1, 0}, // 1
    {0, 1, 1, 0, 1, 1, 1}, // 2
    {1, 1, 0, 0, 1, 1, 1}, // 3
    {1, 0, 0, 1, 0, 1, 1}, // 4
    {1, 1, 0, 1, 1, 0, 1}, // 5
    {1, 1, 1, 1, 1, 0, 1}, // 6
    {1, 0, 0, 0, 1, 1, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 0, 1, 1, 1, 1}  // 9
};

RCSwitch mySwitch = RCSwitch();

void showDigit(byte digitIndex, byte num, CRGB color)
{
    if (digitIndex >= NUM_DIGITS || num > 9)
        return;

    byte offset = digitIndex * LEDS_PER_DIGIT;
    for (byte i = 0; i < LEDS_PER_DIGIT; i++)
    {
        leds[offset + i] = digitSegments[num][i] ? color : CRGB::Black;
    }
}

void showNumber(int number)
{
    CRGB color = CRGB::Green;
    if (number >= LIMIT_PEOPLE)
    {
        color = CRGB::Red;
    }

    for (int i = 0; i < NUM_DIGITS; i++)
    {
        byte digit = number % 10;
        number /= 10;
        showDigit(i, digit, color);
    }
    FastLED.show();
}

void setup()
{
    Serial.begin(9600);

    mySwitch.enableReceive(0); // Enable receiver on interrupt 0 (pin 2)
    Serial.println("RXB12 ready");

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(200);
    FastLED.clear();
    FastLED.show();
    Serial.println("Fast LED ready");

    showNumber(0);

    Serial.println("ARDUINO ready");
}

void loop()
{
    if (mySwitch.available())
    {
        long receivedValue = mySwitch.getReceivedValue();
        if (receivedValue > 0 && receivedValue <= COUNTER_LIMIT)
        {
            Serial.print("Received : ");
            Serial.print(receivedValue);
            Serial.print(" / bit length : ");
            Serial.println(mySwitch.getReceivedBitlength());
            showNumber(receivedValue - 1);
        }
        else
        {
            Serial.println("Out of range");
        }

        mySwitch.resetAvailable();
    }
}
