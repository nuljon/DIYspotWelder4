#include "WeldPattern.h"
#include "ThermalControl.h"

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
void WeldPattern::display(char num, rgb_lcd&, bool&){
	if (configured)loadWeldConfig(num);
	else useDefaultConfig(num);
	configured == true;
	lcd.clear();						// clear the screen
	lcd.setCursor(0, 0);				// from top left
	lcd.print("Weld Pattern ");			// col 13
	lcd.print(num);						// col 14
	lcd.setCursor(0, 1);				// on the next line
	lcd.print(byte(weldData.preWeldPulse));
	lcd.print(" / ");
	lcd.print(byte(weldData.pausePulse));
	lcd.print(" / ");
	if (weldData.weldPulse) {
		lcd.print(byte(weldData.weldPulse));
		lcd.setRGB(0, 0, 255);		// set default backlight color 
	}
	else {
		lcd.print("Cont.");			// continuous pulse
		lcd.setRGB(255, 255, 0);		// set yellow backlight color 
	}
}
void WeldPattern::display(char num) {
	display(num, lcd, configured);
}
void WeldPattern::loadWeldConfig(byte pattern) {
	int address = int(pattern) * 100;
	EEPROM.readBlock(address, weldData);
}
void WeldPattern::saveWeldConfig(const int address, weldPatternStruct& weldData) {
	EEPROM.writeBlock(address, weldData);
}
void WeldPattern::useDefaultConfig(byte pattern) {
	switch (pattern) 
	{
	case 1: 
		weldData.preWeldPulse = 50;
		weldData.pausePulse = 500;
		weldData.weldPulse = 250;
		saveWeldConfig(100, weldData);
		break;
	case 2:
		weldData.preWeldPulse = 50;
		weldData.pausePulse = 500;
		weldData.weldPulse = 500;
		saveWeldConfig(200, weldData);
		break;
	case 3:
		weldData.preWeldPulse = 50;
		weldData.pausePulse = 500;
		weldData.weldPulse = 0;
		saveWeldConfig(300, weldData);
		break;
	}
}
void WeldPattern::program()
{
}
