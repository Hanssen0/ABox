#include "Timer.h"
#include "ConstValue.h"
#include "STC15Pins.h"
#include "MAX7219.h"
#include "MPU6050.h"
#include "Feedback.h"
#include "Input.h"
#include "EventSystem.h"
//-Events:
//--0.Feedback|
//--      4          
//--  Trun LED to state 1|Trun LED to state 0|Input: Falling edge|Input: Level changed
//--	          3                  2                 1                   0
//-State:
//--0.|Mode|state
//--   7-4   3-0
//---Mode 0:
//----State 0: All LED off
//----State 1: All LED on
//---Mode 1: Control LED with acceleration
//----State 0: 0x00, 0x00, 0x00
//----State 1: 0x00, 0x33, 0x00
//----State 2: 0x00, 0x00, 0xcc
//----State 3: 0xcc, 0x00, 0x00
//----State 4: 0x00, 0xcc, 0x00
//----State 5: 0x33, 0x00, 0x00
//----State 6: 0x00, 0x00, 0x33
//-Level:
//--|Mode 1:Viscosity of object|Level of press|
//--               7-4               3-0
//
//Send 8 bits
//Initialize everything (chips,input and timers)
void Init_ADC() {
  P3M1 = 0x04;
  P1ASF = 0x80;
  ADC_RES = 0x00;
}
void Init() {
	//----------Initialize framework----------
	events[0] = 0x00;
	state_table[0] = 0x00;
	level = 0;
  Init_max7219();
  Init_I2c();
  Init_mpu6050();
  Init_feedback();
  Init_input();
  Init_ADC();
  Init_1ms_Timer();
}
void main() {
	Init();
	Write_max7219(0x0c, 0x01);
	Write_mpu6050(0x6b, 0x00);
	Start_1ms_timer();
	while (1==1) {
    Update_display_max7219();
    Update_event_system();
	}
}
