#ifndef ABOX_DELAY_HEADER
#define ABOX_DELAY_HEADER
#include "STC15Pins.h"
#include "ConstValue.h"
enum {
  Times_of_1ms_size = 2
};
enum {
  Times_of_1ms_long_size = 1
};
unsigned char Times_of_1ms[Times_of_1ms_size];
unsigned int Times_of_1ms_long[Times_of_1ms_long_size];
enum {
  Timer_Feedback = 0,
  Timer_Delay = 1
};
enum {
  Timer_Long_Press = 0
};
void Reset_timer() {
  unsigned char i; 
  for (i = 0; i < Times_of_1ms_size; ++i)
    Times_of_1ms[i] = 0x00;
  for (i = 0; i < Times_of_1ms_long_size; ++i)
    Times_of_1ms_long[i] = 0x0000;
}
void Init_1ms_Timer() {
	AUXR |= The_3rd_bit;
	IE2 |= The_3rd_bit;
	EA = ON;
}
inline void Stop_1ms_timer() {
  AUXR &= ~The_5th_bit;
  Reset_timer();
}
void Start_1ms_timer()
{
  const unsigned char Times_of_cycle_for_1ms_High = 0xb1;
  const unsigned char Times_of_cycle_for_1ms_Low = 0xe0;
	Stop_1ms_timer();
	T2H = Times_of_cycle_for_1ms_High;
	T2L = Times_of_cycle_for_1ms_Low;
	AUXR |= The_5th_bit;
}
void Count_1ms() __interrupt 12 {
  unsigned char i;
  for (i = 0; i < Times_of_1ms_size; ++i)
    if (Times_of_1ms[i] != 0x00) --Times_of_1ms[i];
  for (i = 0; i < Times_of_1ms_long_size; ++i)
    if (Times_of_1ms_long[i] != 0x0000) --Times_of_1ms_long[i];
}
inline void _nop_() {
  __asm__("nop");
}
void Delay3us() {
	unsigned char i;
	_nop_();
	_nop_();
	i = 12;
	while (--i);
}
void Delay_38ms(){
	unsigned char i, j, k;
	_nop_();
	_nop_();
	i = 3;
	j = 154;
	k = 122;
	do {
		do {
			while (--k);
		} while (--j);
	} while (--i);
}
#endif
