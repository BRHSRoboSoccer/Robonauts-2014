 int charger = 25;
 int kicker = 24;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(charger, OUTPUT);
  pinMode(kicker, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(kicker,HIGH);
  delay(500);
  digitalWrite(kicker,LOW);
  delay(2000);
  digitalWrite(charger, HIGH);
  delay(10000);
  digitalWrite(charger, LOW);
  delay(1000);
}
