#define TRIG_PIN 12
#define ECHO_PIN 11

long currentDistance; // in centimter

void setup() {
   pinMode(ECHO_PIN, INPUT);
   digitalWrite(TRIG_PIN, LOW);
   pinMode(TRIG_PIN, OUTPUT);
   Serial.begin(9600);
   Serial.print("Clock: ");
   Serial.println(F_CPU);
   Serial.println("");
}

void loop() {
   calculateCurrentDistance();
   Serial.print(currentDistance);
   Serial.println(" cm");
   delay(1000);
}

void calculateCurrentDistance() {
    long d = 0;
    long _duration = 0;

    // set trigger pin to HIGH for at least 10 microseconds to 
    // trigger the acoustic bursts
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    // set tigger pin to LOW again 
    digitalWrite(TRIG_PIN, LOW);

    // The echo pin is HIGH as long as the bursts are not captured.
    _duration = pulseIn(ECHO_PIN, HIGH);

    // calculation based on the data sheet
    currentDistance = _duration / 58;

    // calculation based on the speed of the bursts (340 m/s)
    // 340 * 100 / 1000000 / 2
    // currentDistance = _duration * 0.017;

    // some delay before trigger next bursts
    delay(25);
}