/*
  Seminario de Informatica y Telecomunicaciones
  Clase 30/5 - TP 5 Firebase
  Hecho por Santiago Rapetti y Ariel Slonimsqui.

*/

#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <ESP32Time.h>
#include "time.h"
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DHT_U.h>

#define SCREEN_WIDTH 128 // Anchura OLED display en pixeles
#define SCREEN_HEIGHT 64 // Altura OLED display en pixeles

#define DHTPIN 23     // Pin digital conectado al sensor DHT
#define DHTTYPE    DHT11     // DHT 11

#define SW_1 34
#define SW_2 35

#define PANTALLA_1 0
#define ESPERA_1 1
#define PANTALLA_2 2
#define SUMA_CICLO 3
#define RESTA_CICLO 4
#define ESPERA_2 5

#include "addons/TokenHelper.h" // Provide the token generation process info.
#include "addons/RTDBHelper.h" // Provide the RTDB payload printing info and other helper functions.

#define WIFI_SSID "ORT-IoT"
#define WIFI_PASSWORD "OrtIOTnew22$2"

#define API_KEY "AIzaSyCFqGtRI-i6us4ldknJVphgCKA-LFwKw_k" // Insert Firebase project API Key

#define USER_EMAIL "santyrap2005@gmail.com" // Insert Authorized Email and Corresponding Password
#define USER_PASSWORD "santiago"

#define DATABASE_URL "https://tp-5-st-default-rtdb.firebaseio.com/" // Insert RTDB URLefine the RTDB URL

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

FirebaseJson json;

WiFiUDP ntpUDP;// Define NTP Client to get time
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String uid;// Variable to save USER UID

String databasePath;// Database main path (to be updated in setup with the user UID)

String tempPath = "/temperature"; // Database child nodes
String humPath = "/humidity";
String presPath = "/pressure";
String timePath = "/timestamp";

String parentPath;// Parent Node (to be updated in every loop)

int timestamp; // Variable to save current epoch time

int estadoMaquina;
int estadoMaquinaCiclo;

int estadoSwitch1;
int estadoSwitch2;

int cicloSegundos = 30;
int timer;

int valorUmbral = 27;

float temp;
float hum;

unsigned long milisActuales;
unsigned long milisPrevios;

float temperature;
float humidity;
float pressure;

unsigned long sendDataPrevMillis = 0;

DHT_Unified dht(DHTPIN, DHTTYPE);


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

void setup() {

  Serial.begin(115200);

  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);

  initWiFi();
  timeClient.begin();

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  config.api_key = API_KEY; // Assign the api key (required)

  auth.user.email = USER_EMAIL; // Assign the user sign in credentials
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL; // Assign the RTDB URL (required)

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  config.token_status_callback = tokenStatusCallback;   // Assign the callback function for the long running token generation task /see addons/TokenHelper.h

  config.max_token_generation_retry = 5; // Assign the maximum retry of token generation

  Firebase.begin(&config, &auth); // Initialize the library with the Firebase authen and config

  Serial.println("Getting User UID"); // Getting the user UID might take a few seconds
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }

  uid = auth.token.uid.c_str();  // Print user UID
  Serial.print("User UID: ");
  Serial.println(uid);

  databasePath = "/UsersData/" + uid + "/readings";  // Update database path
}

void loop() {

  Serial.println(estadoMaquina);

  estadoSwitch1 = digitalRead(SW_1);
  estadoSwitch2 = digitalRead(SW_2);


  milisActuales = millis(); // Guarda el tiempo en milisegundos desde que se inicio el programa

  if ((milisActuales - milisPrevios) >= 1000) {

    timer = timer + 1;

    milisPrevios = milisActuales;

  }

  if (timer >= cicloSegundos) {
    timer = 0;

  }

  if (cicloSegundos < 0) {
    cicloSegundos = 0;
  }



  maquinaDeEstados ();
  nuevosDatosFirebase();

}

void initWiFi() { // Initialize WiFi

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();

}

unsigned long getTime() { // Function that gets current epoch time

  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

void nuevosDatosFirebase () {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  dht.humidity().getEvent(&event);
  hum = event.relative_humidity;

  if (Firebase.ready() && (millis() - sendDataPrevMillis > (cicloSegundos * 1000) || sendDataPrevMillis == 0)) {  // Send new readings to database

    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath = databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(temp));
    json.set(humPath.c_str(), String(hum));
    // json.set(presPath.c_str(), "33"); el dht 11 no mide presion
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }

}

void maquinaDeEstados () {

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

      if (estadoSwitch1 == LOW) {

        estadoMaquina = SUMA_CICLO;
      }

      if (estadoSwitch2 == LOW) {

        estadoMaquina = RESTA_CICLO;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_2;
      }

      break;

    case SUMA_CICLO:

      pantalla2();

      if (estadoSwitch1 == HIGH) {
        cicloSegundos = cicloSegundos + 30;

        estadoMaquina = PANTALLA_2;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_2;
      }

      break;

    case RESTA_CICLO:

      pantalla2();

      if (estadoSwitch2 == HIGH) {
        cicloSegundos = cicloSegundos - 30;

        estadoMaquina = PANTALLA_2;
      }

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_2;
      }

      break;

    case ESPERA_2:

      pantalla2();

      if ( estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = PANTALLA_1;
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
  display.print("Valor Umbral: ");
  display.println(valorUmbral);
  display.display();
}

void pantalla2() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Ciclo: ");
  display.print(cicloSegundos);
  display.print(" seg/s");
  display.display();

}
