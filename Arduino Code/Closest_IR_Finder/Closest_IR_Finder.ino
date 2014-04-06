int IRSensor[14];
int IRData[2][14];

void setup(){
  Serial.begin(9600);
  
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
  
  Serial.println("Here we go");
}

void loop(){
  resetIRSensors();
  readIRSensors();
  findMaxIR();
}

void resetIRSensors(){
  for(int i = 0; i<14; i++){
    IRData[0][i] = i;
    IRData[1][i] = 0;
  }
}

void readIRSensors(){
  for(int j=0; j<200; j++){
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
