main.ihx: main.c
	sdcc --std-sdcc99 main.c
flash:
	sudo stcgal main.ihx
flash_trim:
	sudo stcgal -t 20000 main.ihx
