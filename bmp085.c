/*
bmp085 lib 0x01

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
//#include "../timeout.h"
#include "i2c.h"
#include "bmp085.h"

// i2c write
void bmp085_writemem(uint8_t reg, uint8_t value) {
	i2c_start_wait(BMP085_ADDR | I2C_WRITE);
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
}

// i2c read
void bmp085_readmem(uint8_t reg, uint8_t buff[], uint8_t bytes) {
	uint8_t i =0;
	i2c_start_wait(BMP085_ADDR | I2C_WRITE);
	i2c_write(reg);
	i2c_rep_start(BMP085_ADDR | I2C_READ);
	for(i=0; i<bytes; i++) {
		if(i==bytes-1)
			buff[i] = i2c_readNak();
		else
			buff[i] = i2c_readAck();
	}
	i2c_stop();
}


int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;

// read the calibration data from sensor memory
void getcalibration() {
	uint8_t buff[2];
	memset(buff, 0, sizeof(buff));
	//ac1 = read16(BMP085_CAL_AC1);
	bmp085_readmem(BMP085_REGAC1, buff, 2);
	ac1 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGAC2, buff, 2);
	ac2 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGAC3, buff, 2);
	ac3 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGAC4, buff, 2);
	ac4 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	bmp085_readmem(BMP085_REGAC5, buff, 2);
	ac5 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	bmp085_readmem(BMP085_REGAC6, buff, 2);
	ac6 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	bmp085_readmem(BMP085_REGB1, buff, 2);
	b1 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGB2, buff, 2);
	b2 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGMB, buff, 2);
	mb = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGMC, buff, 2);
	mc = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGMD, buff, 2);
	md = ((int)buff[0] <<8 | ((int)buff[1]));
}	


int32_t readRawTemperature(void) {
	uint8_t buff[2];
	memset(buff, 0, sizeof(buff));
	//read raw temperature
	bmp085_writemem(BMP085_REGCONTROL, BMP085_REGREADTEMPERATURE);
	_delay_ms(5); // min. 4.5ms read Temp delay
	bmp085_readmem(BMP085_REGCONTROLOUTPUT, buff, 2);
	
	int32_t ut,x1,x2;
	//uncompensated temperature value
	ut = ((int32_t)buff[0] << 8 | ((int32_t)buff[1])); 
	//compensate
	x1 = ((int32_t)ut - ac6) * ac5 >> 15;
	x2 = ((int32_t)mc << 11) / (x1 + md);
	int32_t comp_ut = x1 + x2;
	
	return comp_ut;
}

int32_t readRawPressure(int32_t rawtemperature ) {
	uint8_t buff[3];
	memset(buff, 0, sizeof(buff));
	bmp085_writemem(BMP085_REGCONTROL, BMP085_REGREADPRESSURE + (BMP085_MODE << 6));
	_delay_ms(2 + (3<<BMP085_MODE));
	bmp085_readmem(BMP085_REGCONTROLOUTPUT, buff, 3);
	
	int32_t up,x1,x2,x3,b3,b6,p;
	uint32_t b4,b7;
	// uncompensated pressure value
	up = ((((int32_t)buff[0] <<16) | ((int32_t)buff[1] <<8) | ((int32_t)buff[2])) >> (8-BMP085_MODE)); 

	//calculate raw pressure, compensated
	b6 = rawtemperature - 4000;
	x1 = (b2* (b6 * b6) >> 12) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((((int32_t)ac1) * 4 + x3) << BMP085_MODE) + 2) >> 2;
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (50000 >> BMP085_MODE);
	p = b7 < 0x80000000 ? (b7 << 1) / b4 : (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	int32_t comp_up = p + ((x1 + x2 + 3791) >> 4);
	return comp_up;
  
}

void  bmp085_init() {
	#if BMP085_I2CINIT == 1
	//init i2c
	i2c_init();
	_delay_us(10);
	#endif
	// read calibration data
	getcalibration();
}

float convertRawTemperature(int32_t rawtemperature) {
	float ct = ((rawtemperature + 8)>>4) / 10;
	return ct;
}

int32_t convertRawPressure(int32_t rawpressure) {
	return rawpressure + BMP085_UNITPAOFFSET;
}

// we assume sea level pressure is 101325 Pa
float convertAltitude(int32_t pressure) {
	return ((1 - pow(pressure/(double)101325, 0.1903 )) / 0.0000225577) + BMP085_UNITMOFFSET;
	//  float altitude = 44330 * (1.0 - pow(pressure /(double)101325.0,0.1903));
}

// -------------------------------------------------------------------------- //
float bmp085_readTemperature() {
	return convertRawTemperature(readRawTemperature());
}

int32_t bmp085_readPressure() {
	int32_t rawtemperature = readRawTemperature();
	int32_t rawpressure = readRawPressure(rawtemperature);
	return convertRawPressure(rawpressure);	
}

float bmp085_readAltitude() {
	return convertAltitude(bmp085_readPressure());
}

void bmp085_readAll(float *temperature, int32_t *pressure, float *altitude) {
	int32_t rawtemperature = readRawTemperature();
	int32_t rawpressure = readRawPressure(rawtemperature);
	//
	*temperature = convertRawTemperature(rawtemperature);
	*pressure = convertRawPressure(rawpressure);
	*altitude = convertAltitude(*pressure);
}
