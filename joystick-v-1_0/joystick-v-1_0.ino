const int button_pin = 2; 
const int X_pin = A0;
const int Y_pin = A1;

void setup() {
  
  Serial.begin(9600);

  // The data direction mode for all pins is by default set to INPUT on startup 
  // The DDRx flag is set to 0 in this case.
  // pinMode(button_pin, OUTPUT);
  // pinMode(X_pin, OUTPUT);
	// pinMode(Y_pin, OUTPUT);

  Serial.print("A0 is in mode INPUT in case DDRC0 returns 0: ");
  Serial.println((DDRC & (1 << PC0)) >> PC0);
  Serial.print("A1 is in mode INPUT in case DDRC1 returns 0: ");
  Serial.println((DDRC & (1 << PC1)) >> PC1);
  Serial.print("Digial pin 2 is in mode INPUT in case DDRD2 returns 0: ");
  Serial.println((DDRD & (1 << PD2)) >> PD2);

  // The push button pin must be set to INPUT_PULLUP.
  // In case you push the button the electrical circuit is closed.
  // While you do so the pin goes to LOW.
  // You either can use Arduino pinMode to set the INPUT_PULLUP mode 
  // or do it directly on the register.
  // pinMode(button_pin, INPUT_PULLUP);
  Serial.print("Digial pin 2 is LOW in case PORTD2 returns 0: ");
  Serial.println((PORTD & (1 << PORTD2)) >> PORTD2);
  PORTD |= (1 << PORTD2); // PULLUP ( = HIGH)
  Serial.print("Digial pin 2 is HIGH in case PORTD2 returns 1: ");
  Serial.println((PORTD & (1 << PORTD2)) >> PORTD2);
}

void loop() {
  Serial.print("Push Button status read from Arduino lib: ");
  Serial.println(digitalRead(button_pin));
  Serial.print("Push Button status read from the register: ");
  Serial.println((PIND & (1 << PIND2)) >> PIND2);
  Serial.print("X-axis: ");
  Serial.println(analogRead(X_pin));
  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_pin));
  Serial.println("");
  delay(500);
}
