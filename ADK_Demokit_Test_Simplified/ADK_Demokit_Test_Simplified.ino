#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

#define  LED1         13

AndroidAccessory acc("Google, Inc.",            // Manufacturer
                     "DemoKit",                 // Model
                     "DemoKit Arduino Board",   // Description
                     "1.0",                     // Version
                     "http://www.android.com",  // URI
                     "0000000012345678");       // Serial

void setup();
void loop();

void init_led()
{
  pinMode(LED1, OUTPUT);
}

void setup()
{
  Serial.begin(115200);
  Serial.print("\r\nADK Started\r\n");

  init_led();

  // Power On Android Accessory interface and init USB controller
  acc.powerOn();
}

void loop()
{
  byte data[3];
  if (acc.isConnected()) {
    int len = acc.read(data, sizeof(data), 1);
    if (len > 0) {
      if (data[0] == 0x3) {
        if (data[1] == 0x0) {
          digitalWrite(LED1, data[2] ? HIGH : LOW);
          Serial.print("Toggle LED\r\n");
        }
      }
    }
  }
  else{
    Serial.print("Not Connected\n");
  }

  delay(10);
}
