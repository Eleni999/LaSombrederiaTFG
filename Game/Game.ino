#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>
#include <ezButton.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include "BibliotecaImagenes.h"
#include "EscenaReloj.h"

// Pantalla
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 10, 9, 12, 11, 13);

// Interruptor
ezButton toggleSwitch(2);
bool sistemaEncendido = true;
bool ultimoEstadoInterruptor = false;

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
int ledcentral = NUM_LEDS/10;
uint32_t color;

// Ajustes UI
enum SubEstadosAjustes {
    AJUSTES_PRINCIPAL,
    VELOCIDAD_TEXTO,
    CAMBIAR_HORA
};
SubEstadosAjustes subEstadosAjustes = AJUSTES_PRINCIPAL;
int seleccionPrincipalAjustes = 0;
bool enAjustes = false;
int seleccionAjustes = 0;
int velocidadAnimacion = 5;
int adjHora = 12, adjMin = 0, adjSeg = 0;

// Té
unsigned long duracionTe = 9000;
unsigned long inicioTe = 0;
int indiceFraseAleatoria = 0;
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
    int velGuardada = 5;
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

// EEPROM y Reset
unsigned long ultimoGuardado = 0;
const unsigned long intervaloGuardado = 10000; 
unsigned long inicioPulsadoReset = 0;
bool resetCompletado = false;
const unsigned long TIEMPO_RESET = 10000;

void manejarScrollTe(U8G2 &u8g2, int fraseIdx) {
const char* frase = obtenerFrase(fraseIdx);
    u8g2.setFont(u8g2_font_5x7_tr);
    int anchoTexto = u8g2.getStrWidth(frase);
    
    // 1. Calculamos cuánto tiempo ha pasado
    unsigned long transcurrido = millis() - tiempoEstado;
    
    // 2. Calculamos el progreso (de 0.0 a 1.0)
    // Usamos float para que el movimiento sea fluido
    float progreso = (float)transcurrido / (float)duracionEstado;
    
    // 3. Evitamos que el progreso pase de 1.0 (final de la animación)
    if (progreso > 1.0) progreso = 1.0;
    
    // 4. Calculamos la posición X
    // El texto empieza en 128 (derecha) y debe terminar en -anchoTexto (izquierda)
    // La distancia total a recorrer es (128 + anchoTexto)
    scrollX = 128 - (int)(progreso * (128 + anchoTexto));
    
    u8g2.drawStr(scrollX, 63, frase);
}

void dibujoAjustesUI_Nueva(U8G2 &u8g2) {
    u8g2.setFont(u8g2_font_6x10_tf);
    
    if (subEstadosAjustes == AJUSTES_PRINCIPAL) {
        u8g2.drawStr(10, 15, "AJUSTES");
        u8g2.drawHLine(10, 17, 100);
        
        if (seleccionPrincipalAjustes == 0) u8g2.drawStr(5, 35, "> Velocidad Texto");
        else u8g2.drawStr(15, 35, "Velocidad Texto");
        
        if (seleccionPrincipalAjustes == 1) u8g2.drawStr(5, 50, "> Cambiar Hora");
        else u8g2.drawStr(15, 50, "Cambiar Hora");
        
        if (seleccionPrincipalAjustes == 2) u8g2.drawStr(5, 64, "> ACEPTAR");
        else u8g2.drawStr(15, 64, "ACEPTAR");
    }
    else if (subEstadosAjustes == VELOCIDAD_TEXTO) {
        u8g2.drawStr(10, 15, "VELOCIDAD");
        const char* opciones[] = {"Lento", "Medio", "Rapido"};
        int vals[] = {1, 5, 10};
        
        for(int i=0; i<3; i++) {
            int yPos = 35 + (i * 12);
            u8g2.drawStr(20, yPos, opciones[i]);
            if (velocidadAnimacion == vals[i]) u8g2.drawFrame(18, yPos - 9, 50, 12);
            if (seleccionAjustes == i) u8g2.drawHLine(20, yPos + 1, u8g2.getStrWidth(opciones[i]));
        }
    }
    else if (subEstadosAjustes == CAMBIAR_HORA) {
        char buf[9];
        sprintf(buf, "%02d:%02d", adjHora, adjMin);
        u8g2.setFont(u8g2_font_timR18_tr);
        u8g2.drawStr(35, 41, buf);
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.drawStr(10, 15, "EDITAR HORA");
        if (seleccionAjustes == 0) u8g2.drawHLine(35, 43, 25); 
        else u8g2.drawHLine(68, 43, 25); 
    }
}

// Alertas y LEDs
unsigned long ultimoCicloParpadeo = 0;
int contadorDestellos = 0;
bool ledEncendidoParpadeo = false;
unsigned long ultimoCambioDestello = 0;
const unsigned long PAUSA_LARGA = 30000;
const int TIEMPO_DESTELLO = 200;

uint32_t calcularColorAlerta() {
    if (necesidadHambre && necesidadSueno && necesidadSucio) return strip.Color(255, 0, 0);
    if (necesidadSueno && necesidadSucio) return strip.Color(0, 100, 100);
    if (necesidadHambre && necesidadSueno) return strip.Color(100, 100, 0);
    if (necesidadHambre && necesidadSucio) return strip.Color(128, 255, 0);
    if (necesidadHambre) return strip.Color(204, 118, 0);
    if (necesidadSueno) return strip.Color(0, 0, 139);
    if (necesidadSucio) return strip.Color(0, 100, 0);
    return strip.Color(0, 0, 0);
}

void manejarAlertaApagado() {
    if (!necesidadHambre && !necesidadSueno && !necesidadSucio) {
        strip.clear(); strip.show(); return;
    }
    unsigned long ahora = millis();
    if (contadorDestellos >= 6) {
        if (ahora - ultimoCicloParpadeo >= PAUSA_LARGA) contadorDestellos = 0;
        return;
    }
    if (ahora - ultimoCambioDestello >= TIEMPO_DESTELLO) {
        ultimoCambioDestello = ahora;
        ledEncendidoParpadeo = !ledEncendidoParpadeo;
        contadorDestellos++;
        if (ledEncendidoParpadeo) {
            uint32_t colorAlerta = calcularColorAlerta();
            for(int i=0; i<NUM_LEDS; i++) strip.setPixelColor(i, colorAlerta);
        } else strip.clear();
        strip.show();
        if (contadorDestellos >= 6) ultimoCicloParpadeo = ahora;
    }
}

void sistemaInterruptor() {
    toggleSwitch.loop();
    sistemaEncendido = toggleSwitch.getState();
}

void guardarNecesidades() {
    EEPROM.put(0, necesidades);
}

void cargarNecesidades() {
    EEPROM.get(0, necesidades);
    if (necesidades.hambre < 0 || necesidades.hambre > 50 || necesidades.sueno < 0 || necesidades.sueno > 50 || necesidades.sucio < 0 || necesidades.sucio > 50) {
        necesidades.hambre = 30; necesidades.sueno = 30; necesidades.sucio = 30;
    }
}

// --- LÓGICA DE AJUSTES CORREGIDA ---
void manejarEntradaAjustes() {
    if (subEstadosAjustes == AJUSTES_PRINCIPAL) {
        switch (seleccionPrincipalAjustes) {
            case 0: 
                subEstadosAjustes = VELOCIDAD_TEXTO; 
                seleccionAjustes = (velocidadAnimacion == 1 ? 0 : (velocidadAnimacion == 5 ? 1 : 2));
                break;
            case 1: 
                subEstadosAjustes = CAMBIAR_HORA; 
                seleccionAjustes = 0; 
                adjHora = hour(); 
                adjMin = minute(); 
                break;
            case 2: // ACEPTAR
                reloj.setTime(adjHora, adjMin, 0);
                guardarNecesidades();
                enAjustes = false;
                estadoActual = IDE;
                subEstadosAjustes = AJUSTES_PRINCIPAL;
                break;
        }
    } else if (subEstadosAjustes == VELOCIDAD_TEXTO) {
        if (seleccionAjustes == 0) velocidadAnimacion = 1;
        else if (seleccionAjustes == 1) velocidadAnimacion = 5;
        else if (seleccionAjustes == 2) velocidadAnimacion = 10;
        subEstadosAjustes = AJUSTES_PRINCIPAL;
    } else if (subEstadosAjustes == CAMBIAR_HORA) {
        if (seleccionAjustes == 0) {
            seleccionAjustes = 1; // Pasar a minutos
        } else {
            reloj.setTime(adjHora, adjMin, 0);
            subEstadosAjustes = AJUSTES_PRINCIPAL;
        }
    }
}

void setup() {
    u8g2.begin();
    u8g2.setBitmapMode(1);
    reloj.begin();
    cargarNecesidades();
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

    if (sistemaEncendido && !ultimoEstadoInterruptor) {
        // Justo en el momento que se enciende:
        estadoRReloj = true;           // Forzamos que salga el reloj
        arranqueCompleto = false;      // Reiniciamos el flag de arranque
        iniciarSistema = millis();     // Reiniciamos el cronómetro de 4 segundos
    }
    ultimoEstadoInterruptor = sistemaEncendido;
    if (millis() - ultimoGuardado > intervaloGuardado) {
        guardarNecesidades();
        ultimoGuardado = millis();
    }

    if (!sistemaEncendido) {
       u8g2.clearBuffer(); 
        u8g2.sendBuffer();
        strip.clear(); 
        strip.show();
        acturlizarNecesidades();
        actualizarEstadoIA();
        manejarAlertaApagado();
        return;
    }

    u8g2.clearBuffer();
    Menu();
    Reloj();

    if (!arranqueCompleto && millis() - iniciarSistema >= 3000) {
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
                if (baseActual == BASE_HAMBRE) { dibujarHambre(u8g2); color = strip.Color(255, 129, 0); }
                else { dibujoIDE(u8g2); color = strip.Color(255, 156, 61); }
                if (necesidadSucio) dibujoSucio(u8g2);
                if (necesidadSueno) dibujarSueno(u8g2);
                break;
            case FELIZ: dibujarFeliz(u8g2); color = strip.Color(255, 156, 61); break;
            case BANAR: color = strip.Color(3, 255, 244); dibujarBano(u8g2); break;
            case COMER: dibujarComer(u8g2); color = strip.Color(245, 188, 78); break;
            case DORMIR: dibujarDormir(u8g2); color = strip.Color(66, 2, 49); break;
            case MUERTE: animacionLedMuerte(); dibujarMuerte(u8g2); break;
            case TE: manejarScrollTe(u8g2, indiceFraseAleatoria); dibujarTe(u8g2); color = strip.Color(245, 188, 78); break;
            case AJUSTES: dibujoAjustesUI_Nueva(u8g2); break;
        }
    }
    
    if (estadoActual != MUERTE) {
        strip.setBrightness(92);
        strip.setPixelColor(ledcentral, color);
    }
    strip.show();
    u8g2.sendBuffer();
    delay(10);
}

void Menu() {
    if (!sistemaEncendido) return;
    estadoActualCLK = digitalRead(pinCLK);
    if (estadoActualCLK != estadoAnteriorCLK && estadoActualCLK == LOW) {
        if (millis() - ultimoCmabioEncoder >= debouceEncoder) {
            bool direccion = (digitalRead(pinDT) != estadoActualCLK);
            if (enAjustes) {
                if (subEstadosAjustes == AJUSTES_PRINCIPAL) {
                    seleccionPrincipalAjustes += (direccion ? 1 : -1);
                    if (seleccionPrincipalAjustes > 2) seleccionPrincipalAjustes = 0;
                    if (seleccionPrincipalAjustes < 0) seleccionPrincipalAjustes = 2;
                }
                else if (subEstadosAjustes == VELOCIDAD_TEXTO) {
                    seleccionAjustes += (direccion ? 1 : -1);
                    if (seleccionAjustes > 2) seleccionAjustes = 0;
                    if (seleccionAjustes < 0) seleccionAjustes = 2;
                }
                else if (subEstadosAjustes == CAMBIAR_HORA) {
                    if (seleccionAjustes == 0) { // Horas
                        adjHora += (direccion ? 1 : -1);
                        if (adjHora > 23) adjHora = 0; if (adjHora < 0) adjHora = 23;
                    } else { // Minutos
                        adjMin += (direccion ? 1 : -1);
                        if (adjMin > 59) adjMin = 0; if (adjMin < 0) adjMin = 59;
                    }
                }
            }
            else if (estadoMenu) {
                seleccion += (direccion ? 1 : -1);
                if (seleccion > 4) seleccion = 0;
                if (seleccion < 0) seleccion = 4;
            }
            ultimoCmabioEncoder = millis();
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
            inicioPulsadoReset = millis();
            resetCompletado = false;
            if (millis() - ultimoClick < tiempoDobleClick) contadorClicks++;
            else contadorClicks = 1;
            ultimoClick = millis();
        } else {
            if (!resetCompletado && (millis() - inicioPulsadoReset >= 10000)) {
                ejecutarReset(); resetCompletado = true;
            }
            if (!arranqueCompleto && (millis() - tiempoPulsado >= 2000)) {
                estadoRReloj = false; 
                arranqueCompleto = true;
                 botonPresionado = false;
            }
        }
    } else {
        if (botonPresionado) {
            if (enAjustes) manejarEntradaAjustes();
            botonPresionado = false;
        }
    }

    unsigned long tiempoTranscurrido = millis() - ultimoClick;
    if (contadorClicks == 1 && tiempoTranscurrido > tiempoDobleClick) {
        if (estadoMenu && !enAjustes) interccionEstados();
        else if (estadoRReloj) estadoRReloj = false;
        contadorClicks = 0;
    }
    if (contadorClicks == 2) {
        if (!estadoRReloj) {
            estadoMenu = !estadoMenu; enAjustes = false; subEstadosAjustes = AJUSTES_PRINCIPAL;
        }
        contadorClicks = 0;
    }
}

void interccionEstados() {
    estadoMenu = false;
    estadoCurso = true;
    tiempoEstado = millis();
    switch (seleccion) {
        case 0: 
        estadoActual = TE;
         duracionEstado = 6000;
         indiceFraseAleatoria = random(0, 11); 
        if (velocidadAnimacion <= 1) {
        duracionEstado = 12000; 
        } else if (velocidadAnimacion <= 5) {
        duracionEstado = 6000;
         } else {
        duracionEstado = 3000;
        }
    
    tiempoEstado = millis();
         break;
        case 1: estadoActual = COMER; necesidades.hambre += 12; break;
        case 2: estadoActual = DORMIR; necesidades.sueno += 15; break;
        case 3: estadoActual = BANAR; necesidades.sucio = 30; break;
        case 4: estadoActual = AJUSTES; enAjustes = true; estadoCurso = false; seleccionAjustes = 0; break;
    }
    necesidades.hambre = constrain(necesidades.hambre, 0, 30);
    necesidades.sueno = constrain(necesidades.sueno, 0, 30);
    necesidades.sucio = constrain(necesidades.sucio, 0, 30);
}

void acturlizarNecesidades() {
    if (estadoCurso) return;
    if ((millis() - ultimoTick) >= 50000) {
        ultimoTick = millis();
        necesidades.hambre--; necesidades.sueno--; necesidades.sucio--;
    }
}

void actualizarEstadoIA() {
    if (estadoCurso || enAjustes || estadoActual == AJUSTES) return;
    if (necesidades.hambre <= 0 || necesidades.sueno <= 0 || necesidades.sucio <= 0) {
        estadoActual = MUERTE; return;
    }
    necesidadHambre = necesidades.hambre < 15;
    necesidadSueno = necesidades.sueno < 15;
    necesidadSucio = necesidades.sucio < 15;
    
    if (necesidadHambre) baseActual = BASE_HAMBRE;
    else baseActual = BASE_IDE;

    if (necesidades.hambre >= 29 && necesidades.sueno >= 29 && necesidades.sucio >= 29) {
        estadoActual = FELIZ;
    } else {
        estadoActual = IDE;
    }
}

unsigned long tiempoAnterior;
int ledActual = 0;
int intervaloLed = 50;

void animacionLedMuerte() {
    if (millis() - tiempoAnterior > intervaloLed) {
        tiempoAnterior = millis();
        strip.clear();
        for (int i = 0; i <= ledActual; i++) {
            int brillo = map(i, 0, NUM_LEDS - 1, 255, 0);
            strip.setPixelColor(i, strip.Color(brillo, 0, 0));
        }
        strip.show();
        ledActual++;
        if (ledActual >= NUM_LEDS) ledActual = NUM_LEDS;
    }
}

void ejecutarReset(){
    necesidades.hambre = 30; necesidades.sueno = 30; necesidades.sucio = 30;
    necesidadHambre = false; necesidadSueno = false; necesidadSucio = false;
    estadoActual = FELIZ;
    for(int j=0; j<3; j++) {
        for(int i=0; i<NUM_LEDS; i++) strip.setPixelColor(i, strip.Color(255, 255, 255));
        strip.show(); delay(100);
        strip.clear(); strip.show(); delay(100);
    }
    guardarNecesidades();
}
