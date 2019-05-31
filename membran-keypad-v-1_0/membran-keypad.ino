char keyPadChars[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[] = {9, 8, 7, 6};
byte colPins[] = {5, 4, 3, 2};

char previousKeyPressed;
char currentKeyPressed;
unsigned long startTime = 10;

void setup(){
  Serial.begin(9600);

  // iterate over keypad characters and print them to Serial
  /*
  for(byte rowIdx = 0; rowIdx < sizeof(keyPadChars) / sizeof(keyPadChars[0]); rowIdx++) {
    for(byte colIdx = 0; colIdx < sizeof(keyPadChars[0]) / sizeof(keyPadChars[0][0]); colIdx++) {
      Serial.println(keyPadChars[rowIdx][colIdx]);
    }
  }
  */

  // initialize all pins
  for(byte rowIdx = 0; rowIdx < (sizeof(rowPins) / sizeof(rowPins[0])); rowIdx++) {
    pinMode(rowPins[rowIdx], OUTPUT);
    digitalWrite(rowPins[rowIdx], LOW);
  }

  for(byte colIdx = 0; colIdx < (sizeof(colPins) / sizeof(colPins[0])); colIdx++) {
    pinMode(colPins[colIdx], INPUT_PULLUP);
  }

}

void loop(){

  if((millis() - startTime) > 10) {
    determineCurrentKeyPressed();
    
    if(currentKeyPressed != previousKeyPressed) {
      Serial.print("Key pressed: ");
      Serial.println(currentKeyPressed);
      previousKeyPressed = currentKeyPressed;
    }

    startTime = millis();
  }
}

void determineCurrentKeyPressed() {

  int lowColPinIdx = -1;

  // Column pins are configured in INPUT_PULLUP mode.
  // Row pins are configured in LOW mode.
  // Once a button is pressed the corresponding column pin will enter LOW status.
  for(byte colIdx = 0; colIdx < sizeof(colPins) / sizeof(colPins[0]); colIdx++) {
    if(digitalRead(colPins[colIdx]) == LOW && lowColPinIdx == -1) {
      lowColPinIdx = colIdx;
      break;
    }
  }

  int rowPinIdx = -1;

  if(lowColPinIdx != -1) {
    // Currently a button is pressed and we know the column index in the pin array. 
    // Probe the row pins by switch them from LOW to HIGH.
    // On each probe check if the column pin is still LOW.
    // We found the button in case the column pin changes to HIGH.
    for(byte rowIdx = 0; rowIdx < sizeof(rowPins) / sizeof(rowPins[0]); rowIdx++) {
      digitalWrite(rowPins[rowIdx], HIGH);

      if(digitalRead(colPins[lowColPinIdx]) == HIGH && rowPinIdx == -1) {
        rowPinIdx = rowIdx;  
      }

      digitalWrite(rowPins[rowIdx], LOW);

      if(rowPinIdx != -1) {
        break;
      }
    }   
  } else {
    // Either no button was ever pressed or previous button pressed was released.
    // By reset next two variables we make sure that the same button can be pressed again 
    // and it will be detected by the program.
    currentKeyPressed = 0;
    previousKeyPressed = 0;
  }

  if(lowColPinIdx != -1 && rowPinIdx != -1) {
    currentKeyPressed = keyPadChars[rowPinIdx] [lowColPinIdx];
  }
}
