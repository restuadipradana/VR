#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

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

#define engOn    (0)     //perintah "engine on"
#define engStart    (1)     //perintah "engine start"
#define trnRight    (2)      //perintah "right"
#define trnLeft    (3)     //perintah "left"
#define lightOn    (4)      //perintah "light on"
#define engOff    (5)     //perintah "engine off"
#define lightOff    (6)     //perintah "light off"


//----DFp
SoftwareSerial mySoftwareSerial(5, 6); // RX, TX DFp
DFRobotDFPlayerMini myDFPlayer;

//0001 seatbelt
//0002 engine ready
//0003 machine off


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
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(20);    //Set volume value. From 0 to 30
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
  
}


//------------------------------------------------------------
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of record which is recognized. 
             buf[2]  -->  Recognizer index(position) value of the recognized record.
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/
void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}


//------------------------------------------------------------

void loop()
{
  
  
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      
      case engOn:    //ON
        digitalWrite(engine, LOW);
//        if(digitalRead(kondisi) == HIGH){
        myDFPlayer.play(1);  //putar mp3 0001
//        }
        break;
      
      case engStart:    //perintah nyalakan
        /** engine start*/
        digitalWrite(starter, LOW);
        myDFPlayer.play(2);  //putar mp3 0002
        break;
      
      case trnRight:     //perintah matikan
        /** turn right*/
        digitalWrite(right, LOW);
        delay(3000);
        digitalWrite(right, HIGH);
        break;

      case trnLeft:     //perintah matikan
        /** turn left*/
        digitalWrite(left, LOW);
        delay(3000);
        digitalWrite(left, HIGH);
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
        myDFPlayer.play(3);  //putar mp3 0003
        break;
      
      default:
        Serial.println("Record function undefined");
        break;
    }
    /** voice recognized */
    printVR(buf);
  }
}
