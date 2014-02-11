#include <Usb.h>
#include <AndroidAccessory.h>  

#define ARRAY_SIZE 12  

AndroidAccessory acc("BRHS RoboSoccer", "ADK Tester", "Testing the ADK","1.0", "URI", "Serial");  

char hello[ARRAY_SIZE] = {'h','e','l','l','o',' ', 'w','o','r','l','d','!'};  

void setup(){
  Serial.begin(115200);
  acc.powerOn();
} 

void loop(){
  if (acc.isConnected()) {
    for(int x = 0; x < ARRAY_SIZE; x++) {
      Serial.print(hello[x]);
      delay(250);
    }
    Serial.println();
    delay(250);
  }
} 
