/*
  Seminario de Informatica y Telecomunicaciones
  Clase 11/4 - TP Maquina de Estados Reloj - Ejercicio 2
  Hecho por Santiago Rapetti y Ariel Slonimsqui.
*/

#include <Arduino.h>
#include <ESP32Time.h>
#include "time.h"
#include <WiFi.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define SCREEN_WIDTH 128 // Anchura OLED display en pixeles
#define SCREEN_HEIGHT 64 // Altura OLED display en pixeles

#define DHTPIN 23     // Pin digital conectado al sensor DHT
#define DHTTYPE    DHT11     // DHT 11

#define SW_1 34
#define SW_2 35

#define PANTALLA_1 0 //muestro temperatura y hora 
#define ESPERA_1 1 //apretar y soltar botones
#define PANTALLA_2 2 // muestro umbral
#define SUMA_GMT 3 //sumo gmt
#define RESTA_GMT 4 //resto gmt
#define ESPERA_2  5

int estadoMaquina;

int estadoSwitch1;
int estadoSwitch2;

int hora;
int minutos;
int gmt = -3;

float temp;

long unsigned int timestamp; // hora
const char *ntpServer = "south-america.pool.ntp.org";
long gmtOffset_sec = -10800;
const int daylightOffset_sec = 0;

const char* ssid = "ORT-IoT";
const char* password = "OrtIOTnew22$2";

struct tm timeinfo;
ESP32Time rtc;

void pedir_lahora(void); // Declaracion de funcion
void setup_rtc_ntp(void); // Declaracion de funcion

DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

void setup() {

  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);

  Serial.begin(115200);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();

  Serial.println("Connecting to Wi-Fi...");

  initWiFi();
  setup_rtc_ntp();

}



void loop() {

  estadoSwitch1 = digitalRead(SW_1);
  estadoSwitch2 = digitalRead(SW_2);

  if (gmtOffset_sec > 43200) {
    gmtOffset_sec = 43200;
  }

  if (gmtOffset_sec < -43200) {
    gmtOffset_sec = -43200;
  }

  if (gmt < -12) {
    gmt = -12;
  }

  if (gmt > 12) {
    gmt = 12;
  }

  maquinaDeEstados();

}

void maquinaDeEstados() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  switch (estadoMaquina) {

    case PANTALLA_1:

      pantalla1();

      if (estadoSwitch1 == LOW &&  estadoSwitch2 == LOW) {
        estadoMaquina = 1;
      }

      break;

    case ESPERA_1:

      pantalla1();

      if (estadoSwitch1 == HIGH &&  estadoSwitch2 == HIGH) {
        estadoMaquina = 2;
      }

      break;

    case PANTALLA_2:

      pantalla2();

      if (estadoSwitch2 == LOW) {

        estadoMaquina = 3;
      }

      if (estadoSwitch1 == LOW) {

        estadoMaquina = 4;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      break;

    case SUMA_GMT:

      pantalla2();


      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      if (estadoSwitch2 == HIGH) {

        gmtOffset_sec = gmtOffset_sec + 3600;
        gmt = gmt + 1;

        display.clearDisplay();
        setup_rtc_ntp();

        estadoMaquina = 2;
      }


      break;

    case RESTA_GMT:

      pantalla2();

      if (estadoSwitch1 == HIGH) {

        gmtOffset_sec = gmtOffset_sec - 3600;
        gmt = gmt - 1;

        display.clearDisplay();
        setup_rtc_ntp();

        estadoMaquina = 2;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      break;

    case ESPERA_2:

      pantalla2();

      if (estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = 0;
      }

      break;

  }
}

void initWiFi() { // Funcion que inicializa el Wifi

  WiFi.begin(ssid , password );
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();

}

void setup_rtc_ntp(void) {

  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  timestamp = time(NULL);
  rtc.setTime(timestamp + gmtOffset_sec);

}

void pantalla1() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temperatura Actual: ");
  display.println(event.temperature);
  display.println("Hora: ");

  minutos = timeinfo.tm_min;

  if (!getLocalTime(&timeinfo))
  {
    timestamp = rtc.getEpoch() - gmtOffset_sec;
    timeinfo = rtc.getTimeStruct();
    display.print(&timeinfo, "%H:%M");
  }

  else {

    timestamp = time(NULL);
    if (minutos != timeinfo.tm_min) {
      display.clearDisplay();
    }
    display.print(&timeinfo, "%H:%M");

  }

  display.display();

}

void pantalla2() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("GMT: ");
  display.println(gmt);
  display.display();

}
