#include <Arduino.h>
#include "Senzor.h"
#include "Buton.h"
#include "Potentiometru.h"
#include "Motor.h"
#include "Rainmaker.h"
#include "Types.h"

// Define the pin where the photodiode is connected
const int photodiodePin = 32; // ADC1 channel 6 on GPIO 34

void senzor_init() {
  // Initialize the ADC pin
  analogReadResolution(12); // Set resolution to 12-bit (0-4095)
  analogSetAttenuation(ADC_11db); // Set attenuation to 11dB for full scale voltage of 0 to 3.9V
}

void senzor_main() {
	static int prevVal = 0;

	int sensorValue = analogRead(photodiodePin);  // Read the analog value from channel A (0 - 4095)
	int currentVal = map(sensorValue, 0, 4095, 100, 0);  // Map the value to 100-0 for channel A

	if(prevVal != currentVal)
	{
		/* scapam de fluctuatii */
		if((AUTO == operationMode) && (abs(prevVal - currentVal) > 5))
		{
			pozitieJaluzea = currentVal;
	    	vitezaMotor = INCET;
			rainmaker_update_and_report(2);
			motor_update();
		}
	}

	prevVal = currentVal;
}
