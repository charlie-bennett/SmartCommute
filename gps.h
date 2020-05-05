#ifndef GPS_H_
#define GPS_H_

uint8_t is_coordinates(char*);
void gps_init();
void pollGPS(char);
char* popGPS();

#ENDIF