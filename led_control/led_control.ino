#include "FastLED.h"
// http://www.tweaking4all.nl/hardware/arduino/arduino-ws2812-led/

#define NUM_LEDS 30  // Number of RGB LEDs in the strip

// Set pin numbers:
const int LED_PIN            = 5;
const int manual_control_pin = 6;  // The ID of the switch pin
const int infrared_pin       = 7;  // The ID of the infrared sensor
const int DELAY              = 100;
const long ONE_SECOND        = (1000)/DELAY;
const long ONE_MINUTE        = 60*ONE_SECOND;
const long FIVE_MINUTES      = 5*ONE_MINUTE;
const long TIMER_TIMEOUT     = 5*ONE_SECOND;

enum state {
  ON_BY_BUTTON,
  ON_BY_INFRARED,
  OFF
};

int   button_state           = 0;           // The initial state of the button
int   prev_button_state      = 0;           // Store the previous button state
int   infrared_state         = 0;           // The initial state of the infrared sensor
state led_state              = OFF;         // Current LED state
state prev_led_state         = OFF;
long  led_on_timer           = 0;           // Timer to count how long the LED is on

CRGB leds[NUM_LEDS];                        // Led array

void setup()
{ 
  FastLED.addLeds<NEOPIXEL, LED_PIN, RGB>(leds, NUM_LEDS);
  pinMode(infrared_pin,       INPUT);
  pinMode(manual_control_pin, INPUT);
  
  // Initialize LEDs off
  allLedsOff();
  
  Serial.begin(115200);
  Serial.flush();
}

void loop()
{
  // Read button value
  prev_button_state = button_state;
  button_state      = digitalRead(manual_control_pin);
  infrared_state    = digitalRead(infrared_pin);
  
  if ( prev_button_state != button_state )
    determineNewState(button_state, infrared_state);
  else
    determineNewState(0, infrared_state);
    
  checkTimeouts();
  executeLedState();
  
  //Serial.println("I saw something");
  
  delay(DELAY);
}

void determineNewState(int button_state, int infrared_state)
{
  Serial.print("Button state:");
  Serial.print(button_state);
  Serial.print(", Infrared state:");
  Serial.println(infrared_state);
  
  Serial.print("Prev state:");
  Serial.println(prev_led_state);

  prev_led_state = led_state;
  
  switch (prev_led_state)
  {
    case OFF:
      if ( button_state == HIGH )
        led_state = ON_BY_BUTTON;
      else if ( infrared_state == HIGH )
        led_state = ON_BY_INFRARED;
    break;
    case ON_BY_BUTTON:
      if ( button_state == HIGH )
        led_state = OFF;
    break;
    case ON_BY_INFRARED:
      if ( button_state == HIGH )
        led_state = OFF;
      else if ( infrared_state == HIGH )
        led_on_timer = 0;
    break;
  }
  
  Serial.print("New state:");
  Serial.println(led_state);
  
}

void checkTimeouts()
{
  Serial.print("Timer:");
  Serial.print(led_on_timer);
  Serial.print("/");
  Serial.println(TIMER_TIMEOUT);
  
  if ( led_on_timer > TIMER_TIMEOUT )
  {
    led_state    = OFF;
    led_on_timer = 0;
  }
}

void executeLedState()
{
  // Something changed
  if ( prev_led_state != led_state )
     if ( led_state == OFF )
       allLedsOff();
     else
       allLedsWhite();
  else // The same state
    if ( led_state == ON_BY_INFRARED )
      led_on_timer++;
}


void allLedsWhite()
{
  // Turn LEDs on 
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    leds[i] = CRGB::Gold;
    
  FastLED.show();       // Display LEDs 
}

void allLedsOff()
{
  // Turn LEDs off 
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    leds[i] = CRGB::Black;
    
  FastLED.show();       // Display LEDs 
}
