#include "stm32f0xx_hal.h"

#define WRITE_HIGH(reg, bit) reg |= (1<<bit)
#define WRITE_LOW(reg, bit) reg &= ~(1<<bit)

uint16_t millis;

class FoxLCD { //������������� ������ FoxLCD
	//���������� ��������� ���������� ������ ������ 
	I2C_HandleTypeDef *i2c;
	uint8_t adress;
	
	uint8_t displaysettings; //��������� ������� - ������� �������, ������ �������� �� ������ 
	public:
		//����������� ������
		FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress);
		void init(void); 											//������� ������������� �������
		void sendcmd(uint8_t cmd); 						//������� �������� ������� � ������� (RS ��������� � ����)
		void senddata(uint8_t data); 					//������� �������� ������ � ������� (RS ��������� � �������)
		void printchar(char c);								//������� ������ ������� �� �������
		void print(char msg[]);								//������� ������ ������ �� �������
		void clear(void);											//������� ������� �������
		void home(void);											//������� �������� ������� � ��������� ���������
		void display(uint8_t state);					//������� ���������/���������� �������
		void cursor(uint8_t state);						//������� ���������/���������� ����������� �������
		void blink(uint8_t state);						//������� ���������/���������� �������� �������
    void setcursor(uint8_t x, uint8_t y);	//������� ����������� ������� �� ���� X � Y
		void chartocgram(uint8_t number, uint8_t chararray[8]); //������� ��������� ������������ ������� � CGRAM. ����� �������� ������� ��� �������� �� 0 �� 7
};
FoxLCD::FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress) { //����������� ������������� ���������� ��� ������ ����� ���������
	//������������ ������� �������� ���������� ����������
	this -> i2c = i2c;
	this -> adress = adress<<1;
}
void FoxLCD::init(void) {
	displaysettings = 0x0C;
	
	//�������� 40 �� ��� ���������������� ������ �������
	HAL_Delay(40);
	//�������� ������ ������������� �������
	//��������� 4-� ������� ����������
	sendcmd(0x30); //0b00100000
	sendcmd(0x30); //0b00100000
	//��������� ������� � ������������ ������
	sendcmd(0xC0); //0b11000000
	HAL_Delay(1); //�������� ���������� �������
	//��������� ������� � ����������� ���������� ��������
	sendcmd(0x00); //0b00000000
	sendcmd(0xC0); //0b11000000
	HAL_Delay(1); //�������� ���������� �������
	//������� �������
	sendcmd(0x00); //0b00000000
	sendcmd(0x10); //0b00010000
	HAL_Delay(2); //�������� ���������� �������
	//��������� ����� - ��������������� �����, �������� ������� ���������
	sendcmd(0x00); //0b00000000
	sendcmd(0x60); //0b01100000
	HAL_Delay(1); //�������� ���������� �������
	home();
	HAL_Delay(2);
	sendcmd(0x0C);
	HAL_Delay(1);
	clear();
	HAL_Delay(2);
}

//������� �������� ������� � ������� (RS ��������� � ����)
void FoxLCD::sendcmd(uint8_t cmd) {
	uint8_t i2cbuf[1];
	uint8_t highcmd = (cmd & 0xF0);
	i2cbuf[0] = highcmd | (1<<3) | (1<<2);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
	HAL_Delay(1);
	i2cbuf[0] = highcmd | (1<<3);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
	uint8_t lowcmd = ((cmd << 4) & 0xF0);
	i2cbuf[0] = lowcmd | (1<<3) | (1<<2);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
	HAL_Delay(1);
	i2cbuf[0] = lowcmd | (1<<3);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
}

//������� �������� ������ � ������� (RS ��������� � �������)
void FoxLCD::senddata(uint8_t data) {
	uint8_t i2cbuf[1];
	uint8_t highcmd = (data & 0xF0);
	i2cbuf[0] = highcmd | (1<<3) | (1<<0) | (1<<2);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
	HAL_Delay(1);
	i2cbuf[0] = highcmd | (1<<3) | (1<<0);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
	uint8_t lowcmd = ((data << 4) & 0xF0);
	i2cbuf[0] = lowcmd | (1<<3) | (1<<0) | (1<<2);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
	HAL_Delay(1);
	i2cbuf[0] = lowcmd | (1<<3) | (1<<0);
	HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
}

//������� ������ ������� �� �������
void FoxLCD::printchar(char c) {
	senddata(c);
}
//������� ������ ������ �� �������
void FoxLCD::print(char msg[]) {
	char i = 0;
	while(*msg) {
		i = *msg++;
		//��������� ������� �������� "�� ����" (������ ��� �������� � ���������� � ���������������)
		switch(i) {
			case '�':
				printchar('A');
			  break;
			case '�':
				printchar('a');
			  break;
			case '�':
				printchar(0xA0);
			  break;
			case '�':
				printchar(0xB2);
			  break;
			case '�':
				printchar('B');
			  break;
			case '�':
				printchar(0xB3);
			  break;
			case '�':
				printchar(0xA1);
			  break;
			case '�':
				printchar(0xB4);
			  break;
			case '�':
				printchar(0xE0);
			  break;
			case '�':
				printchar(0xE3);
			  break;
			case '�':
				printchar('E');
			  break;
			case '�':
				printchar('e');
			  break;
			case '�':
				printchar(0xA2);
			  break;
			case '�':
				printchar(0xB5);
			  break;
			case '�':
				printchar(0xA3);
			  break;
			case '�':
				printchar(0xB6);
			  break;
			case '�':
				printchar(0xA4);
			  break;
			case '�':
				printchar(0xB7);
			  break;
			case '�':
				printchar(0xA5);
			  break;
			case '�':
				printchar(0xB8);
			  break;
			case '�':
				printchar(0xA6);
			  break;
			case '�':
				printchar(0xB9);
			  break;
			case '�':
				printchar('K');
			  break;
			case '�':
				printchar(0xBA);
			  break;
			case '�':
				printchar(0xA7);
			  break;
			case '�':
				printchar(0xBB);
			  break;
			case '�':
				printchar('M');
			  break;
			case '�':
				printchar(0xBC);
			  break;
			case '�':
				printchar('H');
			  break;
			case '�':
				printchar(0xBD);
			  break;
			case '�':
				printchar('O');
			  break;
			case '�':
				printchar('o');
			  break;
			case '�':
				printchar(0xA8);
			  break;
			case '�':
				printchar(0xBE);
			  break;
			case '�':
				printchar('P');
			  break;
			case '�':
				printchar('p');
			  break;
			case '�':
				printchar('C');
			  break;
			case '�':
				printchar('c');
			  break;
			case '�':
				printchar('T');
			  break;
			case '�':
				printchar(0xBF);
			  break;
			case '�':
				printchar(0xA9);
			  break;
			case '�':
				printchar('y');
			  break;
			case '�':
				printchar(0xAA);
			  break;
			case '�':
				printchar(0xE4);
			  break;
			case '�':
				printchar('X');
			  break;
			case '�':
				printchar('x');
			  break;
			case '�':
				printchar(0xE1);
			  break;
			case '�':
				printchar(0xE5);
			  break;
			case '�':
				printchar(0xAB);
			  break;
			case '�':
				printchar(0xC0);
			  break;
			case '�':
				printchar(0xAC);
			  break;
			case '�':
				printchar(0xC1);
			  break;
			case '�':
				printchar(0xE2);
			  break;
			case '�':
				printchar(0xE6);
			  break;
			case '�':
				printchar(0xAD);
			  break;
			case '�':
				printchar(0xC2);
			  break;
			case '�':
				printchar(0xAE);
			  break;
			case '�':
				printchar(0xC3);
			  break;
			case '�':
				printchar('b');
			  break;
			case '�':
				printchar(0xC4);
			  break;
			case '�':
				printchar(0xAF);
			  break;
			case '�':
				printchar(0xC5);
			  break;
			case '�':
				printchar(0xB0);
			  break;
			case '�':
				printchar(0xC6);
			  break;
			case '�':
				printchar(0xB1);
			  break;
			case '�':
				printchar(0xC7);
			  break;
			default:
				printchar(i);
				break;
		}
		HAL_Delay(1);
	}
}
//������� ������� �������
void FoxLCD::clear(void) {
	sendcmd(0x01);
	HAL_Delay(2);
}
//������� �������� ������� � ��������� ���������
void FoxLCD::home(void) {
	sendcmd(0x02);
	HAL_Delay(2);
}

//������� ���������/���������� �������
void FoxLCD::display(uint8_t state) {
  state != 0 ? WRITE_HIGH(displaysettings, 2) : WRITE_LOW(displaysettings, 2);
	sendcmd(displaysettings);
	HAL_Delay(1);
}
//������� ���������/���������� ����������� �������
void FoxLCD::cursor(uint8_t state) {
  state != 0 ? WRITE_HIGH(displaysettings, 1) : WRITE_LOW(displaysettings, 1);
	sendcmd(displaysettings);
	HAL_Delay(1);
}
//������� ���������/���������� �������� �������
void FoxLCD::blink(uint8_t state) {
  state != 0 ? WRITE_HIGH(displaysettings, 0) : WRITE_LOW(displaysettings, 0);
	sendcmd(displaysettings);
	HAL_Delay(1);
}

//������� ����������� ������� �� ���� X � Y
void FoxLCD::setcursor(uint8_t x, uint8_t y) {
	//������� ��� ��������� �������������� ��������
	switch(y) {
		case 0:
			sendcmd(x | 0x80);
			break;
		case 1:
			sendcmd((0x40 + x) | 0x80);
			break;
		case 2:
			sendcmd((0x14 + x) | 0x80);
			break;
		case 3:
			sendcmd((0x54 + x) | 0x80);
			break;
	}
	HAL_Delay(1);
}

//������� ��������� ������������ ������� � CGRAM
void FoxLCD::chartocgram(uint8_t number, uint8_t chararray[8]) {
	sendcmd(0x40 + number*8);
	for (uint8_t i = 0; i < 8; i++) {
		senddata(chararray[i]);
	}
}
