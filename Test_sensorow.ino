// Wymagane biblioteki w Arduino IDE: 
// OneWire, DallasTemperature, 
// Adafruit_BMP280 (lub odpowiednik BME), Adafruit_Sensor, Wire.

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BME280.h>

// Definicje pinów
#define PIN_DS18B20 4     // Pin magistrali 1-Wire
#define PIN_LED 2         // Wbudowana dioda LED na ESP32
#define PIN_BUTTON 0     // Przycisk do wyzwalania akcji/przerwania
#define I2C_SDA 21 // Domyślnie I2C (SDA: GPIO 21 na ESP32)
#define I2C_SCL 22 // Domyślnie I2C (SCL: GPIO 22 na ESP32)

#define DELAY_LOOP 100

// Inicjalizacja obiektów
OneWire oneWire(PIN_DS18B20); // Odpalamy magistralę 1-Wire
DallasTemperature sensors(&oneWire); // Podłączamy uruchomioną magistralę do biblioteki sensora
//Adafruit_BMP280 bmp; 
Adafruit_BME280 bmp; 

volatile bool buttonPressed = false;

// Funkcja obsługi przerwania (ISR)
void IRAM_ATTR handleButtonInterrupt() {
    buttonPressed = true; // Wykryto przerwanie - przycisk został wciśnięty ustaw zmienną przyciskową.
}

void setup() {
    Serial.begin(115200);
    
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    // Konfiguracja przerwania na przycisku (zbocze opadające, wciśnięcie)
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), handleButtonInterrupt,FALLING);
    
    // Uruchomienie czujników
    sensors.begin(); // DS18B20
    Wire.begin(I2C_SDA, I2C_SCL); // Odpalamy I2C dla BME/BMP
    
    if (!bmp.begin(0x76, &Wire)) { // Obecności BME na I2C pod adresem 0x76 lub 0x77)
        Serial.println("\nNie wykryto czujnika BMP/BME!\nMoze DS18B20 zadziała");
    }
    else {
        Serial.println("\nSensor BME/BMP gotowy");
    }
}

void loop() {
    // Symulacja reakcji na zdarzenie (np. wybudzenie)
    if (buttonPressed) { // Sprawdzamy czy był wciśnięty przycisk
        // Jak był, to mryg diodką i dokonujemy pomiarów
        digitalWrite(PIN_LED, HIGH);
        Serial.println("[Wykryto ZDARZENIE] - wymuszenie pomiaru!");
        buttonPressed = false; // Musimy „skasować” powiadomienie o wciśnięciu
        delay(100);
        digitalWrite(PIN_LED, LOW);

        // 1. Odczyt z magistrali 1-Wire (DS18B20)
        sensors.requestTemperatures();
        float tempDS = sensors.getTempCByIndex(0);
        
        // 2. Odczyt z magistrali I2C (BMP280)
        float tempBMP = bmp.readTemperature();
        float higBMP = bmp.readHumidity();
        float pressure = bmp.readPressure() / 100.0F; // Konwersja na hPa
        
        // Prezentacja danych
        Serial.println("--- ODCZYT WĘZŁA SENSORYCZNEGO ---");
        Serial.print("Temp.  DS18B20 (1-Wire): "); Serial.print(tempDS); Serial.println(" *C");
        Serial.print("Temp.  BMP280 (I2C):     "); Serial.print(tempBMP); Serial.println(" *C");
        Serial.print("Wilg.  BMP280 (I2C):     "); Serial.print(higBMP); Serial.println("%");
        Serial.print("Press. BMP280:      "); Serial.print(pressure); Serial.println(" hPa");
        Serial.println("----------------------------------");
    }

    // Przejście w stan oczekiwania (w praktyce powinien być stosowany: tryb  Light Sleep)
    delay(DELAY_LOOP); 
}