#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 10, 9, 12, 11, 13);

const int buzzerPin = 8; // Pin donde está conectado el buzzer

// Notas en Hz
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;

int salto = 0;

void setup() {
  u8g2.begin();
  u8g2.setContrast(255);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // --- ANIMACIÓN DEL CONEJO ---
  u8g2.clearBuffer();

 

  // DIBUJO DEL CONEJO (Cuerpo)
  // Orejas
  u8g2.drawEllipse(55, 30 + salto, 4, 12, U8G2_DRAW_ALL); 
  u8g2.drawEllipse(73, 30 + salto, 4, 12, U8G2_DRAW_ALL);
  
  // Cabeza (Blanca/Encendida)
  u8g2.drawDisc(64, 40 + salto, 12, U8G2_DRAW_ALL); 
  u8g2.setDrawColor(0); // Ojos negros
  u8g2.drawPixel(60, 38 + salto);
  u8g2.drawPixel(68, 38 + salto);
  
  // Traje
  u8g2.setDrawColor(1);
  for(int i = 0; i < 15; i += 2) {
    u8g2.drawLine(54 + i, 52 + salto, 54 + i, 64 + salto); // Traje rayado
  }
  u8g2.drawFrame(54, 52 + salto, 20, 12); // Contorno del traje

  u8g2.sendBuffer();
  
  // --- ANIMACIÓN DE SALTO ---
  salto = (salto == 0) ? -2 : 0; 

  // --- REPRODUCCIÓN DE LA MELODÍA ---
  tone(buzzerPin, C, 150); 
  delay(200);
  tone(buzzerPin, D, 150);
  delay(200);
  tone(buzzerPin, E, 150);
  delay(200);
  tone(buzzerPin, F, 150);
  delay(200);
  tone(buzzerPin, G, 150);
  delay(200);
  tone(buzzerPin, A, 150);
  delay(200);
  tone(buzzerPin, B, 150);
  delay(200);
  tone(buzzerPin, C, 300);
  delay(350);
}
