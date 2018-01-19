#ifndef ABOX_EVENT_SYSTEM_HEADER
#define ABOX_EVENT_SYSTEM_HEADER
#include "Events.h"
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
unsigned char events[1], state_table[1], level;
inline void Start_single_tap_feedback() {
  Times_of_1ms[Timer_Feedback] = 0x46;
  Send_start_feedback();
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
      Set_display_max7219(0, 0x00);
			break;
		case 0x01:
		case 0x04:
      Set_display_max7219(1, 0x00);
			break;
		case 0x02:
		case 0x06:
      Set_display_max7219(2, 0x00);
			break;
		}
		switch (stable_state) {
		case 0x01:
      Set_display_max7219(1, 0x33);
			break;
		case 0x02:
      Set_display_max7219(2, 0xcc);
			break;
		case 0x03:
      Set_display_max7219(0, 0xcc);
			break;
		case 0x04:
      Set_display_max7219(1, 0xcc);
			break;
		case 0x05:
      Set_display_max7219(0, 0x33);
			break;
		case 0x06:
      Set_display_max7219(2, 0x33);
			break;
		}
		state_table[0] &= 0xf0;
		state_table[0] |= stable_state;
    Start_single_tap_feedback();
  }
}
//Button: Press level changed
void Process_button_press_level_changed_events() {
	switch (Get_input_level()) {
   Press_level_press:
		if ((state_table[0] & 0xf0) == 0x00) {
      Start_single_tap_feedback();
		}
    break;
	 Press_level_1:
   Press_level_2:
    Start_single_tap_feedback();
	}
}
void Check_power_stat() {
  if (battery_charging == 0) {
    Set_display_max7219(0, 0x5B);
    Set_display_max7219(1, 0xF8);
    Set_display_max7219(2, 0x40);
  }else if (battery_full == 0) {
    Set_display_max7219(0, 0xFF);
    Set_display_max7219(1, 0xFF);
    Set_display_max7219(2, 0xFF);
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
    Set_display_max7219(0, ADC_RES);
    Set_display_max7219(1, 0x00);
    Set_display_max7219(2, 0x00);
  }
}
inline void Long_feedback() {
  feedback = 0;
  Delay_38ms();
  Delay_38ms();
	feedback = 1;
}
void Shutdown() {
	__bit Restart = Failed;
	//Turn everything off
	Write_max7219(0x0c, 0x00);
	Write_mpu6050(0x6b, 0x48);
	_rasing_edge = 0;
  Long_feedback();
	while (Restart != Successed) {
		Stop_1ms_timer();
		PCON = 0x02;
		_nop_();
    if(input_is_falling_edge() == TRUE) continue;
		if (input_is_rasing_edge() == TRUE) {
			Start_1ms_timer();
			Times_of_1ms_long[Timer_Long_Press] = Press_level_restart_time;
			while (input_pin == 0) {
				 if (Times_of_1ms_long[Timer_Long_Press] == 0x0000) {
					 Restart = Successed;
					 break;
				 }
			}
		}
	}
  Long_feedback();
	//Return to normal
	Write_max7219(0x0c, 0x01);
	Write_mpu6050(0x6b, 0x00);
}
//Button: Falling edge
void Process_button_falling_edge_events() {
	//Press
	switch (Get_input_level()) {
   Press_level_press:
		if ((state_table[0] & 0xf0) == 0x00) {
			if ((state_table[0] & 0x0f) == 0x00) {
				state_table[0] |= 0x01;
				events[0] |= 0x08;
			}else {
				state_table[0] &= 0xf0;
				events[0] |= 0x04;
			}
		}
    break;
   Press_level_1:
		if ((state_table[0] & 0xf0) == 0x00) {
			state_table[0] = 0x10;
      Set_display_max7219(0, 0x00);
      Set_display_max7219(1, 0x00);
      Set_display_max7219(2, 0x00);
			level &= 0x0f;
		}else {
			state_table[0] &= 0x0f;
	//Process led state
      if ((state_table[0] & 0x0f) == 0x00) events[0] |= 0x04;
      else events[0] |= 0x08;
    }
    break;
  //Level 2 press
   Press_level_2:
    Check_power_stat();
  }
  Set_input_level(0x0);
}
void Process_LED_events() {
	//Mode 0
	if ((state_table[0] & 0xf0) == 0x00) {
		if ((events[0] & 0x04) != 0x00) {
			events[0] &= 0xfb;
      Set_display_max7219(0, 0x00);
      Set_display_max7219(1, 0x00);
      Set_display_max7219(2, 0x00);
		}
		if ((events[0] & 0x08) != 0x00) {
			events[0] &= 0xf7;
      Set_display_max7219(0, 0xff);
      Set_display_max7219(1, 0xff);
      Set_display_max7219(2, 0xff);
			events[0] |= 0xe0;
		}
	}
	//Mode 1
	if ((state_table[0] & 0xf0) == 0x10) {
		if (Times_of_1ms[Timer_Delay] == 0x00) {
			Times_of_1ms[Timer_Delay] = 0x0f;
			if ((level & 0xf0) == 0x00) Mode_1_processing(0x0200);
			else Mode_1_processing(0x7ffe);
		}
	}
}

//Button pressed
void Process_press_button() {
	//Rasing edge
	if (input_is_rasing_edge() == TRUE) {
    Send_input_level_change();
		Set_input_level(Press_level_press);//Button level: Press
		Times_of_1ms_long[Timer_Long_Press] = Press_level_1_time;
	}
	//Level 1
	if (Get_input_level() == Press_level_press && Times_of_1ms_long[Timer_Long_Press] == 0x0000) {
    Send_input_level_change();
		Set_input_level(Press_level_1);//Button level: Press
		Times_of_1ms_long[Timer_Long_Press] = Press_level_2_time;
	}
  //Level 2
	if (Get_input_level() == Press_level_1 && Times_of_1ms_long[Timer_Long_Press] == 0x0000) {
    Send_input_level_change();
		Set_input_level(Press_level_2);//Button level: Level 2
		Times_of_1ms_long[Timer_Long_Press] = Press_level_shutdown_time;
	}
	//Level shutdown
	if (Get_input_level() == Press_level_2 && Times_of_1ms_long[Timer_Long_Press] == 0x0000) {
    Set_input_level(0x0);
		Shutdown();
	}
}
void Process_feedback() {
  if (Pop_start_feedback() == TRUE) {
    Turn_on_feedback();
  }
  if (Feedback_status() == ON && Times_of_1ms[Timer_Feedback] == 0x00) Turn_off_feedback();
}
void Update_event_system() {
  if (input_pin == 0) Process_press_button();
	if (Pop_input_level_change() == TRUE) {
		Process_button_press_level_changed_events();
	}
	if (input_is_falling_edge() == TRUE) {
		Process_button_falling_edge_events();
	}
	Process_LED_events();
  Process_feedback();
}
#endif
