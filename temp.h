#ifndef _TEMP_H
#define _TEMP_H

#include <avr/io.h>

typedef struct {
	uint8_t *w1_port;
	uint8_t *w1_ddr;
	uint8_t *w1_pin;
	uint8_t w1_bit;
} w1_config;

unsigned char temp_18b20(w1_config conf, int *temp);

#endif /* _TEMP_H */
