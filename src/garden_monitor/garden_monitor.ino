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
int type = 0;
int digits[4];
int last_hum = 0;
int last_temp = 0;

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
  int new_temp = 0;
  int new_hum = 0;
  if(flagCode){
    int irValue = IR_Decode(flagCode);
    //Serial.println(irValue, HEX);
    handleControl(irValue);
    IR_Release();
  }

  if(type == 0)
  {
    new_hum = DHTMeasure(type, true);
    if (new_hum != last_hum)
    {
      last_hum = new_hum;
      parse_digits(last_hum);
    }
  }
  else
  {
    new_temp = DHTMeasure(type, true);
    if (new_temp != last_temp)
    {
      last_temp = new_temp;
      parse_digits(last_temp);
    }
  }

  for (int i = 0; i < 4; i++) {
    // Select a single 7-segment display
    electDigitalDisplay (i);
    // Send data to 74HC595
    writeData(num[digits[i]], i);
    delay(1);
    // Clear the display content
    writeData(0xff, i);
  }
}

int DHTMeasure(int type, bool check){
  int measure = 0;
  int aux = 0;
  int chk = DHT.read11(DHT11Pin);

  if(chk == DHTLIB_OK){
    if(type == 0){
      Serial.println("Humidity: " + String(DHT.humidity) + "%");
      measure = (int) (DHT.humidity * 100);
    }
    else{
      Serial.println("Temperature: " + String(DHT.temperature) + "C");
      measure = (int) (DHT.temperature * 100);
    }
  }

  aux = measure;

  if(check == true){
    return aux;
  }
  else
  {
    parse_digits(measure);
    return 0;
  }
}

void parse_digits(int num)
{
  int i = 3;
  while(num > 0){
    digits[i] = num%10;
    num = num/10;
    i--;
  }
}

void hum(){
  digitalWrite(ledYellow, LOW);     // Turn off yellow LED 
  digitalWrite(ledBlue, HIGH);      // Turn on blue LED
  type = 0;
  DHTMeasure(type, false);
}

void temp(){
  digitalWrite(ledBlue, LOW);       // Turn off blue LED 
  digitalWrite(ledYellow, HIGH);    // Turn on yellow LED
  type = 1;
  DHTMeasure(type, false);
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