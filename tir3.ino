#define EXT_LED_PIN 2 // 18

void setup() {
  pinMode(EXT_LED_PIN, OUTPUT);
}

void loop() {
  // Rozjasnianie (wypelnienie 0 -> 255)
  for (int duty = 0; duty <= 255; duty += 5) {
    analogWrite(EXT_LED_PIN, duty);
    delay(15);
  }
  // Sciemnianie (wypelnienie 255 -> 0)
  for (int duty = 255; duty >= 0; duty -= 5) {
    analogWrite(EXT_LED_PIN, duty);
    delay(15);
  }
  delay(500);
}
