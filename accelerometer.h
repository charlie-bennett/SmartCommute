
uint8_t initAccelerometer();
uint8_t pollAccelerometer();
uint8_t readAccelerometer(float*);
void push_back_FIFO(float*, float**, uint8_t, uint8_t&);
void get_accelerometer_moving_average(float*);


