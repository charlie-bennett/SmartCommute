#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "auxillary_io.c"
#include "i2c.c"


volatile int vol_up = 0;
volatile int vol_down = 0;
volatile int skip = 0;
volatile int pause = 0;


volatile unsigned char input;
volatile char bluetoothbuffer[]
volatile char lcdbuffer[]

volatile unsigned int millisecondsElapsed = 0;
volatile unsigned int capturemillisU = 0;
volatile unsigned int capturemillisD = 0;
volatile unsigned int capturemillisS = 0;


//LCD (half duplex) , BLUETOOTH, GPS (half duplex)


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

}

void setup() {
	// Enable Timer Interrupt
	TIMSK1 |= (1 << OCIE1A);
	// with a factor of 8 and timer count up to 2000, emulates a millisecond timer
	OCR1A = 2000; 
	TCCR1B |= (1 << CS11);

	//all ports B are inputs
	DDRB |= 0x00;
	PORTB &= 0x00;

	DDRC |= 0x04;
}

/*read z and y axis to find the speed, assuming low power mode on Accelerometer */
double readAccelerometer(){
	uint8_t statusy = i2c_io(0x31, 0x2C, 2,  )
	uint8_t statusz = i2c_io(0x31, 0x2A, 2,  )
}


# define FOSC 7372800
# define BDIV ( FOSC / 100000 - 16) / 2 + 1

sei()
setup();
i2c_init ( BDIV );
serial_init(3);
ADC_init();

int main(void)
{
	double yaxis = 0;
	double zaxis = 0;
	while(1) {
		//check flags for button pushes
		if(vol_up == 0){
			//send data through bluetooth module
			vol_up = 1;
		}
		if(vol_down == 0) {
			//send data through bluetooth module
			vol_down = 1;
		}
		if(skip == 0) {
			//send data through bluetooth module
			skip = 1;
		}

		//read accelerometer
	}

}


ISR(TIMER1_COMPA_vect)
{
	input = PINB;
	if( input & (1) == 0) {
		capturemillisU = capturemillisU + 1;
		if(capturemillisU >= 20){
			vol_up = 0;
			capturemillisU = 0;
		}
	}
	else {
		vol_up = 1;
		capturemillisU = 0;
	}

	if( input & (1 << 1) == 0) {
		capturemillisD = capturemillisD + 1;
		if(capturemillisD >= 20) {
			vol_down = 0;
			capturemillisD = 0;
		}
	}
	else {
		vol_down = 1;
		capturemillisD = 0;
	}

	if( input & (1 << 2) == 0) {
		capturemillisS = capturemillisS + 1;
		if(capturemillisS >= 40){
			skip = 0;
			capturemillisS = 0;	
		}

	}
	else {
		if(capturemillisS >= 20){
			pause = 0;
			skip = 1;
		}
		else{
			skip = 1;
			pause = 1;
		}
		capturemillisS = 0;
	}
}