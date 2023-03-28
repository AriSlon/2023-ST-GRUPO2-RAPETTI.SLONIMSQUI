/*
  Seminario de Informatica y Telecomunicaciones
  Clase 28/3 - Ejercicio 4

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
#define SW_1 35
#define SW_2 34

#define PANTALLA_1 0
#define ESPERA_2 1 2
#define ESPERA+ 0
#define PANTALLA_2 3
#define ESPERA- 1
#define ESPERA_1 4


int estadoMaquina;
int estadoSwitch1;
int estadoSwitch2;


float valorUmbral = 30;
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

  temp = event.temperature;

  estadoSwitch1 = digitalRead(SW_1);
  estadoSwitch2 = digitalRead(SW_2);



  switch (estadoMaquina) {

    case 0:

      display.clearDisplay();

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Temperatura Actual: ");
      display.println(event.temperature);
      display.display();

      display.println("Valor Umbral: ");
      display.println(valorUmbral);
      display.display();

      if (estadoSwitch1 == LOW &&  estadoSwitch2 == LOW) {
        estadoMaquina = 1;
      }

break;

    case 1:

      display.clearDisplay();

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Temperatura Actual: ");
      display.println(event.temperature);
      display.display();

      display.println("Valor Umbral: ");
      display.println(valorUmbral);
      display.display();

      if (estadoSwitch1 == HIGH &&  estadoSwitch2 == HIGH) {
        estadoMaquina = 2;
      }



break;

    case 2:

      display.println("Valor Umbral: ");
      display.println(valorUmbral);
      display.display();

      if (estadoSwitch1 == LOW) {

        estadoMaquina = 4;
      }


      if (estadoSwitch2 == LOW) {

        estadoMaquina = 4;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

break;

    case 3:

      display.println("Valor Umbral: ");
      display.println(valorUmbral);
      display.display();

      if (estadoSwitch1 == HIGH) {
        valorUmbral = valorUmbral ++;

        estadoMaquina = 3;
      }

break;

    case 4:

      display.println("Valor Umbral: ");
      display.println(valorUmbral);
      display.display();

      if (estadoSwitch2 == HIGH) {
        valorUmbral = valorUmbral - 1;

        estadoMaquina = 3;
      }

break;

    case 5:

      display.println("Valor Umbral: ");
      display.println(valorUmbral);
      display.display();

      if ( estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = 0;
      }

      break;

  }



}
