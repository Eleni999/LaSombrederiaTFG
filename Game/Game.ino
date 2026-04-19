#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>
#include <ezButton.h>
#include <Adafruit_NeoPixel.h>

#include "BibliotecaImagenes.h"
#include "EscenaReloj.h"

// Panralla
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(
  U8G2_R0, 10, 9, 12, 11, 13

);

//Interruptor

ezButton toggleSwitch(2); 
bool sistemaEncendido = true;

//Encoder 
const int pinCLK =3;
const int pinDT = 4;
const int pinSW = 5;
int estadoAnteriorCLK;
int seleccion = 0; 
int estadoActualCLK;
int estadoActualSW;

int contadorClicks = 0;
unsigned long ultimoClick = 0; 
const unsigned long tiempoDobleClick = 400;

//Reloj
unsigned long tiempoPulsado =0;
bool botonPresionado = false;
bool estadoRReloj = true; 
EscenaReloj reloj;

//INICIO
unsigned long iniciarSistema = 0;
bool arranqueCompleto = false;

//Menu
bool estadoMenu = false;
//Led
#define PIN 6
#define NUM_LEDS 7  
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
int ledcentral =NUM_LEDS/6; 
uint32_t color;

enum EstadoConejo {

    IDE,
    BANAR,
    COMER,
    DORMIR,
    TE,
    MUERTE,
    AJUSTES,
    FELIZ
 };
 enum EstadoBase{
  BASE_IDE,
  BASE_HAMBRE
 };
 struct Necesidadades {
   int hambre = 30;
   int sueno = 30;
   int sucio = 30; 
 };

//EstadosConejo
EstadoConejo estadoActual = IDE;
EstadoBase baseActual =  BASE_IDE;
unsigned long ultimoTick = 0;
Necesidadades necesidades; 
bool necesidadHambre = false;
bool necesidadSueno = false; 
bool necesidadSucio = false; 

unsigned long  tiempoEstado = 0;
unsigned long  duracionEstado = 3000;
bool estadoCurso = false; 

void sistemaInterruptor(){
  toggleSwitch.loop();
  sistemaEncendido = toggleSwitch.getState();
}
void setup() {
  u8g2.begin();
 u8g2.setBitmapMode(1);
 reloj.begin(); 

toggleSwitch.setDebounceTime(50);

 // Pines
 pinMode(pinCLK, INPUT_PULLUP);
 pinMode(pinDT, INPUT_PULLUP);
 pinMode(pinSW, INPUT_PULLUP);
 estadoAnteriorCLK = digitalRead(pinCLK);
 
  strip.begin();
  strip.show();

  iniciarSistema = millis();
  estadoRReloj = true;

};

void loop() {

 sistemaInterruptor();

  sistemaInterruptor();

  if(!sistemaEncendido){
    u8g2.clearBuffer();
    u8g2.sendBuffer();

    strip.clear();
    strip.show();
    return; 
  }
  u8g2.clearBuffer();

Menu();
Reloj();
if ( !arranqueCompleto &&  millis()-iniciarSistema >=4000 ){
  arranqueCompleto = true;
  estadoRReloj = false; 
  estadoActual = IDE;
}
if ( !estadoRReloj && !estadoMenu ){
 acturlizarNecesidades();
 //interccionEstados();
 actualizarEstadoIA();
}

if (estadoCurso){
  if( millis()- tiempoEstado >=duracionEstado){
    estadoCurso = false;
    estadoActual = IDE;
  }
}

if (estadoRReloj ){
  reloj.update();
  reloj.draw(u8g2);

}else if(estadoMenu){
dibujarIconos(u8g2, seleccion);
}
else{
  
 switch (estadoActual){
  case IDE:
  if(baseActual == BASE_HAMBRE){
     dibujarHambre(u8g2);
  } else{
   dibujoIDE(u8g2);
   color = strip.Color(242, 226, 167);
  }
   if (necesidadSucio){ 
  dibujoSucio(u8g2);
   color = strip.Color(91, 146, 235);} 
   if(necesidadSueno){
    dibujarSueno(u8g2);
   }
  break;
  case FELIZ:
  dibujarFeliz(u8g2);
  color = strip.Color(242, 226, 167);
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

  if (!sistemaEncendido) return; 

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

  if (!sistemaEncendido) return; 
 estadoActualSW = digitalRead(pinSW);

  if (estadoActualSW == LOW) {
    if (!botonPresionado) {
      tiempoPulsado = millis();
      botonPresionado = true;

      if (millis() - ultimoClick < tiempoDobleClick) { 
        contadorClicks ++; 
      } else {
        contadorClicks = 1; 
      }
      ultimoClick = millis(); 

      strip.setPixelColor(ledcentral, strip.Color(4, 23, 48));
    }
    else {
      if (!arranqueCompleto && millis() - tiempoPulsado >= 2000) {
        estadoRReloj = false; 
        arranqueCompleto = true;
        estadoActual = IDE;
        botonPresionado = false;
        strip.clear();
      }
    }
  }
  else {
    botonPresionado = false;
  }

  if (!estadoRReloj && contadorClicks == 2 && millis() - ultimoClick < tiempoDobleClick) { 
    estadoMenu = true; 
    estadoCurso = false; 
    contadorClicks = 0; 
  }
  if(!estadoRReloj && estadoMenu && contadorClicks == 1 && millis() - ultimoClick > tiempoDobleClick){
  interccionEstados(); 
  contadorClicks = 0;
 }
}
 void interccionEstados(){
  
  if (!sistemaEncendido) return; 
  estadoMenu = false;

  tiempoEstado = millis();
  estadoCurso = true;
    switch (seleccion){

    case 0:
      estadoActual = TE;
    break;

    case 1:
      estadoActual = COMER;
      necesidades.hambre +=12;
      if(necesidades.hambre > 30) necesidades.hambre = 30;
    break;

    case 2:
      estadoActual = DORMIR;
      necesidades.sueno +=15;
      if(necesidades.sueno > 30) necesidades.sueno = 30;
    break;

    case 3:
      estadoActual = BANAR;
      necesidades.sucio =30;;
      if(necesidades.sucio > 30) necesidades.sucio = 30;
    break;

    case 4:
      estadoActual = AJUSTES;
    break;
  }
  necesidades.hambre = constrain(necesidades.hambre, 0, 32);
  necesidades.sueno  = constrain(necesidades.sueno, 0, 32);
  necesidades.sucio  = constrain(necesidades.sucio, 0, 30);
 }
 void acturlizarNecesidades(){
  if (estadoCurso) return; 
  
  if ((millis()-ultimoTick) >=50000){
    ultimoTick = millis();
    necesidades.hambre--;
    necesidades.sueno --;
    necesidades.sucio --;
  }
 }
 void actualizarEstadoIA(){
   
   if(estadoCurso) return; 
   //muerte
   if( necesidades.hambre <= 0 || necesidades.sueno <=0 || necesidades.sucio <=0){

    estadoActual = MUERTE;
    return;
   }
    necesidadHambre = necesidades.hambre < 15;
    necesidadSueno = necesidades.sueno < 15;
    necesidadSucio = necesidades.sucio< 15;
    
   if(necesidadHambre ){
     baseActual =  BASE_HAMBRE;
   }else{
     baseActual =  BASE_IDE;
   }
   if(necesidades.hambre >= 30 && necesidades.sueno >= 30 && necesidades.sucio >= 30){
    estadoActual = FELIZ;
    return;
   }
    if (estadoActual != COMER && estadoActual != DORMIR && estadoActual != BANAR && estadoActual != TE && estadoActual != FELIZ && estadoActual != MUERTE){
       estadoActual = IDE;
    }
    
  }

