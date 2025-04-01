#include "Buton.h"
#include "Potentiometru.h"
#include "Senzor.h"
#include "Motor.h"
#include "Rainmaker.h"

void setup() {
  Serial.begin(115200);
  
  buton_init();
  potentiometru_init();
  senzor_init();
  motor_init();
  rainmaker_init();
}

void loop() {
  button_main();
  potentiometru_main();
  senzor_main();
  motor_main();
  rainmaker_main();

  delay(500);
}
