#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>

// --- OLED SPI --- 
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 10, 9, 12, 11, 13);

// --- RTC ---
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Inicializar OLED
  u8g2.begin();
  u8g2.setContrast(255);

  // Inicializar RTC
  if (!rtc.begin()) {
    Serial.println("No se encontró RTC");
    while (1);
  }

  // Ajustar hora inicial si el RTC perdió energía
  if (rtc.lostPower()) {
    Serial.println("RTC perdió energía, ajustando hora inicial");
    // AJUSTA AQUÍ a la hora real actual: AAAA, MM, DD, HH, MM, SS
    rtc.adjust(DateTime(2026, 12, 13, 19, 21, 0));
  }

  // Inicializar TimeLib con hora del RTC
  DateTime nowRTC = rtc.now();
  setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());
}

void loop() {
  // Leer hora del RTC
  DateTime nowRTC = rtc.now();
  setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());

  // Limpiar buffer OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // Crear string solo con la hora
  char buf[9]; // HH:MM:SS
  sprintf(buf, "%02d:%02d:%02d", hour(), minute(), second());

  // Mostrar hora en OLED
  u8g2.drawStr(20, 30, buf);

  // Enviar buffer a la pantalla
  u8g2.sendBuffer();

  delay(500); // actualiza cada medio segundo
}