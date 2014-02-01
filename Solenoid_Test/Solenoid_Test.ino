 int charger = 2;
 int button = 8;
 int kicker = 48;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(charger, OUTPUT);
  pinMode(button, INPUT);
  pinMode(kicker, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(kicker,HIGH);
  delay(500);
  digitalWrite(kicker,LOW);
  delay(2000);
  digitalWrite(charger, HIGH);
  delay(7500);
  digitalWrite(charger, LOW);
}
