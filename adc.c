#include <avr/io.h>

#include "adc.h"

#define MASKBITS 0x0f

void adc_init(unsigned char channel)
{
    // Initialize the ADC
	ADMUX |= (1 << 6);
	ADMUX &= ~(1 << 7);
	
	ADMUX &= ~MASKBITS;
	ADMUX |= (channel & MASKBITS);
	
	ADCSRA |= 0x07;
	ADMUX |= (1 << 5);
	ADCSRA |= (1 << 7);
	
	ADMUX &= ~(1<<4);
	ADCSRA &= 0x87;

}

unsigned uint8_t adc_sample()
{
    // Convert an analog input and return the 8-bit result
	ADCSRA |= (1 << 6);
	
	while(( ADCSRA & (1 << 6)) != 0)
	{}

	uint8_t result = ADCH;
	
	return result;
	
}
