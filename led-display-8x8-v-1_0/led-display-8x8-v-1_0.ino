// SPI communication PINs
#define SPI_MOSI        12  // Master Out -> Slave In PIN
#define SPI_CS          11  // chip select PIN
#define SPI_SCK         10  // serial clock PIN

// MAX7219 registers used
#define OP_SHUTDOWN     12  // shutdown register, address XC
#define OP_DECODEMODE    9  // decode mode register, address X9
#define OP_DISPLAYTEST  15  // display test register, XF
#define OP_SCANLIMIT    11  // scan limit register, XB 

int darkDigit = 1;

void setup() {

   Serial.begin(9600);
   Serial.println("Setup START");

   // configure PINs
   pinMode(SPI_MOSI, OUTPUT);
   pinMode(SPI_CS, OUTPUT);
   pinMode(SPI_SCK, OUTPUT);
   // set CS PIN to HIGH as currently no data is transmitted
   digitalWrite(SPI_CS, HIGH);

   // wake up MAX7219 by set shutdown register to 1
   spiTransfer(OP_SHUTDOWN, 1);
   // spiTransfer(OP_DISPLAYTEST, 1); // turn on all 64 LEDs for testing

   // disable the decode mode so we are able to control
   // all 64 LEDs independently
   spiTransfer(OP_DECODEMODE, 0);

   // enable all 8 digits
   spiTransfer(OP_SCANLIMIT, 7);

   // clear display
   for(int i = 1; i <= 8; i++) {
      spiTransfer(i, B0);
   }

   // Turn off the 8 segements of the first digit available on the matrix.
   // Turn on the 8 segments of digit 2 to 8 available on the matrix.
   spiTransfer(darkDigit, B0); // digit 1
   spiTransfer(2, B11111111); // digit 2
   spiTransfer(3, B11111111); // digit 3
   spiTransfer(4, B11111111); // digit 4
   spiTransfer(5, B11111111); // digit 5
   spiTransfer(6, B11111111); // digit 6
   spiTransfer(7, B11111111); // digit 7
   spiTransfer(8, B11111111); // digit 8

   //testMostSignificantBit();
   //testLeastSignificantBit();

   Serial.println("Setup END");
}

void loop() { 

   // Turn on the 8 segments of previous dark digit.
   spiTransfer(darkDigit, B11111111);

   // Calculate new dark digit.
   darkDigit == 8 ? darkDigit = 0 : darkDigit += 1;

   // Turn off 8 segments of new dark digit.
   spiTransfer(darkDigit, B0);

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


void testMostSignificantBit() {
   byte two = B00000010;
   Serial.println("-----------------------------------------------------");
   Serial.println("Test most significant bit (MSB) with B2 value - START");
   for(int i = 0; i < 8; i++) {
      Serial.print("idx ");
      Serial.print(i);
      Serial.print(": ");
      // This expression is used in wiring_shift.c Arduino lib to shift out data
      // with MSB first.
      Serial.println(!!(two & (1 << (7 - i))));
   }
   Serial.println("Test most significant bit (MSB) with B2 value - END");
 }

 void testLeastSignificantBit() {
   byte two = B00000010;
   Serial.println("-----------------------------------------------------");
   Serial.println("Test least significant bit (LSB) with B2 value - START");
   for(int i = 0; i < 8; i++) {
      Serial.print("idx ");
      Serial.print(i);
      Serial.print(": ");
      // This expression is used in wiring_shift.c Arduino lib to shift out data
      // with LSB first.
      Serial.println(!!(two & (1 << i)));

      // Nice trick :)
      // Serial.println((two & (1 << i)));
      // Serial.println(!(two & (1 << i)));
   }
   Serial.println("Test least significant bit (LSB) with B2 value - END");
 }
