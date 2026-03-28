#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>


#include "BibliotecaImagenes.h"
#include "EscenaReloj.h"

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(
  U8G2_R0, 10, 9, 12, 11, 13
);
EscenaReloj reloj;
void setup() {
  u8g2.begin();
 u8g2.setBitmapMode(1);
  reloj.begin(); 
}

void loop() {
u8g2.clearBuffer();

reloj.update();
dibujarIconos(u8g2); 
reloj.draw(u8g2);

u8g2.sendBuffer(); 
delay(1000);

}
