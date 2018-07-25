// Header: FoxLCD_I2C
// File Name: FoxLCD_I2C.h
// Author: Victor Nikitchuk
// Date: 25.07.2018

#ifndef __FOXLCD_H__
#define __FOXLCD_H__

#define CHAR_10       0x7B
#define CHAR_12       0x7C
#define CHAR_15       0x7D
#define CHAR_ENTER    0x7E
#define CHAR_UP        0xD9
#define CHAR_DOWN      0xDA
#define CHAR_BELL      0xED
#define CHAR_1DEVIDE4  0xF0
#define CHAR_1DEVIDE3  0xF1
#define CHAR_1DEVIDE2  0xF2

class FoxLCD { //������������� ������ FoxLCD
  public:
    FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress);
    void init(void);                       //������� ������������� �������
    void sendcmd(uint8_t cmd);             //������� �������� ������� � ������� (RS ��������� � ����)
    void senddata(uint8_t data);           //������� �������� ������ � ������� (RS ��������� � �������)
    void printchar(char c);                //������� ������ ������� �� �������
    void print(char msg[]);                //������� ������ ������ �� �������
    void clear(void);                      //������� ������� �������
    void home(void);                      //������� �������� ������� � ��������� ���������
    void display(uint8_t state);          //������� ���������/���������� �������
    void cursor(uint8_t state);            //������� ���������/���������� ����������� �������
    void blink(uint8_t state);            //������� ���������/���������� �������� �������
    void setcursor(uint8_t x, uint8_t y);  //������� ����������� ������� �� ���� X � Y
    void chartocgram(uint8_t number, uint8_t chararray[8]); //������� ��������� ������������ ������� � CGRAM. ����� �������� ������� ��� �������� �� 0 �� 7
    void backlight(bool state);            //������� ���������� ���������� �������
};
#endif /* __FOXLCD_H__ */
