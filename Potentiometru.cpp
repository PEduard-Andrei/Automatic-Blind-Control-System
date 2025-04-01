#include <Arduino.h>
#include "Potentiometru.h"
#include "Rainmaker.h"
#include "Motor.h"

const int potPinA = 34;  // Channel A

int pozitieJaluzea = 0; /* 0 - inseamna inchisa */
jaluzea_viteza_motor vitezaMotor = INCET; /* 0,1 - incet, rapid */

void potentiometru_init()
{
	analogReadResolution(12);  // Set the resolution of analog read to 12 bits for ESP32
}

void potentiometru_main()
{
	static int prevVal = 0;

	int potValueA = analogRead(potPinA);  // Read the analog value from channel A (0 - 4095)
	int currentVal = map(potValueA, 0, 4095, 0, 100);  // Map the value to 0 - 100 for channel A

	if(prevVal != currentVal)
	{
		/* scapam de fluctuatii */
		if(abs(prevVal - currentVal) > 2)
		{
			if(abs(prevVal - currentVal) < 40 ) 	 {vitezaMotor = INCET;}
			else if(abs(prevVal - currentVal) >= 40) {vitezaMotor = RAPID;}

			pozitieJaluzea = currentVal;
			rainmaker_update_and_report(2);
			motor_update();
		}
	}

	prevVal = currentVal;
}
