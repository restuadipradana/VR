#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <SimpleTimer.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
SimpleTimer timer;
//-------VR
VR myVR(2,3);    // 2:RX 3:TX, VR

uint8_t records[7]; // save record
uint8_t buf[64];

int engine = A0;     // relay1 engine cond
int starter = A1;    // relay2 starter
int right = A2;     // relay3 right
int left = A3;    // relay4 left
int light = A4;     // relay1 light cond

//int kondisi = 8;    //kondisi mesin

#define engOn    (0)     //perintah "turn on"
#define engStart    (1)     //perintah "start"
#define trnRight    (2)      //perintah "right"
#define trnLeft    (3)     //perintah "left"
#define lightOn    (4)      //perintah "light on"
#define engOff    (5)     //perintah "engine off"
#define lightOff    (6)     //perintah "light off"


//----DFp
SoftwareSerial mySoftwareSerial(5, 6); // RX, TX DFp
DFRobotDFPlayerMini myDFPlayer;

//file mp3
//0001 seatbelt
//0002 engine ready
//0003 machine off

char auth[] = "43d6cf5c959f46858e0cf20d9836cfdb";

#define W5100_CS  10

void skiri(){
  digitalWrite(left, LOW);
 timer.setTimeout(3000, smati); // turn off after 1min
}
void skanan(){
  digitalWrite(right, LOW);
 timer.setTimeout(3000, smati); // turn off after 1min
}
void smati(){
  // do you pump off functions here
  digitalWrite(left, HIGH);
  digitalWrite(right, HIGH);
}

void rio()
{
    int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      
      case engOn:    //ON
        digitalWrite(engine, LOW);
        myDFPlayer.play(1);  //putar mp3 0001
        break;
      
      case engStart:    //perintah nyalakan
        /** engine start*/
        digitalWrite(starter, LOW);
        myDFPlayer.play(2);  //putar mp3 0002
        break;
      
      case trnRight:     //perintah matikan
        /** turn right*/
        skanan();
        break;

      case trnLeft:     //perintah matikan
        /** turn left*/
        skiri();
        break;

      case lightOn:    //perintah nyalakan
        /** turn on Kontak*/
        digitalWrite(light, LOW);
        break;

      case lightOff:    //perintah nyalakan
        /** turn off light*/
        digitalWrite(light, HIGH);
        break;

      case engOff:    //perintah nyalakan
        /** turn off engine*/
        digitalWrite(engine, HIGH);
        digitalWrite(starter, HIGH);
        digitalWrite(light, HIGH);
        myDFPlayer.play(3);  //putar mp3 0003
        break;
      
      default:
        Serial.println("Record function undefined");
        break;
    }
    /** voice recognized */

    
  }
}

//void reconnect(){
//  int mytimeout = millis() / 1000;
//  while (Blynk.connect() == false) { 
//    if((millis() / 1000) > mytimeout + 8){  // try for less than 9 seconds
//      break;
//    }
//  }
//}

void setup()
{
  pinMode(engine, OUTPUT);
  pinMode(starter, OUTPUT);
  pinMode(right, OUTPUT);
  pinMode(left, OUTPUT);
  pinMode(light, OUTPUT);

//  pinMode(kondisi, INPUT); 

  digitalWrite(engine, HIGH);   //set pembalik logika relay
  digitalWrite(starter, HIGH);  //set pembalik logika relay
  digitalWrite(right, HIGH);   //set pembalik logika relay
  digitalWrite(left, HIGH);  //set pembalik logika relay
  digitalWrite(light, HIGH);   //set pembalik logika relay
  
  mySoftwareSerial.begin(9600);   //serial ke DFp
  
  Serial.begin(115200); //serial ke komputer
  //debug DFp
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(15);    //Set volume value. From 0 to 30
  //myDFPlayer.play(3);       //putar mp3 0003


  
  myVR.begin(9600); // serial ke VR
  //debug vr
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }

  if(myVR.load((uint8_t)engOn) >= 0){
    Serial.println("on loaded");
  }
  if(myVR.load((uint8_t)engStart) >= 0){
    Serial.println("start loaded");
  }
  if(myVR.load((uint8_t)trnRight) >= 0){
    Serial.println("right loaded");
  }
  if(myVR.load((uint8_t)trnLeft) >= 0){
    Serial.println("left loaded");
  }
  if(myVR.load((uint8_t)lightOn) >= 0){
    Serial.println("lOn loaded");
  }
  if(myVR.load((uint8_t)lightOff) >= 0){
    Serial.println("lOff loaded");
  }
  if(myVR.load((uint8_t)engOff) >= 0){
    Serial.println("off loaded");
  }

  Blynk.begin(auth);
  int mytimeout = millis() / 1000;
  while (Blynk.connect() == false) { 
    if((millis() / 1000) > mytimeout + 8)
      {  // try for less than 9 seconds
        break;
       }
   } 

  timer.setInterval(10L, rio);
//  timer.setInterval(9000L, reconnect);
  
}




//------------------------------------------------------------



//------------------------------------------------------------

void loop()
{
timer.run();
  if(Blynk.connected()){
    Blynk.run();
  }
  
}
