#include <Flasher2.h>
#include <Sweeper2.h>

Flasher2 led1(12, 1000, 2000);
Flasher2 led2(13, 2000, 2000);
Sweeper2 servo(200);

void setup()
{
  servo.Attach(10);
}

/**
 * Simple state machine approach in update functions instead of working with delay("x").
 */
void loop()
{
  led1.Update();
  led2.Update();
  servo.Update();
}
