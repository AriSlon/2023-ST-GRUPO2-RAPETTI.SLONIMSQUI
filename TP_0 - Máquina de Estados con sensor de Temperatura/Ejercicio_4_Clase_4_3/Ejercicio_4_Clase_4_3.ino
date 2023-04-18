/*
  Seminario de Informatica y Telecomunicaciones
  Clase 28/3 - Ejercicio 4

  Autores: Rapetti - Slonimsqui

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

#define RELAY 18
#define SW_1 34
#define SW_2 35

#define PANTALLA_1 0 //muestro temperatura y umbral 
#define ESPERA_1 1 //apretar y soltar botones
#define PANTALLA_2 2 // muestro umbral
#define SUMA 3 //sumo al umbral
#define RESTA 4
#define ESPERA_2  5

int estadoMaquina;

int estadoSwitch1;
int estadoSwitch2;

int valorUmbral = 30;

float temp;

unsigned long milisActuales;
unsigned long milisPrevios;


DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

void setup() {

  pinMode(RELAY, OUTPUT);
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

  if ((milisActuales - milisPrevios) >= 100) {
  
    maquinaDeEstados();
    milisPrevios = milisActuales;

  }

}

  void maquinaDeEstados() {

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    temp = event.temperature;

    switch (estadoMaquina) {

      case PANTALLA_1:

        pantalla1();

        if (temp > valorUmbral) {
          digitalWrite(RELAY, HIGH);
        } else {
          digitalWrite(RELAY, LOW);
        }


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

      case SUMA:

        pantalla2();


        if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
          estadoMaquina = 5;
        }

        if (estadoSwitch1 == HIGH) {
          valorUmbral = valorUmbral + 1;

          estadoMaquina = 2;
        }


        break;

      case RESTA:

        pantalla2();

        if (estadoSwitch2 == HIGH) {
          valorUmbral = valorUmbral - 1;

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
    display.println("Valor Umbral: ");
    display.println(valorUmbral);
    display.display();
  }

  void pantalla2() {

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Valor Umbral: ");
    display.println(valorUmbral);
    display.display();

  }
