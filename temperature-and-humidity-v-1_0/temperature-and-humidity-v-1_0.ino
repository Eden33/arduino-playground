#include <Arduino.h>

byte data[5];
static const int dataPin = 2;

void setup() {
    Serial.begin(9600);
}

void loop() {

    unsigned long pinStatusInMicros = 0;

    //testBitOperationsNeeded();
    //return;

    // data sheet step 1
    // DHT11 (slave) is in INPUT mode
    // the dataPin is HIGH

    delay(2000);
    //Serial.println(digitalRead(dataPin));
    
    // data sheet step 2
    // start signal:
    // 1) Write LOW to the dataPin for at least 18 milliseconds.
    // 2) Write HIGH to the dataPin for short period of time. 
    //    Unfortunatelly this time period isn't documented well in the data sheet. 
    //    You can only find it in the "Data Timing Diagram" on page 4.
    pinMode(dataPin, OUTPUT);
    digitalWrite(dataPin, LOW);
    delay(20);
    digitalWrite(dataPin, HIGH);
    delayMicroseconds(15); 
    pinMode(dataPin, INPUT);

    // data sheet step 3
    // In case the "start signal" of step 2 was accepted slave replies with:
    // 80 microseconds LOW followed by 80 microseconds HIGH
    pinStatusInMicros = getPinStatusInMicros(LOW);
    if(pinStatusInMicros == 0 || pinStatusInMicros > 80) {
        Serial.print("Handshake failed in step 3 LOW state check. LOW in micros: ");
        Serial.println(pinStatusInMicros);
       return;
    }
    
     // Serial.print("LOW slave handshake in micros: ");
     // Serial.println(pinStatusInMicros); 

    pinStatusInMicros = getPinStatusInMicros(HIGH);
    if(pinStatusInMicros == 0 || pinStatusInMicros > 80) {
        Serial.print("Handshake failed in step 3 HIGH state check. HIGH in micros:");
        Serial.println(pinStatusInMicros);
        return;
    }

    // Serial.print("HIGH slave handshake in micros: ");
    // Serial.println(pinStatusInMicros);

    // data sheet step 4
    // receive the 40 data bits
    // Byte 1 contains the integer part of the relative humidity
    // Byte 2 contains the fractional part of the relative humidity
    // Byte 3 contains the integer part of the temperature
    // Byte 4 contains the fractional part of the temperature
    // Byte 5 contains parity bit data

    for(int dataIdx = 0; dataIdx < sizeof(data)/sizeof(data[0]); dataIdx++) {
        for(int bitIdx = 7; bitIdx >= 0; bitIdx--) {
            
            // each bit starts with 50 micros LOW
            pinStatusInMicros = getPinStatusInMicros(LOW);
            if(pinStatusInMicros == 0 || pinStatusInMicros > 50) {
                return;
            }

            // depending on HIGH time interval it's a 0 or 1 bit
            pinStatusInMicros = getPinStatusInMicros(HIGH);
            if(pinStatusInMicros == 0) {
                return;
            }
            if(pinStatusInMicros < 50) {
                data[dataIdx] &= ~(0x1 << bitIdx);
            } else {
                data[dataIdx] |= 0x1 << bitIdx;
            }
        }
    }

/*     for(int dataIdx = 0; dataIdx < sizeof(data)/sizeof(data[0]); dataIdx++) {
        Serial.println(data[dataIdx], BIN);
    } */
        
    // check parity byte according to data sheet specification
    byte parity = data[4];
    byte check = data[0] + data[1] + data[2] + data[3];
    if(parity != check) {
        //printData();
        Serial.print("Parity byte check failed!");
        Serial.print(" Parity Byte: ");
        Serial.print(parity, BIN);
        Serial.print(" Check calculated: ");
        Serial.println(check, BIN);
        return;
    }
    
    //printData();
    Serial.print(data[2]);
    Serial.print(".");
    Serial.print(data[3]);
    Serial.print(" C Temp, ");
    Serial.print(data[0]);
    Serial.print(".");
    Serial.print(data[1]);
    Serial.println(" % RH");
}

/**
 * Returns 0 in case the dataPin was not in expected status while executed.
 * In case the dataPin was in expected status the approximated time frame it remained in this status is returned.
 * The returned time frame is an approximated value that will be lower than it was in real.
 **/ 
long getPinStatusInMicros(int expectedPinStatus) {
    unsigned long pinStatusCheckStartMicros = 0;
    unsigned long pinStatusCheckRandMicros = 0;

    pinStatusCheckStartMicros = micros();
    pinStatusCheckRandMicros = pinStatusCheckStartMicros;

    while(digitalRead(dataPin) == expectedPinStatus && (pinStatusCheckRandMicros - pinStatusCheckStartMicros <= 100)) {
        pinStatusCheckRandMicros = micros();
    }
    return pinStatusCheckRandMicros - pinStatusCheckStartMicros;
}

void printData() {
    for(int i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
        Serial.print(data[i], BIN);
        Serial.print(" ");
    }
    Serial.println("");
}

void testBitOperationsNeeded() {
    byte test = B01010011;
    Serial.println("--------");
    Serial.println(test, BIN); // 01010011
    test |= 0x1 << 7;
    Serial.println(test, BIN); // 11010011
    test &= ~(0x1 << 6);
    Serial.println(test, BIN); // 10010011
    delay(5000);
}
