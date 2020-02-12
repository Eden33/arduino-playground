#include <Arduino.h>
#include <LiquidCrystal.h>

#define INIT_PHASE        1
#define WAIT_FOR_MOTION   2
#define MOTION_DETECTED   3
#define IDEL_TIME         4

int currentState = INIT_PHASE;
int initDelayInSeconds = 60;

unsigned long motionDetectedAtMillis = 0;

int pirPin = 7;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  currentState = INIT_PHASE;

  // set up the LCD and print initial init phase info
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("PIR sensor will");
  lcd.setCursor(0,1);
  lcd.print("be ready in: ");
  lcd.setCursor(13,1);
  char buffer[2];
  sprintf(buffer, "%2i", initDelayInSeconds);
  lcd.print(buffer);
}

void loop() {
  unsigned long currentMillis = millis();

  switch(currentState) {
    case INIT_PHASE:
      if(currentMillis >= (initDelayInSeconds * 1000UL)) {
        currentState = WAIT_FOR_MOTION;
        lcd.clear();
      } else {
        int secondsLeft = max(0, (initDelayInSeconds - int (currentMillis / 1000)));
        char buffer[2];
        sprintf(buffer, "%2i", secondsLeft);
        lcd.setCursor(13,1);
        lcd.print(buffer);
      }
      break;
    case WAIT_FOR_MOTION:
      lcd.setCursor(0,0);
      lcd.print("No motion.");
      if(digitalRead(pirPin) == HIGH) {
        currentState = MOTION_DETECTED;
        motionDetectedAtMillis = millis();
        lcd.clear();
      }
      break;
    case MOTION_DETECTED: 
      if(currentMillis <= motionDetectedAtMillis + 3000UL) {
        lcd.setCursor(0,0);
        lcd.print("Motion detected.");
      } else {
        currentState = IDEL_TIME;
        lcd.clear();
      }
      break;
    case IDEL_TIME:
      if(currentMillis <= motionDetectedAtMillis + 6000UL) {
        lcd.setCursor(0,0);
        lcd.print("Idle time: ");

        int secondsLeft = max(0, (motionDetectedAtMillis + 6000UL - currentMillis) / 1000);
        char buffer[2];
        sprintf(buffer, "%i", secondsLeft);
        lcd.setCursor(11,0);
        lcd.print(buffer);
      } else {
        currentState = WAIT_FOR_MOTION;
        lcd.clear();
      }
      break;
    default:
      break; // NoP
  }

  delay(100);
}