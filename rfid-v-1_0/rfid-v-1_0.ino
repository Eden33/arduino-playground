
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN         9          
#define SS_PIN          10         

MFRC522 mfrc522(SS_PIN, RST_PIN); 
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

MFRC522::MIFARE_Key key;
MFRC522::PICC_Type piccType;

const byte sector         = 1;
const byte blockAddr      = 4;
const byte trailerBlock   = 7;

#define STATE_READ_NEXT			1
#define STATE_DISPLAY_COUNTER	2
#define STATE_RESET_COUNTER		3
#define STATE_DISPLAY_ERROR 	4

const int stateDelayInMillis = 1500;

int currentState = STATE_READ_NEXT;
unsigned long currentStateSinceMillis = 0;
const char * errorInfo;

byte dataBuffer[18];
byte dataBufferSize;
const byte resetReadCounterThreshold = 3;


void setup() {
	Serial.begin(9600);	

	// init SPI and MFRC522 library
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xff;
    }

	// Prepare data buffer we use for read/write operations
	dataBufferSize = sizeof(dataBuffer) / sizeof(dataBuffer[1]);
	for(byte i = 0; i < dataBufferSize; i++) {
		dataBuffer[i] = 0;
	}

	lcd.begin(16, 2);
}

void loop() {

	//dump_picc_to_serial();
	//return;

	unsigned long currentMillis = millis();

	switch(currentState) {
		case STATE_READ_NEXT:
			lcd.setCursor(0,0);
			lcd.write("No RFID tag ");
			lcd.setCursor(0,1);
			lcd.write("detected.");
			mfrc522_updateCounter();
			break;
		case STATE_DISPLAY_COUNTER:
			if((currentMillis - currentStateSinceMillis) < stateDelayInMillis) {
				lcd.setCursor(0, 0);
				lcd.write("RFID scanned ");
				lcd.setCursor(13, 0);
				String scanCount = String(dataBuffer[0]);
				lcd.write(scanCount.c_str());
				lcd.setCursor(0, 1);
				lcd.write("times.");
			} else {
				currentState = STATE_READ_NEXT;
				currentStateSinceMillis = millis();
				lcd.clear(); // prepare for new message
			}
			break;
		case STATE_RESET_COUNTER:
			if((currentMillis - currentStateSinceMillis) < stateDelayInMillis) {
				lcd.setCursor(0, 0);
				lcd.write("Reset scan");
				lcd.setCursor(0, 1);
				lcd.write("counter.");
			} else {
				currentState = STATE_READ_NEXT;
				currentStateSinceMillis = millis();
				lcd.clear(); // prepare for new message
			}
			break;
		case STATE_DISPLAY_ERROR:
			if((currentMillis - currentStateSinceMillis) < stateDelayInMillis) {
				lcd.setCursor(0, 0);
				lcd.write(errorInfo);
			} else {
				currentState = STATE_READ_NEXT;
				currentStateSinceMillis = millis();
				errorInfo = ""; // clear error msg
				lcd.clear(); // prepare for new message
			}
			break;
		default:
		    break; // NoP
	}
}

void mfrc522_updateCounter() {

	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

    piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
			currentState = STATE_DISPLAY_ERROR;
			currentStateSinceMillis = millis();
			errorInfo = "Invalid RFID tag";
			lcd.clear(); // prepare for new message
        	return;
    }

	if(!mfrc522_auth()) {
		return;
	}

    MFRC522::StatusCode status;

	//Read data from the block
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, dataBuffer, &dataBufferSize);

    if (status != MFRC522::STATUS_OK) {
		currentState = STATE_DISPLAY_ERROR;
		currentStateSinceMillis = millis();
		errorInfo = "RFID read fail";
		lcd.clear(); // prepare for new message
		halt_picc_communication();
        return;
    }

	dataBuffer[0]++;

	boolean isReset = false;
	if(dataBuffer[0] > resetReadCounterThreshold) {
		dataBuffer[0] = 0;
		isReset = true;
	}

	if(!mfrc522_auth()) {
		return;
	}

    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBuffer, 16);
    if (status != MFRC522::STATUS_OK) {
 		currentState = STATE_DISPLAY_ERROR;
		currentStateSinceMillis = millis();
		errorInfo = "RFID write fail";
		lcd.clear(); // prepare for new message
		halt_picc_communication();
        return;
    }

	if(isReset) {
		currentState = STATE_RESET_COUNTER;
		currentStateSinceMillis = millis();
		lcd.clear(); // prepare for new message
		halt_picc_communication();
		return;
	}

	currentState = STATE_DISPLAY_COUNTER;
	currentStateSinceMillis = millis();
	lcd.clear(); // prepare for new message
	halt_picc_communication();
}

boolean mfrc522_auth() {

	// Authenticate using key A
	MFRC522::StatusCode status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
	
	if (status != MFRC522::STATUS_OK) {
		currentState = STATE_DISPLAY_ERROR;
		currentStateSinceMillis = millis();
		errorInfo = "RFID auth fail";
		lcd.clear(); // prepare for new message
		return false;
	}
	return true;
}


/**
 * Dump EEPROM to serial.
 **/
void dump_picc_to_serial() {
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

/**
 * After this was executed you are required to remove the PICC from PCD before 
 * next read can happen.
 **/
void  halt_picc_communication() {
	mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}
