#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Na wszelki wypadek, dla kultury kody i dobrych praktyk
  delay(100);
  Serial.println("ESP32: Rozpoczynam skanowanie otoczenia radiowego...");
}

void loop() {
  int count = WiFi.scanNetworks();
  
  if (count == 0) {
    Serial.println("Nie wykryto zadnych sieci.");
  } else {
    Serial.printf("Znaleziono %d sieci Wi-Fi:\n", count);
    for (int i = 0; i < count; ++i) {
      Serial.printf("  %2d) SSID: %-20s | RSSI: %3d dBm\n", 
                    i + 1, 
                    WiFi.SSID(i).c_str(), 
                    WiFi.RSSI(i));
    }
  }
  Serial.println("----------------------------------------------");
  delay(8000);
}
