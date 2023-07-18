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

int comPin[] = {19, 18, 17, 16}; // Common pin (anode) of 4 digit 7-segment display, one per 7-segment display

#define dataPin = 20; 
#define latchPin = 21;
#define clockPin = 22;

byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90}; //Define encoding of characters 0-9

// Declare global variables
int counter = 0;    // Counter if the water pump is ON or not
int type = 0;       // Variable for the type of measure of the DHT11 sensor
int digits[4];      // Array of the 4 digits of the DHT11 measure
int last_hum = 0;   // Last humidity measure
int last_temp = 0;  // Last temperature measure

void setup() {
  // Establishing OUTPUT pins
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

  // Init values
  hum();
  digitalWrite(ledRed, HIGH);
}

void loop() {
  int new_temp = 0; // New temperature measure
  int new_hum = 0;  // New humidity measure
  
  // If there is pressed any button of the remote control, we convert it as HEX and we send it to the handleControl. After that we let the IR receiver listen again.
  if(flagCode){ 
    int irValue = IR_Decode(flagCode); 
    handleControl(irValue);
    IR_Release();
  }
  
  if(type == 0) // If the program mode is humidity
  {
    // We check new humidity and if is different than the last_hum, we overwrite the last_hum variable with the new_hum value, and we parse the number
    new_hum = DHTMeasure(type, true);
    if (new_hum != last_hum)
    {
      last_hum = new_hum;
      parse_digits(last_hum);
    }
  }
  else // If the program mode is temperature
  {
    // We check new temperature and if it has varied 0.5 C, we overwrite the last_temp variable with the new_temp value, and we parse the number
    new_temp = DHTMeasure(type, true);
    if ((new_temp % 50) == 0 && new_temp != last_temp)
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
  int chk = DHT.read11(DHT11Pin);

  if(chk == DHTLIB_OK){
    if(type == 0){
      measure = (int) (DHT.humidity * 100); // We multiply the value per 100 and we cast it as integer
    }
    else{
      measure = (int) (DHT.temperature * 100); // We multiply the value per 100 and we cast it as integer
    }
  }

  if(check == true){ // If the check flag is active we return the value
     return measure;
  }
  else // If is not we parse the number
  {
    parse_digits(measure);
    return 0;
  }
}

void parse_digits(int num) // We set 4 sized array every digit of the measure value
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
  type = 0;                         // Set type -> humidity
  DHTMeasure(type, false);          // Measure of humidity
}

void temp(){
  digitalWrite(ledBlue, LOW);       // Turn off blue LED 
  digitalWrite(ledYellow, HIGH);    // Turn on yellow LED
  type = 1;                         // Set type -> temperature
  DHTMeasure(type, false);          // Measure of temperature
}

void handleControl(unsigned long value) {
  // Handle the commands
  switch (value) {
    case 0xFFA25D:                       // Receive POWER ON/OFF button
      // We make a feedback sound at pressing the remote control button
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      if (counter == 0){                       
        digitalWrite(ledGreen, HIGH);    // Turn ON green LED
        digitalWrite(ledRed, LOW);       // Turn OFF red LED
        digitalWrite(waterPump, HIGH);   // Turn ON the water pump
        counter = 1;                     // Establishing the waterPump as ON
        break;
      }
      else{
        digitalWrite(ledGreen, LOW);     // Turn OFF green LED
        digitalWrite(ledRed, HIGH);      // Turn ON red LED
        digitalWrite(waterPump, LOW);    // Turn OFF the water pump
        counter = 0;                     // Establishing the water pump as OFF
        break;
      }
    case 0xFF30CF:                      // Receive the number '1' HUM Mode
      // We make a feedback sound at pressing the remote control button
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      hum();                            // Calling the humidity process
      break;
    case 0xFF18E7:                      // Receive the number '2' TEMP Mode
      // We make a feedback sound at pressing the remote control button
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      temp();                           // Calling the temperature process
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
  if (i == 1) // If the 7-segment display is the second one
  {
     shiftOut(dataPin,clockPin, LSBFIRST, value & 0x7f); // We send it but activate the dot display also
  }
  else
  {
    shiftOut(dataPin, clockPin, LSBFIRST, value); 
  }
  // Make latchPin output high level, then 74HC595 will update data to parallel output
  digitalWrite(latchPin, HIGH);
}
