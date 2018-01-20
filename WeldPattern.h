#ifndef _WELDPATTERN_h
#define _WELDPATTERN_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif



#include <Button.h>
#include "ThermalControl.h"
//
class WeldPattern
{
	friend class ThermalControl;
	static const uint8_t weldSwitchPin;
	static const uint8_t zeroCrossPin;
	static const int sinusMax_us = 4167; // 1000ms / 60Hz / 4  = 4167us
	static int temperature;
	static bool configured;

	const uint8_t& welderPin;
	rgb_lcd& lcd;
	

	struct weldPatternStruct {
		char version;
		unsigned long preWeldPulse;
		unsigned long pausePulse;
		unsigned long weldPulse;
	} weldData;

	const int weldConfigAddress;

	bool loadWeldConfig(byte pattern);
	void saveWeldConfig(const int, weldPatternStruct&);
	void useDefaultConfig(byte pattern);
	
	void weld(Button&, rgb_lcd&, const uint8_t&);
	 void start();
	 void pulseWeld(unsigned long pulseTime,Button&, rgb_lcd&, const uint8_t&);
	 void pause(unsigned long pauseTime);
	 void display(char num, rgb_lcd& );
	 void display(const char*, const int, int);

 public:
	 WeldPattern(const int address) :weldConfigAddress(address) {}
	 
	 static void begin();

	 void weld(Button&);
	 
	 void display(char num);

	 void WeldPattern::program(uint8_t, Button&, Button&, Button&);


};


#endif

