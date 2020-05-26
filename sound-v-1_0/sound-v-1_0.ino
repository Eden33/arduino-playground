// the KY-037 PIN and max analog value it may return
static const uint8_t pinSoundSensor = A0;
static const int soundSensorMaxAnalogValue = 1023;

// variables to measure and control the noise
// of the KY-037 sound sensor
int currentNoise = -1;
int backgroundNoise = -1;
int maxNoise = -1;

// the measured noice is mapped to a level between 0 and 8
#define SOUND_LEVEL_MIN 0
#define SOUND_LEVEL_MAX 8
int currentNoiseLevel = 0; 

// the 3 buttons used for configuration
static const uint8_t pinDoUpdateBackroundNoise = 2;
static const uint8_t pinDoDecreaseMaxNoise = 4;
static const uint8_t pinDoIncreaseMaxNoise = 3;

// The MAX7219 PINs and registers used to control 
// the 8x8 LED matrix
#define SPI_MOSI        12  // Master Out -> Slave In PIN
#define SPI_CS          11  // chip select PIN
#define SPI_SCK         10  // serial clock PIN
#define OP_SHUTDOWN     12  // shutdown register, address XC
#define OP_DECODEMODE    9  // decode mode register, address X9
#define OP_SCANLIMIT    11  // scan limit register, XB 

void setup() {

   Serial.begin(9600);
   Serial.println("Setup START");

   // MAX7219 (8x8 LED matrix) - configuration START

   // configure PINs
   pinMode(SPI_MOSI, OUTPUT);
   pinMode(SPI_CS, OUTPUT);
   pinMode(SPI_SCK, OUTPUT);
   // set CS PIN to HIGH as currently no data is transmitted
   digitalWrite(SPI_CS, HIGH);

   // wake up MAX7219 by set shutdown register to 1
   spiTransfer(OP_SHUTDOWN, 1);

   // disable the decode mode so we are able to control
   // all 64 LEDs independently
   spiTransfer(OP_DECODEMODE, 0);

   // enable all 8 digits
   spiTransfer(OP_SCANLIMIT, 7);

   // clear display
   for(int i = 1; i <= 8; i++) {
      spiTransfer(i, B0);
   }

   // MAX7219 (8x8 LED matrix) - configuration END

   // all 3 buttons are HIGH in case not pressed
   pinMode(pinDoUpdateBackroundNoise, INPUT_PULLUP);
   pinMode(pinDoDecreaseMaxNoise, INPUT_PULLUP);
   pinMode(pinDoIncreaseMaxNoise, INPUT_PULLUP);

   Serial.println("Setup END");
}

void loop() { 

   //testAssureNoiseInValidRange();
   //testMapFunction();
   //return;

   backgroundNoise = getBackgroundNoise();
   maxNoise = getMaxNoise();
   currentNoise = analogRead(pinSoundSensor);
   currentNoise = assureNoiseInValidRange(currentNoise, 
                                             backgroundNoise, maxNoise);

   currentNoiseLevel = map(currentNoise, 
                              backgroundNoise, maxNoise,
                              SOUND_LEVEL_MIN, SOUND_LEVEL_MAX);

   Serial.print("Background noise: ");
   Serial.print(backgroundNoise);
   Serial.print(" Current noise: ");
   Serial.print(currentNoise);
   Serial.print(" Max noice: ");
   Serial.print(maxNoise);
   Serial.print(" Noice level: ");
   Serial.println(currentNoiseLevel);

   for(int i = (SOUND_LEVEL_MIN + 1); i <= SOUND_LEVEL_MAX; i++) {
      if(i <= currentNoiseLevel) {
         spiTransfer(i, B11111111);
      } else {
         spiTransfer(i, B0);
      }
   }

   // The display scan rate is 800 Hz in case all 
   // 8 digits on the MAX7219 are enabled.
   // This means the min update delay that makes sence is approximatelly ~12,5 milliseconds.
   // However, this is not an eye-catcher anymore ;)
   // delay(13);
   delay(100);
}

void spiTransfer( volatile byte opcode, volatile byte data) {
   digitalWrite(SPI_CS, LOW);
   shiftOut(SPI_MOSI, SPI_SCK, MSBFIRST, opcode);
   shiftOut(SPI_MOSI,SPI_SCK, MSBFIRST, data);
   digitalWrite(SPI_CS,HIGH);
}

/**
 * Calculates the background noise and stores it locally.
 * 
 * The function also returns the value calcuated to improve 
 * the code readability of the loop function.
 * 
 * @return The current background noise.
 **/
int getBackgroundNoise() {

   // we calculate the background noise
   // in case the variable is not initialized
   // or the update button is pressed
   if(digitalRead(pinDoUpdateBackroundNoise) == LOW 
      || backgroundNoise == -1) {

      // delay the calculation as the the button press
      // caused some noise
      delay(2000); 

      int sampleCount = 20;
      long sum = 0;
      int val = 0;
      for(int i = 0; i < sampleCount; i++) {
         delay(100);
         val = analogRead(pinSoundSensor);
         sum += val;
         Serial.println(val);
      }

      backgroundNoise = sum / sampleCount;
      Serial.print("Current background noise set to: ");
      Serial.println(backgroundNoise);
   }

   return backgroundNoise;
}

/**
 * Calculates the current max noise and stores it locally.
 * 
 * The function also returns the value calcuated to improve 
 * the code readability of the loop function.
 * 
 * @return The current max noise.
 **/
int getMaxNoise() {

   int singleStep = 10;
   int delayPerStep = 200;
   if(maxNoise == -1) {
      maxNoise = soundSensorMaxAnalogValue;
   }
   if(digitalRead(pinDoIncreaseMaxNoise) == LOW) {
      delay(delayPerStep);
      maxNoise += singleStep;
   } else if(digitalRead(pinDoDecreaseMaxNoise) == LOW) {
      delay(delayPerStep);
      maxNoise -= singleStep;
   }
   if(maxNoise > soundSensorMaxAnalogValue) {
      maxNoise = soundSensorMaxAnalogValue;
   }
   if(maxNoise <= backgroundNoise) {
      maxNoise = backgroundNoise + 1;
   }
   return maxNoise;
}

/**
 * Assures that the noise is in valid range for noise level calculation.
 * 
 * Valid range is minValue <= noiseValue <= maxValue.
 * 
 * @return The noise in valid range to calculate the noise level.
 **/
int assureNoiseInValidRange(int noiseValue, int minValue, int maxValue) {
   int diffMin = noiseValue - minValue;
   if(diffMin < 0) {
      noiseValue = minValue + abs(diffMin);
   }
   if(noiseValue > maxValue) {
      noiseValue = maxValue;
   }
   return noiseValue;
}

void testMapFunction() {
   Serial.println("-------");
   int test = map(10, 0, 100, 0, 200);
   Serial.println(test); // 20
   test = map(572, (1024 - 517), 1024, 0, 8);
   Serial.println(test); // 1
   test = map(300, (1024 - 517), 1024, 0, 8);
   Serial.println(test); // -3 -> out of new range
   test = map(2000, (1024 - 517), 1024, 0, 8);
}

void testAssureNoiseInValidRange() {
   Serial.println("-------------");
   int test = assureNoiseInValidRange(517, 517, 1024);
   Serial.println(test);
   test = assureNoiseInValidRange(1024, 517, 1024);
   Serial.println(test);
   test = assureNoiseInValidRange(516, 517, 1024);
   Serial.println(test);
   test = assureNoiseInValidRange(1025, 517, 1024);
   Serial.println(test);
}
