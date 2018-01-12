main.ihx: main.c STC15Pins.h ConstValue.h I2C.h Delay.h MAX7219.h MPU6050.h
	sdcc --std-sdcc99 main.c
flash:
	sudo stcgal main.ihx
flash_trim:
	sudo stcgal -t 20000 main.ihx
