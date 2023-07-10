/**********************************************************************
  Filename    : Control LED through Infrared Remote
  Description : Remote control the LED with the infrared remote control.
  Auther      : www.freenove.com
  Modification: 2021/10/13
**********************************************************************/
#include "IR.h"
#include <dht.h>

dht DHT;
#define ledYellow 12
#define ledBlue 13
#define ledGreen 14
#define ledRed 15
#define irPin 16
#define waterPump 17
#define buzzerPin 18
#define latchPin 19
#define clockPin 20
#define dataPin 21
#define DHT11Pin 22

//     A
//    ---
// F |   | B
//   | G |
//    ---
// E |   | C
//   |   |
//    ---
//     D


// A, B, C, D, E, F, G, .
int pinDisplay[] = {0,1,2,3,4,5,6,7};
int displaySelect[] = {8,9,10,11};

int c = 0;

void setup() {
  Serial.begin(115200);
  IR_Init(irPin);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(waterPump, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  for(int i = 0; i < 8; i++){
    pinMode(pinDisplay[i], OUTPUT); 
  }
  for(int i = 0; i < 4; i++){
    pinMode(displaySelect[i], OUTPUT); 
  }
  init();
}

float DHTMeasure(int type){
  int chk = DHT.read11(DHT11Pin);
  if(chk == DHTLIB_OK){
    if(type == 0){
      Serial.println("humidity: " + String(DHT.humidity) + "%, temperature: " + String(DHT.temperature) + "C");
      return DHT.humidity;
    }
    else{
      return DHT.temperature;
    }
  }
  return -1.0;
}

void init(){
  hum();
  digitalWrite(ledRed, HIGH);
  numDisplay(1,0);
  delay(250);
  numDisplay(2,1);
  delay(250);
  numDisplay(6,2);
  delay(250);
  numDisplay(8,3);
  delay(250);
}

void hum(){
  digitalWrite(ledYellow, LOW);     // Turn off yellow LED 
  digitalWrite(ledBlue, HIGH);      // Turn on blue LED
  float hum = DHTMeasure(0);
}

void temp(){
  digitalWrite(ledBlue, LOW);       // Turn off blue LED 
  digitalWrite(ledYellow, HIGH);    // Turn on yellow LED
  float temp = DHTMeasure(1);
}

void loop() {
  if(flagCode){
    int irValue = IR_Decode(flagCode);
    Serial.println(irValue, HEX);
    handleControl(irValue);
    IR_Release();
  }
}

void handleControl(unsigned long value) {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
  // Handle the commands
  switch (value) {
    case 0xFFA25D:                       // Receive POWER ON/OFF button
      if (c == 0){                       
        digitalWrite(ledGreen, HIGH);    // Turn ON green LED
        digitalWrite(ledRed, LOW);       // Turn OFF red LED
        c = 1;
        break;
      }
      else{
        digitalWrite(ledGreen, LOW);     // Turn OFF green LED
        digitalWrite(ledRed, HIGH);      // Turn ON red LED
        c = 0;
        break;
      }
    case 0xFF30CF:                      // Receive the number '1' HUM Mode
      hum();
      break;
    case 0xFF18E7:                      // Receive the number '2' TEMP Mode
      temp();
      break;
  }
}

void numDisplay(int num, int DS){
  for(int i = 0; i < 4; i++)
  {
    digitalWrite(displaySelect[i], LOW);
  }  
  digitalWrite(displaySelect[DS], HIGH);

  switch(num){
    case 0:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], LOW);
      digitalWrite(pinDisplay[4], LOW);
      digitalWrite(pinDisplay[5], LOW);
      digitalWrite(pinDisplay[6], HIGH);
      digitalWrite(pinDisplay[7], HIGH);
    case 1:
      digitalWrite(pinDisplay[0], HIGH);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], HIGH);
      digitalWrite(pinDisplay[4], HIGH);
      digitalWrite(pinDisplay[5], HIGH);
      digitalWrite(pinDisplay[6], HIGH);
      digitalWrite(pinDisplay[7], HIGH);
    case 2:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], HIGH);
      digitalWrite(pinDisplay[3], LOW);
      digitalWrite(pinDisplay[4], LOW);
      digitalWrite(pinDisplay[5], HIGH);
      digitalWrite(pinDisplay[6], LOW);
      digitalWrite(pinDisplay[7], HIGH);
    case 3:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], LOW);
      digitalWrite(pinDisplay[4], HIGH);
      digitalWrite(pinDisplay[5], HIGH);
      digitalWrite(pinDisplay[6], LOW);
      digitalWrite(pinDisplay[7], HIGH);
    case 4:
      digitalWrite(pinDisplay[0], HIGH);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], HIGH);
      digitalWrite(pinDisplay[4], HIGH);
      digitalWrite(pinDisplay[5], LOW);
      digitalWrite(pinDisplay[6], LOW);
      digitalWrite(pinDisplay[7], HIGH);
    case 5:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], HIGH);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], LOW);
      digitalWrite(pinDisplay[4], HIGH);
      digitalWrite(pinDisplay[5], LOW);
      digitalWrite(pinDisplay[6], LOW);
      digitalWrite(pinDisplay[7], HIGH);
    case 6:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], HIGH);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], LOW);
      digitalWrite(pinDisplay[4], LOW);
      digitalWrite(pinDisplay[5], LOW);
      digitalWrite(pinDisplay[6], LOW);
      digitalWrite(pinDisplay[7], HIGH);
    case 7:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], HIGH);
      digitalWrite(pinDisplay[4], HIGH);
      digitalWrite(pinDisplay[5], HIGH);
      digitalWrite(pinDisplay[6], HIGH);
      digitalWrite(pinDisplay[7], HIGH);
    case 8:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], LOW);
      digitalWrite(pinDisplay[4], LOW);
      digitalWrite(pinDisplay[5], LOW);
      digitalWrite(pinDisplay[6], LOW);
      digitalWrite(pinDisplay[7], HIGH);
    case 9:
      digitalWrite(pinDisplay[0], LOW);
      digitalWrite(pinDisplay[1], LOW);
      digitalWrite(pinDisplay[2], LOW);
      digitalWrite(pinDisplay[3], LOW);
      digitalWrite(pinDisplay[4], HIGH);
      digitalWrite(pinDisplay[5], LOW);
      digitalWrite(pinDisplay[6], LOW);
      digitalWrite(pinDisplay[7], HIGH);    
  }

  if(DS == 1)
  {
    digitalWrite(pinDisplay[7], LOW);
  }
}
