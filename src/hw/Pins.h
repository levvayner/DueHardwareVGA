#ifndef _PINS_H
#define _PINS_H

//#define PIN_CE		3	//ACTIVE HIGH	- pull down to use chip.
//#define PIN_WE		4	//ACTIVE HIGH	- pull down for 100ns - 1000ns *AFTER* setting address and data. OE should be HIGH. CE should be HIGH.
//#define PIN_OE		2

#define USB_DATA1 18
#define USB_CLK1  19
#define USB_DATA2 16
#define USB_CLK2  17

#define PS2_DATA  42
#define PS2_CLK   43 

//#define PIN_RESOLUTION 5 //SET Resolution
#define PIN_BANK_SELECT 5

#define DOUBLE_BUFFER

#ifdef DOUBLE_BUFFER
    #define PIN_READY 31
#endif
#endif