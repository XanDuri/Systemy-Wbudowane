#include "xc.h"
#include <libpic30.h>

#define FCY 4000000UL
#define LCD_E LATDbits.LATD4
#define LCD_RW LATDbits.LATD5
#define LCD_RS LATBbits.LATB15
#define LCD_DATA LATE
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ON 0x0C
#define LCD_OFF 0x08
#define LCD_CONFIG 0x38
#define LCD_CURSOR 0x80
#define LINE1 0x00
#define LINE2 0x40
#define LCD_CUST_CHAR 0x40

void __delay_us(unsigned long us){
    __delay32(us*FCY/1000000);
}
void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}

void LCD_sendCommand(unsigned char command){
    LCD_RW = 0;
    LCD_RS = 0;
    LCD_E = 1;
    LCD_DATA = command;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

void LCD_clearLine(unsigned char line){
    LCD_setCursor(line, 0);
    for (int i = 0; i < 16; i++) {
        LCD_sendData(' '); // Wype?nienie linii spacjami
    }
}

void LCD_setCursor(unsigned char row, unsigned char col){
    unsigned char address;
    if (row == 1){
        address = LCD_CURSOR + LINE1 + col;
    } else if (row == 2){
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_init(){
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
    
}

void LCD_saveCustChar(unsigned char slot, unsigned char *array) {
    unsigned char i;
    LCD_sendCommand(LCD_CUST_CHAR + (slot * 8));
    for (i = 0; i < 8; i++) {
        LCD_sendData(array[i]);
    }
}

void scrollText(){
    char* text1 = "PROMOCJA!        ";
    char* text2 = "        JUZ TYLKO DZIS";
    for (int i = 0; i <= 16 - 9; i++) { // PROMOCJA! ma 9 znaków
        LCD_setCursor(1, i);
        LCD_print(text1 + i);
        __delay_ms(300);
        LCD_clearLine(1);
    }
    for (int i = 16; i >= 0; i--) {
        if (i < 16 - 0) { // JUZ TYLKO DZIS ma 13 znaków
            LCD_setCursor(2, i);
            LCD_print(text2 + 16 - 8);
        }
        __delay_ms(300);
        LCD_clearLine(2);
    }
}

void displayScrollingBanner(){
    char* message = "    SZYNKA PIECZONA - 3,99 ZL";
    int msgLength = 29; // D?ugo?? napisu

    // Zak?adamy, ?e szeroko?? wy?wietlacza to 16 znaków
    for (int pos = 0; pos < msgLength + 16 + 2; pos++) { // Dodajemy 2, aby uwzgl?dni? miejsce na niestandardowe znaki
        LCD_sendCommand(LCD_CLEAR); // Czy?? ekran przed ka?d? now? klatk?

        // P?tla do wypisywania tekstu
        for (int i = 0; i < 16; i++) {
            if (pos + i - 16 >= 0 && pos + i - 16 < msgLength) {
                LCD_setCursor(1, i);
                LCD_sendData(message[pos + i - 16]);
            }
        }

        // Umieszczenie niestandardowych znaków
        if (pos >= msgLength - 4 && pos < msgLength + 16) {
            int charPos = 16 - (pos - (msgLength));
            if (charPos >= 0 && charPos < 16) {
                LCD_setCursor(1, charPos);
                LCD_sendData(0); // Lewy górny róg
                LCD_setCursor(2, charPos);
                LCD_sendData(2); // Lewy dolny róg

                LCD_setCursor(1, charPos + 1);
                LCD_sendData(1); // Prawy górny róg
                LCD_setCursor(2, charPos + 1);
                LCD_sendData(3); // Prawy dolny róg
            }
        }

        __delay_ms(400);
    }
}

unsigned char symbol2[8] = {
    0b00000,
    0b00011,
    0b00111,
    0b00111,
    0b00011,
    0b10001,
    0b11111,
    0b00000
};

unsigned char symbol3[8] = {
    0b00000,
    0b10000,
    0b11000,
    0b11000,
    0b11000,
    0b10000,
    0b11100,
    0b00000
};

unsigned char symbol4[8] = {
    0b00001,
    0b00011,
    0b00111,
    0b00111,
    0b00011,
    0b00001,
    0b00111,
    0b00000
};

unsigned char symbol5[8] = {
    0b11100,
    0b11100,
    0b11100,
    0b11110,
    0b01110,
    0b00010,
    0b00110,
    0b00000
};

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
    LCD_init();
    LCD_saveCustChar(0, symbol2);
    LCD_saveCustChar(1, symbol3);
    LCD_saveCustChar(2, symbol4);
    LCD_saveCustChar(3, symbol5);

    while(1){
        scrollText();
        displayScrollingBanner();
        __delay_ms(2000);
        LCD_sendCommand(LCD_CLEAR);
        __delay_ms(500);
    }
    return 0;
}
