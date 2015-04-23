#include <Timer.h>
#include <Event.h>

#include "FastLED.h"
// http://www.tweaking4all.nl/hardware/arduino/arduino-ws2812-led/

#define NUM_LEDS 30  // Number of RGB LEDs in the strip

// test colors
int all_the_colors[] = {
 CRGB::AliceBlue
, CRGB::Amethyst
, CRGB::AntiqueWhite
, CRGB::Aqua
, CRGB::Aquamarine
, CRGB::Azure
, CRGB::Beige
, CRGB::Bisque
, CRGB::Black
, CRGB::BlanchedAlmond
, CRGB::Blue
, CRGB::BlueViolet
, CRGB::Brown
, CRGB::BurlyWood
, CRGB::CadetBlue
, CRGB::Chartreuse
, CRGB::Chocolate
, CRGB::Coral
, CRGB::CornflowerBlue
, CRGB::Cornsilk
, CRGB::Crimson
, CRGB::Cyan
, CRGB::DarkBlue
, CRGB::DarkCyan
, CRGB::DarkGoldenrod
, CRGB::DarkGray
, CRGB::DarkGreen
, CRGB::DarkKhaki
, CRGB::DarkMagenta
, CRGB::DarkOliveGreen
, CRGB::DarkOrange
, CRGB::DarkOrchid
, CRGB::DarkRed
, CRGB::DarkSalmon
, CRGB::DarkSeaGreen
, CRGB::DarkSlateBlue
, CRGB::DarkSlateGray
, CRGB::DarkTurquoise
, CRGB::DarkViolet
, CRGB::DeepPink
, CRGB::DeepSkyBlue
, CRGB::DimGray
, CRGB::DodgerBlue
, CRGB::FireBrick
, CRGB::FloralWhite
, CRGB::ForestGreen
, CRGB::Fuchsia
, CRGB::Gainsboro
, CRGB::GhostWhite
, CRGB::Gold
, CRGB::Goldenrod
, CRGB::Gray
, CRGB::Green
, CRGB::GreenYellow
, CRGB::Honeydew
, CRGB::HotPink
, CRGB::IndianRed
, CRGB::Indigo
, CRGB::Ivory
, CRGB::Khaki
, CRGB::Lavender
, CRGB::LavenderBlush
, CRGB::LawnGreen
, CRGB::LemonChiffon
, CRGB::LightBlue
, CRGB::LightCoral
, CRGB::LightCyan
, CRGB::LightGoldenrodYellow
, CRGB::LightGreen
, CRGB::LightGrey
, CRGB::LightPink
, CRGB::LightSalmon
, CRGB::LightSeaGreen
, CRGB::LightSkyBlue
, CRGB::LightSlateGray
, CRGB::LightSteelBlue
, CRGB::LightYellow
, CRGB::Lime
, CRGB::LimeGreen
, CRGB::Linen
, CRGB::Magenta
, CRGB::Maroon
, CRGB::MediumAquamarine
, CRGB::MediumBlue
, CRGB::MediumOrchid
, CRGB::MediumPurple
, CRGB::MediumSeaGreen
, CRGB::MediumSlateBlue
, CRGB::MediumSpringGreen
, CRGB::MediumTurquoise
, CRGB::MediumVioletRed
, CRGB::MidnightBlue
, CRGB::MintCream
, CRGB::MistyRose
, CRGB::Moccasin
, CRGB::NavajoWhite
, CRGB::Navy
, CRGB::OldLace
, CRGB::Olive
, CRGB::OliveDrab
, CRGB::Orange
, CRGB::OrangeRed
, CRGB::Orchid
, CRGB::PaleGoldenrod
, CRGB::PaleGreen
, CRGB::PaleTurquoise
, CRGB::PaleVioletRed
, CRGB::PapayaWhip
, CRGB::PeachPuff
, CRGB::Peru
, CRGB::Pink
, CRGB::Plaid
, CRGB::Plum
, CRGB::PowderBlue
, CRGB::Purple
, CRGB::Red
, CRGB::RosyBrown
, CRGB::RoyalBlue
, CRGB::SaddleBrown
, CRGB::Salmon
, CRGB::SandyBrown
, CRGB::SeaGreen
, CRGB::Seashell
, CRGB::Sienna
, CRGB::Silver
, CRGB::SkyBlue
, CRGB::SlateBlue
, CRGB::SlateGray
, CRGB::Snow
, CRGB::SpringGreen
, CRGB::SteelBlue
, CRGB::Tan
, CRGB::Teal
, CRGB::Thistle
, CRGB::Tomato
, CRGB::Turquoise
, CRGB::Violet
, CRGB::Wheat
, CRGB::White
, CRGB::WhiteSmoke
, CRGB::Yellow
, CRGB::YellowGreen };

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

// Button stuff
int  button_state      = 0;
int  prev_button_state = 0;
bool on_by_button      = false;

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
}

void loop()
{
  // Read button value
  prev_button_state = button_state;
  button_state      = digitalRead(manual_control_pin);

  if ( button_state == HIGH && button_state != prev_button_state)
  {
    loopTroughColors();
    status_point = 0;
//    on_by_button = !on_by_button;
//    if ( on_by_button)
//    {
//      FastLED.setBrightness(30);
//      loopTroughColors();
//      //setAllLedsToColor(CRGB::Red, true);
//    }
//    else
//    {
//      setAllLedsToColor(CRGB::Black, true);
//      status_point = 0;
//    }
  }
  
  if (!on_by_button)
  {
    FastLED.setBrightness(95);
    if ( digitalRead(infrared_pin) == HIGH )
    {
      resetTurnOffEvent();
  
      turn_off_event = update_leds_timer.after(LED_TIME_OUT, turnOff);
      set_point      = SET_POINT_HIGH;
    }
      
    update_leds_timer.update();
  }
  
  delay(10);
}

void loopTroughColors()
{
  int size = sizeof(all_the_colors);
  Serial.print("Looping through ");
  Serial.print(size);
  Serial.println("colors");
  for ( int i = 0 ; i < size ; ++i )
  {
    Serial.print("Color ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(all_the_colors[i]);
    setAllLedsToColor(all_the_colors[i], true);
    delay(100);
  } 
  
}
void resetTurnOffEvent()
{
  // If a callback is stored
  if ( turn_off_event > 0 )
    update_leds_timer.stop(turn_off_event);
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
  
  if (status_point < step_size*(NUM_LEDS/2))
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
  
  status_point += step_size;
  
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

void setAllLedsToValue( int value, bool show )
{
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    setLedToValue(i, value, false);
    
  if (show)
    FastLED.show();
}

void setLedToColor( int led, CRGB color, bool show )
{
  leds[led] = color;
  if (show)
    FastLED.show();
}

void setAllLedsToColor( CRGB color, bool show )
{  
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    setLedToColor(i, color, false);
    
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
