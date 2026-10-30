#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Na wszelki wypadek, dla kultury kody i dobrych praktyk
  delay(100);
  Serial.println("ESP32: Rozpoczynam skanowanie otoczenia radiowego...");
}

void loop() {
  int count = WiFi.scanNetworks(); // Zwraca ilość odnalezionych sieci
  
  if (count == 0) { // Jeżeli brak sieci WiFI - ilość zero, daj znać, że nic nie znaeziono
    Serial.println("Nie wykryto zadnych sieci.");
  } else {
    Serial.printf("Znaleziono %d sieci Wi-Fi:\n", count); // Podaj ilość znalezionych sieci
    for (int i = 0; i < count; ++i) { // Wyświetl kolejne sieci wifi. Nazwa i moc sygnału
      Serial.printf("  %2d) SSID: %-20s | RSSI: %3d dBm\n", // %2d wyświetla numer kolejny (i + 1) maks. 2 cyfry, %-20s wyświetla nazwę do 20 znaków sformatowane (WiFi.SSID(i).c_str()), %3d wyśiwtla moc sygnału, maks. 3 cyfry (WiFi.RSSI(i))
                    i + 1, 
                    WiFi.SSID(i).c_str(), 
                    WiFi.RSSI(i));
    }
  }
  Serial.println("----------------------------------------------");
  delay(8000);
}
