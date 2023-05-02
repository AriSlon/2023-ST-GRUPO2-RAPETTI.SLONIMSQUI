#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 // Anchura OLED display en pixeles
#define SCREEN_HEIGHT 64 // Altura OLED display en pixeles

#define DHTPIN 23     // Pin digital conectado al sensor DHT
#define DHTTYPE    DHT11     // DHT 11

#define SW_1 34
#define SW_2 35

#define BOTtoken "6128870296:AAGyVnwhAqvWyJD2jCMDukpjcu15meeOXkI"  // your Bot Token (Get from Botfather)
#define CHAT_ID "842740065"

#define PANTALLA_1 0 //muestro temperatura y umbral 
#define ESPERA_1 1 //apretar y soltar botones
#define PANTALLA_2 2 // muestro umbral
#define SUMA 3 //sumo minutos
#define RESTA 4 //sumo horas
#define ESPERA_2  5

int estadoMaquina;

int estadoSwitch1;
int estadoSwitch2;

int hora;
int minutos;

int valorUmbral;

unsigned long milisActuales;
unsigned long milisPrevios;

float temp;

DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

// credenciales de wifi
const char* ssid = "ORT-IoT";
const char* password = "OrtIOTnew22$2";



WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


int botRequestDelay = 1000; /// intervalo
unsigned long lastTimeBotRan; /// ultimo tiempo

const int ledPin = 2; /// pin led
bool ledState = LOW;

// funcion que se comunica con telegram
void handleNewMessages(int numNewMessages) {
  Serial.println("Mensaje nuevo");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // inicio de verificacion
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {  ////si el id no corresponde da error . en caso de que no se quiera comprobar el id se debe sacar esta parte
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    ///fin de verificacion

    // imprime el msj recibido
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    /// si rebice /led on enciende el led
    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }

    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }

    if (text == "/state") {
      if (digitalRead(ledPin)) {
        bot.sendMessage(CHAT_ID, "LED is ON", "");
      }
      else {
        bot.sendMessage(CHAT_ID, "LED is OFF", "");
      }
    }
  }


}

void setup() {

  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);


  Serial.begin(115200);

  dht.begin();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  ///bloquea el programa si no se puede conectar a internet
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "Bot Hola mundo", "");
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

  maquinaDeEstados();

}

void maquinaDeEstados() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

  switch (estadoMaquina) {

    case PANTALLA_1:

      pantalla1();

      if (temp > valorUmbral) {
        bot.sendMessage(CHAT_ID, "Umbral de temperatura superado", "");

      } else {

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
