/*  
Seminario de Informatica y Telecomunicaciones
Clase 21/3 - Ejercicio 2

*/

#define LED_BOARD 25

void setup() {

pinMode(LED_BOARD, OUTPUT);

}

void loop() {

digitalWrite(LED_BOARD, HIGH);
delay(1000);
digitalWrite(LED_BOARD, LOW);
delay(1000);

}
