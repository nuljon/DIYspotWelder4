#include "ThermalControl.h"

long coolingSpeedMap(long input, long inmin, long inmax, long outmin, long outmax) //constrains input before mapping
{
	if (input <= inmin) return outmin;  // if input too low then return minimum range
	if (input >= inmax) return outmax;  // if input too high then returm maximum range
	return (input - inmin) * (outmax - outmin) / (inmax - inmin) + outmin; // return calculated output
}
bool ThermalControl::configured = false;

void ThermalControl::begin(void)
{
	pinMode(thermPin, INPUT);
	pinMode(fanPin, OUTPUT);
	pinMode(controlPin, OUTPUT);
	lcd.begin(16, 2);
	configured = loadThermalConfig(); // load config
	PRINT("\n\nwelder configured status: ", configured);// debug PRINT status
	if (!configured) {						// if not configured
		thermalControlData.version = 1;		// assign defaults
		thermalControlData.threshold = 120;
		thermalControlData.cutOff = 320;
		saveThermalConfig();
	}
	int size = sizeof(thermalControlStruct);
	PRINT("\nSize of Thermal Contral Data in EEprom: ", size); // debug PRINT
}

bool ThermalControl::loadThermalConfig(void) {
	EEPROM.readBlock(thermalConfigAddress, thermalControlData);
	return (thermalControlData.version == 1);
}
void ThermalControl::saveThermalConfig(void) {
	EEPROM.updateBlock(thermalConfigAddress, thermalControlData);
	PRINTS("\n\nsaving thermalConfig\n");
}

void ThermalControl::set(Button& decrementButton, Button& incrementButton, Button& weldButton) {
	static uint8_t currentField = 1;			// initialize var to track current field
	const char thresholdString[] = "start fan at";// put the string literal in memory
	const char *sp1 = thresholdString;			// declare a pointer to the address
	int size1 = sizeof(thresholdString);					// put the length in memory
	do {
		display(sp1, size1, thermalControlData.threshold);// send values to display function
		delay(500);								// allow some time for user to read value
		if (!decrementButton.read() && thermalControlData.threshold > 75)  // check -- buton
			--thermalControlData.threshold;		// -- if not too low
		else if (!incrementButton.read() && thermalControlData.threshold < 200) // check ++ button
			++thermalControlData.threshold;		// ++ if not too high
		if (weldButton.pressed()) {				// if weldButton was just pressed
			++currentField;		 				// bump field to next setting
			saveThermalConfig(); 				// save setting
		}
	} while (currentField == 1);

	const char cutOffString[] = "therm cutoff  "; // put the string literal in memory
	const char *sp2 = cutOffString;				 // declare a pointer to the address
	const int size2 = sizeof(cutOffString);		 // put the length in memory
	do {
		display(sp2, size2, thermalControlData.cutOff); // send values to display function
		delay(500);								// allow some time for user to read value
		if (decrementButton.pressed() && thermalControlData.cutOff > 100) // check button
			--thermalControlData.cutOff;		// -- if not too low
		else if (incrementButton.pressed() && thermalControlData.cutOff < 350) // check button
			++thermalControlData.cutOff;		// ++ if not too high
		if (weldButton.pressed()) {				// if weldButton was just pressed
			saveThermalConfig();				// save setting
			currentField = 1;					// reset the current field
		}
	} while (currentField == 2);
}

void ThermalControl::display(const char* str, const int x, int value) {
	lcd.setRGB(0, 255, 0);						// light the display - set mode is green
	lcd.clear();								// clear the screen
	lcd.setCursor(0, 0);						// from top left
	lcd.println(F(" CHANGE SETTING "));			// print title line
	PRINTS("\n\nCHANGE SETTING\n");				// debug PRINT echo output to serial
	lcd.setCursor(0, 1);						// on the next line
	for (uint8_t i = 0; i < x; ++i) {			// iterate the char array
		lcd.print(str[i]);						// print the char to lcd
		PRINT("", str[i]);						// debug PRINT echo char to serial
	}
	lcd.print(value);
	PRINT("", value);							// debug PRINT echo value to serial
}

// TODO break out fanspeed and thermal cutOff from method getTemperature
int ThermalControl::getTemperature(void)
{
	/*
	the temperature sensor is a voltage divider using a 1k thermistor (R1) and 10k resistor (R2)
	calculate the thermistor's resistance using the equation: R1=R2*(Vin / Vout - 1)
	Vin is the max possible analog reading (1023) and Vout is read at thermPin (temp)
	to solve for temperature use Steinhart B Parameter equation:
	1 / T = 1 / To + 1 / B * ln(R / Ro) where To is room temperature in Kelvin
	and B = 3986 for the 1k thermistor)
	*/
	float temp = 0;								// var to calc temperature
	do {
		temp = (float)analogRead(thermPin);			// read the voltage signal
		PRINT("\nsignal on thermPin: ", temp);	// debug PRINT signal to serial
		temp = 10000.0 * (1023.0 / temp - 1.0);		// R2 * (Vin / Vout - 1)
		PRINT("\n resistance: ", temp);			// debug PRINT calcd resistance
		PRINTS(" Ohms");						// debug PRINT units to serial
		float steinhart;						// we need another variable
		steinhart = temp / 1000.0;				// (R / Ro)
		steinhart = log(steinhart);				// ln(R / Ro)
		steinhart /= 3986.0;					// 1/B * ln(R / Ro)
		steinhart += 1.0 / (25.0 + 273.15);		// + (1 / To)
		steinhart = 1.0 / steinhart;			// Invert
		steinhart -= 273.15;					// convert to Celsius
		PRINT(" / ", steinhart);				// debug PRINT temperature
		PRINTS("*C");							// debug PRINT degrees celsius
		steinhart = (steinhart * 1.8) + 32.0;	// convert to farenheit
		PRINT(" / ", steinhart);				// debug PRINT temperature
		PRINTS("*F");							// in degrees farenheit
		temp = steinhart;						// update temp
		PRINT(" / ",temp);						// debug PRINT result to return
		fanSpeed = runFan(int(temp));			// set the fan speed
		if (temp >= thermalControlData.cutOff) {// if overheating
			digitalWrite(controlPin, LOW);		// turn off the weld current
			lcd.setRGB(255, 0, 0);				// alert: red screen
			delay(5000);						// wait 5 seconds to cool
			lcd.setRGB(0, 0, 255);				// revert screen color  
		}
	} while (temp >= thermalControlData.cutOff);// loop while overheated
	return (temp);								// return temperatur(farenheit)
}
uint8_t ThermalControl::runFan(int temp) {		// function to control fan
	if (temp < thermalControlData.threshold) {	// if temperature is below minimum threshold
		fanSpeed = 0;							// set fan speed to 0
		analogWrite(fanPin, fanSpeed);			// turn off the fan
	}
	else {										// set the fanspeed
		uint8_t(fanSpeed) = coolingSpeedMap(long(temp), long(thermalControlData.threshold), 
			long(thermalControlData.cutOff) - 10L, 50L, 255L); // proportional to temperature
		analogWrite(fanPin, fanSpeed);			// send PWM signal to transistor that drives fan
	}
	return (fanSpeed);							// send the temperature back to caller
}
void ThermalControl::display() {				// function displays status on lcd (temp and fan speed)
	lcd.setRGB(0, 0, 255);						// light the display - default is blue 0,0,255
	int temperature = getTemperature();			// get the temperature
	lcd.clear();								// clear the screen
	lcd.setCursor(0, 0);						// from top left
	lcd.println(F("Thermal Control"));			// print title line
	PRINTS("\nThermal Control");				// debug PRINT echo output to serial
	lcd.setCursor(0, 1);						// on the next line
	lcd.print(temperature);						// col 2 or 3	
	PRINT("\n", temperature);					// debug PRINT temperature to serial
	lcd.print(F("*F fan: "));					// col 9 or 10
	byte speed = map(fanSpeed, 0, 255, 0, 100); //convert to percent
	lcd.print(speed);							// col 13 max
	lcd.print("%");								// col 14
	PRINT(" *F fan: ", speed);					// debug PRINT fan speed to serial
}

ThermalControl thermalControl;					// instantiate the class as a global object
