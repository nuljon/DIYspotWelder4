#ifndef _THERMALCONTROL_h
#define _THERMALCONTROL_h

#define DEBUG 0
#if DEBUG
#define PRINTS(s)   { Serial.print(F(s)); }
#define PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#else
#define PRINTS(s)
#define PRINT(s,v)
#endif
#include <Arduino.h>
#include "Button.h"
#include "rgb_lcd.h"
#include "EEPROMex.h"



class ThermalControl
{
	friend class WeldPattern;
	 static const uint8_t thermPin;
	 static const uint8_t fanPin;
	 static const uint8_t controlPin;
	 const int thermalConfigAddress = 0;
	 rgb_lcd lcd; // Grove RGB Backlight library call
	 // rgb_lcd* LCD = lcd;
	 static bool configured;

	 struct thermalControlStruct {
		 char version;
		 int threshold;					// start cooling fan at this temperature
		 int cutOff;					// disable weld current at this temperature
	 } thermalControlData;

	 bool loadThermalConfig(void);
	 void saveThermalConfig(void);

	 long fanSpeed = 0;
	
	 uint8_t runFan(int temperature);	// returns the fanspeed
	 void display(const char*, const int, int);

 public:
	 void begin(void);
	 void display(void);
	 void set(Button& ,Button& ,Button& );
	 int getTemperature(void); // returns degrees farenheight and does thermal control
};
extern ThermalControl thermalControl;


#endif

