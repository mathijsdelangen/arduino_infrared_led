#include "FastLED.h"
// http://www.tweaking4all.nl/hardware/arduino/arduino-ws2812-led/

#define NUM_LEDS 30  // Number of RGB LEDs in the strip

// Set pin numbers:
const int LED_PIN                = 5;
const int manual_control_pin     = 6;  // The ID of the switch pin
const int infrared_pin           = 7;  // The ID of the infrared sensor
const int DELAY                  = 100;
const long ONE_SECOND            = (1000)/DELAY;
const long ONE_MINUTE            = 60*ONE_SECOND;
const long FIVE_MINUTES          = 5*ONE_MINUTE;
const long TIMER_TIMEOUT         = 5*ONE_SECOND;
const long BUTTON_TIMER_TIMEOUT  = ONE_SECOND;

enum state {
  ON_BY_BUTTON,
  ON_BY_INFRARED,
  OFF,
  MAGIC
};

int   button_state           = 0;           // The initial state of the button
int   prev_button_state      = 0;           // Store the previous button state
int   infrared_state         = 0;           // The initial state of the infrared sensor
state led_state              = OFF;         // Current LED state
state saved_led_state        = OFF;
state prev_led_state         = OFF;
long  led_on_timer           = 0;           // Timer to count how long the LED is on
int   infrared_timer         = 0;
int   button_high_timer      = 0;

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
  
  randomSeed(analogRead(1));
}

void loop()
{
  // Read button value
  prev_button_state = button_state;
  button_state      = digitalRead(manual_control_pin);
  infrared_state    = digitalRead(infrared_pin);

  int temp_button_state = prev_button_state != button_state && button_state == LOW;
  determineNewState(temp_button_state, infrared_state);
    
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
  
  prev_led_state = led_state;
  
  Serial.print("Prev state:");
  Serial.println(prev_led_state);
  
  switch (prev_led_state)
  {
    case OFF:
      if ( infrared_state == HIGH )
        led_state = ON_BY_INFRARED;
    break;
    case MAGIC:
      led_state = saved_led_state;
    break;
    case ON_BY_INFRARED:
      if ( infrared_state == HIGH )
        led_on_timer = 0;
    break;
  }
}

void checkTimeouts()
{
  Serial.print("Infrared imer:");
  Serial.print(led_on_timer);
  Serial.print("/");
  Serial.println(TIMER_TIMEOUT);
  
  Serial.print("Button timer:");
  Serial.print(button_high_timer);
  Serial.print("/");
  Serial.println(BUTTON_TIMER_TIMEOUT);
  
  if ( button_state == HIGH )
    button_high_timer++;
  else
    button_high_timer = 0;
    
  if ( button_high_timer > BUTTON_TIMER_TIMEOUT )
  {
    saved_led_state     = led_state;
    led_state           = MAGIC;
    button_high_timer   = 0;
  }
    
  if ( led_on_timer > TIMER_TIMEOUT )
  {
    led_state    = OFF;
    led_on_timer = 0;
  }
  
  Serial.print("New state:");
  Serial.println(led_state);
}

void executeLedState()
{
  // Something changed
  if ( prev_led_state != led_state )
     if ( led_state == OFF )
       fadeOut();
     else if ( led_state == MAGIC)
       magic();
     else
       fadeIn();
  else // The same state
    if ( led_state == ON_BY_INFRARED )
      led_on_timer++;
}

void allLedsWhite()
{
  // Turn LEDs on 
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    leds[i] = CRGB::White;
    
  FastLED.show();       // Display LEDs 
}

void allLedsOff()
{
  // Turn LEDs off 
  for ( int i = 0 ; i < NUM_LEDS ; ++i )
    leds[i] = CRGB::Black;
    
  FastLED.show();       // Display LEDs 
  
  // Wait 5 seconds, hack to not let noise interfere with the infrared sensor
  Serial.println("LED turned off, waiting 5s...");
  delay(5000);
}

void magic()
{
  superFadeIn();
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

void fadeIn()
{
   for(int k = 0; k <= 255; k=k+20)
   {
      for(int i = 0; i < NUM_LEDS; i++)
      {
        leds[i].r = k;
        leds[i].g = k;
        leds[i].b = k;
        FastLED.show();
        delay(1);
      }
      
   }
}
void fadeOut()
{
  for(int k = 255; k >= 0; k=k-5)
      for(int i = 0; i < NUM_LEDS; i++)
      {
        leds[i].r = k;
        leds[i].g = k;
        leds[i].b = k;
        FastLED.show();
        delay(1);
      }
      
  // Wait 5 seconds, hack to not let noise interfere with the infrared sensor
  Serial.println("LED turned off, waiting 5s...");
  delay(5000);
}

void superFadeIn()
{
  for ( int i = 0 ; i < NUM_LEDS/2 ; ++i )
  {
    leds[i]          = CRGB::White;
    leds[NUM_LEDS-i] = CRGB::White;
    FastLED.show();
    delay(200);
    // Turn these off for the next round
    leds[i]          = CRGB::Black;
    leds[NUM_LEDS-i] = CRGB::Black;
  }
  
  for ( int i = 0 ; i < NUM_LEDS/2 ; ++i )
  {
    leds[NUM_LEDS/2-i] = CRGB::White;
    leds[NUM_LEDS/2+i] = CRGB::White;
    FastLED.show();
    delay(200);
  }
  
}
