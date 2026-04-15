#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

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

//Led
#define PIN 6
#define NUM_LEDS 7  
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
int ledcentral =NUM_LEDS/6; 

enum EstadoConejo {

    IDE,
    SUCIO,
    HAMBRE,
    SUENO,
    BANAR,
    COMER,
    DORMIR,
    TE,
    MUERTE,
    FELIZ
 };
 struct Necesidadades {
   int hambre = 30;
   int sueno = 30;
   int sucio = 30; 
 };

//EstadosConejo
EstadoConejo estadoActual = IDE;
unsigned long ultimoTick = 0;
Necesidadades necesidades; 

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
strip.begin();
strip.show();

Menu();
Reloj();

interccionEstados();
actualizarEstadoIA();

if (estadoRReloj ){
  reloj.update();
  reloj.draw(u8g2);
}else{
  
 switch (estadoActual){
  case IDE:
  dibujoIDE(u8g2);
  break;
  case FELIZ:
  dibujarFeliz(u8g2);
  break;
  case SUCIO:
  dibujoSucio(u8g2);
  break;
  case HAMBRE:
  dibujarHambre(u8g2);
  break;
    case SUENO:
  dibujarSueno(u8g2);
  break;
    case BANAR:
  dibujarBano(u8g2);
  break;
  case COMER:
  dibujarComer(u8g2);
  break;
  case DORMIR:
  dibujarDormir(u8g2);
  break;
  case MUERTE:
  dibujarMuerte(u8g2);
  break;
  case TE:
  dibujarTe(u8g2);
  break;

 }
}

u8g2.sendBuffer(); 
delay(50);

}
void Menu(){
  estadoActualCLK = digitalRead(pinCLK);
  

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
 

}
void Reloj(){
  estadoActualSW = digitalRead(pinSW);

  
   if (estadoActualSW == LOW ){ 
    if(!botonPresionado){
      tiempoPulsado = millis();
      botonPresionado = true;

     strip.setPixelColor(ledcentral,strip.Color(4, 23, 48));

    } else {
      if (millis() - tiempoPulsado >= 3000){
        estadoRReloj = ! estadoRReloj ;
       botonPresionado = false;
       
        strip.clear();
      }
    }
  } else {
    botonPresionado = false;
  }
}
 void acturlizarnecesidades(){
  if ((millis()-ultimoTick) >=60000){
    ultimoTick = millis();
    necesidades.hambre--;
    necesidades.sueno --;
    necesidades.sucio --;
  }
 }
 void actualizarEstadoIA(){
   //muerte
   if( necesidades.hambre <= 0 || necesidades.sueno <=0 || necesidades.sucio <=0){

    estadoActual = MUERTE;
   }
   if (necesidades.hambre < 15){
    estadoActual =  HAMBRE;
   }
   if (necesidades.sueno < 15){
    estadoActual = SUENO;
   }
   if (necesidades.sucio< 15){
    estadoActual = SUCIO;
   }
   if(necesidades.hambre >= 25 || necesidades.sueno >= 25 || necesidades.sucio >= 25){
    estadoActual = FELIZ;
   }
   else{
    estadoActual = IDE;
  }

 }
 void interccionEstados(){
  switch (seleccion) {
    case 0: 
    estadoActual =COMER;
    necesidades.hambre +=10;
    if(necesidades.hambre> 30)necesidades.hambre =30;
    break;
    case 1: 
    estadoActual =DORMIR;
    if(necesidades.sueno> 30)necesidades.sueno =30;
    break;
    case 2:
    estadoActual  = BANAR;
    if(necesidades.sueno> 30)necesidades.sueno =30;
    break;
    case 3:
    estadoActual  = TE;
    break;

  }
   estadoActual = IDE;
 }


