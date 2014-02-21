/* 
This is V1 of the final code for the Robonauts
This serves solely as a communicator between the Input/Output and the Android Phone
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h>
#include <NewPing.h>
#include <Usb.h>
#include <AndroidAccessory.h>  

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


AndroidAccessory acc("BRHS RoboSoccer", "ArduinoCode", "Final code for the Arudino", "1.0", "www.brhsrobosoccer.com", "Serial");  
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// And connect the 3 DC motors to ports M1, M2 & M3
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);
Adafruit_DCMotor *backMotor = AFMS.getMotor(3);


IRSensor[14]  = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};  //Setup Array for IR Sensors





void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  
  acc.powerOn();
  
  AFMS.begin();  // create with the default frequency 1.6KHz 
}

void loop() {
  
}


void getUSDate(){
  unsigned int USValue = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
