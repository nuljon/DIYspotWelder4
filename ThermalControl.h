#ifndef _THERMALCONTROL_h
#define _THERMALCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// Debugging switches and macros
#define DEBUG 1 // Switch debug output on and off by 1 or 0

#if DEBUG
#define PRINTS(s)   { Serial.print(F(s)); }
#define PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#else
#define PRINTS(s)
#define PRINT(s,v)
#endif

#include "rgb_lcd.h"
#include "EEPROMex.h"
#include "Button.h"


class ThermalControl
{
	 static const uint8_t thermPin;
	 static const uint8_t fanPin;
	 static const uint8_t welderPin;
	 const int thermalConfigAddress = 0;
	 rgb_lcd lcd;							// Grove RGB Backlight library call
	 static bool configured;

	 struct thermalControlStruct {
		 char version;
		 int threshold;					// start cooling fan at this temperature
		 int cutOff;					// disable weld current at this temperature
	 } thermalControlData;

	 bool loadThermalConfig(void);
	 void saveThermalConfig(void);

	 uint8_t fanSpeed = 0;
	
	 uint8_t runFan(int temperature);	// returns the fanspeed
	 void display(const char*, const int, int);

 public:
	 void begin(void);
	 void display(void);
	 void program(Button& ,Button& ,Button& );
	 int getTemperature(void); // returns degrees farenheight and does thermal control
};
extern ThermalControl thermalControl;


#endif

