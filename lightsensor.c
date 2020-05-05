#include <avr/io.h>
#include <stdio.h>
#include "adc.h"

#define LS_FIFO_SIZE 32
uint8_t LS_WP = 0;
uint8_t rawvalue;
uint8_t LS_BUFFER[LS_FIFO_SIZE];

uint8_t i = 0;
float runsum;


void push_back_LSFIFO(uint8_t val, uint8_t* buffer, uint8_t BUFFER_SIZE, uint8_t* current_loc)
{

  buffer[((*current_loc) % BUFFER_SIZE)] = val;
  (*current_loc)++;
  return;
}

void poll_light_sensor()
{
  adc_init(0X01);
  rawvalue = adc_sample();
  push_back_LSFIFO(rawvalue, LS_BUFFER, LS_FIFO_SIZE, LS_WP);

}

float get_light_sensor_moving_average()
{
  for (i = 0; i < LS_FIFO_SIZE; i++)
  {
    runsum += LS_BUFFER[i];
  }
  if (LS_WP < LS_FIFO_SIZE) return runsum / ((float) LS_WP);
  return runsum / ((float) LS_FIFO_SIZE);
}
