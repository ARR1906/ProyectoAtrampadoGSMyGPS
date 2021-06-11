#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
void setup() {
  // put your setup code here, to run once:
   for(int i=0;i<1024;i++){
    EEPROM.write(i,'-');}
}

void loop() {
  // put your main code here, to run repeatedly:

}
