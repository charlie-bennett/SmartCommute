#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "accelerometer.h"
#include "gps.h"
#include "adc.h"
#include "i2c.h"
#include "lightsensor.h"
#include "serial.h"


volatile uint8_t vol_up = 1;
volatile uint8_t vol_down = 1;
volatile uint8_t skip = 1;
volatile uint8_t pause = 1;
volatile uint8_t printBPM = 1;
volatile uint8_t sendlight = 1;
volatile uint8_t readTemp = 1;
volatile uint8_t findspeed = 1;
volatile uint8_t checkGPS = 1;
volatile uint8_t pollSpeed = 1;
volatile uint8_t poll_light = 1;



volatile unsigned char input;
volatile char* message;
char cfloat[10];

volatile uint8_t millisecondsElapsed = 0;
volatile uint8_t secondsElapsed = 0;
volatile uint8_t minutesElapsed = 0;
volatile uint8_t capturemillisU = 0;
volatile uint8_t capturemillisD = 0;
volatile uint8_t capturemillisS = 0;
volatile uint8_t bpm = 0;
uint8_t lastTemp = 0;

uint8_t HEARTBEAT_STATE;
float LS_mavg;
float AC_mavg[3];



//LCD (half duplex) , BLUETOOTH, GPS (half duplex)


void serial_wait()
{
	//check to see if still transmitting, wait till finished transmitting
	while ((UCSR0A & (1 << TXC0)) == 1);
}

void sendLCD()
{
	//set port D to mux 0
	PORTD &= !(1 << 4);
	uint8_t i = 0;
	while (message[i] != '\0')
	{
		serial_out(message[i++]);
	}

}

void sendBluetooth()
{
	//set port D to mux 1
	PORTD |= (1 << 4);
	uint8_t i = 0;
	while (message[i] != '\0')
	{
		serial_out(message[i++]);
	}
}


void setup()
{
	TCCR1B |= (1 << WGM12);
	// Enable Timer Interrupt
	TIMSK1 |= (1 << OCIE1A);
	// with a factor of 8 and timer count up to 62, emulates a millisecond timer
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

# define FOSC 7372800
# define BDIV ( FOSC / 100000 - 16) / 2 + 1


const uint16_t threshold = 550;
double yaxis = 0;
double zaxis = 0;

int main(void)
{
	/*PLEASE NOTE ASSUMPTIONS HERE
	assumed to have already happened in previous setup run (UNTESTED BECAUSE OF CIRCUMSTANCE)
	//setting baud rate of bluetooth to 9600

	//1. set serial baud rate to 115200 - original baud rate of the bluetooth module
	serial_init(3);
	//2. enter command mode
	serial_out('$');
	serial_out('$');
	serial_out('$');
	//3. change the GPIO7 (affects baudrate to 9600) pin to always on at startup, now bluetooth will function at 9600 baud rate
	message = "S%, 8080"
	sendBluetooth();
	serial_wait();
	message = "S^8080"
	sendBluetooth();
	//4. return to 9600 baudrate (if necessary)
	serial_init(47);
	*/

	HEARTBEAT_STATE = 0;
	sei();
	initAccelerometer();
	setup();
	i2c_init ( BDIV );
	serial_init(47);
	gps_init();

	while (1)
	{
		//check flags for button pushes
		if (vol_up == 0)
		{
			//send data through bluetooth module
			serial_wait();
			message = "M,4";
			sendBluetooth();
			vol_up = 1;
		}
		if (vol_down == 0)
		{
			//send data through bluetooth module
			serial_wait();
			message = "M,1";
			sendBluetooth();
			vol_down = 1;
		}
		if (skip == 0)
		{
			//send data through bluetooth module
			serial_wait();
			message = "M,2";
			sendBluetooth();
			skip = 1;
		}

		if (pause == 0)
		{
			serial_wait();
			message = "M,3";
			sendBluetooth();
			pause = 1;
		}

		if (printBPM == 0)
		{
			//send data to LCD, bluetooth module
			serial_wait();
			sprintf(message, "H,%d", bpm * 6);
			sendBluetooth();
			serial_wait();
			sendLCD();
			bpm = 0;
			printBPM = 1;
		}



		//$GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C 
		if (checkGPS == 0)
		{
			serial_wait();
			// Z is for start/stop transmission
			message = "Z,";
			sendBluetooth();
			serial_wait();
			readingBluetooth = 1;
			char* location = popGPS();
			while(location == NULL) {
				location = popGPS();
			}
			readingBluetooth = 0;
			serial_wait();
			sendBluetooth();
			serial_wait();
			message = "G,";
			uint8_t mindex = 2;
			uint8_t i;
			for(i = 20; i < 44; i++)
			{
				message[mindex++] = location[i];
			}
			serial_wait();
			sendBluetooth();
			checkGPS = 1;
		}

		if (buzzerflag == 0)
		{
			//turn on buzzer
			PORTC |= (1 << 3);
			serial_wait();
			message = "ALARM ON";
			sendLCD();
		}

		if(buzzerflag == 1)
		{
			serial_wait();
			message = "OK,";
			sendBluetooth();
			buzzerflag = -1;
		}

		//ACCELEROMETER READ AND DISPLAY HERE
		if (pollSpeed == 0)
		{
			pollAccelerometer();
			pollSpeed = 1;
		}

		if (findspeed == 0)
		{
			get_accelerometer_moving_average(AC_mavg);
			serial_wait();
			dtostrf( AC_mavg[0], 3, 4, cfloat);
			sprintf(message, "i,%s", cfloat); //TODO Andrew
			sendBluetooth();
			serial_wait();
			sendLCD();
			serial_wait();
			dtostrf( AC_mavg[1], 3, 4, cfloat );
			sprintf(message, "i,%s", cfloat); //TODO Andrew
			sendBluetooth();
			serial_wait();
			sendLCD();
			serial_wait();
			dtostrf( AC_mavg[2], 3, 4, cfloat );
			sprintf(message, "i,%s", cfloat); //TODO Andrew
			sendBluetooth();
			serial_wait();
			sendLCD();
			findspeed = 1;
		}

		if (poll_light == 0)
		{
			poll_light_sensor();
			poll_light++;
		}

		if (sendlight == 0)
		{

			LS_mavg = get_light_sensor_moving_average();
			serial_wait();
			dtostrf( LS_mavg, 3, 4, cfloat );
			sprintf(message, "i,%s", cfloat); //TODO Andrew
			sendBluetooth();
			serial_wait();
			sendLCD();
			sendlight = 1;
		}

		if (readTemp == 0)
		{
			adc_init(0x00);
			uint8_t rawvalue = adc_sample();
			if (rawvalue > lastTemp)
			{
				if (rawvalue - lastTemp >= 3)
				{
					//do something
					serial_wait();
					sprintf(message, "T,%d", rawvalue);
					sendBluetooth();
					serial_wait();
					sendLCD();
				}
			}
			else
			{
				if (lastTemp - rawvalue >= 3)
				{
					//do something
					serial_wait();
					sprintf(message, "T,%d", rawvalue);
					sendBluetooth();
					serial_wait();
					sendLCD();
				}
			}

			lastTemp = rawvalue;
			//send Data to bluetooth module
			readTemp = 1;
		}

		//read heartbeatsensor (heartbeat detected)
		adc_init(0x03);
		uint8_t rawvalue = adc_sample();
		if (HEARTBEAT_STATE && rawvalue < threshold)
		{
			bpm++;
			HEARTBEAT_STATE = 0;
		}
		else if (!HEARTBEAT_STATE && rawvalue > threshold) HEARTBEAT_STATE = 1;

	}

}

//every millisecond
ISR(TIMER1_COMPA_vect)
{
	millisecondsElapsed += 1;

	input = PINB;
	if ( (input & (1)) == 0)
	{
		capturemillisU = capturemillisU + 1;
		if (capturemillisU >= 20 && buzzerflag == -1)
		{
			vol_up = 0;
			capturemillisU = 0;
		}

		if(capturemillisU >= 200 && buzzerflag == 0)
		{
			buzzerflag = 1;
		}
	}
	else
	{
		vol_up = 1;
		capturemillisU = 0;
	}

	if ( (input & (1 << 1)) == 0)
	{
		capturemillisD = capturemillisD + 1;
		if (capturemillisD >= 20)
		{
			vol_down = 0;
			capturemillisD = 0;
		}
	}
	else
	{
		vol_down = 1;
		capturemillisD = 0;
	}

	if ( (input & (1 << 2)) == 0)
	{
		capturemillisS = capturemillisS + 1;
		if (capturemillisS >= 200)
		{
			skip = 0;
			capturemillisS = 0;
		}

	}
	else
	{
		if (capturemillisS >= 20)
		{
			pause = 0;
			skip = 1;
		}
		else
		{
			skip = 1;
			pause = 1;
		}
		capturemillisS = 0;
	}

	if (millisecondsElapsed % 5 == 0)
	{
		pollSpeed = 0;
	}

	if (millisecondsElapsed == 30)
	{
		findspeed = 0;
		millisecondsElapsed = 0;
	}

}

//every second
ISR(TIMER0_COMPA_vect)
{

	secondsElapsed += 1;

	//TODO: DOUBLECHECK THIS PART
	if ((!secondsElapsed % 5))
	{
		sendlight = 0;
	}

	if (!(secondsElapsed % 2))
	{
		poll_light = 0;
	}
	if (!(secondsElapsed % 10))
	{
		checkGPS = 0;
		printBPM = 0;
	}
	if (secondsElapsed == 60)
	{

		minutesElapsed += 1;
		secondsElapsed += 0;
	}

	if (minutesElapsed == 5)
	{
		readTemp = 0;
		minutesElapsed = 0;
	}

}