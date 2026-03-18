#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

// --- OLED SPI --- 
// SW SPI: CS=12, DC=11, RESET=13, CLK=10, DIN=9
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 10, 9, 12, 11, 13);

void setup() {
  u8g2.begin();
  u8g2.setContrast(255);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr); // tipo de letra
  u8g2.drawStr(20, 30, " Hola Mundo ! "); // texto
  u8g2.sendBuffer();
}
