#ifndef _LCD_H
#define _LCD_H

void lcd_com(unsigned char data);
void lcd_dat(unsigned char data);
void lcd_init(void);
void set_curs_pos(unsigned char posv, unsigned char posh);
void shft_scr_left();
void shft_scr_right();
void print_str(unsigned char s[], unsigned char from_s, unsigned char s_length);
void print_pos_str(unsigned char s[], unsigned char from_s, unsigned char s_length, unsigned char pos_v, unsigned char pos_h);

#endif
