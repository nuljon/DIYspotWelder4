#ifndef _THERMALCONTROL_h
#define _THERMALCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "rgb_lcd.h"
#include "EEPROMex.h"


class ThermalControl
{
	 static const uint8_t thermPin;
	 static const uint8_t fanPin;
	 const int thermalConfigAddress = 0;
	 rgb_lcd lcd;							// Grove RGB Backlight library call
	 bool configured = false;

	 struct thermalControlStruct {
		 char version;
		 int activationThreshold;
		 int cutOff;
	 } thermalControlData;

	 bool loadThermalConfig(void);
	 void saveThermalConfig(void);
/*
	 int activationThreshold = 150;	// degrees farenheight to begin active cooling
	 int cutOff = 350; // lockout welder at this temperature
	 */
	 uint8_t fanSpeed = 0;
	
	 uint8_t runFan(int temperature);	// returns the fanspeed

 public:
	 void begin(void);
	 void display(void);
	 void program(void);
	 int getTemperature(void); // returns degrees farenheight and does thermal control
};
extern ThermalControl thermalControl;


#endif

