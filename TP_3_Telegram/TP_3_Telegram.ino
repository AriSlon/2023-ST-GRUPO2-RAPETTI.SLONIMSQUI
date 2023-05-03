#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 // Anchura OLED display en pixeles
#define SCREEN_HEIGHT 64 // Altura OLED display en pixeles

#define DHTPIN 23     // Pin digital conectado al sensor DHT
#define DHTTYPE    DHT11     // DHT 1

#define SW_1 34
#define SW_2 35

#define PANTALLA_1 0
#define ESPERA_1 1
#define ESPERA_2 2
#define ESPERA_3 3
#define ESPERA_4 4
#define ESPERA_5 5
#define PANTALLA_2 6
#define SUMA_UMBRAL 7
#define RESTA_UMBRAL 8
#define ESPERA_6 9

#define BOTtoken "6128870296:AAGyVnwhAqvWyJD2jCMDukpjcu15meeOXkI"  // cambiar por token 

#define CHAT_ID "5939693005"

const char* ssid = "Ari";
const char* password = "004367225aa";

int estadoMaquina;

int estadoSwitch1;
int estadoSwitch2;

int valorUmbral = 22;

String mensaje = "La temperatura actual es: ";


int botRequestDelay = 1000; /// intervalo
unsigned long lastTimeBotRan; /// ultimo tiempo

float temp;

unsigned long milisActuales;
unsigned long milisPrevios;

unsigned long milisActuales2;
unsigned long milisPrevios2;

boolean flagTemperatura;

DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {

  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);


  Serial.begin(115200);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "Conexion establecida entre el Bot y el microcontrolador", "");
}


void loop() {

  Serial.println(estadoMaquina);

  estadoSwitch1 = digitalRead(SW_1);
  estadoSwitch2 = digitalRead(SW_2);

  milisActuales = millis();
  milisActuales2 = millis();


  if((milisActuales2 - milisPrevios2) >= 1000){
    
    lecturaTempreatura();
    milisPrevios2 = milisActuales2;
    
  }

  if (valorUmbral < 0) {
    valorUmbral = 0;
  }

  if (flagTemperatura == 0) {

    if (temp > valorUmbral) {

      flagTemperatura = 1;

      bot.sendMessage(CHAT_ID, "La temperatura supero el valor umbral!!!", "");
    }

  }

  if (flagTemperatura == 1) {


    if (temp < valorUmbral) {

      flagTemperatura = 0;

      bot.sendMessage(CHAT_ID, "La temperatura es menor al valor umbral", "");


    }

  }

  lecturaTiempoBot();

  maquinaDeEstados();

}

void maquinaDeEstados() {

  switch (estadoMaquina) {


    case PANTALLA_1:

      pantalla1();

      if (estadoSwitch1 == LOW) {
        estadoMaquina = ESPERA_1;
      }

      break;

    case ESPERA_1:

      pantalla1();

      if (estadoSwitch1 == HIGH) {
        estadoMaquina = ESPERA_2;
      }

      break;

    case ESPERA_2:

      pantalla1();

      if (estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_3;
      }

      break;

    case ESPERA_3:

      pantalla1();

      if (estadoSwitch2 == HIGH) {
        milisPrevios = milisActuales;
        estadoMaquina = ESPERA_4;
      }

      break;

    case ESPERA_4:

      pantalla1();

      if (estadoSwitch1 == LOW) {
        estadoMaquina = ESPERA_5;
      }

      if ((milisActuales - milisPrevios) >= 5000) {
        milisPrevios = milisActuales;
        estadoMaquina = PANTALLA_1;
      }

      break;

    case ESPERA_5:

      pantalla1();

      if (estadoSwitch1 == HIGH) {
        estadoMaquina = PANTALLA_2;
      }

      break;

    case PANTALLA_2:

      pantalla2();

      if (estadoSwitch1 == LOW) {
        estadoMaquina = SUMA_UMBRAL;
      }

      if (estadoSwitch2 == LOW) {
        estadoMaquina = RESTA_UMBRAL;
      }

      if (estadoSwitch1 == LOW &&  estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_6;
      }

      break;

    case SUMA_UMBRAL:

      pantalla2();

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_6;
      }

      if (estadoSwitch1 == HIGH) {
        valorUmbral = valorUmbral + 1;

        estadoMaquina = PANTALLA_2;
      }

      break;

    case RESTA_UMBRAL:

      pantalla2();

      if (estadoSwitch1 == LOW && estadoSwitch2 == LOW) {
        estadoMaquina = ESPERA_6;
      }

      if (estadoSwitch2 == HIGH) {
        valorUmbral = valorUmbral - 1;

        estadoMaquina = PANTALLA_2;
      }

      break;

    case ESPERA_6:

      pantalla2();

      if (estadoSwitch1 == HIGH && estadoSwitch2 == HIGH) {
        estadoMaquina = PANTALLA_1;
      }

      break;

  }

}

void lecturaTiempoBot () {

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("Veo los msj nuevos");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();

  }


}

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
    if (text == "/temperatura_actual") {
      bot.sendMessage(chat_id, (String)temp, "");
    }

  }

}

void pantalla1() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temperatura Actual: ");
  display.println(temp);
  display.println("Valor Umbral: ");
  display.print(valorUmbral);
  display.display();
}

void pantalla2() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Valor Umbral: ");
  display.println(valorUmbral);
  display.display();

}

void lecturaTempreatura() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

}
