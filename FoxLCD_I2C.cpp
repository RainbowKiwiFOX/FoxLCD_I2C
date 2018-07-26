#include "stm32f0xx_hal.h"
#include <stdio.h>

#define WRITE_HIGH(reg, bit) reg |= (1<<bit)
#define WRITE_LOW(reg, bit) reg &= ~(1<<bit)


class FoxLCD { //Инициализация класса FoxLCD
  //Глобальные приватные переменные внутри класса 
  I2C_HandleTypeDef *i2c;
  uint8_t adress;
  
  bool backlight_state;
  uint8_t displaysettings; //Настройки дисплея - дисплей включен, курсор выключен не мигает 
  public:
    //Конструктор класса
    FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress);
    void init(void);                       //Функция инициализации дисплея
    void sendcmd(uint8_t cmd);             //Функция отправки команды в дисплей (RS подключен к нулю)
    void senddata(uint8_t data);           //Функция отправки данных в дисплей (RS подключен к единице)
    void printchar(char c);                //Функция печати символа на дисплей
    void print(char msg[]);                //Функция печати текста на дисплее
    void print(bool boolean);              //Функция печати на дисплее логического типа данных (true/false)
    void print(int number);                //Функция печати на дисплее целого знакового числа
    void print(float number);                //Функция печати на дисплее знакового числая с плавающей запятой
    void clear(void);                      //Функция очистки дисплея
    void home(void);                       //Функция возврата каретки в начальное состояние
    void display(uint8_t state);           //Функция включения/отключения дисплея
    void cursor(uint8_t state);            //Функция включения/отключения отображения курсора
    void blink(uint8_t state);             //Функция включения/отключения моргания курсора
    void setcursor(uint8_t x, uint8_t y);  //Функция перемещения курсора по осям X и Y
    void chartocgram(uint8_t number, uint8_t chararray[8]); //Функция генерации собственного символа в CGRAM. Можно положить символы под номерами от 0 до 7
    void backlight(bool state);            //Функция управления подсветкой дисплея
};
FoxLCD::FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress) { //Конструктор четырёхбитного интерфейса без чтения флага занятости
  //Присваивание входных значений глобальным переменным
  this -> i2c = i2c;
  this -> adress = adress<<1;
}
void FoxLCD::init(void) {
  displaysettings = 0x0C;
  backlight_state = true;
  //Задержка 40 мс для гарантированного старта дисплея
  HAL_Delay(40);
  //Отправка команд инициализации дисплея
  //Установка 4-х битного интерфейса
  sendcmd(0x30); //0b00100000
  sendcmd(0x30); //0b00100000
  //Включение дисплея в двухстрочном режиме
  sendcmd(0xC0); //0b11000000
  HAL_Delay(1); //Ожидание выполнения команды
  //Включение дисплея с выключенным немигающим курсором
  sendcmd(0x00); //0b00000000
  sendcmd(0xC0); //0b11000000
  HAL_Delay(1); //Ожидание выполнения команды
  //Очистка дисплея
  sendcmd(0x00); //0b00000000
  sendcmd(0x10); //0b00010000
  HAL_Delay(2); //Ожидание выполнения команды
  //Настройка ввода - инкрементальный режим, смещение дисплея выключено
  sendcmd(0x00); //0b00000000
  sendcmd(0x60); //0b01100000
  HAL_Delay(1); //Ожидание выполнения команды
  home();
  HAL_Delay(2);
  sendcmd(0x0C);
  HAL_Delay(1);
  clear();
  HAL_Delay(2);
}

//Функция отправки команды в дисплей (RS подключен к нулю)
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

//Функция отправки данных в дисплей (RS подключен к единице)
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

//Функция печати символа на дисплей
void FoxLCD::printchar(char c) {
    //Поддержка русских символов "на лету" (только для дисплеев с кириллицей в знакогенераторе)
    switch(c) {
      case 'А':
        senddata('A');
        break;
      case 'а':
        senddata('a');
        break;
      case 'Б':
        senddata(0xA0);
        break;
      case 'б':
        senddata(0xB2);
        break;
      case 'В':
        senddata('B');
        break;
      case 'в':
        senddata(0xB3);
        break;
      case 'Г':
        senddata(0xA1);
        break;
      case 'г':
        senddata(0xB4);
        break;
      case 'Д':
        senddata(0xE0);
        break;
      case 'д':
        senddata(0xE3);
        break;
      case 'Е':
        senddata('E');
        break;
      case 'е':
        senddata('e');
        break;
      case 'Ё':
        senddata(0xA2);
        break;
      case 'ё':
        senddata(0xB5);
        break;
      case 'Ж':
        senddata(0xA3);
        break;
      case 'ж':
        senddata(0xB6);
        break;
      case 'З':
        senddata(0xA4);
        break;
      case 'з':
        senddata(0xB7);
        break;
      case 'И':
        senddata(0xA5);
        break;
      case 'и':
        senddata(0xB8);
        break;
      case 'Й':
        senddata(0xA6);
        break;
      case 'й':
        senddata(0xB9);
        break;
      case 'К':
        senddata('K');
        break;
      case 'к':
        senddata(0xBA);
        break;
      case 'Л':
        senddata(0xA7);
        break;
      case 'л':
        senddata(0xBB);
        break;
      case 'М':
        senddata('M');
        break;
      case 'м':
        senddata(0xBC);
        break;
      case 'Н':
        senddata('H');
        break;
      case 'н':
        senddata(0xBD);
        break;
      case 'О':
        senddata('O');
        break;
      case 'о':
        senddata('o');
        break;
      case 'П':
        senddata(0xA8);
        break;
      case 'п':
        senddata(0xBE);
        break;
      case 'Р':
        senddata('P');
        break;
      case 'р':
        senddata('p');
        break;
      case 'С':
        senddata('C');
        break;
      case 'с':
        senddata('c');
        break;
      case 'Т':
        senddata('T');
        break;
      case 'т':
        senddata(0xBF);
        break;
      case 'У':
        senddata(0xA9);
        break;
      case 'у':
        senddata('y');
        break;
      case 'Ф':
        senddata(0xAA);
        break;
      case 'ф':
        senddata(0xE4);
        break;
      case 'Х':
        senddata('X');
        break;
      case 'х':
        senddata('x');
        break;
      case 'Ц':
        senddata(0xE1);
        break;
      case 'ц':
        senddata(0xE5);
        break;
      case 'Ч':
        senddata(0xAB);
        break;
      case 'ч':
        senddata(0xC0);
        break;
      case 'Ш':
        senddata(0xAC);
        break;
      case 'ш':
        senddata(0xC1);
        break;
      case 'Щ':
        senddata(0xE2);
        break;
      case 'щ':
        senddata(0xE6);
        break;
      case 'Ъ':
        senddata(0xAD);
        break;
      case 'ъ':
        senddata(0xC2);
        break;
      case 'Ы':
        senddata(0xAE);
        break;
      case 'ы':
        senddata(0xC3);
        break;
      case 'Ь':
        senddata('b');
        break;
      case 'ь':
        senddata(0xC4);
        break;
      case 'Э':
        senddata(0xAF);
        break;
      case 'э':
        senddata(0xC5);
        break;
      case 'Ю':
        senddata(0xB0);
        break;
      case 'ю':
        senddata(0xC6);
        break;
      case 'Я':
        senddata(0xB1);
        break;
      case 'я':
        senddata(0xC7);
        break;
      default:
        senddata(c);
        break;
    }
}
//Функция печати текста на дисплее
void FoxLCD::print(char msg[]) {
  char i = 0;
  while(*msg) {
    i = *msg++;
    printchar(i);
    HAL_Delay(1);
  }
}
//Функция печати на дисплее логического типа данных (true/false)
void FoxLCD::print(bool boolean) {
  boolean ? print("true") : print("false");
}
 //Функция печати на дисплее целого знакового числа
void FoxLCD::print(int number) {
  char buff[11];
  sprintf(buff, "%d", number);
  print(buff);
}
//Функция печати на дисплее знакового числая с плавающей запятой
void FoxLCD::print(float number) {
  char buff[2];
  sprintf(buff, "%.2f", number);
  print(buff);
}
//Функция очистки дисплея
void FoxLCD::clear(void) {
  sendcmd(0x01);
  HAL_Delay(2);
}
//Функция возврата каретки в начальное состояние
void FoxLCD::home(void) {
  sendcmd(0x02);
  HAL_Delay(2);
}

//Функция включения/отключения дисплея
void FoxLCD::display(uint8_t state) {
  state != 0 ? WRITE_HIGH(displaysettings, 2) : WRITE_LOW(displaysettings, 2);
  sendcmd(displaysettings);
  HAL_Delay(1);
}
//Функция включения/отключения отображения курсора
void FoxLCD::cursor(uint8_t state) {
  state != 0 ? WRITE_HIGH(displaysettings, 1) : WRITE_LOW(displaysettings, 1);
  sendcmd(displaysettings);
  HAL_Delay(1);
}
//Функция включения/отключения моргания курсора
void FoxLCD::blink(uint8_t state) {
  state != 0 ? WRITE_HIGH(displaysettings, 0) : WRITE_LOW(displaysettings, 0);
  sendcmd(displaysettings);
  HAL_Delay(1);
}

//Функция перемещения курсора по осям X и Y
void FoxLCD::setcursor(uint8_t x, uint8_t y) {
  //Костыль для поддержки четырёхстрочных дисплеев
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

//Функция генерации собственного символа в CGRAM
void FoxLCD::chartocgram(uint8_t number, uint8_t chararray[8]) {
  sendcmd(0x40 + number*8);
  for (uint8_t i = 0; i < 8; i++) {
    senddata(chararray[i]);
  }
}
//Функция управления подсветкой дисплея
void FoxLCD::backlight(bool state) {
  this -> backlight_state = state;
  sendcmd(0x00);
}
