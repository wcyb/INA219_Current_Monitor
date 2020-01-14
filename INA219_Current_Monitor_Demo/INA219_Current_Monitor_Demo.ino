/*
 Name:		INA219_Current_Monitor_Demo.ino
 Created:	31/12/2019 8:12:49 AM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/

#include "INA219_Current_Monitor.h"

INA219* ina = nullptr;

// the setup function runs once when you press reset or power the board
void setup() {
	Wire.setClock(400000);
	Wire.begin();
	Serial.begin(115200);
	
	//create a new instance of INA219 with parameters: max expected current: 1 A, shunt resistor: 0.1 Ohm, gain: /4 (+-160 mV which in this case allows for measuring max of 1.6 A)
	ina = new INA219(INA219_DEFAULT_ADDRESS, 1000, 100);

	Serial.println(F("...INA219 Current Monitor Demo..."));
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (!(millis() % 1000))//display measurements every second
	{
		double shunt = ina->getShuntVoltageValue();
		double bus = ina->getBusVoltageValue();
		double power = ina->getPowerValue();
		double current = ina->getCurrentValue();

		Serial.print(F("Shunt voltage [mV]: "));
		Serial.println(shunt, 2);

		Serial.print(F("Bus voltage [V]: "));
		Serial.println(bus, 2);

		Serial.print(F("Source voltage [V]: "));
		Serial.println(bus + (shunt / 1000.0), 2);//calculate source voltage by adding the shunt voltage to the bus voltage

		Serial.print(F("Power [mW]: "));
		Serial.println(power * 1000.0, 2);

		Serial.print(F("Current [mA]: "));
		Serial.println(current * 1000.0, 2);

		Serial.println();
	}
}
