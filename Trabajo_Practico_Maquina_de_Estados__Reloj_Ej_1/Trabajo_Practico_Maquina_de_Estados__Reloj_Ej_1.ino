/*
  Seminario de Informatica y Telecomunicaciones
  Clase 11/4 - TP Maquina de Estados Reloj - Ejercicio 1

  Hecho por Santiago Rapetti y Ariel Slonimsqui
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP32Time.h>

#define SCREEN_WIDTH 128 // Anchura OLED display en pixeles
#define SCREEN_HEIGHT 64 // Altura OLED display en pixeles

#define DHTPIN 23     // Pin digital conectado al sensor DHT
#define DHTTYPE    DHT11     // DHT 11

#define RELAY 18
#define SW_1 34
#define SW_2 35

#define PANTALLA_1 0 //muestro temperatura y hora 
#define ESPERA_1 1 //apretar y soltar botones
#define PANTALLA_2 2 // regular temperatura
#define SUMA_HORAS 3 //
#define SUMA_MINUTOS 4 //
#define ESPERA_2  5

int estadoMaquina;

int estadoSwitch1;
int estadoSwitch2;

int segundos;
int minutos;
int hora;
int dia;
int mes;
int year;

float temp;

DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

ESP32Time rtc;

void setup() {

  Serial.begin(115200);

  segundos = 0;
  minutos = 58;
  hora = 14;
  dia = 11;
  mes = 4;
  year = 2023;

  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);

  dht.begin();


  rtc.setTime(segundos, minutos, hora, dia, mes, year);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

}

void loop() {

  estadoSwitch1 = digitalRead(SW_1);
  estadoSwitch2 = digitalRead(SW_2);
  Serial.println(minutos);
  minutos = rtc.getMinute();
  hora = rtc.getHour();


  //Serial.println(estadoMaquina);

  maquinaDeEstados();

  if (hora > 23) {
    hora = 0;
  }

  if (minutos > 59) {
    minutos = 0;
  }



}

void maquinaDeEstados () {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  switch (estadoMaquina) {

    case PANTALLA_1:

      pantalla1();

      rtc.setTime(segundos, minutos, hora, dia, mes, 2023);

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

      if (estadoSwitch1 == LOW &&  estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      if (estadoSwitch1 == LOW) {

        estadoMaquina = 3;
      }

      if (estadoSwitch2 == LOW) {

        estadoMaquina = 4;
      }


      break;

    case SUMA_HORAS:

      pantalla2();

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      if (estadoSwitch1 == HIGH) {
        hora = hora + 1;
        estadoMaquina = 2;
      }

      break;

    case SUMA_MINUTOS:

      pantalla2();

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      if (estadoSwitch2 == HIGH) {
        minutos = minutos + 1;
        estadoMaquina = 2;
      }

      break;

    case ESPERA_2:

      pantalla2();
      rtc.setTime(segundos, minutos, hora, dia, mes, 2023);


      if ( estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = 0;
      }

      break;

  }
}


void pantalla1() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temperatura: ");
  display.println(event.temperature);
  display.println("Hora: ");
  display.print(hora);
  display.print(":");
  display.print(minutos);

  display.display();

}

void pantalla2() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(hora);
  display.print(":");
  display.print(minutos);

  display.display();

}
