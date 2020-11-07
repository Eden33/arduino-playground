#include <Arduino.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
static const long delayAfterDataPrint = 2000;
static const int DHT11_dataPin = 8;
static const int thermistorDataPin = 0;

byte DHT11_tmpData[5];

typedef struct {
  byte data[5] = {0, 0, 0, 0, 0};             
  int dataDisplayCount = 0;
} DHT11_sensor_data_t;

DHT11_sensor_data_t DHT11_latestData;

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
}

void loop() {
    DHT11_read();
    DHT11_print();
    delay(delayAfterDataPrint);
    thermistorReadAndPrint();
    delay(delayAfterDataPrint);
}

void thermistorReadAndPrint() {
    int rawDataVal = analogRead(thermistorDataPin);

    // using the Steinhart–Hart equation to approximate the temparature
    double r = log(10000.0 * (1024.0 / max(1, rawDataVal) - 1));
    double tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * r * r )) * r ); 
    float tempC = tempK - 273.15;            // Convert Kelvin to Celcius

    Serial.print("rawDataVal: ");
    Serial.print(rawDataVal);
    Serial.print(" r: ");
    Serial.print(r);
    Serial.print(" tempK: ");
    Serial.print(tempK);
    Serial.print(" tempC: ");
    Serial.println(tempC);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("Thermistor");

    lcd.setCursor(0, 1);
    lcd.write("Temp C: ");
    lcd.setCursor(8, 1);

    lcd.print(tempC);
}

void DHT11_read() {

    unsigned long pinStatusInMicros = 0;

    // data sheet step 1
    // DHT11 (slave) is in INPUT mode
    // the dataPin is HIGH
    
    // data sheet step 2
    // start signal:
    // 1) Write LOW to the dataPin for at least 18 milliseconds.
    // 2) Write HIGH to the dataPin for short period of time. 
    //    Unfortunatelly this time period isn't documented well in the data sheet. 
    //    You can only find it in the "Data Timing Diagram" on page 4.
    pinMode(DHT11_dataPin, OUTPUT);
    digitalWrite(DHT11_dataPin, LOW);
    delay(20);
    digitalWrite(DHT11_dataPin, HIGH);
    delayMicroseconds(15); 
    pinMode(DHT11_dataPin, INPUT);

    // data sheet step 3
    // In case the "start signal" of step 2 was accepted slave replies with:
    // 80 microseconds LOW followed by 80 microseconds HIGH
    pinStatusInMicros = DHT11_getPinStatusInMicros(LOW);
    if(pinStatusInMicros == 0 || pinStatusInMicros > 80) {
        Serial.print("Handshake failed in step 3 LOW state check. LOW in micros: ");
        Serial.println(pinStatusInMicros);
       return;
    }
    
     // Serial.print("LOW slave handshake in micros: ");
     // Serial.println(pinStatusInMicros_DHT11); 

    pinStatusInMicros = DHT11_getPinStatusInMicros(HIGH);
    if(pinStatusInMicros == 0 || pinStatusInMicros > 80) {
        Serial.print("Handshake failed in step 3 HIGH state check. HIGH in micros:");
        Serial.println(pinStatusInMicros);
        return;
    }

    // Serial.print("HIGH slave handshake in micros: ");
    // Serial.println(pinStatusInMicros_DHT11);

    // data sheet step 4
    // receive the 40 data bits
    // Byte 1 contains the integer part of the relative humidity
    // Byte 2 contains the fractional part of the relative humidity
    // Byte 3 contains the integer part of the temperature
    // Byte 4 contains the fractional part of the temperature
    // Byte 5 contains parity bit data

    for(int dataIdx = 0; dataIdx < sizeof(DHT11_tmpData)/sizeof(DHT11_tmpData[0]); dataIdx++) {
        for(int bitIdx = 7; bitIdx >= 0; bitIdx--) {

            // each bit starts with 50 micros LOW
            pinStatusInMicros = DHT11_getPinStatusInMicros(LOW);
            if(pinStatusInMicros == 0 || pinStatusInMicros > 50) {
                return;
            }

            // depending on HIGH time interval it's a 0 or 1 bit
            pinStatusInMicros = DHT11_getPinStatusInMicros(HIGH);
            if(pinStatusInMicros == 0) {
                return;
            }
            if(pinStatusInMicros < 50) {
                DHT11_tmpData[dataIdx] &= ~(0x1 << bitIdx);
            } else {
                DHT11_tmpData[dataIdx] |= 0x1 << bitIdx;
            }
        }
    }
        
    // check parity byte according to data sheet specification
    byte parity = DHT11_tmpData[4];
    byte check = DHT11_tmpData[0] + DHT11_tmpData[1] + DHT11_tmpData[2] + DHT11_tmpData[3];
    if(parity != check) {
        Serial.print("Parity byte check failed!");
        Serial.print(" Parity Byte: ");
        Serial.print(parity, BIN);
        Serial.print(" Check calculated: ");
        Serial.println(check, BIN);
        return;
    }

    // the data is ok we can proceed display it later 
    // copy to main data array
    for(int dataIdx = 0; dataIdx < sizeof(DHT11_tmpData)/sizeof(DHT11_tmpData[0]); dataIdx++) {
        DHT11_latestData.data[dataIdx] = DHT11_tmpData[dataIdx];
    }

    //reset display count
    DHT11_latestData.dataDisplayCount = 0;
}

void DHT11_print() {

    DHT11_latestData.dataDisplayCount += 1;

    if(DHT11_latestData.dataDisplayCount <= 5) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("DHT11 - RH: ");
        lcd.setCursor(12, 0);

        // relative humidity
        char rh[4];
        sprintf(rh, "%d.%d", DHT11_latestData.data[0], DHT11_latestData.data[1]);
        lcd.write(rh);

        lcd.setCursor(0, 1);
        lcd.write("Temp C: ");
        lcd.setCursor(8, 1);

        char temp[4];
        sprintf(temp, "%d.%d", DHT11_latestData.data[2], DHT11_latestData.data[3]);
        lcd.write(temp);

        Serial.print(DHT11_latestData.data[2]);
        Serial.print(".");
        Serial.print(DHT11_latestData.data[3]);
        Serial.print(" C Temp, ");
        Serial.print(DHT11_latestData.data[0]);
        Serial.print(".");
        Serial.print(DHT11_latestData.data[1]);
        Serial.println(" % RH");
    } else {
        // display warning
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("DHT11 data");
        lcd.setCursor(0, 1);
        lcd.write("outdated!");
    }
}

/**
 * Returns 0 in case the dataPin was not in expected status while executed.
 * In case the dataPin was in expected status the approximated time frame it remained in this status is returned.
 * The returned time frame is an approximated value that will be lower than it was in real.
 **/ 
long DHT11_getPinStatusInMicros(int expectedPinStatus) {
    unsigned long pinStatusCheckStartMicros = 0;
    unsigned long pinStatusCheckRandMicros = 0;

    pinStatusCheckStartMicros = micros();
    pinStatusCheckRandMicros = pinStatusCheckStartMicros;

    while(digitalRead(DHT11_dataPin) == expectedPinStatus && (pinStatusCheckRandMicros- pinStatusCheckStartMicros <= 100)) {
        pinStatusCheckRandMicros = micros();
    }
    return pinStatusCheckRandMicros - pinStatusCheckStartMicros;
}
