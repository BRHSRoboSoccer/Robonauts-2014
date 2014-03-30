int IRSensor[14];
int IRData[2][14];

void setup(){
  Serial.begin(9600);
  
  //Setup Array for IR Sensors
  IRSensor[0]  = 14;
  IRSensor[1]  = 15;
  IRSensor[2]  = 16;
  IRSensor[3]  = 17;
  IRSensor[4]  = 18;
  IRSensor[5]  = 19;
  IRSensor[6]  = 20;
  IRSensor[7]  = 21;
  IRSensor[8]  = 5;
  IRSensor[9]  = 4;
  IRSensor[10] = 3;
  IRSensor[11] = 2;
  IRSensor[12] = 1;
  IRSensor[13] = 0;
  
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
