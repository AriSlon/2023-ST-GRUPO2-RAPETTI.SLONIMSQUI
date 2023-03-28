/*
  Seminario de Informatica y Telecomunicaciones
  Clase 28/3 - Ejercicio 3

*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define DHTPIN 23     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11

#define PIN_RELAY 18

float temp; 


DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

void setup() {

  pinMode(PIN_RELAY, OUTPUT);

  Serial.begin(115200);
  
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

}

void loop() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  temp=event.temperature;
  display.println(event.temperature);
  display.display();


  if(temp>31){
    digitalWrite(PIN_RELAY, HIGH);
  }
  else {
        digitalWrite(PIN_RELAY, LOW);

  }
  
}
