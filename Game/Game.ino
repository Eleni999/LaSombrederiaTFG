#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>


#include "BibliotecaImagenes.h"
#include "EscenaReloj.h"

// Panralla
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(
  U8G2_R0, 10, 9, 12, 11, 13

);

//Encoder 
const int pinCLK =3;
const int pinDT = 4;
const int pinSW = 5;
int estadoAnteriorCLK;
int seleccion = 0; 
int estadoActualCLK;
int estadoActualSW;

//Reloj
unsigned long tiempoPulsado =0;
bool botonPresionado = false;
bool estadoRReloj = false; 
EscenaReloj reloj; ;

void setup() {
  u8g2.begin();
 u8g2.setBitmapMode(1);
 reloj.begin(); 
 
 // Pines
 pinMode(pinCLK, INPUT_PULLUP);
 pinMode(pinDT, INPUT_PULLUP);
 pinMode(pinSW, INPUT_PULLUP);
 estadoAnteriorCLK = digitalRead(pinCLK);
 
};

void loop() {
u8g2.clearBuffer();

Menu();

if (estadoRReloj ){
  reloj.update();
  reloj.draw(u8g2);
}else{
  dibujarIconos(u8g2,seleccion); 
  dibujarConejo(u8g2);
}

u8g2.sendBuffer(); 
delay(50);

}
void Menu(){
  estadoActualCLK = digitalRead(pinCLK);
  estadoActualSW = digitalRead(pinSW);

  if (estadoAnteriorCLK ==HIGH && estadoActualCLK== LOW ){
    if(digitalRead(pinDT) == HIGH){
      seleccion ++;
      
    }else {
      seleccion --;
    }
    if ( seleccion > 4) seleccion  =0;
    if (seleccion < 0) seleccion = 4; 
  }
  estadoAnteriorCLK  = estadoActualCLK;

   if (estadoActualSW == LOW ){ 
    if(!botonPresionado){
      tiempoPulsado = millis();
      botonPresionado = true;
    } else {
      if (millis() - tiempoPulsado >= 3000){
        estadoRReloj = ! estadoRReloj ;
       botonPresionado = false;
      }
    }
  } else {
    botonPresionado = false;
  }
}
