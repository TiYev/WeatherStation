#include <avr/io.h>
#include <util/delay.h>
#include "cmap.h"
#include "lcd.h"
#include "temp.h"
#include <math.h>
#include "addmath.h"
#include "dht.h"

#define SCREEN_SHIFT_DELAY_MS 2000

#define BUF_SIZE	5
#define PRES_INDICATOR_SENSITIVITY 10

int main (void) {
	lcd_init();
	
	//init bmp085
	bmp085_init();
	
	
	unsigned char temp_units[] = {0x01, 'C'};
	/* Экран с приглашением */
	print_pos_str("INT TEMP", 0, 8, 0, 0);
	print_pos_str(temp_units, 0, 2, 1, 0+4);
	
	/* Экран с температурой */
	print_pos_str("EXT TEMP", 0, 8, 0, 8);
	print_pos_str(temp_units, 0, 2, 1, 8+4);
	
	/* Экран с атмосферным давлением */
	print_pos_str("PRE mmHg", 0, 8, 0, 16);
	print_pos_str("PRE kPa", 0, 8, 0, 24);
	
	unsigned char arrow_up[] = {0x02};
	unsigned char arrow_down[] = {0x03};
	
	print_pos_str("?", 0, 1, 1, 16+6);
	
	///* Экран с относительной влажностью */
	print_pos_str("INT  HUM", 0, 8, 0, 32);
	print_pos_str("%", 0, 1, 1, 32+3);
	
	/* Сдвиг экрана после приветствия */
	//_delay_ms(SCREEN_SHIFT_DELAY_MS);
	//shft_scr_left(); // go to screen with temperature measurements (DS18B20)
	
	//------------ Screen shit for test ony! --------------
	//shft_scr_left(); //tmp for test (pressure)
	
	//shft_scr_left(); //tmp for test (Tin + alt)
	
	//shft_scr_left(); //tmp for test (Tin + humidity)
	
	//-----------------------------------------------------
	/**********************************/
	
	int temp, temp2 = 0;
	unsigned char is_detected, is_detected2 = 0;
	
	
	unsigned char tempbuf[3], tempbuf2[3] = {' ', ' ', ' '}; // buffer for temperatures (DS18B20)
	unsigned char presbuf[5], presbuf2[5] = {' ', ' ', ' ', ' ', ' '}; // buffers for pressure (BMP180)
	
	unsigned char humbuff[2] = {' ', ' '}; // buffer for humidity (DHT11)
	
	w1_config conf, conf2;
	conf.w1_ddr = (uint8_t*)&ITEMP_DDR;
	conf.w1_pin = (uint8_t*)&ITEMP_PIN;
	conf.w1_bit = ITEMP_BIT;
	
	conf2.w1_ddr = (uint8_t*)&OTEMP_DDR;
	conf2.w1_pin = (uint8_t*)&OTEMP_PIN;
	conf2.w1_bit = OTEMP_BIT;
	
	// For pressure indicator
	int32_t pressure_buff[BUF_SIZE];
	unsigned char meas_cnt = 0;
	int32_t prev_meas = 0;
	unsigned int i = 0;
	int32_t sum = 0;
	
	unsigned char first_measure = 1;
	//////////////////////////
	
	while(1) {
		
		
		
		
		//------------------------ Замер давления, температуры, высоты (BMP180) ---------------------
		int32_t p = 0;
		float t = 0;
		float a = 0;
		bmp085_readAll(&t, &p, &a);
		
		// Pressure indicator
		pressure_buff[meas_cnt] = p / PRES_INDICATOR_SENSITIVITY;
		
		meas_cnt = meas_cnt + 1;
		
		if (meas_cnt == BUF_SIZE) {
			
			sum = 0;
			for (i = 0; i < BUF_SIZE; i++) {
				sum = sum + pressure_buff[i];
			}
			sum = sum / BUF_SIZE;
			meas_cnt = 0;
			
			if (!first_measure) {
				if (sum > prev_meas) {
					print_pos_str(arrow_up, 0, 1, 1, 16+6);
				}
				else {
					if (sum < prev_meas) {
						print_pos_str(arrow_down, 0, 1, 1, 16+6);
					}
				}
			}
			else {
				first_measure = 0;
			}
			prev_meas = sum;
		}
		////////////////////////////////////////////
		
		// pressure in mmHg		
		ftoa(presbuf, (float)(p/133.32240f));
		print_pos_str(presbuf, 0, 5, 1, 16);
		
		// pressure in kPa
		ftoa(presbuf2, (float)(p/1000.0f));
		print_pos_str(presbuf2, 0, 5, 1, 24);
		//--------------------------------------------------------------------------------------------
		
		
		
		//---------------------------------- Замер влажности и температуры (DHT11) -------------------
		
		int8_t humidity = 0;
		//float humidity = 0.0;
		
		
		if(dht_gethumidity(&humidity) != -1) {
			itoa(humidity, humbuff, 10);
			print_pos_str(humbuff, 0, 2, 1, 32);
		}
		else {
			print_pos_str("--", 0, 2, 1, 32); /* Вывод двух знаков "-" */
		}
		
		//--------------------------------------------------------------------------------------------
		
		
		
		
		//---------------------------- Замер температуры (DS18B20) -----------------------------------
		
		is_detected = temp_18b20(conf, &temp);
		is_detected2 = temp_18b20(conf2, &temp2);
		
		
		// TODO: добавить ограничение диапазона по температуре
		
		// Выводим температуру в помещении
		if (is_detected)
		{
			itoa(temp, tempbuf, 10);
			print_pos_str(tempbuf, 0, 3, 1, 0);
		}
		else
		{
			print_pos_str("---", 0, 3, 1, 0); /* Вывод трех знаков "-" */
		}
		// Выводим температуру на улице
		if (is_detected2)
		{
			itoa(temp2, tempbuf2, 10);
			print_pos_str(tempbuf2, 0, 3, 1, 8);
		}
		else
		{
			print_pos_str("---", 0, 3, 1, 8); /* Вывод трех знаков "-" */
		}
		
		//--------------------------------------------------------------------------------------------
		
		
		/* Раскомментировать для передвижения экрана */
		_delay_ms(SCREEN_SHIFT_DELAY_MS);
		shft_scr_left();
		
		_delay_ms(SCREEN_SHIFT_DELAY_MS);
		shft_scr_left();
		
		_delay_ms(SCREEN_SHIFT_DELAY_MS);
		shft_scr_left();
		
		_delay_ms(SCREEN_SHIFT_DELAY_MS);
		shft_scr_left();
		
		_delay_ms(SCREEN_SHIFT_DELAY_MS);
		shft_scr_left();
		
		
		//shft_scr_right();
		//shft_scr_right();
		//shft_scr_right();
		//shft_scr_right();
		//shft_scr_right();
		
		/*********************************************/
		
	}
}
