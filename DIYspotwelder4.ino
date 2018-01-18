// Debugging switches and macros
#define DEBUG 1 // Switch debug output on and off by 1 or 0

#if DEBUG
#define PRINTS(s)   { Serial.print(F(s)); }
#define PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#else
#define PRINTS(s)
#define PRINT(s,v)
#endif

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
const uint8_t ThermalControl::welderPin = 2;	// Optocoupler controlling weld current
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
Button decrementButton(prevPin);			// menu button for programming interface
Button incrementButton(nextPin);				// menu button for programming interface
Button programButton(programPin);		// toggle swithch program mode 

WeldPattern weldPattern1(100);	// passes the EEprom memory location to store config
WeldPattern weldPattern2(200);
WeldPattern weldPattern3(300);

void initializeHardware(void);

enum mode { mode0 = 0, mode1, mode2, mode3 };

mode getMode(void);


void setup() {
#if DEBUG
	Serial.begin(9600);				// open serial port if DEBUG enabled
#endif
	PRINTS("\nDIYspotwelder4.ino - DEBUGGER ON\n");
	PRINTS("\ninitializing hardware");
	initializeHardware();
	PRINTS("\nturning on LED");
	digitalWrite(ledPin, HIGH);		// power on indicator

}
void loop() {
	PRINTS("\nread the mode from rotory switch ...........\n");
	mode selectedMode = getMode();	
	PRINT("\nselectedMode: ",selectedMode);
	switch (selectedMode){
	case mode0:
		PRINTS("\n\nexecuting mode0");
		if (programButton.pressed()) thermalControl.program(decrementButton,incrementButton,weldButton);
		else thermalControl.display();
#if DEBUG
		delay(5000);		//useful for testing bare board withpout periferals
#endif
		break;
	case mode1:
		PRINTS("\n\nexecuting mode1");
		if (programButton.pressed()) weldPattern1.program();
		else weldPattern1.display(1);
		if (weldButton.pressed()) weldPattern1.weld(weldButton);
		break;
	case mode2:
		PRINTS("\n\nexecuting mode2");
		if (programButton.pressed()) weldPattern2.program();
		else weldPattern2.display(2);
		if (weldButton.pressed()) weldPattern2.weld(weldButton);
		break;
	case mode3:
		PRINTS("\n\nexecuting mode3");
		if (programButton.pressed()) weldPattern3.program();
		else weldPattern3.display(3);
		if (weldButton.pressed()) weldPattern3.weld(weldButton);		
		break;
	}
}

void initializeHardware(void){
	// Button.begin sets pinMode INPUT_PULLUP for active low switch connections
	weldButton.begin();
	PRINTS("\nweldButton initialized");
	mode1Button.begin();
	PRINTS("\nmode1Button initialized");
	mode2Button.begin();
	PRINTS("\nmode2Button initialized");
	mode3Button.begin();
	PRINTS("\nmode3Button initialized");
	decrementButton.begin();
	PRINTS("\ndecrementButton initialized");
	incrementButton.begin();
	PRINTS("\nincrementButton initialized");
	programButton.begin();
	PRINTS("\nprogramButton initialized");
	thermalControl.begin();
	PRINTS("\nthermalControl initialized");
	weldPattern1.begin();
	PRINTS("\n zero cross detection enabled");
	pinMode(ledPin, OUTPUT);
	PRINTS("\nLED initialized");

}

mode getMode(void)
{
	if (mode1Button.pressed()) return (mode1);
	else if (mode2Button.pressed()) return (mode2);
	else if (mode3Button.pressed()) return (mode3);
	else return (mode0);
}
