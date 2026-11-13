#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BME280.h>

const char* ssid = "QART";
const char* password = "malamysz";
WebServer server(80);

#define PIN_DS18B20 4
#define PIN_LED 2 // Wyjście PWM grzałki
#define I2C_SDA 21
#define I2C_SCL 22

// Konfiguracja PWM dla ESP32 Core 3.x
const int pwmFreq = 5000;
const int pwmResolution = 8;

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);
Adafruit_BME280 bmp;

// Parametry PD
float setpoint = 28.0;
float KP = 50.0;
float KD = 100.0;

// Zmienne do podglądu dydaktycznego
float currentError = 0.0;
float errorP = 0.0;
float errorD = 0.0;
float previousTemp = 0.0;
int pwmOutput = 0;

float currentTemp = 0.0;
float currentHum = 0.0;

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta http-equiv='refresh' content='2'>";
    html += "<title>Dydaktyczny Termostat PD</title>";
    html += "<style>body { font-family: sans-serif; margin: 20px; max-width: 600px; }";
    html += ".box { border: 1px solid #ccc; padding: 15px; margin-top: 15px; background: #f9f9f9; border-radius: 5px; }";
    html += "input[type=number] { width: 80px; padding: 5px; font-size: 16px; }";
    html += "input[type=submit] { padding: 6px 15px; font-size: 16px; background: #007BFF; color: white; border: none; border-radius: 3px; cursor: pointer; }";
    html += "</style></head><body>";
    
    html += "<h1>Węzeł IoT: Termostat PD</h1>";
    
    // FORMULARZ ZMIANY TEMPERATURY
    html += "<div class='box' style='background: #eef5ff; border-color: #b8daff;'>";
    html += "<form action='/set' method='GET'>";
    html += "<b>Nowa temp. zadana:</b> ";
    html += "<input type='number' name='new_temp' step='0.5' value='" + String(setpoint, 1) + "' required> &deg;C ";
    html += "<input type='submit' value='Zmień Setpoint'>";
    html += "</form></div>";

    html += "<div class='box'><b>Odczyty środowiskowe:</b><br>";
    html += "Temperatura (DS18B20): <b>" + String(currentTemp, 2) + " &deg;C</b><br>";
    html += "Wilgotność (BME280): <b>" + String(currentHum, 1) + " %</b></div>";
    
    html += "<div class='box'><b>Parametry Regulatora PD:</b><br>";
    html += "Wartość zadana (Setpoint): <b style='color: #007BFF; font-size: 18px;'>" + String(setpoint, 1) + " &deg;C</b> (KP=" + String(KP,0) + ", KD=" + String(KD,0) + ")<br>";
    html += "Uchyb bieżący (Error): <b>" + String(currentError, 2) + " &deg;C</b><br>";
    html += "Człon Proporcjonalny (<b>errorP</b>): <b>" + String(errorP, 2) + "</b><br>";
    html += "Człon Różniczkowy (<b>errorD</b>): <b>" + String(errorD, 2) + "</b></div>";
    
    html += "<div class='box'><b>Wysterowanie wyjścia:</b><br>";
    html += "Suma poprawek (korekta): <b>" + String(errorP + errorD, 2) + "</b><br>";
    html += "Wyjście PWM Grzałki (0-255): <b style='color: red; font-size: 18px;'>" + String(pwmOutput) + "</b></div>";
    
    html += "</body></html>";
    
    server.send(200, "text/html", html);
}

// Obsługa zmiany temperatury
void handleSetTemp() {
    if (server.hasArg("new_temp")) {
        String val = server.arg("new_temp");
        setpoint = val.toFloat();
        Serial.println("[WWW] Zmieniono setpoint na: " + String(setpoint));
    }
    // Przekierowanie z powrotem na stronę główną
    server.sendHeader("Location", "/");
    server.send(303);
}

void setup() {
    Serial.begin(115200);
    
    // Nowe API LEDC
    ledcAttach(PIN_LED, pwmFreq, pwmResolution);
    
    sensors.begin();
    Wire.begin(I2C_SDA, I2C_SCL);
    bmp.begin(0x76, &Wire);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nIP: http://" + WiFi.localIP().toString());

    server.on("/", handleRoot);
    server.on("/set", handleSetTemp); // Rejestracja endpointu formularza
    server.begin();
}

void loop() {
    server.handleClient();
    
    static unsigned long lastTime = 0;
    unsigned long now = millis();
    
    // Regulacja co 1 sekundę
    if (now - lastTime >= 1000) {
        sensors.requestTemperatures();
        currentTemp = sensors.getTempCByIndex(0);
        currentHum = bmp.readHumidity();
        
        // Algorytm PD
        currentError = setpoint - currentTemp;
        errorP = currentError * KP;

        // Człon D na podstawie zmiany zmiennej procesowej (temperatury)
        // Zwróć uwagę na kolejność: previousTemp - currentTemp
        errorD = (previousTemp - currentTemp) * KD; 

        float korekta = errorP + errorD;
        previousTemp = currentTemp; // Zapisujemy czystą temperaturę do kolejnego cyklu

        pwmOutput = (int)korekta;
        if (pwmOutput > 255) pwmOutput = 255;
        if (pwmOutput < 0) pwmOutput = 0;
        
        ledcWrite(PIN_LED, pwmOutput);
        
        // Logowanie do Serial Monitora
        Serial.print("Set: "); Serial.print(setpoint);
        Serial.print(" | T: "); Serial.print(currentTemp);
        Serial.print(" | Err: "); Serial.print(currentError);
        Serial.print(" | P: "); Serial.print(errorP);
        Serial.print(" | D: "); Serial.print(errorD);
        Serial.print(" | PWM: "); Serial.println(pwmOutput);
        
        lastTime = now;
    }
}