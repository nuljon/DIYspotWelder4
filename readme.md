DIY Spotwelder v4.0

by Jon Nuljon January 2018

A resistance welder with these features:
  - Dual Pulse Welds- user configurable timing for pre-weld pulse, pause, main weld pulse
  - 4 selectable modes display values and allow user to make changes via front panel interface

		mode 0:  Thermal Control threshold and cut-off
		mode 1:  dual weld pulse pattern
		mode 2:  dual weld pulse pattern
		mode 3:  user can set for continuous weld pattern
  - Thermal protection via temperature controled cooling fan and weld current cutoff at user defined temperatures 
  - LCD display - changes color based on mode and state
  - user programmable settings - saved in eeprom memory - now working!
  - Powered by MAINS  
         	This is achieved by rewinding the core(s) of microwave oven transformer for 2-4v upto 1200A
		 	a second smaller transformer that creates ~ 12v (rectified) for the fan
		 	and further regulated 5v DC for the control circuitry:
			various switches for mode select and programmable interface,
			a thermistor to detect the temperarure, transistor to switch a DC fan motor for cooling, 
         	a Grove RGB backlight LCD display to give visual signals (resitance welders should not arc),
			a few resistors, LED, capacitors, diodes, 
			software runs on Arduino micro controler mounted on a van Dalen borad that I altered to support thermal sensing/control and the i2c connection to rgb LCD display


Software works with the circuit:
[DIYspotwelder4.pdf](https://github.com/nuljon/DIYspotWelder4/blob/master/DIYspotwelder4.pdf)
which is a derivitive of Albert van Dalen's 
http://www.avdweb.nl/arduino/hardware-interfacing/spot-welder-controller.html

![schematic]

As to calculating weld pulse start time, it is preferred to start weld pulses at peak power. For inductive loads,
the amperage will follow the voltage phase by 90 degrees. So weld start time is calculated based on mains voltage zero crossing point (from negative crossing to positive voltage).

MAINS POWER where I live is delivered by 60Hz sinus cycle. Each cycle duration is 6.667ms (1 sec divided by 60).
  Divide this by 4 to calculate 90 degree lag time, which equals 4167 microseconds (from zero cross until peak amps).

The rotory switch I use has 4 positions but one is NC - which I code as mode0 and use to display
the transformer core temperature and fanspeed. If program toggle is switched, this mode is also used
to configure the thermal control activation threshold and weld current cutoff temperature. Activation threshold is the temperature whereby the fan motor starts to spin. As the sensed temperature increases, the speed is ramped up using a PWM signal from the Arduino.

For each of the 3 weldPattern instances, modes 1, 2, and 3, the toggle switch will also trigger a method call that allows the user to set weld pulse timing. Custom values can be configured for the preWeldPulse, pause between pulses, and the weldPulse. All changes are visible from the front panel display. Setting weldPulse to 0 is interpreted as continuous weld, and the weld current will stop either upon releasing the weld button, pedal, or whatever trigger device. The weld current will also stop if sensor indicates temperature of the core has reached or exceeded the cutoff setting.

[schemaic]:https://github.com/nuljon/DIYspotWelder4/blob/master/SpotWelderCircuitSchematic.png