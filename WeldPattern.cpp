#include "WeldPattern.h"
#include "ThermalControl.h"
bool WeldPattern::configured = false;

void WeldPattern::start() {
	//to prevent inrush current, turn-on at the sinus max         _     _
	while (digitalRead(zeroCrossPin)); //AC > zero do nothing     _\ _ /^
	while (!digitalRead(zeroCrossPin)); //AC < zero do nothing      \_/ |
	delayMicroseconds(sinusMax_us); //beginning start after sinusMax_us |
}
void WeldPattern::pulseWeld(unsigned long pulse, Button& weldButton,rgb_lcd&, const uint8_t& welderPin) {
	lcd.setRGB(255, 255, 255); //flash white backlight while welding
	bool stop = false;
	unsigned long start = millis();
	while (!stop) {
		digitalWrite(welderPin, HIGH);
		if (pulse == 0L) stop = weldButton.released();
		else stop = (millis() > start + pulse);
		int temperature = thermalControl.getTemperature();
	}
	digitalWrite(welderPin, LOW);
	lcd.setRGB(0, 0, 255); //revert backlight color
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
	weld(weldButton, lcd, welderPin);
}
void WeldPattern::weld(Button& weldButton, rgb_lcd&, const uint8_t& welderPin) {
	
	start();
	pulseWeld(weldData.preWeldPulse, weldButton, lcd, welderPin);
	pause(weldData.pausePulse);
	start();
	pulseWeld(weldData.weldPulse, weldButton, lcd, welderPin);
}
void WeldPattern::display(char num, rgb_lcd&){
	configured = loadWeldConfig(num);
	if (!configured) useDefaultConfig(num);
	lcd.clear();						// clear the screen
	lcd.setCursor(0, 0);				// from top left
	lcd.print("Weld Pattern ");			// col 13
	lcd.print(num);						// col 14
	lcd.setCursor(0, 1);				// on the next line
	lcd.print(byte(weldData.preWeldPulse));
	lcd.print("/");
	lcd.print(byte(weldData.pausePulse));
	lcd.print("/");
	if (weldData.weldPulse) {
		lcd.print(byte(weldData.weldPulse));
		lcd.setRGB(0, 0, 255);		// set default backlight color 
	}
	else {
		lcd.print(F("Cnt"));			// continuous pulse
		lcd.setRGB(255, 255, 0);		// set yellow backlight color 
	}
	int temperature = thermalControl.getTemperature();
	lcd.print(" ");
	lcd.print(temperature);
	lcd.print("*");
}
void WeldPattern::display(char num) {
	display(num, lcd);
}
bool WeldPattern::loadWeldConfig(byte pattern) {
	int address = int(pattern) * 100;
	EEPROM.readBlock(address, weldData);
	return (weldData.version == 4);
}
void WeldPattern::saveWeldConfig(const int address, weldPatternStruct& weldData) {
	EEPROM.updateBlock(address, weldData);
}
void WeldPattern::useDefaultConfig(byte pattern) {
	switch (pattern) 
	{
	case 1: 
		weldData.version = 4;
		weldData.preWeldPulse = 50;
		weldData.pausePulse = 500;
		weldData.weldPulse = 250;
		saveWeldConfig(100, weldData);
		break;
	case 2:
		weldData.version = 4;
		weldData.preWeldPulse = 50;
		weldData.pausePulse = 500;
		weldData.weldPulse = 500;
		saveWeldConfig(200, weldData);
		break;
	case 3:
		weldData.version = 4;
		weldData.preWeldPulse = 50;
		weldData.pausePulse = 500;
		weldData.weldPulse = 0;
		saveWeldConfig(300, weldData);
		break;
	}
}
void WeldPattern::program(uint8_t pattern, Button& decrementButton, Button& incrementButton, Button& weldButton)
{
	int address = pattern * 100;
	configured = loadWeldConfig(pattern);
	if (!configured) useDefaultConfig(pattern);
	static uint8_t field = 1;			// tracks which setting we are programming
	if (field == 1) {
		const char str[] = "preweld pulse";
		const char* sp = str;
		const int length = sizeof(str);
		display(sp, length, weldData.preWeldPulse); // first setting to program
		if (decrementButton.pressed() && weldData.preWeldPulse > 50)
			weldData.preWeldPulse -= 5;
		if (incrementButton.pressed() && weldData.preWeldPulse < 1000)
			weldData.preWeldPulse += 5;
		if (weldButton.pressed()) {
			++field;				// bump field to next setting
			saveWeldConfig(address, weldData);	// save setting
		}
		delay(500);				// allow some time for user to read value
	}
	if (field == 2) {
		const char str[] = "paused pulse ";
		const char* sp = str;
		const int length = sizeof(str);
		display(sp, length, weldData.pausePulse); // second setting to program
		if (decrementButton.pressed() && weldData.pausePulse > 50)
			weldData.preWeldPulse -= 5;
		if (incrementButton.pressed() && weldData.pausePulse < 1000)
			weldData.preWeldPulse += 5;
		if (weldButton.pressed()) {
			field = 3;		// bump field to next setting
			saveWeldConfig(address, weldData);	// save setting
		}
		delay(500);				// allow some time for user to read value
	}
	if (field == 3) {
		const char str[] = "weld pulse ";
		const char* sp = str;
		const int length = sizeof(str);
		display(sp, length, weldData.pausePulse); // second setting to program
		if (decrementButton.pressed() && weldData.weldPulse > 50)
			weldData.preWeldPulse -= 5;
		if (incrementButton.pressed() && weldData.weldPulse < 1000)
			weldData.preWeldPulse += 5;
		if (weldButton.pressed()) {
			field = 1;		// reset setting
			saveWeldConfig(address, weldData);	// save setting
		}
		delay(500);				// allow some time for user to read value
	}
}
void WeldPattern::display(const char* str, const int length, int value) {
	lcd.setRGB(0, 255, 0);				// light the display - program mode is green
	lcd.clear();						// clear the screen
	lcd.setCursor(0, 0);				// from top left
	lcd.println(F(" CHANGE SETTING "));		// print title line
	PRINTS("\n\nCHANGE SETTING\n");
	lcd.setCursor(0, 1);				// on the next line
	for (uint8_t i = 0; i < length; ++i) {
		lcd.print(str[i]);			// col 10
		PRINT("", str[i]);
	}
	lcd.print(value);
	PRINT("", value);
}