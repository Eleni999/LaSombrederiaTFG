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

// Pantalla
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 10, 9, 12, 11, 13);

// Interruptor
ezButton toggleSwitch(2); 
bool sistemaEncendido = true;

// Encoder 
const int pinCLK = 3;
const int pinDT = 4;
const int pinSW = 5;
int estadoAnteriorCLK;
int seleccion = 0; 
int estadoActualCLK;
int estadoActualSW;

int contadorClicks = 0;
unsigned long ultimoClick = 0; 
const unsigned long tiempoDobleClick = 400;

unsigned long ultimoCmabioEncoder = 0;
const unsigned long debouceEncoder = 2;

// Reloj
unsigned long tiempoPulsado = 0;
bool botonPresionado = false;
bool estadoRReloj = true; 
EscenaReloj reloj;

// INICIO
unsigned long iniciarSistema = 0;
bool arranqueCompleto = false;

// Menu
bool estadoMenu = false;

// Led
#define PIN 6
#define NUM_LEDS 7  
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
int ledcentral = NUM_LEDS/6; 
uint32_t color;

// Ajustes UI
bool enAjustes = false;
int velocidadAnimacion = 2; 
int seleccionAjustes = 0; 
int adjHora = 12, adjMin = 0, adjSeg = 0;

// Té
unsigned long duracionTe = 9000;
unsigned long inicioTe = 0;
int indiceFraseAleatoria= 0;
int scrollX = 128;

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

enum EstadoBase {
    BASE_IDE,
    BASE_HAMBRE
};

struct Necesidadades {
    int hambre = 30;
    int sueno = 30;
    int sucio = 30; 
};

EstadoConejo estadoActual = IDE;
EstadoBase baseActual = BASE_IDE;
unsigned long ultimoTick = 0;
Necesidadades necesidades; 
bool necesidadHambre = false;
bool necesidadSueno = false; 
bool necesidadSucio = false; 

unsigned long tiempoEstado = 0;
unsigned long duracionEstado = 3000;
bool estadoCurso = false; 

void manejarScrollTe(U8G2 &u8g2, int fraseIdx) {
    const char* frase = obtenerFrase(fraseIdx); 
    u8g2.setFont(u8g2_font_5x7_tr);
    int anchoTexto = u8g2.getStrWidth(frase);
    
    unsigned long transcurrido = millis() - tiempoEstado;

    float progreso = (float)transcurrido / (float)duracionEstado;
    scrollX = 128 - (progreso * (128 + anchoTexto));
    
    u8g2.drawStr(scrollX, 63, frase);
}
// Prototipos de funciones nuevas para UI
void dibujoAjustesUI(U8G2 &u8g2);
void manejarEntradaAjustes();

void sistemaInterruptor() {
    toggleSwitch.loop();
    sistemaEncendido = toggleSwitch.getState();
}

void setup() {
    u8g2.begin();
    u8g2.setBitmapMode(1);
    reloj.begin(); 

    toggleSwitch.setDebounceTime(50);

    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);
    pinMode(pinSW, INPUT_PULLUP);
    estadoAnteriorCLK = digitalRead(pinCLK);
    
    strip.begin();
    strip.show();

    iniciarSistema = millis();
    estadoRReloj = true;
}

void loop() {
    sistemaInterruptor();

    if (!sistemaEncendido) {
        u8g2.clearBuffer();
        u8g2.sendBuffer();
        strip.clear();
        strip.show();
        return; 
    }

    u8g2.clearBuffer();
   
    
    Menu();
    Reloj();

    if (!arranqueCompleto && millis() - iniciarSistema >= 4000) {
        arranqueCompleto = true;
        estadoRReloj = false; 
        estadoActual = IDE;
    }

    if (!estadoRReloj && !estadoMenu && !enAjustes) {
        acturlizarNecesidades();
        actualizarEstadoIA();
    }

    if (estadoCurso) {
        if (millis() - tiempoEstado >= duracionEstado) {
            estadoCurso = false;
            estadoActual = IDE;
        }
    }

    if (estadoRReloj) {
        reloj.update();
        reloj.draw(u8g2);
    } else if (estadoMenu) {
        dibujarIconos(u8g2, seleccion);
    } else {
        switch (estadoActual) {
            case IDE:
                if (baseActual == BASE_HAMBRE) {
                    dibujarHambre(u8g2);
                } else {
                    dibujoIDE(u8g2);
                    color = strip.Color(242, 226, 167);
                }
                if (necesidadSucio) { 
                    dibujoSucio(u8g2);
                    color = strip.Color(91, 146, 235);
                } 
                if (necesidadSueno) {
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
                manejarScrollTe(u8g2, indiceFraseAleatoria);
                dibujarTe(u8g2);
                break;
            case AJUSTES:
               
                 dibujoAjustesUI(u8g2, seleccionAjustes, adjHora, adjMin, adjSeg);
               
                break;
        }
    }

    u8g2.sendBuffer();
  delay(10);
}

void manejarEntradaAjustes() {
    switch (seleccionAjustes) {
        case 0: velocidadAnimacion = 1; break; // Lento
        case 1: velocidadAnimacion = 5; break; // Medio
        case 2: velocidadAnimacion = 10; break; // Rapido
        case 3: adjHora = (adjHora + 1) % 24; break;
        case 4: adjMin = (adjMin + 1) % 60; break;
        case 5: adjSeg = (adjSeg + 1) % 60; break;
        case 6: // Aceptar
     
            reloj.setTime(adjHora, adjMin, adjSeg);
            enAjustes = false;
            estadoActual = IDE;
            break;
    }
}

void Menu() {
    if (!sistemaEncendido) return; 
    estadoActualCLK = digitalRead(pinCLK);
    if (estadoActualCLK != estadoAnteriorCLK) {
        if (millis() - ultimoCmabioEncoder >= debouceEncoder) {
            ultimoCmabioEncoder = millis();
            if (estadoActualCLK == HIGH) {
                if (enAjustes) {
                    if (digitalRead(pinDT) != estadoActualCLK) seleccionAjustes++;
                    else seleccionAjustes--;
                    seleccionAjustes = constrain(seleccionAjustes, 0, 6);
                }
                else if (estadoMenu) {
                    if (digitalRead(pinDT) != estadoActualCLK) seleccion++;
                    else seleccion--;
                    if (seleccion > 4) seleccion = 0;
                    if (seleccion < 0) seleccion = 4;
                }
            }
        }
    }
    estadoAnteriorCLK = estadoActualCLK;
}

void Reloj() {
if (!sistemaEncendido) return; 
    estadoActualSW = digitalRead(pinSW);

    if (estadoActualSW == LOW) {
        if (!botonPresionado) {
            botonPresionado = true;
            tiempoPulsado = millis();
            if (millis() - ultimoClick < tiempoDobleClick) contadorClicks++;
            else contadorClicks = 1;
            ultimoClick = millis(); 
        } else {
            if (!arranqueCompleto && (millis() - tiempoPulsado >= 2000)) {
                estadoRReloj = false; 
                arranqueCompleto = true;
                botonPresionado = false;
            }
        }
    } 
    else { 
        if (botonPresionado) {
            if (enAjustes) manejarEntradaAjustes();
            botonPresionado = false;
        }
    }

    unsigned long tiempoTranscurrido = millis() - ultimoClick;
    if (contadorClicks == 1 && tiempoTranscurrido > tiempoDobleClick) {
        if (estadoMenu && !enAjustes) interccionEstados(); 
        else if (estadoRReloj) { /* Salir de protector de pantalla con un click */
             estadoRReloj = false;
        }
        contadorClicks = 0; 
    }
    if (contadorClicks == 2) {
        if (!estadoRReloj) {
            estadoMenu = !estadoMenu; 
            enAjustes = false;
        }
        contadorClicks = 0;
    }
}



void interccionEstados() {
    estadoMenu = false;
    estadoCurso = true;
    tiempoEstado = millis();
   // if (!sistemaEncendido) return; 

    switch (seleccion) {
        case 0: estadoActual = TE; 
            duracionEstado = 6000; 
            indiceFraseAleatoria = random(0, 11);; break;
        case 1:
            estadoActual = COMER;
            necesidades.hambre += 12;
            if (necesidades.hambre > 30) necesidades.hambre = 30;
            break;
        case 2:
            estadoActual = DORMIR;
            necesidades.sueno += 15;
            if (necesidades.sueno > 30) necesidades.sueno = 30;
            break;
        case 3:
            estadoActual = BANAR;
            necesidades.sucio = 30;
            break;
        case 4:
            estadoActual = AJUSTES;
            enAjustes = true;
            estadoCurso = false;
            seleccionAjustes = 0;
            break;
    }
    necesidades.hambre = constrain(necesidades.hambre, 0, 32);
    necesidades.sueno = constrain(necesidades.sueno, 0, 32);
    necesidades.sucio = constrain(necesidades.sucio, 0, 30);
}

void acturlizarNecesidades() {
  if (estadoCurso) return; 
    if ((millis() - ultimoTick) >= 50000) {
        ultimoTick = millis();
        necesidades.hambre--;
        necesidades.sueno--;
        necesidades.sucio--;
    }
}

void actualizarEstadoIA() {
    if (estadoCurso || enAjustes || estadoActual == AJUSTES) return; 
    if (necesidades.hambre <= 0 || necesidades.sueno <= 0 || necesidades.sucio <= 0) {
        estadoActual = MUERTE;
        return;
    }
    necesidadHambre = necesidades.hambre < 15;
    necesidadSueno = necesidades.sueno < 15;
    necesidadSucio = necesidades.sucio < 15;
    
    if (necesidadHambre) {
        baseActual = BASE_HAMBRE;
    } else {
        baseActual = BASE_IDE;
    }
    if (necesidades.hambre >= 30 && necesidades.sueno >= 30 && necesidades.sucio >= 30) {
        estadoActual = FELIZ;
        return;
    }
    if (estadoActual != COMER && estadoActual != DORMIR && estadoActual != BANAR && estadoActual != TE && estadoActual != FELIZ && estadoActual != MUERTE) {
        estadoActual = IDE;
    }
}
