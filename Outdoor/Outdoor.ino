/*
  Arduino Wireless Communication Tutorial
      Outdoor unit - Transmitter 
      
  by Dejan Nedelkovski, www.HowToMechatronics.com

Libraries:
NRF24L01 - TMRh20/RF24, https://github.com/tmrh20/RF24/
DHT22 - DHTlib, https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib
LowPower - https://github.com/rocketscream/Low-Power 
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#include <dht.h>
#include "DHT.h"

#include <LowPower.h>

#define dataPin 8 // DHT22 data pin
#define DHTTYPE DHT22  //DHT22 type
//dht DHT; // Creates a DHT object
DHT dht(dataPin, DHTTYPE);

RF24 radio(10, 9); // CE, CSN
const byte address[6] = "00001";

char thChar[32] = "";
String thString = "";

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

}
void loop() {
 // int readData = DHT.read22(dataPin); // Reads the data from the sensor
//  int t = DHT.temperature; // Gets the values of the temperature
//  int h = DHT.humidity; // Gets the values of the humidity
  int t = dht.readHumidity(); // Gets the values of the temperature
  int h = dht.readTemperature(); // Gets the values of the humidity

  
  thString = String(t) + String(h);
  thString.toCharArray(thChar, 12);
  // Sent the data wirelessly to the indoor unit
  for (int i = 0; i <= 3; i++) {           // Send the data 3 times
    radio.write(&thChar, sizeof(thChar));
    delay(50);
  }
  // Sleep for 2 minutes, 15*8 = 120s
  for (int sleepCounter = 15; sleepCounter > 0; sleepCounter--)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
