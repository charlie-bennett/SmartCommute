
extern volatile int8_t buzzerflag;
extern volatile int8_t readingBluetooth;
extern volatile char lcdSpeed[6];
extern volatile int8_t speedFlag;
extern uint8_t is_coordinates(char*);
extern void gps_init();
extern void pollGPS(char);
extern char* popGPS();
