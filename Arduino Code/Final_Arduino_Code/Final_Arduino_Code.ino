/* 
 * This is V1 of the final code for the Robosquids
 */

/*
 *
 * INCLUDE STATEMENTS
 *
 */

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <NewPing.h>
#include <Usb.h>
#include <AndroidAccessory.h>  

/*
*
 * DEFINING PINS
 *
 */

#define TRIGGER_PIN_FRONT  14  // Arduino pin tied to trigger pin on the FRONT facing ultrasonic sensor.
#define ECHO_PIN_FRONT     15  // Arduino pin tied to echo pin on the FRONT facing ultrasonic sensor.
#define TRIGGER_PIN_LEFT  16  // Arduino pin tied to trigger pin on the LEFT facing ultrasonic sensor.
#define ECHO_PIN_LEFT     17  // Arduino pin tied to echo pin on the LEFT facing ultrasonic sensor.
#define TRIGGER_PIN_BACK  18  // Arduino pin tied to trigger pin on the BACK facing ultrasonic sensor.
#define ECHO_PIN_BACK     19  // Arduino pin tied to echo pin on the BACK facing ultrasonic sensor.
#define TRIGGER_PIN_RIGHT  20  // Arduino pin tied to trigger pin on the RIGHT facing ultrasonic sensor.
#define ECHO_PIN_RIGHT     21// Arduino pin tied to echo pin on the RIGHT facing ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

int HMC6352Address = 0x42;
int slaveAddress;
byte headingData[2];
int headingValue;

int solenoidCharger = 22;
int solenoidKicker = 23;

int dribblerSwitch = 24;

int leftLightSensor = A0;
int rightLightSensor = A1;

int photogate = A3;

int laser = 44;


int IRSensor[14] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};  //Setup Array for IR Sensors


/*
*
 * SETUP SENSORS
 *
 */


int light_threshold =  50;

NewPing sonarFront(TRIGGER_PIN_FRONT, ECHO_PIN_FRONT, MAX_DISTANCE); // NewPing setup of pins and maximum distance for front US Sensor.
NewPing sonarLeft(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE); // NewPing setup of pins and maximum distance for left US Sensor.
NewPing sonarBack(TRIGGER_PIN_BACK, ECHO_PIN_BACK, MAX_DISTANCE); // NewPing setup of pins and maximum distance for back US Sensor.
NewPing sonarRight(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE); // NewPing setup of pins and maximum distance for right US Sensor.

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// And connect the 3 DC motors to ports M1, M2 & M3
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);
Adafruit_DCMotor *backMotor = AFMS.getMotor(3);


/*
*
 * SETUP GLOABAL VARIABLES TO STORE SENSOR DATA
 *
 */

unsigned int USValueFront; //Setup Variable for the data from the front US Sensor
unsigned int USValueLeft; //Setup Variable for the data from the left US Sensor
unsigned int USValueBack; //Setup Variable for the data from the back US Sensor
unsigned int USValueRight; //Setup Variable for the data from the right US Sensor

int irValue[14];  //Setup Array to hold values of all IR Sensors

int leftLightValue = 0;
int rightLightValue = 0;

int leftMotorPower;
int rightMotorPower;
int backMotorPower;

int initialCompass;
int absoluteCompass;
int relativeCompass;

int dribblerOn = 0;
int kickerKick = 0;

int haveBall = 0;


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  initializeCompass();  
  Wire.begin();
  digitalWrite(laser, HIGH);
}

void loop() {
  //Get Sensor Data
  getIRData();
  getUSData();
  getLightData();
  getCompassReading();
  checkHasBall();
  
  
  //Output motors
  driveRobot();

  //Output Dribbler
  if(dribblerOn == 1){
    digitalWrite(dribblerSwitch, HIGH);
  }
  else{
    digitalWrite(dribblerSwitch, LOW);
  }

  //Output Kicker
  if(dribblerOn == kickerKick){
    kickBall();
  }
}

void checkHasBall(){
  if(digitalRead(photogate) > 150){
    haveBall = 0;
  }
  else{
    haveBall = 1;
  }
}

void initializeCompass(){
  slaveAddress = HMC6352Address >> 1;   // This results in 0x21 as the address to pass to TWI
  Wire.beginTransmission(slaveAddress);
  Wire.write("A");              // The "Get Data" command
  Wire.endTransmission();
  delay(10);                   // The HMC6352 needs at least a 70us (microsecond) delay
  // after this command.  Using 10ms just makes it safe
  // Read the 2 heading bytes, MSB first
  // The resulting 16bit word is the compass heading in 10th's of a degree
  // For example: a heading of 1345 would be 134.5 degrees
  Wire.requestFrom(slaveAddress, 2);        // Request the 2 byte heading (MSB comes first)
  for(int w =0; Wire.available() && w < 2; w++){ 
    headingData[w] = Wire.read();
  }
  headingValue = headingData[0]*256 + headingData[1];  // Put the MSB and LSB together
  initialCompass = headingValue / 10;
}

void getCompassReading(){
  Wire.beginTransmission(slaveAddress);
  Wire.write("A");              // The "Get Data" command
  Wire.endTransmission();
  delay(10);                   // The HMC6352 needs at least a 70us (microsecond) delay
  // after this command.  Using 10ms just makes it safe
  // Read the 2 heading bytes, MSB first
  // The resulting 16bit word is the compass heading in 10th's of a degree
  // For example: a heading of 1345 would be 134.5 degrees
  Wire.requestFrom(slaveAddress, 2);        // Request the 2 byte heading (MSB comes first)
  for(int w =0; Wire.available() && w < 2; w++)
  { 
    headingData[w] = Wire.read();
  }
  headingValue = headingData[0]*256 + headingData[1];  // Put the MSB and LSB together
  absoluteCompass = headingValue / 10;
  relativeCompass = absoluteCompass - initialCompass;
}

void getIRData(){
  for(int j = 0; j<14; j++){  //wipe all previous data from last cycle
    irValue[j] = 0;
  }

  for(int i=0; i<10;i++){  // Get 10 samples per cycle
    for(int j = 0; j<14; j++){  // Get samples from all 14 sensors
      irValue[j] += digitalRead(IRSensor[j]);
    }
    delay(20);  // Wait 20 miliseconds between samples
  }
}

void getUSData(){
  USValueFront = sonarFront.ping(); // Send ping, get ping time in microseconds (uS).
  USValueFront = USValueFront / US_ROUNDTRIP_CM;  // Convert ping time to distance result (0 = outside set distance range, no ping echo)
  USValueLeft = sonarLeft.ping(); // Send ping, get ping time in microseconds (uS).
  USValueLeft = USValueLeft / US_ROUNDTRIP_CM;  // Convert ping time to distance result (0 = outside set distance range, no ping echo)
  USValueBack = sonarBack.ping(); // Send ping, get ping time in microseconds (uS).
  USValueBack = USValueBack / US_ROUNDTRIP_CM;  // Convert ping time to distance result (0 = outside set distance range, no ping echo)
  USValueRight = sonarRight.ping(); // Send ping, get ping time in microseconds (uS).
  USValueRight = USValueRight / US_ROUNDTRIP_CM;  // Convert ping time to distance result (0 = outside set distance range, no ping echo)
}

void getLightData(){
  leftLightValue = analogRead(leftLightSensor);
  rightLightValue = analogRead(rightLightSensor);
}

void kickBall(){
  digitalWrite(solenoidKicker,HIGH);
  delay(500);
  digitalWrite(solenoidKicker,LOW);
  digitalWrite(solenoidCharger, HIGH);
  delay(10000);
  digitalWrite(solenoidCharger, LOW);
}

void driveRobot(){
  if(leftMotorPower==0){
    leftMotor->run(RELEASE);
  }
  else if(leftMotorPower > 0){
    leftMotor->run(FORWARD);
  }
  else{
    leftMotor->run(BACKWARD);
    leftMotorPower *= -1;
  }
  leftMotor->setSpeed(leftMotorPower);

  if(rightMotorPower==0){
    rightMotor->run(RELEASE);
  }
  else if(rightMotorPower > 0){
    rightMotor->run(FORWARD);
  }
  else{
    rightMotor->run(BACKWARD);
    rightMotorPower *= -1;
  }
  rightMotor->setSpeed(rightMotorPower);

  if(leftMotorPower==0){
    backMotor->run(RELEASE);
  }
  else if(backMotorPower > 0){
    backMotor->run(FORWARD);
  }
  else{
    backMotor->run(BACKWARD);
    backMotorPower *= -1;
  }
  backMotor->setSpeed(backMotorPower);
}

