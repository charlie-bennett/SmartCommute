#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


volatile int vol_up = 0;
volatile int vol_down = 0;
volatile int skip = 0;
/*
serial_out - Output a byte to the USART0 port
*/
void serial_out ( char ch )
{
	while (( UCSR0A & (1 << UDRE0 )) == 0);
	UDR0 = ch ;
}
/*
serial_in - Read a byte from the USART0 and return it
*/
char serial_in ()
{
	while ( !( UCSR0A & (1 << RXC0 )) );
	return UDR0 ;
}


/*
serial_init - Initialize the USART port
*/
void serial_init ( unsigned short ubrr ) {
UBRR0 = ubrr ; // Set baud rate
UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
UCSR0B |= (1 << RXEN0 ); // Turn on receiver
UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity ,
// one stop bit , 8 data bits
serial_out("$")
serial_out("$")
serial_out("$")

}



//That was the consonant "m"


int main(void)
{
	sei()
	//all ports B are inputs
	DDRB |= 0x00;
	PORTB &= 0x00;

	//serial_init(3);
	while(1) {
		//check flags for button pushes
		if(vol_up == 0){
			//send data through bluetooth module
		}
		if(vol_down == 0) {
			//send data through bluetooth module
		}
		if(skip == 0) {
			//send data through bluetooth module
		}
		serial_out('U');
		_delay_ms(10);
	}

}


ISR(PCINT1_vect) {
	input = PINB;
	if( input & (1 << 1) == 0) {
		vol_up = 0;
	}
	else {vol_up = 1;}

	if( input & (1 << 2) == 0) {
		vol_down = 0;
	}
	else {vol_down = 1;}

	if( input & (1 << 3) == 0) {
		skip = 0;
	}
	else {skip = 1;}
}