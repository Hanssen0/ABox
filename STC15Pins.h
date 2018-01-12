#ifndef ABOX_STC15_PINS_HEADER
#define ABOX_STC15_PINS_HEADER
//STC15F2K60S2, 20.000MHz
__sbit __at (0x80) max7219_din;
__sbit __at (0x81) max7219_load;
__sbit __at (0x82) max7219_clk;
__sbit __at (0xB2) input_pin;
__sbit __at (0x83) feedback; //PWM: 2500zq
__sbit __at (0x84) sda;
__sbit __at (0x85) scl;
__sbit __at (0x86) battery_charging;
__sbit __at (0x87) battery_full;
__sbit __at (0x88) IT0;
__sbit __at (0xA8) EX0;
__sbit __at (0xAF) EA;
__sfr __at (0x8E) AUXR;
__sfr __at (0xD6) T2H;
__sfr __at (0xD7) T2L;
__sfr __at (0xAF) IE2;
__sfr __at (0x87) PCON;
__sfr __at (0x9D) P1ASF;
__sfr __at (0xBC) ADC_CONTR;
__sfr __at (0xBD) ADC_RES;
__sfr __at (0xB2) P3M0;
__sfr __at (0xB1) P3M1;
//Pins in STC15
#endif
