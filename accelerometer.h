extern uint8_t initAccelerometer();
extern uint8_t pollAccelerometer();
extern uint8_t readAccelerometer(float*);
extern void push_back_FIFO(float*, float*, uint8_t, uint8_t*);
extern void get_accelerometer_moving_average(float*);