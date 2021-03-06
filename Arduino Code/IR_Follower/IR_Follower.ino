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

int chosenDirection;

int maxIR;

int movement[11][3] = 
{{-58, 58, 0},
{-17, 64, -47},
{33, 33, -67},
{64, -17, -47},
{58, -58, 0},
{30, -64, 47},
{-33, -33, 67},
{-64, 17, 47},
{33, 33, 33},
{-33, -33, -33},
{0, 0, 0}};



void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("MMMMotor party!");

  //Setup Array for IR Sensors
  IRSensor[0]  = 19;
  IRSensor[1]  = 18;
  IRSensor[2]  = 17;
  IRSensor[3]  = 16;
  IRSensor[4]  = 15;
  IRSensor[5]  = 14;
  IRSensor[6]  = 29;
  IRSensor[7]  = 31;
  IRSensor[8]  = 33;
  IRSensor[9]  = 35;
  IRSensor[10] = 37;
  IRSensor[11] = 39;
  IRSensor[12] = 41;
  IRSensor[13] = 43;
  
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
   
  // turn on motor M1  
  backMotor->run(RELEASE);

  rightMotor->run(RELEASE);

  leftMotor->run(RELEASE);
  
  for(int i = 0; i<20; i++){
    resetIRSensors();
    readIRSensors();
    findMaxIR();
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
    
    chooseMovement(chosenDirection, 170);
    
    driveRobot();
  }
  chooseMovement(10, 0);
    
  driveRobot();
}

void loop() {
  
  
  
  
  /*for(int i =0; i < 20; i++){
    resetIRSensors();
    readIRSensors();
    findMaxIR();
  }*/
  
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

