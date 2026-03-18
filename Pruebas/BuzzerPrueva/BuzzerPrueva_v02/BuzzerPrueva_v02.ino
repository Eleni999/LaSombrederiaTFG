const int buzzerPin = 8; // Pin donde está conectado el buzzer

// Notas en Hz
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;

void setup() {
  // No necesitamos configurar nada más
}

void loop() {
  // Melodía: C D E F G A B C
  tone(buzzerPin, C, 300); // Nota C por 300 ms
  delay(350);               // Pequeña pausa entre notas
  tone(buzzerPin, D, 300);
  delay(350);
  tone(buzzerPin, E, 300);
  delay(350);
  tone(buzzerPin, F, 300);
  delay(350);
  tone(buzzerPin, G, 300);
  delay(350);
  tone(buzzerPin, A, 300);
  delay(350);
  tone(buzzerPin, B, 300);
  delay(350);
  tone(buzzerPin, C, 500);
  delay(500); // Pausa antes de repetir la melodía
}

