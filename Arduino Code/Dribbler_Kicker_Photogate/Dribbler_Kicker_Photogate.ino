/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438

This sketch creates a fun motor party on your desk *whiirrr*
Connect a unipolar/bipolar stepper to M3/M4
Connect a DC motor to M1
Connect a hobby servo to SERVO1
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h> 

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// And connect a DC motor to port M1
Adafruit_DCMotor *dribbler = AFMS.getMotor(2);
// We'll also test out the built in Arduino Servo library

 int charger = 25;
 int kicker = 24;

int LDR_Pin = A10; //analog pin 1

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Here We Go!");

  pinMode(charger, OUTPUT);
  pinMode(kicker, OUTPUT);

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
   
  // turn on motor M1
  dribbler->setSpeed(160);
  dribbler->run(FORWARD);
  digitalWrite(charger, HIGH);
  delay(7000);
  digitalWrite(charger, LOW);
}

void loop() {
  if(analogRead(LDR_Pin) < 280){
    delay(500);
    dribbler->setSpeed(20);
    delay(750);
    digitalWrite(kicker,HIGH);
    delay(500);
    digitalWrite(kicker,LOW);
    dribbler->setSpeed(100);
    digitalWrite(charger, HIGH);
    delay(12000);
    digitalWrite(charger, LOW);
  } 
}
