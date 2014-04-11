int IRSensor[14];
int IRData[2][14];

void setup(){
  Serial.begin(9600);
  
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
