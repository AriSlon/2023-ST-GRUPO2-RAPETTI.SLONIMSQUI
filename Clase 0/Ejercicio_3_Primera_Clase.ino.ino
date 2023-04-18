/*  
Seminario de Informatica y Telecomunicaciones
Clase 21/3 - Ejercicio 3

*/

#define LED_BOARD 25
#define PIN_SWITCH 35

void setup() {

pinMode(LED_BOARD, OUTPUT);
pinMode(PIN_SWITCH, INPUT_PULLUP);


}

void loop() {

digitalRead(PIN_SWITCH);

if(digitalRead(PIN_SWITCH) == 0){
 
digitalWrite(LED_BOARD, HIGH);

}

if(digitalRead(PIN_SWITCH) == 1){
 
digitalWrite(LED_BOARD, LOW);

}

}
