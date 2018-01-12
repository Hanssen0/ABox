#ifndef ABOX_DELAY_HEADER
#define ABOX_DELAY_HEADER
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
