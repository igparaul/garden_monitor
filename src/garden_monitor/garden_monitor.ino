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
#define DHT11Pin 19

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

  hum();
  digitalWrite(ledRed, HIGH);
}

int DHTMeasure(int type){
  int32_t return_value = 0;
  int chk = DHT.read11(DHT11Pin);
  if(chk == DHTLIB_OK){
    if(type == 0){
      Serial.println("Humidity: " + String(DHT.humidity) + "%");
      return_value = (int32_t) (DHT.humidity * 100);
    }
    else{
      Serial.println("Temperature: " + String(DHT.temperature) + "C");
      return_value = (int32_t) (DHT.temperature * 100);
    }
  }
  return return_value;
}

void hum(){
  digitalWrite(ledYellow, LOW);     // Turn off yellow LED 
  digitalWrite(ledBlue, HIGH);      // Turn on blue LED
  int32_t hum = DHTMeasure(0);
}

void temp(){
  digitalWrite(ledBlue, LOW);       // Turn off blue LED 
  digitalWrite(ledYellow, HIGH);    // Turn on yellow LED
  int temp = DHTMeasure(1);
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