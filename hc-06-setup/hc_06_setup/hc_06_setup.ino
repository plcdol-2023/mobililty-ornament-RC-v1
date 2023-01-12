#include <SoftwareSerial.h>
 
// 세팅참조 https://scribblinganything.tistory.com/146
// https://juahnpop.tistory.com/163
                               
SoftwareSerial HC06(2,3); //RX,TX // RX는 HC06의 TX와 연결시킨다.
 
void setup(){
    Serial.begin(9600);
    HC06.begin(9600);
}
 
void loop(){
    if(HC06.available()){
        Serial.write(HC06.read());
    }
    if(Serial.available()){
        HC06.write(Serial.read());
    }
} 
