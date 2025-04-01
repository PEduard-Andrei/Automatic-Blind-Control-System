#include <Arduino.h>
#include "Buton.h"
#include "Rainmaker.h"

// Define the GPIO pin connected to the touch sensor
const int touchPin = 15;

jaluzea_operation_mode operationMode = MANUAL;

void buton_init()
{
	pinMode(touchPin, INPUT);
}

void button_main()
{
	static int prevState = 0;

	int currentState = digitalRead(touchPin);

	if(prevState != currentState)
	{
		if(currentState == 0)
		{
			operationMode = MANUAL;
			Serial.println("Senzor mod operare : MANUAL");
		}
		else
		{
			operationMode = AUTO;
			Serial.println("Senzor mod operare : AUTO");
		}

		rainmaker_update_and_report(1);
	}

	prevState = currentState;
}
