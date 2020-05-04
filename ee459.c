#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "auxillary_io.c"
#include "i2c.c"
#include "FIFO.c"


volatile uint8_t vol_up = 1;
volatile uint8_t vol_down = 1;
volatile uint8_t skip = 1;
volatile uint8_t pause = 1;
volatile uint8_t printBPM = 1;
volatile uint8_t sendlight = 1;
volatile uint8_t readTemp = 1;
volatile uint8_t findspeed = 1;



volatile unsigned char input;
volatile char bluetoothbuffer[]
volatile char lcdbuffer[]

volatile uint8_t millisecondsElapsed = 0;
volatile uint8_t secondsElapsed = 0;
volatile uint8_t capturemillisU = 0;
volatile uint8_t capturemillisD = 0;
volatile uint8_t capturemillisS = 0;
volatile uint8_t bpm = 0;
uint8_t lastTemp = 0;

volatile struct FIFO lightFIFO = new_FIFO();



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


void sendLCD() {

}

void sendBluetooth() {

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
	TCCR1B |= (1 << WGM12);
	// Enable Timer Interrupt
	TIMSK1 |= (1 << OCIE1A);
	// with a factor of 8 and timer count up to 2000, emulates a millisecond timer
	OCR1A = 62; 
	TCCR1B |= (1 << CS12);

	//all ports B are inputs
	DDRB |= 0x00;
	PORTB &= 0x00;


	TCCR0A |= (1 << WGM01);
	TIMSK0 |= (1 << OCIE0A);
	OCR0A = 62500;


	DDRC |= 0x04;
}

/*read z and y axis to find the speed, assuming low power mode on Accelerometer */
float readAccelerometer(){
	uint8_t xval, yval, zval;
	uint8_t statusx = i2c_io(0x31, 0x29, 2, NULL, 0, &xval, 8)
	uint8_t statusy = i2c_io(0x31, 0x2B, 2, NULL, 0, &yval, 8)
	uint8_t statusz = i2c_io(0x31, 0x2D, 2, NULL, 0, &zval, 8)
	
}

uint16_t powi(input, n)
{
	uint16_t i  = 0; 
	uint16_t output = 1; 
	for (i = 0; i<n; i++) output*=input; 
	return output; 
}

double unit82double(unit8 input, char perc, char length)
{
	char i  = 0; 
	double output =0; 
	for (i  =0; i<length; i++) output+=input[i]*powi(2, i-perc); 
	return output; 
}


# define FOSC 7372800
# define BDIV ( FOSC / 100000 - 16) / 2 + 1

sei()
setup();
i2c_init ( BDIV );
serial_init(3);

int main(void)
{
	const uint16_t threshold = 550;

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

		if(printBPM == 0) {
			//send data to LCD, bluetooth module
			bpm = 0;
			printBPM = 1;
		}

		//ACCELEROMETER READ AND DISPLAY HERE
		if(findspeed == 0){

			findspeed = 1;
		}

		if(sendlight == 0) {

			adc_init(0X01);
			uint8_t rawvalue = adc_sample();
			new_FIFOnode(rawvalue, lightFIFO);
			float avg = calc_average(lightFIFO);
			//send Data to bluetooth module
			sendlight = 1;
		}

		if(readTemp == 0) {
			adc_init(0x00);
			uint8_t rawvalue = adc_sample();
			if(rawvalue > lastTemp){
				if(rawvalue - lastTemp >= 3) {
					//do something
				}
			}
			else {
				if(lastTemp - rawvalue >= 3) {
					//do something
				}
			}
			lastTemp = rawvalue;
			//send Data to bluetooth module
			readTemp = 1;
		}

		//read heartbeatsensor (heartbeat detected)
		adc_init(0x03);
		uint8_t rawvalue = adc_sample();
		if(rawvalue > threshold){
			bpm += 1;
		}

	}

}

//every millisecond
ISR(TIMER1_COMPA_vect)
{
	millisecondsElapsed += 1;

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

	if(millisecondsElapsed == 30){
		findspeed = 0;
		millisecondsElapsed = 0;
	}

}

//every second
ISR(TIMER1_COMPA_vect){

	secondsElapsed += 1;
	printBPM = 0;

	//TODO: DOUBLECHECK THIS PART
	sendlight = 0;

	if(secondsElapsed == 60){
		readTemp = 0;
		secondsElapsed = 0;
	}

}