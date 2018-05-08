#ifndef _WELDPATTERN_h
#define _WELDPATTERN_h
#
#include "ThermalControl.h"
#include <Button.h>
//
class WeldPattern
{	private:
	// class variables (values shared by all weld patterns)
	static const uint8_t welderControlPin; // pin controlling weld current
	static const uint8_t weldSwitchPin;	   // pin to read weld activation signal
	static const uint8_t zeroCrossPin;	   // to AC signal in phase with MAINS power
	static const long sinusMax_us = 4167L; // (1000ms/60Hz/4=4167us) time(us) from voltage zerocross until max current
	static int temperature; // even if many patterns, there is only one transformer core and this is its temperature
	static bool configured; // if one pattern is configured we assume the rest are too

	// member variables
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
	void weld(Button& weldButton, const uint8_t& welderControlPin);
	void pulseWeld(unsigned long pulseTime, Button& weldButton, const uint8_t& welderControlPin);
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

