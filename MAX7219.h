#ifndef ABOX_MAX7219_HEADER
#define ABOX_MAX7219_HEADER
#include "ConstValue.h"
#include "STC15Pins.h"
void Send_char_max7219(unsigned char dat) {
	char loop = 0;
	for(; loop<8; ++loop) {
		max7219_clk = OFF;
		max7219_din = (__bit)(dat & The_8th_bit);
		dat <<= 1;
		max7219_clk = ON;
	}
}
void Write_max7219(unsigned char address, unsigned char dat) {
	max7219_load = OFF;
	Send_char_max7219(address);
	Send_char_max7219(dat);
	max7219_load = ON;
}
#endif
