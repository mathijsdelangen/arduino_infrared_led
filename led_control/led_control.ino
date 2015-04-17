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
  ON,
  OFF
};

int button_state             = 0;           // The initial state of the button
int prev_button_state        = 0;
int infrared_state           = 0;           // The initial state of the infrared sensor
state current_led_state      = OFF;         // Current LED state
state button_on              = OFF;
long led_on_timer            = 0;           // Timer to count how long the LED is on

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
  button_state   = digitalRead(manual_control_pin);
  infrared_state = digitalRead(infrared_pin);
  
  // Button pressed
  if ( button_state == HIGH && button_state != prev_button_state)
  {
    switchMode();
  }
  
  if ( infrared_state == HIGH )
  {
    Serial.println("I saw something");
    switchOn();
  }
  
  ledTimer();
  
  prev_button_state = button_state;
  
  delay(DELAY);
}

void ledTimer()
{ 
  // Button override
  if ( button_on == ON )
    return;
   
  if ( current_led_state == ON )
  {
    if ( led_on_timer == 0 )
    {
      allLedsWhite();
    }
    Serial.println("Increasing timer");
    Serial.println(led_on_timer);
    led_on_timer++;
  }
  
  if ( current_led_state == OFF )
  {
    if ( led_on_timer > 0 )
    {
       allLedsOff();
    }
    led_on_timer = 0;
    return;
  }
  
  Serial.println(led_on_timer);
  Serial.println(TIMER_TIMEOUT);
 
  // Check for exceeded time
  if ( led_on_timer > TIMER_TIMEOUT )
  {
    switchOff();
  }
}

void switchMode()
{
  if ( button_on == OFF )
  {
    button_on = ON;
    allLedsWhite();
  }
  else
  {
    button_on = OFF;
    allLedsOff(); 
  }
}

void switchOn()
{
  // Reset the timer
  led_on_timer      = 0;
  current_led_state = ON;
}

void switchOff()
{
  current_led_state = OFF;
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
