#include <Arduino.h>
#include <limits.h>
#include <Wire.h>

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

static int rightButtonPin = 6;
static int leftButtonPin = 7;
static int editModeButton = 13;

#define BUTTON_PRESSED_TIME_DELTA                 250
#define STATE_OPERATE_NORMAL                      1
#define STATE_ITERATE_OVER_DATE_ELEMENTS          2

typedef struct {
  uint8_t state;                // current state
  unsigned long since;          // start time of state
  unsigned long lastUpdateTime; // state last time updated
  uint8_t dateElementFocusIdx;  // current date element that has the focus
} state_t;

state_t currentState;

typedef struct {
  boolean leftButtonPressed;
  boolean rightButtonPressed;
  boolean editModeButtonPressed;
} buttons_pressed_state_t;

buttons_pressed_state_t currentButtonPressedState;

typedef struct {
  uint8_t displayLineIdx;
  uint8_t displayLinePosition;
  uint8_t displayLenght;
  String displayValue;
  String blinkValue;
  uint8_t dateTimeRegValuesIdx;
} date_element_t;

date_element_t dateElements[6] = {
  {0, 6, 4, "YYYY", "    ", 6,},
  {0, 11, 2, "MM", "  ", 5},
  {0, 14, 2, "DD", "  ", 4},
  {1, 6, 2, "HH", "  ", 2},
  {1, 9, 2, "MM", "  ", 1},
  {1, 12, 2, "SS", "  ", 0}
};

#define DS3231_SLAVE_ADDRESS 0x68

typedef struct {
  uint8_t regValue;
  unsigned char regValueMask;
  boolean isYearValue;
} rtc_reg_value_t;

rtc_reg_value_t rtcRegValues[7] {
    {0, 0x7F, false}, // SS
    {0, 0x7F, false}, // MM
    {0, 0x3F, false}, // HH
    {0, 0x0, false},  // DAY - not used
    {0, 0x3F, false}, // DD
    {0, 0x1F, false}, // MM
    {0, 0xFF, true} // YYYY
};

void setup() {
    
  Wire.begin();
  Serial.begin(9600);

  currentState.state = STATE_OPERATE_NORMAL;
  currentState.since = 0;
  currentState.lastUpdateTime = 0;
  currentState.dateElementFocusIdx = 0;

  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(editModeButton, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Date: 0000 00 00");
  lcd.setCursor(0,1);
  lcd.print("Time: 00:00:00");

  // test system millis overflow
  // unsigned long delta = getDeltaInMillis(100, ULONG_MAX - 250);
  // Serial.print("System millis overflow test: ");
  // Serial.println(delta);
}

void loop() {
  pullLatestDateTimeFromRTC();
  pushLatestDateTimeToLCD();
  updateButtonState();
  updateStateMachine();
}

void updateStateMachine() {
  switch(currentState.state) {
    case STATE_OPERATE_NORMAL: {

      if(editModeButtonPressed()) {
        currentState.state = STATE_ITERATE_OVER_DATE_ELEMENTS;
        unsigned long changeTime = millis();
        currentState.since = changeTime;
        currentState.lastUpdateTime = changeTime;

      }
      break;
    }
    case STATE_ITERATE_OVER_DATE_ELEMENTS: {

      if(editModeButtonPressed()) {
          // focus next date element
      
          currentState.lastUpdateTime = millis();

          // make sure that the date element currently has the focus is visible 
          // before we iterate to the next one 
          lcd.setCursor(dateElements[currentState.dateElementFocusIdx].displayLinePosition,
                    dateElements[currentState.dateElementFocusIdx].displayLineIdx);
          lcd.print(dateElements[currentState.dateElementFocusIdx].displayValue);
          
          //iterate to the next one
          currentState.dateElementFocusIdx++;
          if(currentState.dateElementFocusIdx > 5) {
            currentState.dateElementFocusIdx = 0;
          }

      } else {
        uint8_t regValueIdx = dateElements[currentState.dateElementFocusIdx].dateTimeRegValuesIdx;
        uint8_t regValue = rtcRegValues[regValueIdx].regValue;
        uint8_t regValueUpdated = regValue;
        if(leftButtonPressed()) {
          // decrease value of selected date element
          regValueUpdated--;
        } else if(rightButtonPressed()) {
          // increase value of selected date element
          regValueUpdated++;
        }
        if(regValueUpdated != regValue) {

          currentState.lastUpdateTime = millis();

          writeRegisterValueToRTC(
              regValueIdx,
              humanReadableFormatToRtcRegValue(regValueUpdated, 
                  rtcRegValues[dateElements[currentState.dateElementFocusIdx].dateTimeRegValuesIdx])
          );
        }
      }

      unsigned long delta = getDeltaInMillis(millis(), currentState.lastUpdateTime);

      if(delta > 10000) {
        // no change since configured delta time, enter normal clock
        // operation mode
        currentState.state = STATE_OPERATE_NORMAL;
        unsigned long changeTime = millis();
        currentState.since = changeTime;
        currentState.lastUpdateTime = changeTime;

        // reset focus to first date element
        currentState.dateElementFocusIdx = 0;
      }
      break;
    }
    default: {
      break;
    }
  }
}

void updateButtonState() {
  
  // clear old state
  currentButtonPressedState.leftButtonPressed = false;
  currentButtonPressedState.rightButtonPressed = false;
  currentButtonPressedState.editModeButtonPressed = false;

  unsigned long delta = getDeltaInMillis(millis(), currentState.lastUpdateTime);

  if(digitalRead(leftButtonPin) == LOW && delta > BUTTON_PRESSED_TIME_DELTA) {
    currentButtonPressedState.leftButtonPressed = true;
  } else if(digitalRead(rightButtonPin) == LOW && delta > BUTTON_PRESSED_TIME_DELTA) {
    currentButtonPressedState.rightButtonPressed = true;
  } else if(digitalRead(editModeButton) == LOW && delta > BUTTON_PRESSED_TIME_DELTA) {
    currentButtonPressedState.editModeButtonPressed = true;
  }
}

boolean editModeButtonPressed() {
  return currentButtonPressedState.editModeButtonPressed;
}

boolean leftButtonPressed() {
  return currentButtonPressedState.leftButtonPressed;
}

boolean rightButtonPressed() {
  return currentButtonPressedState.rightButtonPressed;
}

unsigned long getDeltaInMillis(unsigned long systemMillis, unsigned long currentStateSinceMillis) {
    unsigned long millisSinceStart = 0;

    if(systemMillis >= currentStateSinceMillis) {
        millisSinceStart = systemMillis - currentStateSinceMillis;
    } else {
      // handle system millis overflow
      millisSinceStart += systemMillis;
      millisSinceStart += ULONG_MAX - currentStateSinceMillis;
    }
    return millisSinceStart;
}

/**
 * Pull registers 0x0 to 0x6 from DS3231 into byte buffer.
 **/ 
void pullLatestDateTimeFromRTC() 
{
  int size = 7;
  int received = 0;
  Wire.beginTransmission(DS3231_SLAVE_ADDRESS);
  Wire.write(0x0);
  Wire.endTransmission(false);
  Wire.requestFrom(DS3231_SLAVE_ADDRESS, size);

  while(Wire.available() && received < size)
  { 
    rtcRegValues[received].regValue = 
      rtcRegValueToHumanReadableFormat(Wire.read(), rtcRegValues[received]);
    received++;
  }
}

void writeRegisterValueToRTC(int address, const uint8_t value) {
  Wire.beginTransmission(DS3231_SLAVE_ADDRESS);
  Wire.write(address);      // write the start address
  Wire.write(&value, 1);  // write data byte
  Wire.endTransmission(true); // release the I2C-bus
}

uint8_t rtcRegValueToHumanReadableFormat(int regVal, rtc_reg_value_t regValueHolder) {

  // make sure we only use the relevant bits for calculation
  int regValMasked = regVal & regValueHolder.regValueMask;

  /*
  The registers are desribed in DS3231 data sheet on page 11.
   
  Bits 0 to 3 (= 4 bits) are able to store in theory at max 16 values (=1111).
  However, last mentioned bits store at max 1001 (=10) before they overflow to 10000 (=16) -> part 1 of next formula.
  
  Bits 4 to 7 (= 4 bits) are set with a multiplier of 10 in case bits 0 to 3 overflow
  1001 (=9) -> part 2 of next formula. 

  For example:
  ...-> 9 -> 1001 -> and than it overflows to bit 4 for 10 -> 10000 -> and than 11 -> 10001 -> 
  and than 12 --> 10010 -> ... -> and than 19 -> 11001 -> and than it overflows to bit 5 for 20 -> 100000 
  -> ...
  */
  uint8_t transformedVal = ((regValMasked % 16) + (regValMasked/16*10));

  return transformedVal;
}

uint8_t humanReadableFormatToRtcRegValue(int value, rtc_reg_value_t regValueHolder) {

  /*
   Obviously that this calculation must do the oposite described 
   in #rtcRegValueToHumanReadableFormat. 

   The remainder of the value we would like to write (0 to 1001) must be written to 
   bits 0 to 3 -> part 1 of next formula.

   Everything else is a multiple of 10 and must be written to bits 4 to 7 -> part 2 of 
   next formula.
  */   
  value = ((value % 10) + (value/10*16));

  // make sure only valid bits of the target regestry value set
  int regValMasked = value & regValueHolder.regValueMask;

  /*
    Please note that this method implementation doesn't prevent misconfiguration
    by the end user. What does it mean? For example it's possible to set the days of a month to 32.
    However, the RTC recovers from such misconfiguration over time.
  */

  return regValMasked;
}

/**
 * Push latest date time we store in byte buffer to 
 * the LCD 1602 module.
 **/
void pushLatestDateTimeToLCD() {
  for(unsigned int i = 0; i < sizeof(dateElements)/sizeof(dateElements[0]); i++ ) {
    
      if(rtcRegValues[dateElements[i].dateTimeRegValuesIdx].isYearValue) {
        // Note: There is a Century bit in Register 0x5 which toogles
        // in case the 0x7 register overflows. In this sketch we don't care about it.
        uint32_t year = 2000;
        year += rtcRegValues[dateElements[i].dateTimeRegValuesIdx].regValue;
        dateElements[i].displayValue = String(year);
      } else {
        dateElements[i].displayValue = String(rtcRegValues[dateElements[i].dateTimeRegValuesIdx].regValue);
      }

      while(dateElements[i].displayValue.length() < dateElements[i].displayLenght) {
        dateElements[i].displayValue = "0" + dateElements[i].displayValue;
      }

      lcd.setCursor(dateElements[i].displayLinePosition,
                    dateElements[i].displayLineIdx);

      if(currentState.state == STATE_ITERATE_OVER_DATE_ELEMENTS 
              && currentState.dateElementFocusIdx == i) {     
        // the date element which currently has the focus must blink
        unsigned long delta = getDeltaInMillis(millis(), currentState.since);
        delta = delta % 1400;

        if(delta > 700) {
          lcd.print(dateElements[i].displayValue);
        } else {
          lcd.print(dateElements[i].blinkValue);
        }
      } else {
        lcd.print(dateElements[i].displayValue);
      }
  }
}