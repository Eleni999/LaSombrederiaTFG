#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("No se encontró RTC");
    while (1);
  }

  // Solo ajustar la hora si el RTC perdió energía
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(2026, 12, 23, 19, 10, 0)); // hora inicial correcta
  }
}

void loop() {
  DateTime nowRTC = rtc.now();
  setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());

  Serial.print(day()); Serial.print("/");
  Serial.print(month()); Serial.print("/");
  Serial.print(year());
  Serial.print(" ");
  Serial.print(hour()); Serial.print(":");
  Serial.print(minute()); Serial.print(":");
  Serial.println(second());

  delay(1000);
}
