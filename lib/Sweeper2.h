#ifndef Sweeper2_h
#define Sweeper2_h

#include <Servo.h>

/**
 * This is a copy of source code you can find here:
 * https://learn.adafruit.com/multi-tasking-the-arduino-part-1/a-clean-sweep
 */
class Sweeper2
{
    public: 
        Sweeper2(int interval);
        void Attach(int pin);
        void Detach();
        void Update();
    private:
        Servo servo;              // the servo
        int pos;              // current servo position 
        int increment;        // increment to move for each interval
        int  updateInterval;      // interval between updates
        unsigned long lastUpdate; // last update of position
};

#endif