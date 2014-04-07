#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h> 

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *backMotor = AFMS.getMotor(4);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3);

int IRSensor[14];
int IRData[2][14];

int leftMotorPower;
int rightMotorPower;
int backMotorPower;

int movement[10][4] = 
{{-58, 58, 0},
{-17, 64, -47},
{33, 33, -67},
{64, -17, -47},
{58, -58, 0},
{14, -64, 47}
{-33, -33, 67}
{-64, 17, 47}
{33, 33, 33},
{-33, -33, -33}};



void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("MMMMotor party!");

  //Setup Array for IR Sensors
  IRSensor[0]  = 7;
  IRSensor[1]  = 6;
  IRSensor[2]  = 5;
  IRSensor[3]  = 4;
  IRSensor[4]  = 3;
  IRSensor[5]  = 2;
  IRSensor[6]  = 1;
  IRSensor[7]  = 0;
  IRSensor[8]  = 8;
  IRSensor[9]  = 9;
  IRSensor[10] = 10;
  IRSensor[11] = 11;
  IRSensor[12] = 12;
  IRSensor[13] = 13;

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
   
  // turn on motor M1
  backMotor->setSpeed(75);
  backMotor->run(RELEASE);

  rightMotor->setSpeed(25);
  rightMotor->run(RELEASE);

  leftMotor->setSpeed(25);
  leftMotor->run(RELEASE);
  
  backMotor->run(FORWARD);
  leftMotor->run(BACKWARD);
  rightMotor->run(BACKWARD);
    
    delay(5000);
    
  backMotor->run(RELEASE);

  rightMotor->run(RELEASE);

  leftMotor->run(RELEASE);
}

void loop() {
  
  for(int i =0; i < 20; i++){
    resetIRSensors();
    readIRSensors();
    findMaxIR();
  }
  
}

void chooseMovement(int dir, int spe){
  rightMotorPower = movement[dir][0] * spe / 100;
  leftMotorPower = movement[dir][1] * spe / 100;
  backMotorPower = movement[dir][2] * spe / 100;
  
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
  int maxIR = 0;
  for(int i = 1; i<14; i++){
    if(IRData[1][i] < IRData[1][maxIR]){
      maxIR = IRData[0][i];
    }
  }
  Serial.println(maxIR);
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

