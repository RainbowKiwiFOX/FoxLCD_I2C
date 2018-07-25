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

class FoxLCD { //Инициализация класса FoxLCD
  public:
    FoxLCD(I2C_HandleTypeDef *i2c, uint8_t adress);
    void init(void);                       //Функция инициализации дисплея
    void sendcmd(uint8_t cmd);             //Функция отправки команды в дисплей (RS подключен к нулю)
    void senddata(uint8_t data);           //Функция отправки данных в дисплей (RS подключен к единице)
    void printchar(char c);                //Функция печати символа на дисплей
    void print(char msg[]);                //Функция печати текста на дисплее
    void clear(void);                      //Функция очистки дисплея
    void home(void);                      //Функция возврата каретки в начальное состояние
    void display(uint8_t state);          //Функция включения/отключения дисплея
    void cursor(uint8_t state);            //Функция включения/отключения отображения курсора
    void blink(uint8_t state);            //Функция включения/отключения моргания курсора
    void setcursor(uint8_t x, uint8_t y);  //Функция перемещения курсора по осям X и Y
    void chartocgram(uint8_t number, uint8_t chararray[8]); //Функция генерации собственного символа в CGRAM. Можно положить символы под номерами от 0 до 7
    void backlight(bool state);            //Функция управления подсветкой дисплея
};
#endif /* __FOXLCD_H__ */
