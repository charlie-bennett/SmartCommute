#ifndef GPS_H_
#define GPS_H_

extern volatile int8_t buzzerflag;
extern volatile int8_t readingBluetooth;
uint8_t is_coordinates(char*);
void gps_init();
void pollGPS(char);
char* popGPS();

#ENDIF