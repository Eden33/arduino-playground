#include <Arduino.h>

// Normal NEC transmission is a 67 burst pulse (HIGH <--> LOW). 
// 1 (leading pulse [=LOW]) + 1 (space [=HIGH]) + 4 (bytes) * 8 (bits) * 2 + 1 (end message transfer [=LOW]) = 67
#define RAWBUF 67             

typedef struct {
  uint8_t rcvstate;               // state machine
  unsigned int timer;             // state timer, counts 50uS ticks.
  unsigned int rawbuf[RAWBUF];    // raw data
  uint8_t rawlen;                 // counter of entries in rawbuf
} 
irparams_t;

volatile irparams_t irparams;

#define STATE_IDLE        1
#define STATE_WAIT_HIGH   2
#define STATA_WAIT_LOW    3
#define STATE_STOP        4

volatile int pinStatus = 0;
int offset = 0;
unsigned long receivedData = 0;

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
  resetStateMachine();
}

void loop() {

  if(irparams.rcvstate == STATE_STOP) {
    offset = 0;
    receivedData = 0;

    Serial.println("STATE_STOP.");
    printCapturedTimingInfo();

    // Be graceful 100 microseconds into both directions while
    // checking the NEC transmission timeframes due to sensor lags.

    // The leading pulse [=LOW] is 9 ms for normal transmissions 
    // and repeat code transmissions. In 9 ms the IR triggered 180 times.
    if (irparams.rawbuf[offset] < (180 - 2)  || irparams.rawbuf[offset] > (180 + 2)) {
      Serial.println("Leading pulse error.");
      resetStateMachine();
      return;
    }

    offset++;

    // In case it's a normal transmission: The leading pulse is followed by 4,5 milliseconds timeframe 
    // in which the data pin is HIGH. In 4,5 milliseconds the IR triggered 90 times.
    if(irparams.rawbuf[offset] < (90 - 2)  || irparams.rawbuf[offset] > (90 + 2)) {

      // This is not a normal transmission as we reached this code.  
      // Check if it is a repeat transmission: In a repeat transmission the leading pulse is 
      // followed by 2,25 milliseconds timeframe instead of 4,5.
      // In 2,25 milliseconds the IR triggered 45 times.
      if(irparams.rawbuf[offset] > (45 - 2)  || irparams.rawbuf[offset] < (45 + 2)) {
        Serial.println("This is a repeat transmission.");

        //TODO: handle me

        resetStateMachine();
      }

      Serial.println("Space error after leading pulse.");
      resetStateMachine();
      return;
    }

    offset++;

    // Translate the data bits received (in total 4 bytes).
    // 0 is encoded by 562,5 microseconds LOW followed by 562,5 HIGH
    // --> In 562,5 microseconds the IR triggered 11 times.
    // 1 is encoded by 562,5 microseconds LOW followed by 1,6875 milliseconds HIGH
    // --> In 1,6875 milliseconds the IR triggered 33 times.
    for(int i = 0; i < 32; i++) {
      if(irparams.rawbuf[offset] < (11 - 2)  || irparams.rawbuf[offset] > (11 + 2)) {
        Serial.print("Invalid data bit LOW timeframe at idx: ");
        Serial.println(i);
        Serial.print("Invalid timeframe value: ");
        Serial.println(irparams.rawbuf[offset]);
        resetStateMachine();
        return;
      }
      offset++;
      if(irparams.rawbuf[offset] > (11 - 2)  && irparams.rawbuf[offset] < (11 + 2)) {
        receivedData <<= 1;
      } else if(irparams.rawbuf[offset] > (33 - 2)  || irparams.rawbuf[offset] < (33 + 2)) {
        receivedData = (receivedData << 1) | 1;
      }  else {
        Serial.print("Invalid data bit HIGH timeframe at idx: ");
        Serial.println(i);
        Serial.print("Invalid timeframe value: ");
        Serial.println(irparams.rawbuf[offset]);
        resetStateMachine();
        return;
      }
      offset++;
    }
    
    printKeyPressed();

    resetStateMachine();
  }
}

void resetStateMachine() {
    // disable interrupts
    char cSREG = SREG;
    cli(); 

    irparams.timer = 0;
    irparams.rawlen = 0;
    irparams.rcvstate = STATE_IDLE;

    // enable interrupts
    SREG = cSREG;
}

void printCapturedTimingInfo() {
    for(int i = 0; i < RAWBUF; i++) {
      Serial.print(i);
      
      if(i % 2 == 0) {
        Serial.print(" LOW: ");
      } else {
        Serial.print(" HIGH: ");
      }

      Serial.println(irparams.rawbuf[i]);
    }
}

void printKeyPressed() {
  switch(receivedData) {
    case 0xFFA25D: Serial.println("POWER");         break;
    case 0xFF629D: Serial.println("VOL+");          break;
    case 0xFFE21D: Serial.println("FUNC/STOP");     break;
    case 0xFF22DD: Serial.println("FAST BACK");     break;
    case 0xFF02FD: Serial.println("PLAY/PAUSE");    break;
    case 0xFFC23D: Serial.println("FAST FORWARD");  break;
    case 0xFFE01F: Serial.println("DOWN");          break;
    case 0xFFA857: Serial.println("VOL-");          break;
    case 0xFF906F: Serial.println("UP");            break;
    case 0xFF6897: Serial.println("0");             break;
    case 0xFF9867: Serial.println("EQ");            break;
    case 0xFFB04F: Serial.println("ST/REPT");       break;
    case 0xFF30CF: Serial.println("1");             break;
    case 0xFF18E7: Serial.println("2");             break;
    case 0xFF7A85: Serial.println("3");             break;
    case 0xFF10EF: Serial.println("4");             break;
    case 0xFF38C7: Serial.println("5");             break;
    case 0xFF5AA5: Serial.println("6");             break;
    case 0xFF42BD: Serial.println("7");             break;
    case 0xFF4AB5: Serial.println("8");             break;
    case 0xFF52AD: Serial.println("9");             break;
    case 0xFFFFFFFF: Serial.println("REPEAT");     break;  
    default: 
      Serial.println("Unknown button");
  }
}

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

  if(irparams.rawlen >= RAWBUF) {
    irparams.rcvstate = STATE_STOP;
  }

  switch(irparams.rcvstate) {
    case STATE_IDLE:

      // Once the data pin toggles to LOW start measuring the leading pulse.
      if (pinStatus == LOW) {
          irparams.rawlen = 0;
          irparams.timer = 0;
          irparams.rcvstate = STATE_WAIT_HIGH;
          // Serial.println("Switch to STATE_WAIT_HIGH");
      }
      break;

    case STATE_WAIT_HIGH:
      
      // In case data pin toogled to HIGH: Record the time duration of previous LOW.
      if(pinStatus == HIGH) {
        irparams.rawbuf[irparams.rawlen] = irparams.timer;
        irparams.rawlen++;
        irparams.timer = 0;
        irparams.rcvstate = STATA_WAIT_LOW;
        // Serial.println("Switch to STATA_WAIT_LOW");
      }
      break;

    case STATA_WAIT_LOW:

      // In case data pin toogled to LOW: Record the time duration of previous HIGH.
      if(pinStatus == LOW) {
        irparams.rawbuf[irparams.rawlen] = irparams.timer;
        irparams.rawlen++;
        irparams.timer = 0;
        irparams.rcvstate = STATE_WAIT_HIGH; 
        // Serial.println("Switch to STATE_WAIT_HIGH");
      }
      break;

  default:
      // do nothing we are in STATE_STOP
    break;
  }
}