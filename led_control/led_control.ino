#include "FastLED.h"
// http://www.tweaking4all.nl/hardware/arduino/arduino-ws2812-led/
#define NUM_LEDS 30  // Number of RGB LEDs in the strip

// Set pin numbers:
const int LED_PIN            = 5;
const int manual_control_pin = 6;  // The ID of the switch pin
const int infrared_pin       = 7;  // The ID of the infrared sensor
const int DELAY              = 100;
const int ONE_MINUTE         = (60*1000)/DELAY;
const int TIMER_TIMEOUT      = ONE_MINUTE;

enum state {
  ON,
  OFF
};

int button_state             = 0;           // The initial state of the button
int infrared_state           = 0;           // The initial state of the infrared sensor
state current_led_state      = OFF;         // Current LED state
int led_on_timer             = 0;           // Timer to count how long the LED is on

CRGB leds[NUM_LEDS];                        // Led array

void setup()
{ 
  FastLED.addLeds<NEOPIXEL, LED_PIN, RGB>(leds, NUM_LEDS);
  pinMode(infrared_pin,       INPUT);
  pinMode(manual_control_pin, INPUT);
}

void loop()
{
  // Read button value
  button_state   = digitalRead(manual_control_pin);
  infrared_state = digitalRead(infrared_pin);
  
  // Button pressed
  if ( button_state == HIGH )
  {
    switchMode();
    delay(2000);
  }
  
  if ( infrared_state == HIGH )
  {
     switchOn();
  }
  
  ledTimer();
  
  delay(DELAY);
}

void ledTimer()
{
  if ( current_led_state == ON )
  {
    if ( led_on_timer == 0 )
    {
       // Turn LED on 
       for ( int i = 0 ; i < NUM_LEDS ; ++i )
        leds[i] = CRGB::GhostWhite;
        
      FastLED.show();       // Display LEDs 
    }
    led_on_timer++;
  }
  
  if ( current_led_state == OFF )
  {
    if ( led_on_timer > 0 )
    {
       // Turn LED off 
       for ( int i = 0 ; i < NUM_LEDS ; ++i )
        leds[i] = CRGB::Black;
        
      FastLED.show();       // Display LEDs 
    }
    led_on_timer = 0;
  }
  
  // Check for exceeded time
  if ( led_on_timer > ONE_MINUTE )
  {
    switchOff();
  }
}

void switchMode()
{
  if ( current_led_state == OFF )
    current_led_state = ON;
  else
    current_led_state = OFF;
}

void switchOn()
{
  // Reset the timer
  led_on_timer      = 0;
  current_led_state = ON;
}

void switchOff()
{
  // Reset the timer
  current_led_state = OFF;
}
