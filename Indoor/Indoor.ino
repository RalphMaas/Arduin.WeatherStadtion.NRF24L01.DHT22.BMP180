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
const byte address[6] = "00001";

char text[32] = "";
String rtcTime, rtcDate;
int draw_state = 0;
unsigned long previousMillis = 0;
long interval = 5000;



void setup() {
   Serial.begin(9600);
   lcd.begin(16,2);
   lcd.home();

   radio.begin();
   radio.openReadingPipe(0, address);
   radio.setPALevel(RF24_PA_MIN);
   radio.startListening();
 
  rtc.begin();
}
void loop() {
  if (radio.available()) {
    radio.read(&text, sizeof(text)); // Read incoming data
    Serial.println(text);
  //  outTemp = String(text[0]) + String(text[1]) + char(176) + "C"; // Outdoor Temperature
  //  outHum = String(text[2]) + String(text[3]) + "%"; // Outdoor Humidity
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
    lcd.print("Temp & Humi");

} 

void DisplayPressure()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pressure");
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
