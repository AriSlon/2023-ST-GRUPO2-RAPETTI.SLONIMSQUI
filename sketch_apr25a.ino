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

#define LED 2

#define PANTALLA_1 0 //muestro temperatura y umbral 
#define ESPERA_1 1 //apretar y soltar botones
#define PANTALLA_2 2 // muestro umbral
#define SUMA_CICLO 3 //sumo minutos
#define RESTA_CICLO 4 //sumo horas
#define ESPERA_2  5
#define PANTALLA_3 6
#define SUMA_INTERVALO 7 //sumo minutos
#define RESTA_INTERVALO 8 //sumo horas
#define ESPERA_3 9


#define CICLO_RESTANDO 0
#define ACTIVACIÓN 1
#define DESACTIVACIÓN 0

int estadoMaquina;
int estadoMaquina2;


int estadoSwitch1;
int estadoSwitch2;

int hora;
int minutos;

int intervaloSegs = 30;
int intervaloSegsSeteado;
int cicloMinSeteado = 1;
int cicloMin;


unsigned long milisActualesCiclo;
unsigned long milisPreviosCiclo;

unsigned long milisActualesIntervalo;
unsigned long milisPreviosIntervalo;

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

  milisActualesCiclo = millis(); // Guarda el tiempo en milisegundos desde que se inicio el programa
  milisActualesIntervalo = millis(); // Guarda el tiempo en milisegundos desde que se inicio el programa

  maquinaDeEstados();

  //horario();
   
  Serial.println(estadoMaquina2);

}

void maquinaDeEstados() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  switch (estadoMaquina) {

    case PANTALLA_1:

      pantalla1();

      maquinaDeEstado2();

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

    case SUMA_CICLO:

      pantalla2();


      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      if (estadoSwitch2 == HIGH) {
        cicloMinSeteado = cicloMinSeteado + 1;

        estadoMaquina = 2;
      }


      break;

    case RESTA_CICLO:

      pantalla2();

      if (estadoSwitch1 == HIGH) {
        cicloMinSeteado = cicloMinSeteado - 1;

        estadoMaquina = 2;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 5;
      }

      break;

    case ESPERA_2:

      pantalla2();

      if ( estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = 6;
      }

      break;



    case PANTALLA_3:

      pantalla3();

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 9;
      }

      if (estadoSwitch2 == LOW) {

        estadoMaquina = 8;
      }

      if (estadoSwitch1 == LOW) {

        estadoMaquina = 7;
      }

      break;

    case SUMA_INTERVALO:

      pantalla3();


      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 9;
      }

      if (estadoSwitch2 == HIGH) {
        intervaloSegsSeteado = intervaloSegsSeteado + 30;

        estadoMaquina = 6;
      }


      break;

    case RESTA_INTERVALO:

      pantalla3();

      if (estadoSwitch1 == HIGH) {
        intervaloSegsSeteado = intervaloSegsSeteado - 30;

        estadoMaquina = 6;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = 9;
      }

      break;

    case ESPERA_3:

      pantalla3();

      if ( estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = 0;
      }

      break;

  }
}

//Maquina de Estado para pasar

void maquinaDeEstado2() {

  switch (estadoMaquina2) {

    case 0:

      if ((milisActualesCiclo - milisPreviosCiclo) >= 60000) {
        cicloMin = cicloMin + 1;
        milisPreviosCiclo = milisActualesCiclo;

      }

      if (cicloMin == cicloMinSeteado) {
        estadoMaquina2 = 1;
      }

      break;

    case 1:

      digitalWrite(LED, HIGH);
      if ((milisActualesIntervalo - milisPreviosIntervalo) >= 1000) {
        intervaloSegs = intervaloSegs + 1;
        milisPreviosIntervalo = milisActualesIntervalo;
      }

      if (intervaloSegs == intervaloSegsSeteado) {
        estadoMaquina2 = 2;
      }


      break;

    case 2:

      digitalWrite(LED, LOW);
      cicloMin = 0;
      intervaloSegs = 0;

      estadoMaquina2 = 0;

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
  display.print("Intervalo: ");
  display.println(intervaloSegsSeteado);
  //display.println("Temperatura Actual: ");
  //display.println(event.temperature);
  display.print("Ciclo: ");
  display.println(cicloMin);
  display.display();

}

void pantalla3() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Intervalo: ");
  display.print(intervaloSegsSeteado);
  display.display();
}

void pantalla2() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Ciclo: ");
  display.print(cicloMin);
  display.display();
}

void horario() {
//  if (millis() - currentMillis >= 1000) {
//    seg += 1 ;
//    currentMillis = millis();
  }
