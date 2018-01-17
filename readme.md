DIY Spotwelder v4.0

by Jon Nuljon January 2018

A resistance welder with these Functions:
  - Dual Pulse Welds- user configurable timing for pre-weld pulse, pause, main weld pulse
  - 4 selectable default modes: 
			mode 0:  Thermal Controls
			mode 1:  operates per user configured timing
			mode 2:  operates per user configured timing
			mode 3:  continuous main weld pulse until the weld switch is released ( defined by weldPulse = 0)
  - Thermal protection via temperature controled cooling fan and weld current cutoff at user defined temperatures 
  - LCD display
  - user programmable settings - in progress
  - Powered by MAINS  
         	This is achieved by rewinding the core(s) of microwave oven transformer for 2-4v upto 1200A
		 	a second smaller transformer that creates ~ 12v (rectified) for the fan
		 	and further regulated 5v DC for the control circuitry:
			various switches for mode select and programmable interface,
			a thermistor to detect the temperarure, transistor to switch a DC fan motor for cooling, 
         	a Grove RGB backlight LCD display to give visual signals (resitance welders should not arc),
			a few resistors, LED, capacitors, diodes, 
			Arduino micro controler to rule the the whole pile and run the code.


works with the circuit, DIYspotwelder4.pdf   
circuit is an adaptation from Albert van Dalen's board
http://www.avdweb.nl/arduino/hardware-interfacing/spot-welder-controller.html


It is preferred to start weld pulse at peak power. For inductive loads on AC,
the amperage over time will follow the voltage by 90 degress. So calculate the 
weld start time based on mains voltage zero crossing points.

MAINS POWER where I live is delivered by 60Hz sinus cycle
  each cycle duration is 16.667ms (1 sec divided by 60)
  divide by 4 to calculate 90 degree lag equals 4167us (from zero cross until peak amps)

The rotory switch I use has 4 positions but one is NC - which I code as mode0 and use to display
the transformer core temperature and fanspeed. If program toggle is switched, this mode is also used
to configure the thermal control activation threshold and weld current cutoff.						  