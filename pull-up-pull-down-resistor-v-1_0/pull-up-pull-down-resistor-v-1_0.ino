int pushButtonPin = 2;

void setup() {
  Serial.begin(9600);
  // configure the pin as INPUT for pull-down-external and pull-up-external
  pinMode(pushButtonPin, INPUT); 

  // configure the pin as INPUT_PULLUP for pull-up-internal
  // pinMode(pushButtonPin, INPUT_PULLUP);
}

void loop() {
  delay(250);

  // For pull-up-external and pull-up-internal
  // the pin will be 1 (=HIGH) in case the push button is not pressed.
  // Why: In both cases the pin is connected in series to either 
  // the external pull up resistor / or internal pull up resistor 
  // that is connected to 5 Volt power supply. However, as the circuit is not connected
  // to ground this voltage doesn't drop accross the pull up resistor and the pin reads 5 Volt.
  // Finally this changes in case you press the push button. In this case a connection to 
  // ground will be establised, the 5 Volt will drop accross the pull up resistor and
  // the pin will read 0 Volt (=LOW).

  // For pull-down-external
  // the pin will be 0 (=LOW) in case the push button is not pressed.
  // Why: In case the push button is not pressed the pin and the pull down resistor are 
  // already connected in series to ground. The noise incoming from the pin (=floating pin) drops 
  // accross the pull down resistor. Finally this changes in case you press the push button.
  // In this case the connection in series will become a connection in parallel and on
  // the new line 5 Volt is incoming. This 5 Volt drops accross the pull down resistor
  // part of the first line in the paralell curcuit. On the second line this 5 Volt 
  // terminates on the pin which will read 5 Volt.

  int buttonState = digitalRead(pushButtonPin);
  Serial.println(buttonState);
}
