void initIRSensors){
  //Setup Array for IR Sensors
  IRSensor[0]  = 0;
  IRSensor[1]  = 1;
  IRSensor[2]  = 2;
  IRSensor[3]  = 3;
  IRSensor[4]  = 4;
  IRSensor[5]  = 5;
  IRSensor[6]  = 6;
  IRSensor[7]  = 7;
  IRSensor[8]  = 8;
  IRSensor[9]  = 9;
  IRSensor[10] = 10;
  IRSensor[11] = 11;
  IRSensor[12] = 12;
  IRSensor[13] = 13;
}

void readIRSensors(){
  for(int i=0;i<=13;i++){
    CballR[i]    = digitalRead(IRSensor[i]);//IRSensor Value
    CballR[i+14] = i;//IR Sensor Number
    CballR[i+28] = CballR[i];//Repeat IR Sensor Value (Will be used for reordering later on)
  }

  isort_c(CballR,14);//angepasstes Insertionsort für das Array
  max_detector = CballR[14];//die größte angestrahlte Diode wird fest übernommen

  int lichtschrankenwert = analogRead(LichtschrankeRead);//Lichtschranke auslesen

  if (lichtschrankenwert < max_lichtschranke && CballR[34] < ballcapwert && CballR[35] < ballcapwert){//Der Ball liegt in der capturingzone und der Ball liegt vor ihm
    ballcap = true;
  }  
  else if (lichtschrankenwert >= max_lichtschranke || CballR[34] >= ballcapwert || CballR[35] >= ballcapwert){//etwas ist in der lichtschranke, aber Ball liegt zu weit weg oder daneben
    ballcap = false;
  }  


  if (CballR[max_detector + shifting] > max_background){//überprüfen ob höchst angestrahlte diode den ball sieht  
    habeball = false;//kein Ball in Reichweite
  } 
  else {
    habeball = true;//Ball ist im sichtfeld
  } 

  //habeball ist eine Statusvariable in unserer groben Logik

}
