#include "temp.h"
#include <avr/io.h>
#include <util/delay.h>

//определяем порт и бит к которому подключено устройство 1-wire
//#define W1_PORT PORTB
//#define W1_DDR DDRB
//#define W1_PIN PINB
//#define W1_BIT 2

//функция определяет есть ли устройство на шине
unsigned char w1_find(w1_config conf)
{
	unsigned char device;
	(*conf.w1_ddr) |= 1<<(conf.w1_bit);//логический "0"
	_delay_us(485);//ждем минимум 480мкс
	(*conf.w1_ddr) &= ~(1<<(conf.w1_bit));//отпускаем шину
	_delay_us(65);//ждем минимум 60мкс и смотрим что на шине
	
	if(((*conf.w1_pin) & (1<<(conf.w1_bit))) ==0x00)
		device = 1;
	else
		device = 0;
	_delay_us(420);//ждем оставшееся время до 480мкс
	return device;
}

//функция посылает команду на устройство 1-wire
void w1_sendcmd(w1_config conf, unsigned char cmd)
{
	for(unsigned char i = 0; i < 8; i++)//в цикле посылаем побитно
	{
		if((cmd & (1<<i)) == 1<<i)//если бит=1 посылаем 1
		{
			(*conf.w1_ddr) |= 1<<(conf.w1_bit);
			_delay_us(2);
			(*conf.w1_ddr) &= ~(1<<(conf.w1_bit));
			_delay_us(65);
		} 
		else//иначе посылаем 0
		{
			
			(*conf.w1_ddr) |= 1<<(conf.w1_bit);
			_delay_us(65);
			(*conf.w1_ddr) &= ~(1<<(conf.w1_bit));
			_delay_us(5);
		}
	}
}

//функция читает один байт с устройства 1-wire
unsigned char w1_receive_byte(w1_config conf)
{
	unsigned char data=0;
	for(unsigned char i = 0; i < 8; i++)//в цикле смотрим что на шине и сохраняем значение
	{	
		(*conf.w1_ddr) |= 1<<(conf.w1_bit);
		_delay_us(2);
		(*conf.w1_ddr) &= ~(1<<(conf.w1_bit));
		_delay_us(7);
		//if((*conf.w1_pin) & (1<<(conf.w1_bit)) == 0x00)
		if (((*conf.w1_pin) & (1<<(conf.w1_bit))) == 0x00)
			data &= ~(1<<i);
		else
			data |= 1<<i;
		_delay_us(50);//задержка до окончания тайм-слота
	}
	return data;
}

//функция преобразует полученные с датчика 18b20 данные в температуру
unsigned char temp_18b20(w1_config conf, int *temp)
{
	//unsigned char data[2];
	//int temp = 0;
	unsigned char is_detected = 0;
	
	if(w1_find(conf)==1)//если есть устройство на шине
	{
		w1_sendcmd(conf, 0xcc);//пропустить ROM код, мы знаем, что у нас одно устройство или передаем всем
		w1_sendcmd(conf, 0x44);//преобразовать температуру
		_delay_ms(750);//преобразование в 12 битном режиме занимает 750ms
		w1_find(conf);//снова посылаем Presence и Reset
		w1_sendcmd(conf, 0xcc);
		w1_sendcmd(conf, 0xbe);//передать байты ведущему(у 18b20 в первых двух содержится температура)
		//data[0] = w1_receive_byte();//читаем два байта с температурой
		//data[1] = w1_receive_byte();
			//загоняем в двух байтную переменную
		//temp = data[1];
		//temp = temp<<8;
		//temp |= data[0];
		*((char *)temp) = w1_receive_byte(conf);
		*((char *)temp + 1) = w1_receive_byte(conf);
		//переводим в градусы
		//temp *= 0.0625;//0.0625 градуса на единицу данных
		*temp = *temp >> 4;
		
		if(*temp > 1000)//если температура <0
		{
			*temp = 4096 - *temp;
			*temp = -(*temp);
		}
		is_detected = 1;
	}
	else
	{
		is_detected = 0;
	}
	return is_detected;
}
