/*
  Seminario de Informatica y Telecomunicaciones
  Clase 25/4 - TP Opcional Riego
  Hecho por Santiago Rapetti y Ariel Slonimsqui.
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
#define LED 25

#define PANTALLA_1 0 //muestro temperatura y umbral 
#define ESPERA_1 1 //apretar y soltar botones
#define PANTALLA_2 2 // muestro umbral
#define SUMA_TIEMPO_CICLO 3 //sumo minutos
#define RESTA_TIEMPO_CICLO 4 //sumo horas
#define ESPERA_2  5
#define PANTALLA_3 6
#define SUMA_TIEMPO_RIEGO 7 //sumo minutos
#define RESTA_TIEMPO_RIEGO 8 //sumo horas
#define ESPERA_3  9


int estadoMaquina;
int estadoMaquinaRiego;

int estadoSwitch1;
int estadoSwitch2;

int hora;
int minutos;

int tiempoRiego = 30;
int intervaloRiego = 60;

unsigned long milisActuales;
unsigned long milisPrevios;

int timer;

float temp;

DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

void setup() {

  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);
  pinMode(LED, OUTPUT);



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

  if ((milisActuales - milisPrevios) >= 1000) {

    timer = timer + 1;

    milisPrevios = milisActuales;

  }
  maquinaDeEstadosRiego ();

  maquinaDeEstados();

  if (intervaloRiego < 60) {
    intervaloRiego = 60;
  }

  if (tiempoRiego < 30) {
    tiempoRiego = 30;
  }


}

void maquinaDeEstados() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  switch (estadoMaquina) {

    case PANTALLA_1:

      pantalla1();



      if (estadoSwitch1 == LOW &&  estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_1;
      }

      break;

    case ESPERA_1:

      pantalla1();

      if (estadoSwitch1 == HIGH &&  estadoSwitch2 == HIGH) {
        estadoMaquina = PANTALLA_2;
      }

      break;

    case PANTALLA_2:

      pantalla2();

      if (estadoSwitch2 == LOW) {

        estadoMaquina = SUMA_TIEMPO_CICLO;
      }

      if (estadoSwitch1 == LOW) {

        estadoMaquina = RESTA_TIEMPO_CICLO;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_2;
      }

      break;

    case SUMA_TIEMPO_CICLO:

      pantalla2();


      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_2;
      }

      if (estadoSwitch2 == HIGH) {
        intervaloRiego = intervaloRiego + 60;

        estadoMaquina = PANTALLA_2;
      }


      break;

    case RESTA_TIEMPO_CICLO:

      pantalla2();

      if (estadoSwitch1 == HIGH) {
        intervaloRiego = intervaloRiego - 60;

        estadoMaquina = PANTALLA_2;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_2;
      }

      break;

    case ESPERA_2:

      pantalla2();

      if ( estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = PANTALLA_3;
      }

      break;

    case PANTALLA_3:

      pantalla3();

      if (estadoSwitch2 == LOW) {

        estadoMaquina = SUMA_TIEMPO_RIEGO;
      }

      if (estadoSwitch1 == LOW) {

        estadoMaquina = RESTA_TIEMPO_RIEGO;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_3;
      }

      break;

    case SUMA_TIEMPO_RIEGO:

      pantalla3();


      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_3;
      }

      if (estadoSwitch2 == HIGH) {
        tiempoRiego = tiempoRiego + 30;

        estadoMaquina = PANTALLA_3;
      }


      break;

    case RESTA_TIEMPO_RIEGO:

      pantalla3();

      if (estadoSwitch1 == HIGH) {
        tiempoRiego = tiempoRiego - 30;

        estadoMaquina = PANTALLA_3;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_3;
      }

      break;

    case ESPERA_3:

      pantalla3();

      if ( estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = PANTALLA_1;
      }

      break;

  }
}

void maquinaDeEstadosRiego () {

  switch (estadoMaquinaRiego) {

    case 0:

      if (timer >= intervaloRiego) {
        digitalWrite(LED, HIGH);
        timer = 0;

        estadoMaquinaRiego = 1;
      }

      break;

    case 1:

      if (timer >= tiempoRiego) {
        digitalWrite(LED, LOW);
        timer = 0;

        estadoMaquinaRiego = 0;

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
  display.println();
  display.print("Riego: ");

  if (digitalRead(LED) == HIGH) {
    display.println("Activado");
  }
  if (digitalRead(LED) == LOW) {
    display.println("Desactivado");
  }

  display.println();
  display.print("Ciclo: ");
  display.print(intervaloRiego);
  display.display();

}

void pantalla2() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Ciclo: ");
  display.print(intervaloRiego / 60);
  display.println(" minutos");
  display.display();
}

void pantalla3() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Tiempo de Riego: ");
  display.print(tiempoRiego);
  display.display();
}
