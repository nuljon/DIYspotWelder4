#include "WeldPattern.h"
#include "ThermalControl.h"
bool WeldPattern::configured = false;

void WeldPattern::start() {	// function to start the weld pulse at_ the _ peak amperage
	while (digitalRead(zeroCrossPin)); //AC > zero do nothing    /_\ _ /^
	while (!digitalRead(zeroCrossPin)); //AC < zero do nothing      \_/ |
	delayMicroseconds(sinusMax_us); //beginning start after sinusMax_us |
}
void WeldPattern::pulseWeld(unsigned long pulse, Button& weldButton, const uint8_t& welderControlPin) {
	PRINT("\n executing weld pulse: ", pulse);	//debug PRINT pulse
	thermalControl.lcd.setRGB(255, 255, 255); //flash white backlight while welding
	bool stop = false;
	unsigned long start = millis(); // mark the start time
	while (!stop) {
		digitalWrite(welderControlPin, HIGH);				// turn on the weld current
		PRINT("\n controlPin: ",welderControlPin);
		PRINT("\t reading: ", digitalRead(welderControlPin));
		PRINTS("\n zzzzzzzzzt");					//debug PRINT
		if (pulse == 0l) {							// pulse is set to continuous
			stop = weldButton.read();				// stop if button not pressed
			PRINT("\nweld button status: ", weldButton.read());//debug PRINT button
		}											// otherwise
		else stop = (millis() >= start + pulse);	// stop on reaching pulse length
		int temperature = thermalControl.getTemperature(); // thermal control can suspend weld loop
	}
	digitalWrite(welderControlPin, LOW);					// turn off the weld current
	thermalControl.lcd.setRGB(0, 0, 255);			//revert backlight color
}
void WeldPattern::pause(unsigned long wait) {
	delay(wait);
}
void WeldPattern::begin()
{
	pinMode(zeroCrossPin, INPUT); 
}
void WeldPattern::weld(Button& weldButton)
{
	PRINTS("\n PuBLiC WeLD function CaLLING PRIVATE WELD");	
	weld(weldButton, welderControlPin);
}
void WeldPattern::weld(Button& weldButton, const uint8_t& welderControlPin) {
	
	PRINT("\n executing weld pattern: ", int(weldData.preWeldPulse));//debug PRINT legth of prew-weld pulse
	PRINT("/", int(weldData.pausePulse));			//debug PRINT length of pause
	PRINT("/", int(weldData.weldPulse));			//debug PRINT length of weld pulse
	start();										// detect zerocrossing and time the pulse start
	pulseWeld(this -> weldData.preWeldPulse, weldButton, welderControlPin);
	pause(this -> weldData.pausePulse);
	start();										// detect zerocrossing and time the pulse start
	pulseWeld(this -> weldData.weldPulse, weldButton, welderControlPin);
}
// TODO handle screen writes displaying greater than 16 columns
void WeldPattern::display(uint8_t num){				// display weldPattern(num)
	configured = loadWeldConfig(num);				// retrieve requested weldata for display
	if (!configured) useDefaultConfig(num);			// if not in memory use defaults
	PRINT("\nCONFIGURED STATUS: ", int(configured));// debug PRINT status config in memory to serial
	thermalControl.lcd.clear();						// clear the lcd screen
	thermalControl.lcd.setCursor(0, 0);				// from top left
	thermalControl.lcd.print("Weld Pattern ");		// lcd print title = column 13
	thermalControl.lcd.print(num);					// lcd print pattern number = column 14
	PRINT("\nDisplaying WeldPattern", num);			// debug PRINT weldPattern(num) to serial
	thermalControl.lcd.setCursor(0, 1);				// on the next line of lcd
	thermalControl.lcd.print(int(this -> weldData.preWeldPulse));//lcd print preWeld pulse value
	PRINT("\n", weldData.preWeldPulse);				 //debug PRINT preWeldPulse to serial
	thermalControl.lcd.print("/");					// lcd print slash
	thermalControl.lcd.print(int(this -> weldData.pausePulse));  // lcd print pause pulse value
	PRINT("/", weldData.pausePulse);				 //debug PRINT pausePulse to serial
	thermalControl.lcd.print("/");					// lcd print slash
	if (this -> weldData.weldPulse > 0) {			// if displaying pattern 1 or 2
		thermalControl.lcd.print(int(this -> weldData.weldPulse)); // lcd print the pulse value
		thermalControl.lcd.setRGB(0, 0, 255);		// set default backlight color
		PRINT("/", weldData.weldPulse);				//debug PRINT weldPulse to serial
	}
	else {
		thermalControl.lcd.print(F("Cnt"));			// continuous pulse
		thermalControl.lcd.setRGB(255, 255, 0);		// set yellow backlight color
		PRINT("cnt", weldData.weldPulse);			//debug PRINT weldPulse to serial
	}
	int temperature = thermalControl.getTemperature();
	thermalControl.lcd.print(" ");
	thermalControl.lcd.print(temperature);
	thermalControl.lcd.print("*");
	PRINT(" ",temperature);							//debug PRINT temperature to serial
}

bool WeldPattern::loadWeldConfig(byte pattern) {
	int address = int(pattern) * 100;
	EEPROM.readBlock(address, this -> weldData);
	return (this -> weldData.version == 1);
}
void WeldPattern::saveWeldConfig(const int address, weldPatternStruct& weldData) {
	EEPROM.updateBlock(address, this -> weldData);
}
void WeldPattern::useDefaultConfig(uint8_t pattern) {
	switch (pattern) 
	{
	case 1: 
		this -> weldData.version = 1;
		this -> weldData.preWeldPulse = 50;
		this -> weldData.pausePulse = 250;
		this -> weldData.weldPulse = 250;
		saveWeldConfig(100, this -> weldData);
		break;
	case 2:
		this -> weldData.version = 1;
		this -> weldData.preWeldPulse = 100;
		this -> weldData.pausePulse = 500;
		this -> weldData.weldPulse = 500;
		saveWeldConfig(200, this -> weldData);
		break;
	case 3:
		this -> weldData.version = 1;
		this -> weldData.preWeldPulse = 50;
		this -> weldData.pausePulse = 50;
		this -> weldData.weldPulse = 0;
		saveWeldConfig(300, this -> weldData);
		break;
	}
}
void WeldPattern::set(uint8_t pattern, Button& decrementButton, Button& incrementButton, Button& weldButton)
{
	int address = pattern * 100;
	configured = loadWeldConfig(pattern);
	if (!configured) useDefaultConfig(pattern);
	static uint8_t field = 1;			// tracks which setting we are programming
  const char str1[] = "preweld pulse";
    const char* sp1 = str1;
    const int length1 = sizeof(str1);
	do{
		display(sp1, length1, this -> weldData.preWeldPulse); // first setting to program
		delay(500);							// allow some time for user to read value
		if (!decrementButton.read() && this -> weldData.preWeldPulse > 50L)
			this -> weldData.preWeldPulse -= 5L;
		if (!incrementButton.read() && this -> weldData.preWeldPulse < 1000L)
			this -> weldData.preWeldPulse += 5L;
		if (weldButton.pressed()) {
			saveWeldConfig(address, this -> weldData);	// save setting
     ++field;   // bump field to next setting
		}
		
	} while (field == 1);
 const char str2[] = "paused pulse ";
   const char* sp2 = str2;
    const int length2 = sizeof(str2);
	do {
		display(sp2, length2, this -> weldData.pausePulse); // second setting to program
		delay(500);										  // allow some time for user to read value
		if (!decrementButton.read()  && this -> weldData.pausePulse > 50L)
			this -> weldData.pausePulse -= 5L;
		if (!incrementButton.read() && this -> weldData.pausePulse < 1000L)
			this -> weldData.pausePulse += 5L;
		if (weldButton.pressed()) {
			saveWeldConfig(address, this -> weldData);	// save setting
     ++field;   // bump field to next setting
		}
		
	 } while (field == 2);
   const char str3[] = "weld pulse ";
   const char* sp3 = str3;
    const int length3 = sizeof(str3);
	 do {
		display(sp3, length3, this -> weldData.pausePulse); // second setting to program
		delay(500);										  // allow some time for user to read value
		if (!decrementButton.read() && this -> weldData.weldPulse > 50L)
			this -> weldData.weldPulse -= 5L;
		if (!incrementButton.read() && this -> weldData.weldPulse < 1000L)
			this -> weldData.weldPulse += 5L;
		if (weldButton.pressed()) {
			saveWeldConfig(address, this -> weldData);	// save setting
     field = 1;   // we saved last field so reset setting
		}
		
	 } while (field == 3);
}
// TODO handle screen writes displaying greater than 16 columns
// TODO decouple display methods 
void WeldPattern::display(const char* str, const int length, int value) {
	thermalControl.lcd.setRGB(0, 255, 0);				// light the display - program mode is green
	thermalControl.lcd.clear();						// clear the screen
	thermalControl.lcd.setCursor(0, 0);				// from top left
	thermalControl.lcd.println(F(" CHANGE SETTING "));		// print title line
	PRINTS("\n\nCHANGE SETTING\n");					// debug PRINT line to serial
	thermalControl.lcd.setCursor(0, 1);				// on the next line
	for (uint8_t i = 0; i < length; ++i) {			// iterate chars of string
		thermalControl.lcd.print(str[i]);			// print char to lcd col < 10 of 16 probably
		PRINT("", str[i]);							// debug PRINT char to serial
	}
	thermalControl.lcd.print(value);				// print passed value -
	PRINT("", value);								// debug PRINT value to serial
}
