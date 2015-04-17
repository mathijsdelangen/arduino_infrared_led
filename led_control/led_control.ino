// Set pin numbers:
const int infrared_pin       = 5;  // The ID of the infrared sensor
const int manual_control_pin = 6;  // The ID of the switch pin
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

void setup()
{ 
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
      // turn on the LED
    }
    led_on_timer++;
  }
  
  if ( current_led_state == OFF )
  {
    if ( led_on_timer > 0 )
    {
       // Turn LED off 
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
