#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <RTClib.h>
#include <TimeLib.h>

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 10, 9, 12, 11, 13);

RTC_DS3231 rtc;

void setup() {

    Serial.begin(9600);
  Wire.begin();
  
  u8g2.begin();
  u8g2.setContrast(255);

  if (!rtc.begin()) {
    Serial.println("No se encontró RTC");
     while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC perdió energía, ajustando hora inicial");
    // AJUSTA AQUÍ a la hora real actual: AAAA, MM, DD, HH, MM, SS
    rtc.adjust(DateTime(2026, 3, 23, 18, 31, 0));
  }


  //inicaliza
    DateTime nowRTC = rtc.now();
  setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());


}

void loop() {
  //Lee
  DateTime nowRTC = rtc.now();
   setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());
  u8g2.clearBuffer();
  //ajuesta la fuente
  u8g2.setFont(u8g2_font_timR18_tr);


  char buf[9]; +
  sprintf(buf, "%02d:%02d:%02d", hour(), minute(), second());

  // Mostrar hora en OLED
  u8g2.drawStr(22, 41, buf);

  // Enviar buffer a la pantalla
  u8g2.sendBuffer();

  delay(500);
}