float XYZ_BUFFER[XYZ_BUFFER_SIZE][3];
uint8_t WP = 0;
unit8_t initAccelerometer();
unit8_t pollAccelerometer();
unit8_t readAccelerometer(float*);
void push_back_FIFO(float*, float**, uint8_t, uint8_t&);
float get_moving_average(float**, unit8_t, unit8_t);


