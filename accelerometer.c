#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "accelerometer.h"
#include "i2c.h"

#define XYZ_BUFFER_SIZE 32

/*read z and y axis to find the speed, assuming low power mode on Accelerometer */
#define FIFO_CTRL 0x2E
#define STREAM_MODE 0xFE
#define BYPASS_MODE 0xFC
#define CTRL_REG1 0x20
#define CTRL_REG1_CONFIG 0x97
#define XYZ_BUFFER_SIZE 32
//
/*
ODR3 ODR2 ODR1 ODR0 LPen Zen Yen Xen

ODR3 ODR2 ODR1 ODR0 Power mode selection
0 0 0 0 Power-down mode
0 0 0 1 HR / Normal / Low-power mode (1 Hz)
0 0 1 0 HR / Normal / Low-power mode (10 Hz)
0 0 1 1 HR / Normal / Low-power mode (25 Hz)
0 1 0 0 HR / Normal / Low-power mode (50 Hz)
0 1 0 1 HR / Normal / Low-power mode (100 Hz)
0 1 1 0 HR / Normal / Low-power mode (200 Hz)
0 1 1 1 HR / Normal / Low-power mode (400 Hz)
1 0 0 0 Low power mode (1.60 kHz)
*/


float XYZ_BUFFER[XYZ_BUFFER_SIZE][3];
uint8_t WP = 0;
uint8_t addresses[3][2];
float output_temp[3];
uint8_t i = 0;
uint8_t j = 0;

uint8_t accel_addr[3][2] =
{
  {0x28, 0x29}, //X (L,H)
  {0x2a, 0x2b}, //Y (L,H)
  {0x2c, 0x2d}
}; //Z (L,H)


uint8_t initAccelerometer()
{
  uint8_t literal = FIFO_CTRL;
  uint8_t temp;
  uint8_t status = i2c_io(0x31, &literal, 1, &temp, 1, NULL, 0);
  if(status != 0)
    return status;
  temp &= BYPASS_MODE;
  status = i2c_io(0x31, &literal, 1, &temp, 1, NULL, 0);
  return status;
}

uint8_t pollAccelerometer()
{

  uint8_t status = readAccelerometer(output_temp);
  push_back_FIFO(output_temp, XYZ_BUFFER, XYZ_BUFFER_SIZE, &WP);
  return status;
}
uint8_t readAccelerometer(float output[3])
{

  //no need to poll to see if data is ready since it will refresh at
  // 1.6 kHz

  uint8_t raw_values8[3][2];
  int16_t temp;
  uint8_t i = 0;
  uint8_t status = 0;
  for (i = 0; i < 3; i++)
  {
    status |= i2c_io(0x31, accel_addr[i], 1, NULL, 0, raw_values8[i], 1);
    status |= i2c_io(0x31, accel_addr[i] + 1, 1, NULL, 0, raw_values8[i] + 1, 1);
  }

  for (i = 0; i < 3; i++)
  {
    temp = (((uint16_t)raw_values8[i][0]) | ((int16_t)(raw_values8[i][1] << 8)));
    temp >>= 6;
    output[i] = (float)temp / 15987;
  }
  return status;

}

void push_back_FIFO(float* val, float** buffer, uint8_t BUFFER_SIZE, uint8_t* current_loc)
{

  buffer[((*current_loc) % BUFFER_SIZE)][0] = val[0];
  buffer[((*current_loc) % BUFFER_SIZE)][1] = val[1];
  buffer[((*current_loc) % BUFFER_SIZE)][2] = val[2];
  (*current_loc)++;
  return;
}

void get_accelerometer_moving_average(float* output)
{
  float runsum = 0;

  for (j = 0; j < 3; j++)
  {
    for (i = 0; i < XYZ_BUFFER_SIZE; i++)
    {
      runsum += XYZ_BUFFER[i][j];
    }
    if (WP < XYZ_BUFFER_SIZE) output[j] = runsum / ((float) WP);
    else output[j] = runsum / ((float) XYZ_BUFFER_SIZE);

  }
  return;
}


