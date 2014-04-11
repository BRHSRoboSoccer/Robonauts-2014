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
#include <Servo.h> 


/*
 *
 * DEFINING PINS
 *
 */


#define MOTOR_SWITCH_THRESHHOLD   1005
#define LIGHT_BARRIER_THRESHHOLD  200
#define TRIGGER_PIN_LEFT  36  // Arduino pin tied to trigger pin on the LEFT facing ultrasonic sensor.
#define ECHO_PIN_LEFT     38  // Arduino pin tied to echo pin on the LEFT facing ultrasonic sensor.
#define TRIGGER_PIN_BACK  32  // Arduino pin tied to trigger pin on the BACK facing ultrasonic sensor.
#define ECHO_PIN_BACK     34  // Arduino pin tied to echo pin on the BACK facing ultrasonic sensor.
#define TRIGGER_PIN_RIGHT  28  // Arduino pin tied to trigger pin on the RIGHT facing ultrasonic sensor.
#define ECHO_PIN_RIGHT   30  // Arduino pin tied to echo pin on the RIGHT facing ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

int HMC6352Address = 0x42;
int slaveAddress;
byte headingData[2];
int headingValue;

int kicker = 24;
int charger = 25;

int leftLightSensor = A8;
int rightLightSensor = A9;

int photogate = A10;

int motorStopper = A11;
  
//Setup Array for IR Sensors
int IRSensor[14];


/*
*
 * SETUP SENSORS
 *
 */


NewPing sonarLeft(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE); // NewPing setup of pins and maximum distance for left US Sensor.
NewPing sonarBack(TRIGGER_PIN_BACK, ECHO_PIN_BACK, MAX_DISTANCE); // NewPing setup of pins and maximum distance for back US Sensor.
NewPing sonarRight(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE); // NewPing setup of pins and maximum distance for right US Sensor.

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// And connect the 3 DC motors to ports M1, M2 & M3
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *dribbler = AFMS.getMotor(2);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3);
Adafruit_DCMotor *backMotor = AFMS.getMotor(4);


/*
*
 * SETUP GLOABAL VARIABLES TO STORE SENSOR DATA
 *
 */

int movement[11][3] = 
{{-58, 58, 0},  //forward
{-17, 64, -47},  //forward right
{33, 33, -67},  //right
{64, -17, -47},
{58, -58, 0},
{30, -64, 47},
{-33, -33, 67},
{-64, 17, 47},
{33, 33, 33},
{-33, -33, -33},
{0, 0, 0}};



unsigned int USValueLeft; //Setup Variable for the data from the left US Sensor
unsigned int USValueBack; //Setup Variable for the data from the back US Sensor
unsigned int USValueRight; //Setup Variable for the data from the right US Sensor

int IRData[2][14];

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

int motorsOn;

int chosenDirection;

int chargeTimer = 0;

int maxIR;

int haveBall = 0;


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  initializeCompass();  
  Wire.begin();
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  IRSensor[0] = 19;
  IRSensor[1] = 18;
  IRSensor[2] = 17;
  IRSensor[3] = 16;
  IRSensor[4] = 15;
  IRSensor[5] = 14;
  IRSensor[6] = 29;
  IRSensor[7] = 31;
  IRSensor[8] = 33;
  IRSensor[9] = 35;
  IRSensor[10] = 37;
  IRSensor[11] = 39;
  IRSensor[12] = 41;
  IRSensor[13] = 43;

  
  backMotor->run(RELEASE);

  rightMotor->run(RELEASE);

  leftMotor->run(RELEASE);
}

void loop() {
  //Get Sensor Data
  resetIRSensors();
  readIRSensors();
  findMaxIR();
    
  getUSData();
  getLightData();
  getCompassReading();
  checkHasBall();
  getMotorOnSwitch();
  
  makeDecision();
  
  checkChargeTimer();
  
  //Output motors
  driveRobot();

  //Output Dribbler
}
 
void makeDecision(){
  if(motorsOn == 0){
    chooseMovement(10, 0);
    backMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    leftMotor->run(RELEASE);
    dribbler->run(RELEASE);
  }
  else{
    dribbler->run(FORWARD); 
    dribbler->setSpeed(60);
    if(haveBall == 0){
      if(USValueBack < 50){
        chooseMovement(0, 150);
          Serial.print("Back US: ");
          Serial.println(USValueBack);
      }
      else{
        if(leftLightValue > 1023){
          chooseMovement(2, 150);
          Serial.print("Left Light: ");
          Serial.println(leftLightValue);
        }
        else if(rightLightValue > 1023){
          chooseMovement(6, 150);
          Serial.print("Right Light: ");
          Serial.println(rightLightValue);
        }
        else{
          switch(maxIR){
            case 0: chosenDirection = 0;
                    break;
            case 1: chosenDirection = 0;
                    break;
            case 2: chosenDirection = 1;
                    break;
            case 3: chosenDirection = 2;
                    break;
            case 4: chosenDirection = 3;
                    break;
            case 5: chosenDirection = 4;
                    break;
            case 6: chosenDirection = 5;
                    break;
            case 7: chosenDirection = 5;
                    break;
            case 8: chosenDirection = 6;
                    break;
            case 9: chosenDirection = 4;
                    break;
            case 10: chosenDirection = 5;
                    break;
            case 11: chosenDirection = 6;
                    break;
            case 12: chosenDirection = 0;
                    break;
            case 13: chosenDirection = 0;
                    break;
            case 14: chosenDirection = 10;
                    break;
          }
          chooseMovement(chosenDirection, 150);
        }
      }
    }
    else{
      Serial.println("I has ball.");
      if(USValueLeft < 61){
        chooseMovement(1, 150);
      }
      else if(USValueRight < 61){
        chooseMovement(7, 150);
      }
      else{
        if(USValueBack < 1000){
          chooseMovement(0, 150);
        }
        else{
          dribbler->setSpeed(20);
          delay(200);
          digitalWrite(kicker,HIGH);
          delay(500);
          digitalWrite(kicker,LOW);
          chargeTimer = 32;
        }
      }
    }
  }    
}
  
void checkChargeTimer(){
  if(chargeTimer>0){
    digitalWrite(charger, HIGH);
    charger--;
  }
  else{
    digitalWrite(charger, LOW);
  }
}

void resetIRSensors(){
  for(int i = 0; i<14; i++){
    IRData[0][i] = i;
    IRData[1][i] = 0;
  }
}

void readIRSensors(){
  for(int j=0; j<50; j++){
    for(int i=0;i<14;i++){
      IRData[1][i] += digitalRead(IRSensor[i]);
    }
    delay(5);
  }
}

void findMaxIR(){
 maxIR = 0;
  for(int i = 1; i<14; i++){
    if(IRData[1][i] < IRData[1][maxIR] && IRData[1][i] < 200){
      maxIR = IRData[0][i];
    }
  }
  if(IRData[1][maxIR] ==200){
    maxIR = 14;
  }
  Serial.println(maxIR);
}
  
  
void checkHasBall(){
  Serial.print("Photogate:");
  Serial.println(analogRead(photogate) );
  if(analogRead(photogate) > LIGHT_BARRIER_THRESHHOLD){
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

void getMotorOnSwitch(){
  if(analogRead(motorStopper) > MOTOR_SWITCH_THRESHHOLD){
    motorsOn = 1;
  }
  else{
    motorsOn = 0;
  }
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


void getUSData(){
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


void chooseMovement(int dir, int spe){
  rightMotorPower = movement[dir][0] * spe / 100;
  leftMotorPower = movement[dir][1] * spe / 100;
  backMotorPower = movement[dir][2] * spe / 100;
  
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

  if(backMotorPower==0){
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

