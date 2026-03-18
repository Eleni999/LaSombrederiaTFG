#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>
#include <ezButton.h>

// --- OLED SPI ---
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 10, 9, 12, 11, 13);

// --- RTC ---
RTC_DS3231 rtc;

// --- LEDs ---
const int ledBlanco = 6;
const int ledRojo   = 7;

// --- Buzzer ---
const int buzzerPin = 8;

// --- Encoder ---
const int pinCLK = 3;
const int pinDT  = 4;
const int pinSW  = 5;
int estadoAnteriorCLK;
int seleccion = 0;

// --- Interruptor ---
ezButton toggleSwitch(2);

// --- Menú ---
const char* menu[] = {"Comer", "Dormir"};
int numOpciones = 2;

// --- Animación ---
int salto = 0;

// --- Notas ---
int notas[] = {262, 294, 330, 349, 392, 440, 494, 262};
int numNotas = 8;
int indiceNota = 0;
unsigned long tiempoNotaAnterior = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  toggleSwitch.setDebounceTime(50);

  u8g2.begin();
  u8g2.setContrast(255);

  pinMode(ledBlanco, OUTPUT);
  pinMode(ledRojo, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinDT, INPUT_PULLUP);
  pinMode(pinSW, INPUT_PULLUP);
  estadoAnteriorCLK = digitalRead(pinCLK);

  if (!rtc.begin()) {
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(2026, 12, 13, 19, 51, 0));
  }

  DateTime nowRTC = rtc.now();
  setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());
}

void loop() {

  toggleSwitch.loop();
  bool sistemaActivo = toggleSwitch.getState();

  DateTime nowRTC = rtc.now();
  setTime(nowRTC.hour(), nowRTC.minute(), nowRTC.second(),
          nowRTC.day(), nowRTC.month(), nowRTC.year());

  if (sistemaActivo) {
    digitalWrite(ledBlanco, HIGH);
    digitalWrite(ledRojo, HIGH);
  } else {
    digitalWrite(ledBlanco, LOW);
    digitalWrite(ledRojo, LOW);
    noTone(buzzerPin);
  }

  if (sistemaActivo) {
    int estadoActualCLK = digitalRead(pinCLK);

    if (estadoAnteriorCLK == HIGH && estadoActualCLK == LOW) {
      if (digitalRead(pinDT) == HIGH) {
        seleccion++;
        if (seleccion >= numOpciones) seleccion = 0;
      } else {
        seleccion--;
        if (seleccion < 0) seleccion = numOpciones - 1;
      }
    }
    estadoAnteriorCLK = estadoActualCLK;

    if (digitalRead(pinSW) == LOW) {
      Serial.print("Opción seleccionada: ");
      Serial.println(menu[seleccion]);
      delay(300);
    }
  }

  u8g2.clearBuffer();

  if (sistemaActivo) {

    u8g2.setFont(u8g2_font_6x10_tf);

    char buf[9];
    sprintf(buf, "%02d:%02d:%02d", hour(), minute(), second());
    u8g2.drawStr(4, 10, buf);

    u8g2.drawEllipse(55, 30 + salto, 4, 12, U8G2_DRAW_ALL);
    u8g2.drawEllipse(73, 30 + salto, 4, 12, U8G2_DRAW_ALL);
    u8g2.drawDisc(64, 40 + salto, 12, U8G2_DRAW_ALL);

    u8g2.setDrawColor(0);
    u8g2.drawPixel(60, 38 + salto);
    u8g2.drawPixel(68, 38 + salto);
    u8g2.setDrawColor(1);

    for(int i = 0; i < 15; i += 2) {
      u8g2.drawLine(54 + i, 52 + salto, 54 + i, 64 + salto);
    }

    u8g2.drawFrame(54, 52 + salto, 20, 12);

    for (int i = 0; i < numOpciones; i++) {
      int y = 15 + i*12;
      if (i == seleccion) u8g2.drawStr(0, y, ">");
      u8g2.drawStr(10, y, menu[i]);
    }

    salto = (salto == 0) ? -2 : 0;

    unsigned long tiempoActual = millis();
    if (tiempoActual - tiempoNotaAnterior > 200) {
      tiempoNotaAnterior = tiempoActual;
      tone(buzzerPin, notas[indiceNota], 150);
      indiceNota++;
      if (indiceNota >= numNotas) indiceNota = 0;
    }

  }

  u8g2.sendBuffer();
}