//STC15F2K60S2, 20.000MHz
__sbit __at (0x80) max7219_din;
__sbit __at (0x81) max7219_load;
__sbit __at (0x82) max7219_clk;
__sbit __at (0xB2) input_pin;
__bit input_rasing_edge, input_falling_edge;
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
__bit is_feedback_on;
unsigned char Timer_1ms_count_down[2];
//-Count down:
//--0.Feedback
//--1.Processs acceleration (Mode 1)
unsigned int Timer_1ms_count_down_long[1];
//-Count down long:
//--0.Press
enum {
	Press_level_none = 0x00,
	Press_level_press = 0x01,
	Press_level_1 = 0x02,
	Press_level_2 = 0x03
};
enum {
	Press_level_1_time = 0x1F4, //500
  Press_level_2_time = 0x1F4, //500
	Press_level_shutdown_time = 0xFA0, //4000
	Press_level_restart_time = 0x1388 //5000
};
unsigned char events[1], state_table[1], display_matrix[3], level;
//-Events:
//--0.|LED digital 1 changed|LED digital 2 changed|LED digital 3 changed|Feedback|
//--              7                     6                     5             4          
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
//Send 8 bits
void Send_char_max7219(unsigned char dat) {
	char loop = 0;
	for(; loop<8; ++loop) {
		max7219_clk = 0;
		max7219_din = (__bit)(dat & 0x80);
		dat <<= 1;
		max7219_clk = 1;
	}
}
//Write a register
void Write_max7219(unsigned char address, unsigned char dat) {
	max7219_load = 0;
	Send_char_max7219(address);
	Send_char_max7219(dat);
	max7219_load = 1;
}
//Start / Stop I2c;
#define I2c_start() {sda=1;Delay3us();scl=1;Delay3us();sda=0;Delay3us();scl=0;Delay3us();}
#define I2c_end() {sda=0;Delay3us();scl=1;Delay3us();sda=1;Delay3us();}
//Send 8 bits via I2c
__bit I2c_send_char(unsigned char dat) {
	unsigned char loop = 0;
	__bit is_ack = 0;
	for (; loop < 8; ++loop) {
		sda = (__bit)((dat << loop) & 0x80);
		Delay3us();
		scl = 1;
		Delay3us();
		scl = 0;
		Delay3us();
	}
	sda = 1;
	Delay3us();
	scl = 1;
	Delay3us();
	while (++loop < 0xff) {
		if (sda == 0) {
			is_ack = 1;
			break;
		}
	}
	scl = 0;
	Delay3us();
	return is_ack;
}
//Receive 8 bits via I2c
unsigned char I2c_receive_char(__bit is_need_ack) {
	unsigned char dat = 0x00, loop = 0;
	sda = 1;
	Delay3us();
	for (; loop < 8; ++loop) {
		scl = 1;
		Delay3us();
		dat |= ((unsigned char)sda << (7 - loop));
		Delay3us();
		scl = 0;
		Delay3us();
	}
	sda = is_need_ack ^ 1;
	Delay3us();
	scl = 1;
	Delay3us();
	scl = 0;
	Delay3us();
	return dat;
}
//Write mpu6050
__bit Write_mpu6050(unsigned char address, unsigned char dat) {
	__bit is_succeeded = 1;
	I2c_start();
	if (I2c_send_char(0xd0) == 0) is_succeeded = 0;
	if (I2c_send_char(address) == 0) is_succeeded = 0;
	if (I2c_send_char(dat) == 0) is_succeeded = 0;
	I2c_end();
	return is_succeeded;
}
//Read mpu6050
__bit Read_mpu6050(unsigned char address, unsigned int * dat) {
	__bit is_succeeded = 1;
	I2c_start();
	if (I2c_send_char(0xd0) == 0) is_succeeded = 0;
	if (I2c_send_char(address) == 0) is_succeeded = 0;
	I2c_start();
	if (I2c_send_char(0xd1) == 0) is_succeeded = 0;
	*dat = I2c_receive_char(1) << 8;
	*dat |= I2c_receive_char(0);
	I2c_end();
	return is_succeeded;
}
//Interrupt-function of button 
void Input_signal_edge() __interrupt 0 {
	if (input_pin == 0) input_rasing_edge = 1;
	else input_falling_edge = 1;
}
//Stop 1msTimer
#define Stop_1ms_timer() AUXR &= 0xef,Timer_1ms_count_down_long[0]=0x0000,Timer_1ms_count_down[0]=0x00,Timer_1ms_count_down[1]=0x00;
//Start 1msTimer
void Start_1ms_timer()
{
	Stop_1ms_timer();
	T2L = 0xe0;
	T2H = 0xb1;
	AUXR |= 0x10;
}
//Interrupt-function of 1msTimer
void Count_1ms() __interrupt 12 {
	if (Timer_1ms_count_down_long[0] != 0x0000) --Timer_1ms_count_down_long[0];
	if (Timer_1ms_count_down[0] != 0x00) --Timer_1ms_count_down[0];
	if (Timer_1ms_count_down[1] != 0x00) --Timer_1ms_count_down[1];
}
//Initialize everything (chips,input and timers)
void Init() {
	//----------Initialize framework----------
	events[0] = 0x00;
	state_table[0] = 0x00;
	display_matrix[0] = 0x00;
	display_matrix[1] = 0x00;
	display_matrix[2] = 0x00;
	events[0] |= 0xe0;//Initialize display
	Timer_1ms_count_down[0] = 0x00;
	Timer_1ms_count_down[1] = 0x00;
	level = 0;
	//----------Initialize max7219----------
	Write_max7219(0x01, 0x00);//---
	Write_max7219(0x02, 0x00);//Turn off leds
	Write_max7219(0x03, 0x00);//---
	Write_max7219(0x09, 0x00);//No decode
	Write_max7219(0x0a, 0x0f);//Max intensity
	Write_max7219(0x0b, 0x02);//3 line of leds
	Write_max7219(0x0f, 0x00);//Turn off test mode
	//----------Initialize I2c----------
	sda = 1;
	scl = 1;
	//----------Initialize mpu6050----------
	Write_mpu6050(0x6b, 0x48);
	Write_mpu6050(0x19, 0x07);
	Write_mpu6050(0x1A, 0x06);
	Write_mpu6050(0x1B, 0x18);
	Write_mpu6050(0x1C, 0x01);
	//----------Initialize input----------
  P3M1 = 0x04;
	input_rasing_edge = 0;
	input_falling_edge = 0;
	IT0 = 0;
	EX0 = 1;
	//----------Initialize feedback----------
	feedback = 1;
  is_feedback_on = 0;
	//----------Initialize 1ms timer----------
	AUXR |= 0x04;
	IE2 |= 0x04;
	EA = 1;
  //----------Initialize ADC----------
  P1ASF = 0x80;
  ADC_RES = 0x00;
}
//Shutdown Everything
void Shutdown() {
	__bit Restart = 0;
	//Turn everything off
	Write_max7219(0x0c, 0x00);
	Write_mpu6050(0x6b, 0x48);
	input_rasing_edge = 0;
	//Feedback
	feedback = 0;
  Delay_38ms();
  Delay_38ms();
	feedback = 1;
	while (Restart != 1) {
		Stop_1ms_timer();
		PCON = 0x02;
		_nop_();
		if (input_falling_edge == 1) input_falling_edge = 0;
		if (input_rasing_edge == 1) {
			input_rasing_edge = 0;
			Start_1ms_timer();
			Timer_1ms_count_down_long[0] = Press_level_restart_time;
			while (input_pin == 0) {
				 if (Timer_1ms_count_down_long[0] == 0x0000) {
					 Restart = 1;
					 break;
				 }
			}
		}
	}
	//Feedback
	feedback = 0;
  Delay_38ms();
  Delay_38ms();
	feedback = 1;
	//Return to normal
	Write_max7219(0x0c, 0x01);
	Write_mpu6050(0x6b, 0x00);
}
void Mode_1_processing(unsigned int critical_value) {
	unsigned int dat, max;
	unsigned char stable_state = 0x01;
	Read_mpu6050(0x3b, &dat);
	if (dat < 0x8000) {
		max = dat;
	}else {
		max = (0xffff - dat);
		stable_state = 0x02;
	}
	Read_mpu6050(0x3d, &dat);
	if (dat < 0x8000) {
		if (dat > max) {
			max = dat;
			stable_state = 0x03;
		}
	}else {
		if ((0xffff - dat) > max) {
			max = (0xffff - dat);
			stable_state = 0x04;
		}
	}
	Read_mpu6050(0x3f, &dat);
	if (dat < 0x8000) {
		if (dat > max) {
			max = dat;
			stable_state = 0x05;
		}
	}else {
		if ((0xffff - dat) > max) {
			max = (0xffff - dat);
			stable_state = 0x06;
		}
	}
	if (max >= critical_value && stable_state != (state_table[0] & 0x0f)) {
    if ((level & 0xf0) == 0x00) level |= 0x10;
		switch (state_table[0] & 0x0f) {
		case 0x03:
		case 0x05:
			display_matrix[0] = 0x00;
			events[0] |= 0x80;
			break;
		case 0x01:
		case 0x04:
			display_matrix[1] = 0x00;
			events[0] |= 0x40;
			break;
		case 0x02:
		case 0x06:
			display_matrix[2] = 0x00;
			events[0] |= 0x20;
			break;
		}
		switch (stable_state) {
		case 0x01:
			display_matrix[1] = 0x33;
			events[0] |= 0x40;
			break;
		case 0x02:
			display_matrix[2] = 0xcc;
			events[0] |= 0x20;
			break;
		case 0x03:
			display_matrix[0] = 0xcc;
			events[0] |= 0x80;
			break;
		case 0x04:
			display_matrix[1] = 0xcc;
			events[0] |= 0x40;
			break;
		case 0x05:
			display_matrix[0] = 0x33;
			events[0] |= 0x80;
			break;
		case 0x06:
			display_matrix[2] = 0x33;
			events[0] |= 0x20;
			break;
		}
		state_table[0] &= 0xf0;
		state_table[0] |= stable_state;
		Timer_1ms_count_down[0] = 0x1e;//Feedback time: 50ms
		events[0] |= 0x10;
	}
}
//Button: Press level changed
void Process_button_press_level_changed_events() {
	//Press button
	if ((level & 0x0f) == Press_level_press) {
		if ((state_table[0] & 0xf0) == 0x00) {
			Timer_1ms_count_down[0] = 0x1e;//Feedback time: 50ms
			events[0] |= 0x10;
		}
	}
	//Level 1 press
	if ((level & 0x0f) == Press_level_1) {
		Timer_1ms_count_down[0] = 0x1e;//Feedback time: 50ms
		events[0] |= 0x10;
	}
  //Level 2 press
	if ((level & 0x0f) == Press_level_2) {
		Timer_1ms_count_down[0] = 0x1e;//Feedback time: 50ms
		events[0] |= 0x10;
	}
}
void Check_power_stat() {
  if (battery_charging == 0) {
    display_matrix[0] = 0x5B, display_matrix[1] = 0xF8, display_matrix[2] = 0x40;
    events[0] |= 0xe0;
  }else if (battery_full == 0) {
    display_matrix[0] = 0xFF, display_matrix[1] = 0xFF, display_matrix[2] = 0xFF;
    events[0] |= 0xe0;
  }else {
    ADC_CONTR = 0xE0;
    Delay_38ms();
    ADC_CONTR = 0xE8;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    while ((ADC_CONTR & 0x10) == 0);
    ADC_CONTR = 0x00;
    display_matrix[0] = ADC_RES, display_matrix[1] = 0x00, display_matrix[2] = 0x00;
    events[0] |= 0xe0;
  }
}
//Button: Falling edge
void Process_button_falling_edge_events() {
	//Press
	if ((level & 0x0f) == Press_level_press) {
	//When in mode 0
		if ((state_table[0] & 0xf0) == 0x00) {
			if ((state_table[0] & 0x0f) == 0x00) {
				state_table[0] |= 0x01;
				events[0] |= 0x08;
			}else {
				state_table[0] &= 0xf0;
				events[0] |= 0x04;
			}
		}
	}
	//Level 1 press
	if ((level & 0x0f) == Press_level_1) {
	//Change mode
		if ((state_table[0] & 0xf0) == 0x00) {
			state_table[0] = 0x10;
			display_matrix[0] = 0x00, display_matrix[1] = 0x00, display_matrix[2] = 0x00;
			events[0] |= 0xe0;
			level &= 0x0f;
		}else {
			state_table[0] &= 0x0f;
	//Process led state
      if ((state_table[0] & 0x0f) == 0x00) events[0] |= 0x04;
      else events[0] |= 0x08;
    }
	}
  //Level 2 press
  if ((level & 0x0F) == Press_level_2) {
    Check_power_stat();
  }
	level &= 0xf0;
}
void Process_LED_events() {
	//Mode 0
	if ((state_table[0] & 0xf0) == 0x00) {
		if ((events[0] & 0x04) != 0x00) {
			events[0] &= 0xfb;
			display_matrix[0] = 0x00, display_matrix[1] = 0x00, display_matrix[2] = 0x00;
			events[0] |= 0xe0;
		}
		if ((events[0] & 0x08) != 0x00) {
			events[0] &= 0xf7;
			display_matrix[0] = 0xff, display_matrix[1] = 0xff, display_matrix[2] = 0xff;
			events[0] |= 0xe0;
		}
	}
	//Mode 1
	if ((state_table[0] & 0xf0) == 0x10) {
		if (Timer_1ms_count_down[1] == 0x00) {
			Timer_1ms_count_down[1] = 0x0f;
			if ((level & 0xf0) == 0x00) Mode_1_processing(0x0200);
			else Mode_1_processing(0x7ffe);
		}
	}
}
//Events processing
void Process_events() {
	//--Press level changed--
	if ((events[0] & 0x01) != 0x00) {
		events[0] &= 0xfe;
		Process_button_press_level_changed_events();
	}
	//-Falling edge-
	if ((events[0] & 0x02) != 0x00) {
		events[0] &= 0xfd;
		Process_button_falling_edge_events();
	}
	Process_LED_events();
}
//Button pressed
void Process_press_button() {
	//Rasing edge
	if (input_rasing_edge == 1) {
		input_rasing_edge = 0;
		events[0] |= 0x01;//Event: Level changed
		level &= 0xf0;
		level |= Press_level_press;//Button level: Press
		Timer_1ms_count_down_long[0] = Press_level_1_time;
	}
	//Level 1
	if ((level & 0x0F) == Press_level_press && Timer_1ms_count_down_long[0] == 0x0000) {
		events[0] |= 0x01, //Event: Level changed
		level &= 0xf0;
		level |= Press_level_1;//Button level: Level 1
		Timer_1ms_count_down_long[0] = Press_level_2_time;
	}
  //Level 2
	if ((level & 0x0F) == Press_level_1 && Timer_1ms_count_down_long[0] == 0x0000) {
		events[0] |= 0x01, //Event: Level changed
		level &= 0xf0;
		level |= Press_level_2;//Button level: Level 2
		Timer_1ms_count_down_long[0] = Press_level_shutdown_time;
	}
	//Level shutdown
	if ((level & 0x0F) == Press_level_2 && Timer_1ms_count_down_long[0] == 0x0000) {
		level &= 0xf0;//Button level: None
		Shutdown();
	}
}
//Button released
void Process_release_button() {
	//Falling edge
	if (input_falling_edge == 1) {
		events[0] |= 0x02;
		input_falling_edge = 0;
	}
}
void Process_LED_changed() {
	if ((events[0] & 0x80) != 0x00) {
		events[0] &= 0x7f;
		Write_max7219(0x01, display_matrix[0]);
	}
	if ((events[0] & 0x40) != 0x00) {
		events[0] &= 0xbf;
		Write_max7219(0x02, display_matrix[1]);
	}
	if ((events[0] & 0x20) != 0x00) {
		events[0] &= 0xdf;
		Write_max7219(0x03, display_matrix[2]);
	}
}
void Delay_12ms(unsigned int k){
	_nop_();
	_nop_();
  while (--k);
}
void main() {
  unsigned char del = 1;
  unsigned int co = 2400;
  while (1 == 1) {
    feedback = 0;
    Delay_12ms(co);
    feedback = 1;
    Delay_12ms(co);
    if (del == 200) {
      del = 0;
      Delay_38ms();
      Delay_38ms();
      Delay_38ms();
      Delay_38ms();
      Delay_38ms();
      Delay_38ms();
      Delay_38ms();
      Delay_38ms();
    }
    ++del;
  }
	Init();
	Write_max7219(0x0c, 0x01);
	Write_mpu6050(0x6b, 0x00);
	//Work with event queue
	Start_1ms_timer();
	while (1==1) {
		//-Hardwares-
		//--Button--
		if (input_pin == 0) Process_press_button();
		if (input_pin == 1) Process_release_button();
		//--LED--
		Process_LED_changed();
		//--Feedback--
		//---Start feedback---
		if ((events[0] & 0x10) != 0x00) {
			events[0] &= 0xef;
			feedback = 0;
      is_feedback_on = 1;
		}
		//---Stop feedback---
		if (is_feedback_on == 1 && Timer_1ms_count_down[0] == 0x00) feedback = 1, is_feedback_on = 0;
		//-Events-
		Process_events();
	}
}
