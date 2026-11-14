#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BMP280.h>

const char* ssid = "WSB_EDU_24";
const char* password = "Wroc@love17";

WebServer server(80);

#define PIN_DS18B20 4
#define PIN_LED 2
#define PIN_BUTTON 0
#define I2C_SDA 21
#define I2C_SCL 22

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);
Adafruit_BME280 bmp;

float tempDS = 0.0;
float tempBMP = 0.0;
float higBMP = 0.0;
float presBMP = 0.0;

// Funkcja generująca kod strony WWW z aktualnymi danymi
void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta http-equiv='refresh' content='3'>";
    html += "<title>Węzeł IoT ESP32</title></head><body>";
    html += "<h1>Status Węzła Sensorycznego ESP32</h1>";
    html += "<p><b>Temp. DS18B20 (1-Wire):</b> " + String(tempDS) + " &deg;C</p>";
    html += "<p><b>Temp. BME280 (I2C):</b> " + String(tempBMP) + " &deg;C</p>";
    html += "<p><b>Wilg. BME280 (I2C):</b> " + String(higBMP) + " %</p>";
    html += "<p><b>Ciśnienie BME280:</b> " + String(presBMP) + " hPa</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_LED, OUTPUT);
    
    sensors.begin();
    Wire.begin(I2C_SDA, I2C_SCL); // Poprawne odpalenie I2C
    
    if (!bmp.begin(0x76, &Wire)) { // Poprawne przekazanie magistrali
        Serial.println("\nNie wykryto czujnika BMP/BME!");
    } else {
        Serial.println("\nSensor BME/BMP gotowy");
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    }
    digitalWrite(PIN_LED, HIGH);
    
    Serial.println("\nPołączono! IP: http://" + WiFi.localIP().toString());

    server.on("/", handleRoot);
    server.begin();
}

void loop() {
    server.handleClient();
    
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        sensors.requestTemperatures();
        tempDS = sensors.getTempCByIndex(0);
        
        tempBMP = bmp.readTemperature();
        higBMP = bmp.readHumidity();
        presBMP = bmp.readPressure() / 100.0F;
        
        Serial.println("--- AKTUALIZACJA DANYCH ---");
        Serial.print("Temp. DS18B20: "); Serial.println(tempDS);
        Serial.print("Temp. BME280:  "); Serial.println(tempBMP);
        Serial.print("Wilg. BME280:  "); Serial.println(higBMP);
        Serial.print("Pres. BME280:  "); Serial.println(presBMP);
        Serial.println("---------------------------");
        
        lastUpdate = millis();
    }
    delay(10);
}
