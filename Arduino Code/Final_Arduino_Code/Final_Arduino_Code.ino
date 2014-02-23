/* 
* This is V1 of the final code for the Robonauts
* This serves solely as a communicator between the Input/Output and the Android Phone
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

int solenoidCharger = 22;
int solenoidKicker = 23;

int dribblerSwitch = 24;

int lightSensor = A0;

int IRSensor[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};  //Setup Array for IR Sensors


/*
*
* SETUP ANDROID ACCESSORY
*
*/

AndroidAccessory acc("BRHS RoboSoccer", "RobotCode", "Final code for the Arudino", "1.0", "www.brhsrobosoccer.com", "Serial");  // setup Arduino as Android Accessory

#define ARRAY_SIZE 20
#define COMMAND_TEXT 0xF
#define TARGET_DEFAULT 0xF 

byte rcvmsg[8];
byte sntmsg[3 + ARRAY_SIZE]; 

/*
*
* SETUP SENSORS
*
*/

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

int lightValue = 0;

int leftMotorPower;
int rightMotorPower;
int backMotorPower;

int dribblerOn = 0;
int kickerKick = 0;



void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  acc.powerOn();
  AFMS.begin();  // create with the default frequency 1.6KHz 
}

void loop() {
  if (acc.isConnected()) { 
    
    //Get Sensor Data
    getIRData();
    getUSData();
    getLightData();
    
    //Fill Communcication Array
    fillSntMsg();
    acc.write(sntmsg, 3 + ARRAY_SIZE); 
    
    
    //Read incoming message
    readRcvMsg();
    
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
}

void readRcvMsg(){
  int len = acc.read(rcvmsg, sizeof(rcvmsg), 1);
    if (len > 0) {
      if (rcvmsg[0] == COMMAND_TEXT) {
        if (rcvmsg[1] == TARGET_DEFAULT){         //get the textLength from the checksum byte
          if(rcvmsg[2] == 8){
            leftMotorPower = rcvmsg[3];
            rightMotorPower = rcvmsg[4];
            backMotorPower = rcvmsg[5];
            dribblerOn = rcvmsg[6];
            kickerKick = rcvmsg[7];
          }
        }
      }   
    }
}


void fillSntMsg(){
  //Communication protocol
  sntmsg[0] = COMMAND_TEXT;
  sntmsg[1] = TARGET_DEFAULT;
  sntmsg[2] = ARRAY_SIZE; 
  
  //Fill IR Values
   for(int i = 0; i < 15; i++) {
     sntmsg[3 + i] = irValue[i]; 
   }
   
   //Fill US Values  
   sntmsg[18] = USValueFront;
   sntmsg[19] = USValueLeft;
   sntmsg[20] = USValueBack;
   sntmsg[21] = USValueRight;
   
   //Fill Light Sensor Values
   sntmsg[22] = lightValue;
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
  lightValue = analogRead(lightSensor);
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
