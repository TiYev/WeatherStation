#ifndef _CMAP_H
#define _CMAP_H

/* Карта подключений */

/******** Датчик температуры помещения *********/
#define ITEMP_DDR	DDRB
#define ITEMP_PIN	PINB
#define ITEMP_BIT	2

/********** Датчик температуры улицы ***********/
#define OTEMP_DDR	DDRB
#define OTEMP_PIN	PINB
#define OTEMP_BIT	1

/********************* LCD *********************/

/* Порт для подключения линий управления */
#define LCD_CDDR	DDRC
#define LCD_CPORT	PORTC

/* Порт для подключения шины данных */
#define LCD_DDDR	DDRD
#define LCD_DPORT	PORTD

/* Пины линий управления */
#define LCD_RS		PC0
#define LCD_RW		PC1
#define LCD_EN		PC2

/***********************************************/

#endif /* _CMAP_H */
