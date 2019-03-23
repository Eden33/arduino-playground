#include <Flasher2.h>
#include <Sweeper2.h>

#include <Arduino.h>

Flasher2 led1(12, 2000, 2000);
Flasher2 led2(13, 2000, 1000);
Sweeper2 servo(50);
static int servoResetPin = 2;

void setup()
{
  servo.Attach(10);
  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  // set serverResetPin to input pull up and trigger ISR when the pin goes from high to low
  pinMode(servoResetPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(servoResetPin), Reset, FALLING);
}

void Reset()
{
  servo.Reset();
}

// The timer interrupt is executed once a millisecond to update the LEDs and the servo.
// The servo is not updated if the button on PIN 2 is pressed.
SIGNAL(TIMER0_COMPA_vect) 
{
  unsigned long currentMillis = millis();
    
  servo.Update(currentMillis);
  led1.Update(currentMillis);
  led2.Update(currentMillis);
}

void loop()
{
}