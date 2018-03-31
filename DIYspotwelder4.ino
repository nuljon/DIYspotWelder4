// Debugging switches and macros
#define DEBUG 1 // Switch debug output on and off by 1 or 0

#if DEBUG
#define PRINTS(s)   { Serial.print(F(s)); }
#define PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#else
#define PRINTS(s)
#define PRINT(s,v)
#endif

#include "ThermalControl.h" // class handles thermal protection
#include "WeldPattern.h"	// class handles welder functions - 
#include <EEPROMex.h>    // extended eeprom library
#include <EEPROMVar.h>  // 
#include <Wire.h>		  // I2C linrary for RGB LCD comms
#include <rgb_lcd.h>	// library for the Grove RGB Backlight LCD Display - 16 columns 2 rows
#include <Button.h>		// library for buttons and switches reads state, state changes, and debounce


////////////////////////  HARDWARE SETUP
//									make pin assignments here  ///////////////////////////

const uint8_t ThermalControl::thermPin = A0;	// 1k thermistor/10k resistor voltage divider
const uint8_t programPin = 16;					      // program mode toggle switch
const uint8_t ledPin = 12;						        // resistor/LED power on indicator
const uint8_t ThermalControl::fanPin = 11;		// NPN transistor controlling FAN
const uint8_t WeldPattern::zeroCrossPin = 3;  // R5 to AC signal
const uint8_t weldSwitchPin = 4;				      // barrel jack for weld switch (momentary)
const uint8_t ThermalControl::controlPin = 2;	// Optocoupler controlling weld current
const uint8_t mode1Pin = 6;						        // rotory switch POSITION 2 NO
const uint8_t mode2Pin = 7;						        // rotory switch POSITION 3 NO
const uint8_t mode3Pin = 8;						        // rotory switch POSITION 4 NO
const uint8_t prevPin = 9;						        // menu momentary switch NO
const uint8_t nextPin = 10;						        // menu momentary switch NO

// instantiate the objects
Button weldButton(weldSwitchPin);// button actuates the weld pattern
Button mode1Button(mode1Pin);		// rotary switch positions (0 position is no connection)
Button mode2Button(mode2Pin);		// 
Button mode3Button(mode3Pin);		// 
Button decrementButton(prevPin);// buttons for programming interface
Button incrementButton(nextPin);// inc and dec are momentary type
Button programButton(programPin);// toggle switch selects program mode (set)
WeldPattern weldPattern1(100);	// instantiate objects from WeldPattern Class
WeldPattern weldPattern2(200);  // each instance represents a weld pulse pattern
WeldPattern weldPattern3(300);  // parameter is mem location to store pattern

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
		//thermalControl.program(decrementButton, incrementButton, weldButton);
		while (!programButton.read()) thermalControl.set(decrementButton,incrementButton,weldButton);
		thermalControl.display();
		break;
	case mode1:
		PRINTS("\n\nexecuting mode1");
		while (!programButton.read()) weldPattern1.set(1, decrementButton, incrementButton, weldButton);
		weldPattern1.display(1);
		if (!weldButton.read()) {
			PRINTS("\n WELD BUTTON IS PRESSED!");
			weldPattern1.weld(weldButton);
		}
		break;
	case mode2:
		PRINTS("\n\nexecuting mode2");
		while (!programButton.read()) weldPattern2.set(2, decrementButton, incrementButton, weldButton);
		weldPattern2.display(2);
		if (!weldButton.read()) {
			PRINTS("\n WELD BUTTON IS PRESSED!");
			weldPattern2.weld(weldButton);
		}
		break;
	case mode3:
		PRINTS("\n\nexecuting mode3");
		while (!programButton.read()) weldPattern3.set(3, decrementButton, incrementButton, weldButton);
		weldPattern3.display(3);
		if (!weldButton.read()) {
			PRINTS("\n WELD BUTTON IS PRESSED!");
			weldPattern3.weld(weldButton);
		}
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
	
	int modePin1 = digitalRead(mode1Pin);
	PRINT("\nmode switch position 1 reads: ", modePin1);
	int modePin2 = digitalRead(mode2Pin);
	PRINT("\nmode switch position 2 reads: ", modePin2);
	int modePin3 = digitalRead(mode3Pin);
	PRINT("\nmode switch position 3 reads: ", modePin3);
	
	
	if (mode1Button.read() == mode1Button.PRESSED) {
		PRINTS("\npressed mode1");
		return (mode1);
	}
	else if (mode2Button.read() == mode2Button.PRESSED) {
		PRINTS("\npressed mode2");
		return (mode2);
	}
	else if (mode3Button.read() == mode3Button.PRESSED) {
		PRINTS("\npressedmode3");
		return (mode3);
	}
	else return (mode0);
}

