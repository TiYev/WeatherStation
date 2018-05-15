#include <avr/io.h>
#include <util/delay.h>
#include "cmap.h"
#include "lcd.h"
  
/* Функция записи команды в LCD */
void lcd_com(unsigned char data)
{
	LCD_CPORT &= ~(1 << LCD_RS);	/* RS = 0 */
	LCD_CPORT |= (1 << LCD_EN); 	/* EN = 1 (начало записи команды в LCD) */
	LCD_DPORT = data;				/* Вывод команды на шину DB0-7 LCD */
	_delay_us(100);			/* Длительность сигнала EN */
	LCD_CPORT &= ~(1 << LCD_EN);	/* EN = 0 (конец записи команды в LCD) */
	_delay_us(100);			/* Пауза для выполнения команды */
}
  
/* Функция записи данных в LCD */
void lcd_dat(unsigned char data)
{
	LCD_CPORT |= (1 << LCD_RS)|(1 << LCD_EN);	/* RS = 1, EN = 1 (начало записи данных в LCD) */
	LCD_DPORT = data;						/* Вывод данных на шину DB0-7 LCD */
	_delay_us(100);					/* Длительность сигнала EN */
	LCD_CPORT &= ~(1 << LCD_EN);			/* EN = 0 (конец записи данных в LCD) */
	_delay_us(100);
}

/* Создание пользовательских символов */
void create_symbs() {
	/* Создание пустого символа (для корректного его отображения
	 * при работе функции преобразования числа в строку) */
	lcd_com(0x40);	/* Переключение на адресацию CGRAM */
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_com(0x80);
	/****************************/
	
	/* Создание символа градуса */
	lcd_com(0x48);	/* Переключение на адресацию CGRAM */
	lcd_dat(0x06);
	lcd_dat(0x09);
	lcd_dat(0x09);
	lcd_dat(0x06);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_dat(0x00);
	lcd_com(0x80);
	/****************************/
	
	/* Создание стрелки, направленной вверх */
	lcd_com(0x50);	/* Переключение на адресацию CGRAM */
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_dat(0x0E);
	lcd_dat(0x15);
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_com(0x80);
	
	/* Создание стрелки, направленной вниз */
	lcd_com(0x58);	/* Переключение на адресацию CGRAM */
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_dat(0x15);
	lcd_dat(0x0E);
	lcd_dat(0x04);
	lcd_dat(0x04);
	lcd_com(0x80);
}

/* Функция инициализации LCD */
void lcd_init(void)
{
	LCD_CDDR |= (1 << LCD_EN)|(1<<LCD_RW)|(1 << LCD_RS); /* PC2-0 выходы */
	LCD_CPORT = 0x00;							/* Логические нули на выходе */
	LCD_DDDR = 0xFF;							/* PD7-0 выходы */
	LCD_DPORT = 0x00;							/* Логические нули на выходе */
	lcd_com(0x08);							/* Полное выключение дисплея */
	lcd_com(0x38);							/* 8 бит 2 строки */
	_delay_us(100);
	lcd_com(0x38);							/* 8 бит 2 строки */
	_delay_us(100);
	lcd_com(0x38);							/* 8 бит 2 строки */
	lcd_com(0x01);							/* Очистка дисплея */
	_delay_us(100);
	lcd_com(0x06);							/* Сдвиг курсора вправо */
	_delay_ms(10);
	lcd_com(0x0C);							/* Курсор невидим */
	
	/* Создание таблицы пользовательских символов */
	create_symbs();
}

void set_curs_pos(unsigned char pos_v, unsigned char pos_h) {
	switch(pos_v) {
		case 0: lcd_com(0x80 + pos_h);
				break;
		case 1: lcd_com(0xC0 + pos_h);
				break;
	}
}

void shft_scr_left() {
	unsigned char i;
	for (i = 0; i < 8; i++) {
		lcd_com(0x18);
	}
}

void shft_scr_right() {
	unsigned char i;
	for (i = 0; i < 8; i++) {
		lcd_com(0x1C);
	}
}

/* Вывод строки в текущей позиции курсора */
void print_str(unsigned char s[], unsigned char from_s, unsigned char s_length) {
	unsigned char i;
	for (i = from_s; i < from_s + s_length; i++) {
		lcd_dat(s[i]);
	}
}

/* Вывод строки в указанной позиции курсора */
void print_pos_str(unsigned char s[], unsigned char from_s, unsigned char s_length, unsigned char pos_v, unsigned char pos_h) {
	set_curs_pos(pos_v, pos_h);
	unsigned char i;
	for (i = from_s; i < from_s + s_length; i++) {
		lcd_dat(s[i]);
	}
}
