#include <Timer.h>
#include <Event.h>

#include "FastLED.h"
// http://www.tweaking4all.nl/hardware/arduino/arduino-ws2812-led/

#define NUM_LEDS 30  // Number of RGB LEDs in the strip

// Set pin numbers:
const int LED_PIN                = 5;
const int manual_control_pin     = 6;  // The ID of the switch pin
const int infrared_pin           = 7;  // The ID of the infrared sensor

// Set points
const int SET_POINT_HIGH         = 100;
const int SET_POINT_LOW          = 0;
const int MAX_STEPS_TO_SET_POINT = 75;
const int STEP_SIZE              = SET_POINT_HIGH/MAX_STEPS_TO_SET_POINT;

// Timeouts
const long LED_TIME_OUT           = 10 * 1000;
const long LED_TRIGGER_UPDATE     = 50;

// Led properties 
const int MAX_LED_VALUE          = 255;
const int MIN_LED_VALUE          = 0;

Timer update_leds_timer;

int   status_point   = 0;
int   set_point      = 0;
int   turn_off_event = 0;

CRGB leds[NUM_LEDS];                   // Led array

void setup()
{ 
  // Initialize pins and leds
  FastLED.addLeds<NEOPIXEL, LED_PIN, RGB>(leds, NUM_LEDS);
  pinMode(infrared_pin,       INPUT);
  pinMode(manual_control_pin, INPUT);
  
  // Initialize LEDs off
  allLedsOff();
  
  // Debug serial intialization
  Serial.begin(115200);
  Serial.flush();
  
  // Seed for random numbers
  randomSeed(analogRead(1));
  
  // Set up timer
  update_leds_timer.every(LED_TRIGGER_UPDATE, updateLeds);
  
  Serial.println(CRGB::White);
}

void loop()
{
  // Read button value
  //prev_button_state = button_state;
  //button_state      = digitalRead(manual_control_pin);

  if ( digitalRead(infrared_pin) == HIGH )
  {
    // If a callback is stored
    if ( turn_off_event > 0 )
      update_leds_timer.stop(turn_off_event);

    turn_off_event = update_leds_timer.after(LED_TIME_OUT, turnOff);
    set_point      = SET_POINT_HIGH;
  }
    
  update_leds_timer.update();
  
  delay(10);
}

void updateLeds()
{
  // Do nothing is set point is reached
  if ( status_point == set_point )
    return;
    
  if ( status_point < set_point && fadingIn() )
    fadeIn();
  else
    fader();
}

void fader()
{ 
  if ( status_point < set_point )
    status_point += STEP_SIZE;
    
  if ( status_point > set_point )
    status_point -= STEP_SIZE;
    
  double scaling_factor = ((double)MAX_LED_VALUE/(double)SET_POINT_HIGH);
  int led_value         = scaling_factor*status_point;
  
  Serial.print("Status point:");
  Serial.println(status_point);
  Serial.print("Led value:");
  Serial.println(led_value);
  setAllLedsToValue(led_value, true);
}

void turnOff()
{
  Serial.println("Turn off event triggered");
  set_point = SET_POINT_LOW; 
}

bool fadingIn()
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    if (leds[i].r == MIN_LED_VALUE)
      return true;
    if (leds[i].g == MIN_LED_VALUE)
      return true;
    if (leds[i].b == MIN_LED_VALUE)
      return true;
  }
  
  return false;
}

void fadeIn()
{ 
  int step_size = floor(SET_POINT_HIGH / NUM_LEDS);
  status_point += step_size;
  
  if (status_point <= step_size*(NUM_LEDS/2))
  { 
    Serial.print("Turn on the following leds(1):");
    Serial.print(status_point/step_size);
    Serial.print(" ");
    Serial.println(NUM_LEDS-status_point/step_size);
    
    setLedToColor(status_point/step_size,          CRGB::White, false);
    setLedToColor(NUM_LEDS-status_point/step_size, CRGB::White, true);
    FastLED.show();
    
    // Turn these off for the next round
    setLedToColor(status_point/step_size,          CRGB::Black, false);
    setLedToColor(NUM_LEDS-status_point/step_size, CRGB::Black, false);
  }
  else if (status_point <= step_size * NUM_LEDS)
  {
    Serial.print("Turn on the following leds(2):");
    Serial.print(NUM_LEDS/2 + (status_point-step_size*(NUM_LEDS/2))/step_size);
    Serial.print(" ");
    Serial.println(NUM_LEDS/2 - (status_point-step_size*(NUM_LEDS/2))/step_size);
    setLedToColor(NUM_LEDS/2 + (status_point-step_size*(NUM_LEDS/2))/step_size, CRGB::White, false);
    setLedToColor(NUM_LEDS/2 - (status_point-step_size*(NUM_LEDS/2))/step_size, CRGB::White, true);
  }
  
  // Fix possible overload of status point
  status_point = min(status_point,SET_POINT_HIGH);
}

void setLedToValue( int led, int value, bool show )
{
  leds[led].r = value;
  leds[led].g = value;
  leds[led].b = value;
  if (show)
    FastLED.show();
}

void setAllLedsToValue( int value, bool show)
{
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    setLedToValue(i, value, false);
    
  if (show)
    FastLED.show();
}

void setLedToColor( int led, int color, bool show )
{
  leds[led] = color;
  if (show)
    FastLED.show();
}

void setAllLedsToColor( int color, bool show)
{
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    setLedToValue(i, color, false);
    
  if (show)
    FastLED.show();
}

void allLedsWhite()
{
  // Turn LEDs on
  setAllLedsToColor(CRGB::White, true);
}

void allLedsOff()
{
  // Turn LEDs off 
  setAllLedsToColor(CRGB::Black, true);
}

void magic()
{
//  superFadeIn();
  return;
  
  // generate random number
  int magic = random(0,3);
  Serial.print("Magic!!");
  Serial.println(magic);
  switch (magic)
  {
    case 0: FadeInAndOut();   break;
    case 1: growingStripes(); break;
    case 2: oneByOne();       break;
  }
}

/** Extra functionalities from the FastLed tutorial **/
void oneByOne()
{
  for(int j = 0; j < 3; j++) { 
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      memset(leds, 0, NUM_LEDS * 3);
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastLED.show();
      delay(10);
    }
  }
}

void growingStripes()
{
  for(int j = 0; j < 3; j++) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastLED.show();
      delay(10);
    }
    for(int i = NUM_LEDS-1 ; i >= 0; i-- ) {
      switch(j) { 
        case 0: leds[i].r = 0; break;
        case 1: leds[i].g = 0; break;
        case 2: leds[i].b = 0; break;
      }
      FastSPI_LED.show();
      delay(1);
    }
  }
}

void FadeInAndOut()
{
  // Fade in/fade out
  for(int j = 0; j < 3; j++ ) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int k = 0; k < 256; k++) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastLED.show();
      delay(3);
    }
    for(int k = 255; k >= 0; k--) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastLED.show();
      delay(3);
    }
  }
}
