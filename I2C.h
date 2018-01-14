#ifndef ABOX_I2C_HEADER
#define ABOX_I2C_HEADER
#include "ConstValue.h"
#include "STC15Pins.h"
#include "Timer.h"
inline void Init_I2c() {
  sda = ON;
  scl = ON;
}
inline void I2c_start() {
  sda=ON;
  Delay3us();
  scl=ON;
  Delay3us();
  sda=OFF;
  Delay3us();
  scl=OFF;
  Delay3us();
}
inline void I2c_end() {
  sda=OFF;
  Delay3us();
  scl=ON;
  Delay3us();
  sda=ON;
  Delay3us();
}
//Send 8 bits via I2c
__bit I2c_send_char(unsigned char dat) {
	unsigned char loop = 0;
	__bit is_ack = Failed;
	for (; loop < 8; ++loop) {
		sda = (__bit)((dat << loop) & The_8th_bit);
		Delay3us();
		scl = ON;
		Delay3us();
		scl = OFF;
		Delay3us();
	}
	sda = ON;
	Delay3us();
	scl = ON;
	Delay3us();
	while (++loop < 0xff) {
		if (sda == OFF) {
			is_ack = Successed;
			break;
		}
	}
	scl = OFF;
	Delay3us();
	return is_ack;
}
//Receive 8 bits via I2c
unsigned char I2c_receive_char_with_ack(__bit is_need_ack) {
	unsigned char dat = 0x00, loop = 0;
	sda = ON;
	Delay3us();
	for (; loop < 8; ++loop) {
		scl = ON;
		Delay3us();
		dat |= ((unsigned char)sda << (7 - loop));
		Delay3us();
		scl = OFF;
		Delay3us();
	}
	sda = is_need_ack ^ 1;
	Delay3us();
	scl = ON;
	Delay3us();
	scl = OFF;
	Delay3us();
	return dat;
}
#endif
