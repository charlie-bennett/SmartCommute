#include <avr/io.h>
#include <stdio.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include "gps.h"
#include "serial.h"
#define GPS_LENGTH 120
#define GPS_BUFFER_SIZE 4

static char prefix[6] = "$GPRMC";
uint i = 0;
char temp_input;

volatile char GPS_DATA_BUFFER [GPS_BUFFER_SIZE][GPS_LENGTH];
volatile uint8_t GPS_BUFF_LOC[GPS_BUFFER_SIZE];
volatile uint8_t GPS_RX_FLAG;
volatile uint8_t GPS_WP;
volatile uint8_t FIFO_SIZE;
volatile char current_comp[6];
volatile uint8_t invalid_flag;

volatile int8_t buzzerflag = -1;
volatile int8_t readingBluetooth = 0;



uint8_t is_coordinates(char* input)
{
	for (i = 0; i < 6; i++)
	{
		if (prefix[i] != input[i]) return 0;
	}
	return 1;
}

void gps_init()
{
	invalid_flag = 0;
	FIFO_SIZE = 0;
	serial_init(ubrr); //TODO : Andrew set mux bit
	for (i = 0; i < GPS_BUFFER_SIZE; i++) GPS_WB[i] = 0;
	GPS_WP = 0;
	GPS_RX_FLAG = 0;
	UCSR0B |= (1 << RXCIE0); // Enable Receive interrupt
	sei();
}

void pollGPS(char temp_input)
{
	if (!invalid_flag) GPS_DATA_BUFFER[GPS_WP % GPS_BUFFER_SIZE][GPS_BUFF_LOC[GPS_WP]++] = temp_input;
	if (GPS_BUFF_LOC[GPS_WP % GPS_BUFFER_SIZE] == 6)
	{
		if (!is_coordinates(GPS_DATA_BUFFER[GPS_WP % GPS_BUFFER_SIZE])) //TODO
		{
			GPS_BUFF_LOC[GPS_WP % GPS_BUFFER_SIZE] = 0; //reset
			invalid_flag == 1;
		}
	}
	if (temp_input == '\n')
	{

		if (!invalid_flag)
		{
			GPS_WP++;
			if (FIFO_SIZE < GPS_BUFFER_SIZE) FIFO_SIZE++;
		}
		else invalid_flag = 0;


	}
	if (GPS_BUFF_LOC[GPS_WP % GPS_BUFFER_SIZE] == GPS_LENGTH)
	{
		GPS_BUFF_LOC[GPS_WP % GPS_BUFFER_SIZE]--;
	}

	return;
}

char* popGPS()
{
	if (FIFO_SIZE == 0) return NULL;
	return GPS_DATA_BUFFER[(GPS_WP + GPS_BUFFER_SIZE - (FIFO_SIZE--)) % GPS_BUFFER_SIZE];
}


ISR(USART_RX_vect)
{
	char c = UDR0;

	if(readingBluetooth == 0)
    {
        if(c == "D")
        {
            buzzerflag = 0;
        }
    }

    else
    {
    	pollGPS(c);
    }
}

