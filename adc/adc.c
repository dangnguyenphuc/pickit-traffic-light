#include "adc.h"

void init_adc(void)
{
	TRISA = 0x0f;
	ADCON1 = 0x0b;
	ADCON2 = 0x04;
}

int get_adc_value(unsigned char channel)
{
    int result;
    ADCON0 = channel << 2;
	ADCON0bits.ADON = 1;
	ADCON0bits.GO_DONE = 1;
	while(ADCON0bits.GO_DONE ==1);
	result = ADRESH;
	result = (result<<2) + (ADRESL>>6);
	return result;
}

