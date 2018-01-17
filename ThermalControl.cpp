#include "ThermalControl.h"

long coolingSpeedMap(long input, long inmin, long inmax, long outmin, long outmax) //constrains input before mapping
{
	if (input <= inmin) return outmin;  // if input too low then return minimum range
	if (input >= inmax) return outmax;  // if input too high then returm maximum range
	return (input - inmin) * (outmax - outmin) / (inmax - inmin) + outmin; // return calculated output
}

void ThermalControl::begin(void)
{
	pinMode(thermPin, INPUT);
	pinMode(fanPin, OUTPUT);
	pinMode(welderPin, OUTPUT);
	lcd.begin(16, 2);
	configured = loadThermalConfig();
	if (!configured) {						// assign defaults
		thermalControlData.version = 4;
		thermalControlData.activationThreshold = 150;
		thermalControlData.cutOff = 350;
		saveThermalConfig();
	}
}

bool ThermalControl::loadThermalConfig(void) {
	EEPROM.readBlock(thermalConfigAddress, thermalControlData);
	return (thermalControlData.version == 4);
}
void ThermalControl::saveThermalConfig(void) {
	EEPROM.writeBlock(thermalConfigAddress, thermalControlData);
}

void ThermalControl::program(void)
{

}

int ThermalControl::getTemperature(void)
{
	float temp = 0.0;
	do {
		temp = analogRead(thermPin); // read the voltage (0 to 1023 = 0 to 5v)
		//Serial << "analog: " << temp;  // print the value
		// convert the value to the resistance of the thermistor (R1)
		// R1 = R2 * (Vin / Vout - 1)
		temp = 1023.0 / temp - 1; // (Vin / Vout - 1)
		temp = 1000.0 * temp; // R2 * (Vin / Vout -1)
		// Serial << " resistance: " << temp << " Ohms"; //print the resistance
		// to solve for temperature use steinhart B parameter equation
		// 1 / T = 1 / To + 1 / B * ln(R / Ro) where To is room temperature in Kelvin
		// and B = 3986 for the 1k thermistor (marked 102)
		float steinhart; // we need another variable
		steinhart = temp / 1000.0; // (R / Ro)
		steinhart = log(steinhart); // ln(R / Ro)
		steinhart /= 3986.0; // 1/B * ln(R / Ro)
		steinhart += 1.0 / (25.0 + 273.15); // + (1 / To)
		steinhart = 1.0 / steinhart; // Invert
		steinhart -= 273.15; // convert to Celsius
		//Serial << " / " << steinhart << "*C"; //print temperature in degrees celsius
		steinhart = (steinhart * 1.8) + 32.0; // convert to farenheit
		//Serial << " / " << steinhart << "*F"; // print temperature in degrees farenheit
		temp = steinhart;
		//Serial << " / " << temp << endl; // print the result we will return
		fanSpeed = runFan(int(temp));
		if (temp >= thermalControlData.cutOff) {
			digitalWrite(welderPin, LOW);
			lcd.setRGB(255, 0, 0); // alert: red screen
			delay(5000);	// wait 5 seconds for fan to cool
			lcd.setRGB(0, 0, 255);	// revert screen color to default 
		}
	} while (temp >= thermalControlData.cutOff);
	return (temp); // return the temperature (farenheit)
}
uint8_t ThermalControl::runFan(int temp) {
	if (temp < thermalControlData.activationThreshold) {			// if temperature is below minimum threshold
		fanSpeed = 0;							// set fan speed to 0
		analogWrite(fanPin, fanSpeed);			// turn off the fan
	}
	else {
		uint8_t(fanSpeed) = coolingSpeedMap(long(temp), long(thermalControlData.activationThreshold), 
			long(thermalControlData.cutOff) - 50L, 50L, 255L); // fanSpeed increases with temperature
		analogWrite(fanPin, fanSpeed);			// write fanSpeed as PWM signal to NPN that drives fan
	}
	return (fanSpeed);
}
void ThermalControl::display() {
	int temperature = getTemperature();
	lcd.setRGB(0, 0, 255);				// light the display - default is blue 0,0,255
	lcd.clear();						// clear the screen
	lcd.setCursor(0, 0);				// from top left
	lcd.println("Thermal Control");		// print title line
	lcd.setCursor(0, 1);				// on the next line
	lcd.print(temperature);				// col 2 or 3	
	lcd.print("*F fan:");				// col 9 or 10
	byte speed = map(fanSpeed, 0, 255, 0, 100); //convert to percent
	lcd.print(speed);				//col 13 max
	lcd.print("%");					// col 14
}
ThermalControl thermalControl;