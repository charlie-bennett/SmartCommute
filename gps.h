#ifndef GPS_H_
#define GPS_H_

uint8_t is_coordinates(char*);
extern volatile int8_t buzzerflag;
extern volatile int8_t readingBluetooth;
void gps_init();
void pollGPS(char);
char* popGPS();

#ENDIF