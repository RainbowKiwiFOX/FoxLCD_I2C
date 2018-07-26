#include "stm32f0xx_hal.h"
#include <stdio.h>

#define WRITE_HIGH(reg, bit) reg |= (1<<bit)
#define WRITE_LOW(reg, bit) reg &= ~(1<<bit)


class FoxLCD { //������������� ������ FoxLCD
  //���������� ��������� ���������� ������ ������ 
  I2C_HandleTypeDef *i2c;
  uint8_t adress;
  
  bool backlight_state;
  uint8_t displaysettings; //��������� ������� - ������� �������, ������ �������� �� ������ 
  public:
    //����������� ������
    FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress);
    void init(void);                       //������� ������������� �������
    void sendcmd(uint8_t cmd);             //������� �������� ������� � ������� (RS ��������� � ����)
    void senddata(uint8_t data);           //������� �������� ������ � ������� (RS ��������� � �������)
    void printchar(char c);                //������� ������ ������� �� �������
    void print(char msg[]);                //������� ������ ������ �� �������
    void print(bool boolean);              //������� ������ �� ������� ����������� ���� ������ (true/false)
    void print(int number);                //������� ������ �� ������� ������ ��������� �����
    void print(float number);                //������� ������ �� ������� ��������� ������ � ��������� �������
    void clear(void);                      //������� ������� �������
    void home(void);                       //������� �������� ������� � ��������� ���������
    void display(uint8_t state);           //������� ���������/���������� �������
    void cursor(uint8_t state);            //������� ���������/���������� ����������� �������
    void blink(uint8_t state);             //������� ���������/���������� �������� �������
    void setcursor(uint8_t x, uint8_t y);  //������� ����������� ������� �� ���� X � Y
    void chartocgram(uint8_t number, uint8_t chararray[8]); //������� ��������� ������������ ������� � CGRAM. ����� �������� ������� ��� �������� �� 0 �� 7
    void backlight(bool state);            //������� ���������� ���������� �������
};
FoxLCD::FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress) { //����������� ������������� ���������� ��� ������ ����� ���������
  //������������ ������� �������� ���������� ����������
  this -> i2c = i2c;
  this -> adress = adress<<1;
}
void FoxLCD::init(void) {
  displaysettings = 0x0C;
  backlight_state = true;
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
  i2cbuf[0] = highcmd | (backlight_state<<3) | (1<<2);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
  HAL_Delay(1);
  i2cbuf[0] = highcmd | (backlight_state<<3);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
  uint8_t lowcmd = ((cmd << 4) & 0xF0);
  i2cbuf[0] = lowcmd | (backlight_state<<3) | (1<<2);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
  HAL_Delay(1);
  i2cbuf[0] = lowcmd | (backlight_state<<3);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
}

//������� �������� ������ � ������� (RS ��������� � �������)
void FoxLCD::senddata(uint8_t data) {
  uint8_t i2cbuf[1];
  uint8_t highcmd = (data & 0xF0);
  i2cbuf[0] = highcmd | (backlight_state<<3) | (1<<0) | (1<<2);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
  HAL_Delay(1);
  i2cbuf[0] = highcmd | (backlight_state<<3) | (1<<0);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
  uint8_t lowcmd = ((data << 4) & 0xF0);
  i2cbuf[0] = lowcmd | (backlight_state<<3) | (1<<0) | (1<<2);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
  HAL_Delay(1);
  i2cbuf[0] = lowcmd | (backlight_state<<3) | (1<<0);
  HAL_I2C_Master_Transmit(i2c, adress, i2cbuf, 1, HAL_MAX_DELAY);
}

//������� ������ ������� �� �������
void FoxLCD::printchar(char c) {
    //��������� ������� �������� "�� ����" (������ ��� �������� � ���������� � ���������������)
    switch(c) {
      case '�':
        senddata('A');
        break;
      case '�':
        senddata('a');
        break;
      case '�':
        senddata(0xA0);
        break;
      case '�':
        senddata(0xB2);
        break;
      case '�':
        senddata('B');
        break;
      case '�':
        senddata(0xB3);
        break;
      case '�':
        senddata(0xA1);
        break;
      case '�':
        senddata(0xB4);
        break;
      case '�':
        senddata(0xE0);
        break;
      case '�':
        senddata(0xE3);
        break;
      case '�':
        senddata('E');
        break;
      case '�':
        senddata('e');
        break;
      case '�':
        senddata(0xA2);
        break;
      case '�':
        senddata(0xB5);
        break;
      case '�':
        senddata(0xA3);
        break;
      case '�':
        senddata(0xB6);
        break;
      case '�':
        senddata(0xA4);
        break;
      case '�':
        senddata(0xB7);
        break;
      case '�':
        senddata(0xA5);
        break;
      case '�':
        senddata(0xB8);
        break;
      case '�':
        senddata(0xA6);
        break;
      case '�':
        senddata(0xB9);
        break;
      case '�':
        senddata('K');
        break;
      case '�':
        senddata(0xBA);
        break;
      case '�':
        senddata(0xA7);
        break;
      case '�':
        senddata(0xBB);
        break;
      case '�':
        senddata('M');
        break;
      case '�':
        senddata(0xBC);
        break;
      case '�':
        senddata('H');
        break;
      case '�':
        senddata(0xBD);
        break;
      case '�':
        senddata('O');
        break;
      case '�':
        senddata('o');
        break;
      case '�':
        senddata(0xA8);
        break;
      case '�':
        senddata(0xBE);
        break;
      case '�':
        senddata('P');
        break;
      case '�':
        senddata('p');
        break;
      case '�':
        senddata('C');
        break;
      case '�':
        senddata('c');
        break;
      case '�':
        senddata('T');
        break;
      case '�':
        senddata(0xBF);
        break;
      case '�':
        senddata(0xA9);
        break;
      case '�':
        senddata('y');
        break;
      case '�':
        senddata(0xAA);
        break;
      case '�':
        senddata(0xE4);
        break;
      case '�':
        senddata('X');
        break;
      case '�':
        senddata('x');
        break;
      case '�':
        senddata(0xE1);
        break;
      case '�':
        senddata(0xE5);
        break;
      case '�':
        senddata(0xAB);
        break;
      case '�':
        senddata(0xC0);
        break;
      case '�':
        senddata(0xAC);
        break;
      case '�':
        senddata(0xC1);
        break;
      case '�':
        senddata(0xE2);
        break;
      case '�':
        senddata(0xE6);
        break;
      case '�':
        senddata(0xAD);
        break;
      case '�':
        senddata(0xC2);
        break;
      case '�':
        senddata(0xAE);
        break;
      case '�':
        senddata(0xC3);
        break;
      case '�':
        senddata('b');
        break;
      case '�':
        senddata(0xC4);
        break;
      case '�':
        senddata(0xAF);
        break;
      case '�':
        senddata(0xC5);
        break;
      case '�':
        senddata(0xB0);
        break;
      case '�':
        senddata(0xC6);
        break;
      case '�':
        senddata(0xB1);
        break;
      case '�':
        senddata(0xC7);
        break;
      default:
        senddata(c);
        break;
    }
}
//������� ������ ������ �� �������
void FoxLCD::print(char msg[]) {
  char i = 0;
  while(*msg) {
    i = *msg++;
    printchar(i);
    HAL_Delay(1);
  }
}
//������� ������ �� ������� ����������� ���� ������ (true/false)
void FoxLCD::print(bool boolean) {
  boolean ? print("true") : print("false");
}
 //������� ������ �� ������� ������ ��������� �����
void FoxLCD::print(int number) {
  char buff[11];
  sprintf(buff, "%d", number);
  print(buff);
}
//������� ������ �� ������� ��������� ������ � ��������� �������
void FoxLCD::print(float number) {
  char buff[2];
  sprintf(buff, "%.2f", number);
  print(buff);
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
//������� ���������� ���������� �������
void FoxLCD::backlight(bool state) {
  this -> backlight_state = state;
  sendcmd(0x00);
}
