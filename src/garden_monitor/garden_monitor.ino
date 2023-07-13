#include "IR.h"
#include <dht.h>

dht DHT;

#define waterPump 8
#define irPin 9
#define buzzerPin 10
#define DHT11Pin 11
#define ledYellow 12
#define ledBlue 13
#define ledGreen 14
#define ledRed 15

int comPin[] = {19, 18, 17, 16};// Common pin (anode) of 4 digit 7-segment display

int dataPin = 20;
int latchPin = 21;
int clockPin = 22;

// Define the encoding of characters 0-F of the common-anode 7-Segment Display
byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};

int c = 0;
int digits[4];

void setup() {
  Serial.begin(115200);
  IR_Init(irPin);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(waterPump, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(comPin[i], OUTPUT);
  }

  hum();
  digitalWrite(ledRed, HIGH);
}

void loop() {
  if(flagCode){
    int irValue = IR_Decode(flagCode);
    //Serial.println(irValue, HEX);
    handleControl(irValue);
    IR_Release();
  }

  for (int i = 0; i < 4; i++) {
    // Select a single 7-segment display
    electDigitalDisplay (i);
    // Send data to 74HC595
    writeData(num[digits[i]], i);
    delay(5);
    // Clear the display content
    writeData(0xff, i);
  }
}

void DHTMeasure(int type){
  int32_t return_value = 0;
  int chk = DHT.read11(DHT11Pin);
  int i = 3;

  if(chk == DHTLIB_OK){
    if(type == 0){
      Serial.println("Humidity: " + String(DHT.humidity) + "%");
      return_value = (int) (DHT.humidity * 100);
    }
    else{
      Serial.println("Temperature: " + String(DHT.temperature) + "C");
      return_value = (int) (DHT.temperature * 100);
    }
  }

  Serial.println(return_value);

  while(return_value > 0){
    digits[i] = return_value%10;
    return_value = return_value/10;
    i--;
  }

  Serial.println(String(digits[0]) + " " + String(digits[1]) + " " + String(digits[2]) + " " + String(digits[3]));
}

void hum(){
  digitalWrite(ledYellow, LOW);     // Turn off yellow LED 
  digitalWrite(ledBlue, HIGH);      // Turn on blue LED
  DHTMeasure(0);
}

void temp(){
  digitalWrite(ledBlue, LOW);       // Turn off blue LED 
  digitalWrite(ledYellow, HIGH);    // Turn on yellow LED
  DHTMeasure(1);
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

void electDigitalDisplay(byte com) {
  // Close all single 7-segment display
  for (int i = 0; i < 4; i++) {
    digitalWrite(comPin[i], LOW);
  }
  // Open the selected single 7-segment display
  digitalWrite(comPin[com], HIGH);
}

void writeData(int value, int i) {
  // Make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  if (i == 1)
  {
     shiftOut(dataPin,clockPin, LSBFIRST, value & 0x7f);
  }
  else
  {
    shiftOut(dataPin, clockPin, LSBFIRST, value);  // Make latchPin output high level
  }
  // Make latchPin output high level, then 74HC595 will update data to parallel output
  digitalWrite(latchPin, HIGH);
}