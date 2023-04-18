/*
  Seminario de Informatica y Telecomunicaciones
  Clase 11/4 - TP Maquina de Estados Reloj - Ejercicio 1

  Hecho por Santiago Rapetti y Ariel Slonimsqui.

  Comentarios: La hora esta inicializada en 0 horas y 0 minutos.
*/

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

#define PANTALLA_1 0 //muestro temperatura y umbral 
#define ESPERA_1 1 //apretar y soltar botones
#define PANTALLA_2 2 // muestro umbral
#define SUMA_MINUTOS 3 //sumo minutos
#define SUMA_HORA 4 //sumo horas
#define ESPERA_2  5

int estadoMaquina;

int estadoSwitch1;
int estadoSwitch2;

int hora;
int minutos;

unsigned long milisActuales;
unsigned long milisPrevios;

float temp;

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

}

void loop() {

  estadoSwitch1 = digitalRead(SW_1);
  estadoSwitch2 = digitalRead(SW_2);

  Serial.println(estadoMaquina);

  milisActuales = millis(); // Guarda el tiempo en milisegundos desde que se inicio el programa

  if ((milisActuales - milisPrevios) >= 60000) {
    minutos = minutos + 1;
    milisPrevios = milisActuales;

  }

  if ((milisActuales - milisPrevios) >= 3600000) {
    hora = hora + 1;
    milisPrevios = milisActuales;
  }

  if (minutos > 59) {
    minutos = 0;
    hora = hora + 1;
  }

  if (hora > 23) {
    hora = 0;

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

      if (estadoSwitch1 == LOW) {

        estadoMaquina = 3;
      }

      if (estadoSwitch2 == LOW) {

        estadoMaquina = 4;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      break;

    case SUMA_MINUTOS:

      pantalla2();


      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      if (estadoSwitch1 == HIGH) {
        minutos = minutos + 1;

        estadoMaquina = 2;
      }


      break;

    case SUMA_HORA:

      pantalla2();

      if (estadoSwitch2 == HIGH) {
        hora = hora + 1;

        estadoMaquina = 2;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      break;

    case ESPERA_2:

      pantalla2();

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
  display.println("Temperatura Actual: ");
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
  display.println("Hora: ");
  display.print(hora);
  display.print(":");
  display.print(minutos);
  display.display();
}
