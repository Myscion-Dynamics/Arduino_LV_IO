# Arduino LED Light Controller

This repo is for a simple LED light controller driven by an Arduino nano.
The lights are driven by a constant current LED driver with a range of 5mA - 200mA depending on the resistor installed.
The board can also be reconfigured as a generic io controller with a max current of 5A per channel.

The LED channels use a low side constant current driver and ifn the io configuration it uses a low side switch.

# Board Render

![PCB Render](https://github.com/Myscion-Dynamics/Arduino_LV_IO/blob/main/Mechanical/Board_Render.png)

A board mount is included in this repo in the mechanical folder. This should be 3D printed with metric 3M threaded inserts installed.

# Manufactured board

![Assembled PCB](https://github.com/Myscion-Dynamics/Arduino_LV_IO/blob/main/Mechanical/PCBA_Assembled.jpg)

# Software
Software for the LED configuration is included. It generates a random pattern of blinking lights.
The brightness is controlled in software and it monitores the input voltage to avoid overdischaring any batteries.
The software runs on an Arduino nano

# Configurations
For the Low side constant current source LED driver popuolate the following IC's 
U1, U2, U3, U4, U5, U6, U7, U8, U9. U10. U11, U12, U13, U14, U15, U15

For the low side switch configuration populate the following transistors
Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, Q10, Q11, Q12, Q13, Q14, Q15, Q16 

Do not install the parts for both configurations at the same time.
It is posible to mix and match configurations are long as care is taken so only one of each is populated on each channel.