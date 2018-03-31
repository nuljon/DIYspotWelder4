#ifndef _WELDPATTERN_h
#define _WELDPATTERN_h
#
#include "ThermalControl.h"
#include <Button.h>
//
class WeldPattern
{	private:
	// class variables
	static const uint8_t weldSwitchPin;
	static const uint8_t zeroCrossPin;
	static const long sinusMax_us = 4167L; // 1000ms / 60Hz / 4  = 4167us
	static int temperature;
	static bool configured;

	// member variables
	const uint8_t& controlPin;	//reference to pin controlling weld current

	struct weldPatternStruct {
		char version;
		unsigned long preWeldPulse;
		unsigned long pausePulse;
		unsigned long weldPulse;
	} weldData;

	const int weldConfigAddress;

	bool loadWeldConfig(byte pattern);
	void saveWeldConfig(const int, weldPatternStruct&);
	void useDefaultConfig(uint8_t pattern);
	void start();
	void weld(Button& weldButton, const uint8_t& num);
	void pulseWeld(unsigned long pulseTime, Button& weldButton, const uint8_t& controlPin);
	void pause(unsigned long pauseTime);
	void display(const char*, const int, int);

 public:
	 WeldPattern(const int address) :weldConfigAddress(address) {}
	 static void begin();
	 void weld(Button& weldButton);
	 void display(uint8_t num);
	 void WeldPattern::set(uint8_t, Button&, Button&, Button&);


};


#endif

