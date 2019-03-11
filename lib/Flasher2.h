#ifndef Flasher2_h
#define Flasher2_h

/**
 * This is a copy of source code you can find here:
 * https://learn.adafruit.com/multi-tasking-the-arduino-part-1/a-classy-solution
 */
class Flasher2
{
    public: 
        Flasher2(int pin, long on, long off);
        void Update();
    private:
    	int ledPin;      // the number of the LED pin
	    long OnTime;     // milliseconds of on-time
	    long OffTime;    // milliseconds of off-time

        // These variables are used to maintain the current state.
	    int ledState;             		// ledState used to set the LED
	    unsigned long previousMillis;  	// will store last time LED was updated
};

#endif