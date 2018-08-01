/*
  Arduino Wireless Communication Tutorial
        Indoor unit  - Receiver

  by Dejan Nedelkovski, www.HowToMechatronics.com
 Libraries:
 DS3231 - http://www.rinkydinkelectronics.com/library.php?id=73
 U8G2 - https://github.com/olikraus/u8g2
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//LCD 1602
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2c_addr = 0x27;
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

//RTC
DS3231  rtc(SDA, SCL);

//NRF24L01
RF24 radio(10, 9); // CE, CSN
const uint64_t pipes[9] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL,0xF0F0F0F0E7LL,0xF0F0F0F0E9LL,0xF0F0F0F0E9LL };
//const byte address[6] = "00001";

char text[32] = "";
String rtcTime, rtcDate, tempValue, humValue, presValue;
int draw_state = 0;
unsigned long previousMillis = 0;
long interval = 5000;



void setup() {
   Serial.begin(9600);
   lcd.begin(16,2);
   lcd.home();

   radio.begin();
   radio.setDataRate(RF24_250KBPS);
   radio.openReadingPipe(1, pipes[1]);
   //radio.openReadingPipe(2, pipes[2]);

   //radio.openReadingPipe(0, address);
   //radio.setPALevel(RF24_PA_MIN);
   radio.startListening();
 
  rtc.begin();
}
void loop() {
  if (radio.available()) {
    radio.read(&text, sizeof(text)); // Read incoming data
    //Message example:  temperature;humidity;air pressure -> 26.30;48.10;1021.04

    tempValue = getMsgValue(text,';',0);
    humValue = getMsgValue(text,';',1);
    presValue = getMsgValue(text,';',2);

    Serial.println("Temp: " + tempValue);
    Serial.println("Hum: " + humValue);
    Serial.println("Press: " + presValue);
  }
 
 unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
   

    switch (draw_state){
      case 0:
        DisplayDateTime();
        draw_state = 1;
        break;
      case 1: 
        DisplayTemperatureAndHumidity();
        draw_state = 2;
        break;
      case 2:
        DisplayPressure();
        draw_state = 0;
        break;
      default:
        draw_state = 0;
        break;
    }
  }
}

void DisplayDateTime()
{
    String dowa = rtc.getDOWStr();
    Serial.println(dowa);
    dowa.remove(3);
    rtcDate = dowa + " " + rtc.getDateStr();
    Serial.println("date: " + rtcDate);
    rtcTime = rtc.getTimeStr(); // DS3231 RTC time
    rtcTime.remove(5);
    Serial.println("time: "+rtcTime);

        
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(rtcDate);
    lcd.setCursor(5,1);  
    lcd.print(rtcTime);

}

void DisplayTemperatureAndHumidity()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp :");
    lcd.print(tempValue);
    lcd.print(" C");

    lcd.setCursor(0,1);
    lcd.print("Hum  :");
    lcd.print(humValue);
    lcd.print(" %");

} 

void DisplayPressure()
{
    lcd.clear();
     lcd.setCursor(2,0);
    lcd.print("Air pressure");
     lcd.setCursor(3,1);
    lcd.print(presValue);
    lcd.print(" mHg");

}

String getMsgValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
/*
void drawDate() {
  String dowa = rtc.getDOWStr();
  dowa.remove(3);
  rtcDate = dowa + " " + rtc.getDateStr();
  u8g2.setFont(u8g2_font_timB14_tr);
  u8g2.setCursor(0, 15);
  rtcTime = rtc.getTimeStr(); // DS3231 RTC time
  rtcTime.remove(5);
  u8g2.print(rtcDate);
  u8g2.setFont(u8g2_font_fub30_tf);
  u8g2.setCursor(8, 58);
  u8g2.print(rtcTime);
}

void drawInTemperature() {
  readDHT22 = DHT.read22(dataPin); // Reads the data from the sensor
  t = DHT.temperature; // Gets the values of the temperature
  inTemp = String(t) + char(176) + "C";
  u8g2.setFont(u8g2_font_helvR14_tr);
  u8g2.setCursor(24, 15);
  u8g2.print("INDOOR");
  u8g2.setFont(u8g2_font_fub30_tf);
  u8g2.setCursor(36, 58);
  u8g2.print(inTemp);
  u8g2.drawXBMP( 0, 17, Temperature_20Icon_width, Temperature_20Icon_height, Temperature_20Icon_bits);
}

void drawInHumidity() {
  h = DHT.humidity; // Gets the values of the humidity
  inHum = String(h) + "%";
  u8g2.setFont(u8g2_font_helvR14_tr);
  u8g2.setCursor(24, 15);
  u8g2.print("INDOOR");
  u8g2.setFont(u8g2_font_fub30_tf);
  u8g2.setCursor(36, 58);
  u8g2.print(inHum);
  u8g2.drawXBMP( 0, 17, Humidity_20Icon_width, Humidity_20Icon_height, Humidity_20Icon_bits);
}

void drawOutTemperature() {
  u8g2.setFont(u8g2_font_helvR14_tr);
  u8g2.setCursor(12, 15);
  u8g2.print("OUTDOOR");
  u8g2.setFont(u8g2_font_fub30_tf);
  u8g2.setCursor(36, 58);
  u8g2.print(outTemp);
  u8g2.drawXBMP( 0, 17, Temperature_20Icon_width, Temperature_20Icon_height, Temperature_20Icon_bits);
}
void drawOutHumidity() {
  u8g2.setFont(u8g2_font_helvR14_tr);
  u8g2.setCursor(12, 15);
  u8g2.print("OUTDOOR");
  u8g2.setFont(u8g2_font_fub30_tf);
  u8g2.setCursor(36, 58);
  u8g2.print(outHum);
  u8g2.drawXBMP( 0, 17, Humidity_20Icon_width, Humidity_20Icon_height, Humidity_20Icon_bits);
}
*/
