#include <Arduino.h>

//TODO: how much do we really need?
#define RAWBUF 100 // Length of raw duration buffer

typedef struct {
  uint8_t rcvstate;           // state machine
  unsigned int timer;         // state timer, counts 50uS ticks.
  unsigned int rawbuf[RAWBUF];   // raw data
  uint8_t rawlen;             // counter of entries in rawbuf
} 
irparams_t;

volatile irparams_t irparams;

// receiver states
#define STATE_IDLE     2
#define STATE_MARK      3
#define STATE_SPACE    4
#define STATE_STOP     5

volatile int pinStatus = 0;

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

  // initialize IR receiver status
  irparams.rcvstate = STATE_IDLE;
  irparams.timer = 0;
  irparams.rawlen = 0;

}

void loop() {}

ISR(TIMER2_COMPA_vect) {

  pinStatus = digitalRead(11);

  irparams.timer++; // One more 50 microsecond tick

  // The IR sender/receiver communicate in 38 kHz frequency range.
  // This means the data is transmitted with a speed of 1 / 38.000 = 0,0000263 = 26,3 microseconds 

  // the data pin is HIGH by default
  // if(irparams.timer % 50000 == 0) {
  //   Serial.print("Pin status: ");
  //   Serial.println(pinStatus);
  // }

  switch(irparams.rcvstate) {
    case STATE_IDLE: 
      // in case we received LOW for more than 550 microseconds
      if (pinStatus == LOW && irparams.timer >= 11) {
          irparams.rawbuf[0] = irparams.timer;
          irparams.rcvstate = STATE_MARK;
          irparams.rawlen++;
          irparams.timer = 0;
          Serial.println("Switch to STATE_MARK");
      }
      break;
  default:
      if(pinStatus == HIGH && irparams.rcvstate == STATE_MARK
            && irparams.timer > 50000) {
        Serial.println("Switch to STATE_IDLE");
        irparams.rcvstate = STATE_IDLE;
        irparams.rawlen = 0;
        irparams.timer = 0;
      }
    break;
  }
}