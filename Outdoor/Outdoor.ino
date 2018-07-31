#include <SPI.h>
#include <Wire.h>
#include <SFE_BMP180.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"

#include <LowPower.h>

//DHT22
#define DHTPIN 8 // DHT22 data pin
#define DHTTYPE DHT22  //DHT22 type
DHT dht(DHTPIN, DHTTYPE);

//BMP180
#define ALTITUDE 3.0 // Altitude of Middelburg in meters
SFE_BMP180 pressure;

//nRF24L01
RF24 radio(10, 9); // CE, CSN
const byte address[6] = "00001";

//Globale variables
char msgCharArray[32] = "";
String msgStr = "";
float _DHT_Humidity, _DHT_Temperature;
double _temperature,_pressure;

void setup() {
   Serial.begin(9600);
   SetupNRF24L01();
   SetupBMP180();
   SetupDHT22();
}

void loop() {
    ReadBMP180();
    ReadDHT22();
  
    msgStr = "T:"+String(_DHT_Temperature) +";H:"+ String(_DHT_Humidity)+";P:"+ String(_pressure);
    msgStr.toCharArray(msgCharArray, 12);
  /// Sent the data wirelessly to the indoor unit
    for (int i = 0; i <= 3; i++) {           // Send the data 3 times
      Serial.println("Start send : "+ msgStr);
      radio.write(&msgCharArray, sizeof(msgCharArray));
      Serial.println("Sent");
      delay(50);
    }
    
    //   Sleep for 8 sec, 1*8 = 8s
    for (int sleepCounter = 1; sleepCounter > 0; sleepCounter--)
    {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
}

void SetupNRF24L01()
{
   radio.begin();
   radio.openWritingPipe(address);
   radio.setPALevel(RF24_PA_MIN);
   radio.stopListening();
}

void SetupDHT22()
{
  dht.begin();
}

void ReadDHT22()
{
  _DHT_Temperature = dht.readHumidity(); // Gets the values of the temperature
  _DHT_Humidity = dht.readTemperature(); // Gets the values of the humidity
}

void SetupBMP180()
{
  pressure.begin();
}

void ReadBMP180()
{
    char status;
    status = pressure.startTemperature();
    if (status != 0)
    {
    
      delay(status);// Wait for the measurement to complete:
      status = pressure.getTemperature(_temperature);
     if (status != 0)
      {
        status = pressure.startPressure(3);
        if (status != 0)
        {
        // Wait for the measurement to complete:
          delay(status);
          pressure.getPressure(_pressure,_temperature);
        }
      }
    }
}


