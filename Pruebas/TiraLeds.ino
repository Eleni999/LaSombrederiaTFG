
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_LEDS 7  

Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int ledActual = 0;
int colorActual = 0;

void setup() {
  strip.begin();
  strip.show(); // Todo apagado
}

void loop() {
  strip.clear(); // Apagar todos los LEDs

  uint32_t color;

  // Cambiar de color
  switch (colorActual) {
    case 0:
      color = strip.Color(255, 0, 0); // Rojo
      break;
    case 1:
      color = strip.Color(0, 255, 0); // Verde
      break;
    case 2:
      color = strip.Color(0, 0, 255); // Azul
      break;
    case 3:
      color = strip.Color(255, 255, 0); // Amarillo
      break;
    case 4:
      color = strip.Color(255, 0, 255); // Magenta
      break;
    case 5:
      color = strip.Color(0, 255, 255); // Cyan
      break;
  }

  // Encender solo un LED
  strip.setPixelColor(ledActual, color);
  strip.show();

  delay(400);

  // Siguiente LED
  ledActual++;

  if (ledActual >= NUM_LEDS) {
    ledActual = 0;
    colorActual++;
    if (colorActual > 5) colorActual = 0;
  }
}