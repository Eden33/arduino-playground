#include <Arduino.h>

// Normal NEC transmission is a 67 burst pulse (HIGH <--> LOW). 
// 1 (leading pulse [=LOW]) + 1 (space [=HIGH]) + 4 (bytes) * 8 (bits) * 2 + 1 (end message transfer [=LOW]) = 67
#define RAWBUF 67             

typedef struct {
  uint8_t rcvstate;               // state machine
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
volatile unsigned long prevMicros = 0;
volatile unsigned long currentMicros = 0;
int offset = 0;
unsigned long receivedData = 0;
static int irDataPin = 2;


void setup() {
  Serial.begin(9600);
 
  attachInterrupt(digitalPinToInterrupt(irDataPin), irDataPinChange, CHANGE);

  // initialize IR receiver status
  resetStateMachine();
}

void loop() {
  if(irparams.rcvstate == STATE_STOP) {
    if(translateCapturedData()) {
      printKeyPressed();
    }

    // To capture the next transmittion we need to make sure that 
    // there is enough delay between current stop state and the time 
    // before we start recording the next transmittion.
    // A normal NEC transmission takes in total 67,5 milliseconds
    // whereas a repeat transmission takes 108 milliseconds.
    delay(110);

    resetStateMachine();
  }
}

boolean translateCapturedData() {
    offset = 0;
    receivedData = 0;

    printCapturedTimingInfo();

    // Be graceful 150 microseconds into both directions while
    // checking the NEC transmission timeframes due to sensor lags.

    // The leading pulse [=LOW] is 9 ms for normal transmissions 
    // and repeat code transmissions. 
    if (irparams.rawbuf[offset] < (9000 - 150)  || irparams.rawbuf[offset] > (9000 + 150)) {
      Serial.println("Leading pulse error.");
      return false;
    }

    offset++;

    // In case it's a normal transmission: The leading pulse is followed by 4,5 milliseconds timeframe 
    // in which the data pin is HIGH. 
    if(irparams.rawbuf[offset] < (4500 - 150)  || irparams.rawbuf[offset] > (4500 + 150)) {

      // This is not a normal transmission as we reached this code.  
      // Check if it is a repeat transmission: In a repeat transmission the leading pulse is 
      // followed by 2,25 milliseconds timeframe instead of 4,5.
      if(irparams.rawbuf[offset] > (2250 - 150)  || irparams.rawbuf[offset] < (2250 + 150)) {
        // Serial.println("This is a repeat transmission.");
        receivedData = 0xFFFFFFFF;
        return true;
      }

      Serial.println("Space error after leading pulse.");
      return false;
    }

    offset++;

    // Translate the data bits received (in total 4 bytes).
    // 0 is encoded by 562,5 microseconds LOW followed by 562,5 HIGH
    // 1 is encoded by 562,5 microseconds LOW followed by 1,6875 milliseconds HIGH
    for(int i = 0; i < 32; i++) {
      if(irparams.rawbuf[offset] < (562 - 150)  || irparams.rawbuf[offset] > (562 + 150)) {
        Serial.print("Invalid data bit LOW timeframe at idx: ");
        Serial.println(i);
        Serial.print("Invalid timeframe value: ");
        Serial.println(irparams.rawbuf[offset]);
        return false;
      }
      offset++;
      if(irparams.rawbuf[offset] > (562 - 150)  && irparams.rawbuf[offset] < (562 + 150)) {
        receivedData <<= 1;
      } else if(irparams.rawbuf[offset] > (1687 - 150)  || irparams.rawbuf[offset] < (1687 + 150)) {
        receivedData = (receivedData << 1) | 1;
      }  else {
        Serial.print("Invalid data bit HIGH timeframe at idx: ");
        Serial.println(i);
        Serial.print("Invalid timeframe value: ");
        Serial.println(irparams.rawbuf[offset]);
        return false;
      }
      offset++;
    }
    return true;
}

void resetStateMachine() {
    // disable interrupts
    char cSREG = SREG;
    cli(); 

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

void irDataPinChange() {

  // In case the return value of micros() overflows:
  // Reset the state machine and wait for the next button to be pressed on the 
  // remote control. This may happen every 70 minutes if you are unlucky.
  currentMicros = micros();
  if(prevMicros > currentMicros) {
    resetStateMachine();
  }

  pinStatus = digitalRead(irDataPin);

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
          irparams.rcvstate = STATE_WAIT_HIGH;
          prevMicros = currentMicros;
          // Serial.println("Switch to STATE_WAIT_HIGH");
      }
      break;

    case STATE_WAIT_HIGH:
      
      // In case data pin toogled to HIGH: Record the time duration of previous LOW.
      if(pinStatus == HIGH) {
        irparams.rawbuf[irparams.rawlen] = (currentMicros - prevMicros);
        prevMicros = currentMicros;
        irparams.rawlen++;
        irparams.rcvstate = STATA_WAIT_LOW;
        // Serial.println("Switch to STATA_WAIT_LOW");
      }
      break;

    case STATA_WAIT_LOW:

      // In case data pin toogled to LOW: Record the time duration of previous HIGH.
      if(pinStatus == LOW) {
        irparams.rawbuf[irparams.rawlen] = (currentMicros - prevMicros);
        prevMicros = currentMicros;
        irparams.rawlen++;
        irparams.rcvstate = STATE_WAIT_HIGH; 
        // Serial.println("Switch to STATE_WAIT_HIGH");
      }
      break;

  default:
      // do nothing we are in STATE_STOP
    break;
  }
}
