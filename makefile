OUTPUT_PATH = ./Output/
main.ihx: main.c STC15Pins.h ConstValue.h I2C.h Timer.h MAX7219.h MPU6050.h
	mkdir -p $(OUTPUT_PATH)
	sdcc --std-sdcc99 main.c -o $(OUTPUT_PATH)
flash:
	sudo stcgal $(OUTPUT_PATH)main.ihx
flash_trim:
	sudo stcgal -t 20000 $(OUTPUT_PATH)main.ihx
