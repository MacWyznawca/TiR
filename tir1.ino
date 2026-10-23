// #define LED_PIN 2 // Najpier sprawdzamy, czy wybrana płytka ma definicję tego pinu, jak nie do odkomentować

void setup() {
  Serial.begin(115200); // Uwaga! Sprawdż czy ta sama prędkość jest w ustawieniach Monitora! Inaczej będą krzaczki lub niczego nie będzie
  pinMode(LED_PIN, OUTPUT);
  Serial.println("--- ESP32 wystartowal ---");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Stan diody: ON");
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.println("Stan diody: OFF");
  delay(500);
}
