#include <Arduino.h>

void setup() {
  Serial.begin(9600);
 
  // setup the timer 2 interrupt

  // disable interrupts
  char cSREG = SREG;
  cli(); 

  // Configure CTC mode ( = Clear timer on compare match).
  // We override the complete register as otherwise the WGM20 would be set 1 as well
  // (https://arduino.stackexchange.com/questions/830/timer2-does-not-work-as-it-should#845).
  TCCR2A = _BV(WGM21);
  
  // Set the timer clock to 2 MHz.
  // We override the complete register as otherwise the 64 prescaller would be set 
  // as well (https://arduino.stackexchange.com/questions/830/timer2-does-not-work-as-it-should#845).
  TCCR2B = _BV(CS21);

  // Configure "output compare register" to match in a 50 microseconds interval in CTC mode.
  // With the Arduino default frequenzy of F_CUP (16 MHz) the "output compare register" 
  // would overflow every ~16 microseconds as the register increments 16 times a microsecond (16 000 000 clock / 1000 000).
  // Therefore we allready applied prescaling on the timer clock and slowed it down from 16 MHz to 2 MHz.
  // This we now also take into acount while we set the "output comapare register" value.
  OCR2A = F_CPU / 1000000 * 50 / 8;
  
  // The default value of the "Timer/Counter register" is 0 on startup.
  // Set it anyway. 
  // TCNT2 = 0;

  // enable timer
  TIMSK2 |= _BV(OCIE2A);

  // enable interrupts
  SREG = cSREG;
}

void loop() {}

volatile unsigned long ctr = 0;

ISR(TIMER2_COMPA_vect) {
  ctr++;
  if(ctr == 100000) {
    Serial.println("5 seconds");
    ctr = 0;
  }
}