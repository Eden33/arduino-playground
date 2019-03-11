/**
 * This is a copy of source code you can find here:
 * https://learn.adafruit.com/multi-tasking-the-arduino-part-1/a-clean-sweep
 */

#include "Sweeper2.h"
#include <Arduino.h>

Sweeper2::Sweeper2(int interval) 
{
    updateInterval = interval;
    increment = 1;    
}
void Sweeper2::Attach(int pin)
{
servo.attach(pin);
}
  
void Sweeper2::Detach()
{
servo.detach();
}

void Sweeper2::Update()
{
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
        lastUpdate = millis();
        pos += increment;
        servo.write(pos);
        if ((pos >= 180) || (pos <= 0)) // end of sweep
        {
        // reverse direction
        increment = -increment;
        }
    }
}
