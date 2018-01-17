#include <EEPROMVar.h>
#include <EEPROMex.h>
#include <Wire.h>		// I2C for RGB LCD 
#include <rgb_lcd.h>	// library for the Grove RGB Backlight LCD Display - 16 columns 2 rows
#include <Button.h>		// library for buttons
#include "ThermalControl.h"	// class controls thermal protection
#include "WeldPattern.h"	// class controls weld patterns

////////////////////////  HARDWARE SETUP
//									make pin assignments here  ///////////////////////////

const uint8_t ThermalControl::thermPin = A0;	// 1k thermistor/resistor voltage divider
const uint8_t programPin = 16;					// program mode toggle switch
const uint8_t ledPin = 12;						// resistor/LED power on indicator
const uint8_t ThermalControl::fanPin = 11;		// NPN transistor controlling FAN
const uint8_t WeldPattern::zeroCrossPin = 3;    // R5 to AC signal
const uint8_t weldSwitchPin = 4;				// barrel jack for weld switch (momentary)
const uint8_t WeldPattern::welderPin = 2;		// Optocoupler controlling weld current
const uint8_t mode1Pin = 6;						// rotory switch
const uint8_t mode2Pin = 7;						// rotory switch
const uint8_t mode3Pin = 8;						// rotory switch
const uint8_t prevPin = 9;						// menu momentary switch
const uint8_t nextPin = 10;						// menu momentary switch

// instantiate the objects
Button weldButton(weldSwitchPin);		// button actuates the weld pattern
Button mode1Button(mode1Pin);			// rotary switch position weldPattern1 timed pulses
Button mode2Button(mode2Pin);			// rotary switch position weldPattern2 timed pulses
Button mode3Button(mode3Pin);			// rotary switch position weldPattern3 continuous pulse
Button previousButton(prevPin);			// menu button for programming interface
Button nextButton(nextPin);				// menu button for programming interface
Button programButton(programPin);		// toggle swithch program mode 

WeldPattern weldPattern1(100);	// passes the EEprom memory location to store config
WeldPattern weldPattern2(200);
WeldPattern weldPattern3(300);

void initializeHardware(void);

enum mode { mode0 = 0, mode1, mode2, mode3 };

mode getMode(void);

void setup() {
	initializeHardware();
	// power on indicator
	digitalWrite(ledPin, HIGH);
}
void loop() {
	//read the mode from rotory switch
	mode selectedMode = getMode();
	// check for program switch
	if (programButton.pressed()) {
		switch (selectedMode)
		{
		case mode0:
			thermalControl.program();
			break;
		case mode1:
			weldPattern1.program();
			break;
		case mode2:
			weldPattern2.program();
			break;
		case mode3:
			weldPattern3.program();
			break;
		/*default:
			lcd.write("error"); // should never see this
			break;*/
		}
	}
	else {
		//display mode setting per switch and check for weldButton
		switch (selectedMode)
		{
		case mode0:
			thermalControl.display();
			break;
		case mode1:
			weldPattern1.display(1);
			if (weldButton.pressed()) weldPattern1.weld(weldButton);
			break;
		case mode2:
			weldPattern2.display(2);
			if (weldButton.pressed()) weldPattern2.weld(weldButton);
			break;
		case mode3:
			weldPattern3.display(3);
			if (weldButton.pressed()) weldPattern3.weld(weldButton);
			break;
		/*default:
			lcd.write("error"); // should never see this
			break;*/
		}
	}
}

void initializeHardware(void){
	// Button.begin sets pinMode INPUT_PULLUP for active low switch connections
	weldButton.begin();
	mode1Button.begin();
	mode2Button.begin();
	mode3Button.begin();
	previousButton.begin();
	nextButton.begin();
	programButton.begin();
	thermalControl.begin();
	weldPattern1.begin();
	pinMode(ledPin, OUTPUT);

}

mode getMode(void)
{
	if (mode1Button.pressed()) return (mode1);
	else if (mode2Button.pressed()) return (mode2);
	else if (mode3Button.pressed()) return (mode3);
	else return (mode0);
}
