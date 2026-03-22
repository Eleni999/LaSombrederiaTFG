#include <U8g2lib.h>

// SPI software con tus pines
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(
  U8G2_R0, 10, 9, 12, 11, 13
);

void draw_Layer_2(void) {
  u8g2.drawLine(53, 40, 53, 40);
  u8g2.drawLine(53, 40, 57, 42);
  u8g2.drawLine(57, 42, 71, 31);
  u8g2.drawLine(71, 31, 82, 16);
  u8g2.drawLine(82, 16, 68, 11);
  u8g2.drawLine(68, 11, 89, 52);
  u8g2.drawLine(89, 52, 58, 60);
  u8g2.drawLine(58, 60, 87, 30);
  u8g2.drawLine(87, 30, 39, 10);
  u8g2.drawLine(39, 10, 53, 40);
}

void setup() {
  u8g2.begin();
}

void loop() {
  u8g2.clearBuffer();      // 1. limpiar

  u8g2.setDrawColor(1);    // 2. dibujar en blanco

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  u8g2.drawTriangle(46, 21, 56, 42, 37, 42); 
  draw_Layer_2();          // 3. dibujar líneas

  u8g2.sendBuffer();       // 4. mostrar
}