#ifndef ABOX_MAX7219_HEADER
#define ABOX_MAX7219_HEADER
#include "ConstValue.h"
#include "STC15Pins.h"
unsigned char display_matrix[3];
__bit matrix_0_modified;
__bit matrix_1_modified;
__bit matrix_2_modified;
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
void Set_display_max7219(const unsigned char matrix_ord, unsigned char dat) {
  switch (matrix_ord) {
   case 0:
    matrix_0_modified = ON;
    display_matrix[0] = dat;
    break;
   case 1:
    matrix_1_modified = ON;
    display_matrix[1] = dat;
    break;
   case 2:
    matrix_2_modified = ON;
    display_matrix[2] = dat;
    break;
  }
}
void Update_display_max7219() {
  if (matrix_0_modified == ON) {
    matrix_0_modified = OFF;
    Write_max7219(0x01, display_matrix[0]);
  }
  if (matrix_1_modified == ON) {
    matrix_1_modified = OFF;
    Write_max7219(0x02, display_matrix[1]);
  }
  if (matrix_2_modified == ON) {
    matrix_2_modified = OFF;
    Write_max7219(0x03, display_matrix[2]);
  }
}
inline void Set_intensity_max7219(unsigned char dat) {
	Write_max7219(0x0a, dat);
}
inline void Init_max7219() {
  Set_display_max7219(0, 0x00);
  Set_display_max7219(1, 0x00);
  Set_display_max7219(2, 0x00);
  Update_display_max7219();
  Set_intensity_max7219(0x0f);//Max intensity
	Write_max7219(0x09, 0x00);//No decode
	Write_max7219(0x0b, 0x02);//3 line of leds
	Write_max7219(0x0f, 0x00);//Turn off test mode
}
#endif
