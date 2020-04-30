/*read z and y axis to find the speed, assuming low power mode on Accelerometer */

unit8_t addresses[3][2]; 
accel_addr = 
{ {0x08, 0x09} //X (L,H)
 , {0x0a, 0x0b}, //Y (L,H)
 {0x0c, 0x0d} }; //Z (L,H)
  
float readAccelerometer(float output[3]){
  
	uint8_t raw_values8[3][2]; 
  int16_t temp; 
  float output
	uint8_t statusx = i2c_io(0x31, accel_addr[0], 1, NULL, 0, raw_values8[0], 1)
  uint8_t statusx = i2c_io(0x31, accel_addr[0]+1, 1, NULL, 0, raw_values8[0]+1, 1)
	uint8_t statusy = i2c_io(0x31, accel_addr[1], 1, NULL, 0, raw_values8[1], 1)
  uint8_t statusy = i2c_io(0x31, accel_addr[1]+1, 1, NULL, 0, raw_values8[1]+1, 1)
	uint8_t statusz = i2c_io(0x31, accel_addr[2], 1, NULL, 0, raw_values8[2], 1)
  uint8_t statusz = i2c_io(0x31, accel_addr[2]+1, 1, NULL, 0, raw_values8[2]+1, 1)
  uint8_t i = 0; 
  for (i = 0; i<3; i++) 
  {
    temp = (unit16_t)raw_values8[i][0] | int16_t(raw_values8[i][1] <<8)
    temp>>=6; 
    output[i] = (float)temp / 15987; 
  }
  
}


