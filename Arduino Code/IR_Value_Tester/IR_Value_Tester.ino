int irPin = 2;
int irValue;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(irPin, INPUT);
  Serial.println("Here we go");
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  for(int i=0; i<200;i++){
    irValue += digitalRead(irPin);
    delay(5);
  }
  Serial.println(irValue);
  irValue = 0;
}



