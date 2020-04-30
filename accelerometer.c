/*read z and y axis to find the speed, assuming low power mode on Accelerometer */
#define FIFO_CTRL 0x2E
unit8_t addresses[3][2]; 
accel_addr = 
{ {0x28, 0x29} //X (L,H)
 , {0x2a, 0x2b}, //Y (L,H)
 {0x2c, 0x2d} }; //Z (L,H)

void initAccelerometer()
{
   unit_t statusx 
}
  
unit8_t readAccelerometer(float output[3]){
  
  uint8_t raw_values8[3][2]; 
  int16_t temp; 
  float output;
  uint8_t i = 0; 
  uint8_t status = 0; 
  for (i = 0; i<3; i++)
  {
     status|= i2c_io(0x31, accel_addr[i], 1, NULL, 0, raw_values8[i], 1);
     status |= i2c_io(0x31, accel_addr[i]+1, 1, NULL, 0, raw_values8[i]+1, 1);
  }
  
  for (i = 0; i<3; i++) 
  {
    temp = (unit16_t)raw_values8[i][0] | int16_t(raw_values8[i][1] <<8)
    temp>>=6; 
    output[i] = (float)temp / 15987; 
  }
  return status; 
  
}


