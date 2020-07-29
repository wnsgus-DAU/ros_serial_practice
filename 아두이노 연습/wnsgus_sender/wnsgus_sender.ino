#include <SoftwareSerial.h>                                    
SoftwareSerial send(3,4);  
char c;
void setup(){
  Serial.begin(9600); 
  send.begin(9600); 
  Serial.print("start");
}

void loop(){
  if(Serial.available())
    send.write(c);
  
}
