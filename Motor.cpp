#include <Arduino.h>
#include "Motor.h"
#include "Potentiometru.h"
#include "Types.h"
#include <L298N.h>

const int ledChannel = 0; // Use LEDC channel 0
const int freq = 30000;    // Frequency in Hz
const int resolution = 8; // 8 bits resolution, range 0-255
const int enB = 27; // Enable pin for PWM on GPIO 27
const int in3 = 16; // Direction pin 1 on GPIO 16
const int in4 = 4; // Direction pin 2 on GPIO 4
const int c1  = 21;
const int c2  = 19;

volatile int Encodervalue = 0;
volatile int pozitieEncoder = 0;
volatile int directieMotor = 1;

void updateEncoder()
{
	if (digitalRead(c1) > digitalRead(c2))
		Encodervalue--;
	else
		Encodervalue++;

	pozitieEncoder = map(Encodervalue, 0, 1100, 0, 100);

	if(pozitieEncoder == pozitieJaluzea)
	{
		(1 == directieMotor) ? Encodervalue -= 5 : Encodervalue += 5;

		ledcWrite(ledChannel, 0);
		digitalWrite(in3, LOW);
		digitalWrite(in4, LOW);
	}
}

void motor_init()
{
  pinMode(c1, INPUT);
  pinMode(c2, INPUT);

  attachInterrupt(digitalPinToInterrupt(c1), updateEncoder, RISING);

	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
  

	pinMode(enB, OUTPUT);
	ledcSetup(ledChannel, freq, resolution);
	ledcAttachPin(enB, ledChannel);
	ledcWrite(ledChannel, 0);
}

void motor_main()
{
}

void motor_update()
{
	static int prevPosition = 0;

	int currentPosition = pozitieJaluzea;

	Serial.print("Viteza Motor: ");

	switch (vitezaMotor)
	{
	case INCET:
		Serial.println("INCET");
		ledcWrite(ledChannel, 210);
		break;
	case RAPID:
		Serial.println("RAPID");
		ledcWrite(ledChannel, 255);
		break;
	default:
		break;
	}

	Serial.print("Pozitie Jaluzea: ");
	Serial.println(pozitieJaluzea);

	if(prevPosition < currentPosition)
	{
		directieMotor = 1;
		digitalWrite(in3, LOW);
		digitalWrite(in4, HIGH);
	}
	else if(prevPosition > currentPosition)
	{
		directieMotor = 2;
		digitalWrite(in3, HIGH);
		digitalWrite(in4, LOW);
	}

	prevPosition = currentPosition;
}
