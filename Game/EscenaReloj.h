#ifndef ESCENARELOJ_H
#define ESCENARELOJ_H

#include <RTClib.h>
#include <TimeLib.h>

class EscenaReloj {
  private: RTC_DS3231 rtc;
  public : void begin(){
    Wire.begin();
      if (!rtc.begin()) {
    Serial.println("No se encontró RTC");
     while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC perdió energía, ajustando hora inicial");
    // AJUSTA AQUÍ a la hora real actual: AAAA, MM, DD, HH, MM, SS
    rtc.adjust(DateTime(2026, 3, 23, 18, 31, 0));
  }

      DateTime nowRTC = rtc.now();
  setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());
  }
 
      void update() {
      DateTime nowRTC = rtc.now();

      setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
              nowRTC.day(), nowRTC.month(), nowRTC.year());
    }

  void draw(U8G2 &u8g2){
    
  char buf[9]; 
  sprintf(buf, "%02d:%02d:%02d", hour(), minute(), second());

  // Mostrar hora en OLED
  u8g2.drawStr(22, 41, buf);
  }

};
#endif
/*

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
}*/