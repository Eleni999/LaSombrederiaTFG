const int buzzerPin = 8;

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Genera un tono de 2 kHz (2000 Hz) en el pin buzzerPin
  tone(buzzerPin, 2000); 
}

