#ifndef __SERIAL_H__
#define __SERIAL_H__

#define PACKET_128 128
#define PACKET_1k 1024

char get_char(int );

int put_char(int , const unsigned char );

int set_uart(int);

int set_serial(int );

void close_serial(int );

#endif
