// PIC24FJ128GA010 Configuration Bit Settings
// For more on Configuration Bits
// consult your device data sheet
// CONFIG2
#pragma config POSCMOD = XT // XT Oscillator mode selected
#pragma config OSCIOFNC = ON // OSC2/CLKO/RC15 as port I/O (RC15)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor disabled
#pragma config FNOSC = PRI // Primary Oscillator (XT, HS, EC)
#pragma config IESO = ON // Int Ext Switch Over Mode enabled
// CONFIG1
#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF // Watchdog Timer disabled
#pragma config ICS = PGx2 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF // Writes to program memory allowed
#pragma config GCP = OFF // Code protection is disabled
#pragma config JTAGEN = OFF // JTAG port is disabled
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include "buttons.h"
#include <math.h>
#include <xc.h>

unsigned char grayToBinary(unsigned char num) {
    unsigned char mask;
    for (mask = num >> 1; mask != 0; mask = mask >> 1) {
        num = num ^ mask;
    }
    return num;
}

unsigned char lfsr = 0b111001;

unsigned char getNextRandomBit() {
    unsigned char bit = ((lfsr >> 0) ^ (lfsr >> 1)) & 1;
    lfsr = (lfsr >> 1) | (bit << 5);
    return bit;
}

int main(void) {
    unsigned char portValue;
    
    // Konfiguracja portów
    AD1PCFG = 0xFFFF;
    TRISA = 0x0000;
    portValue = 0x00;
    
    int program = 0;
    unsigned char snakePosition = 0b00000111;
    int direction = 1;
    int kolejka = 0x00;
    
    while(1){
        if(BUTTON_IsPressed(BUTTON_S3)){
            if(program==0){
                program=9;
            }else{
                program--;
                __delay32(1500000);
            }
        }
        if(BUTTON_IsPressed(BUTTON_S4)){
            if(program==9){
                program=0;
            }else{
                program++;
                __delay32(1500000);
            }
        }
        switch(program){
            case 0:
                for(int i=0;i<=255;i++){
                    LATA = i;
                    __delay32(1500000);
                    if(BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                }
                break;
            case 1:
                for(int i=255;i>=0;i--){
                    LATA = i;
                    __delay32(1500000);
                    if(BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                }
                break;
            case 2:
                for (int i = 0; i <= 255; i++) {
                    LATA = grayToBinary(i);
                    __delay32(1500000);
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                }
                break;
            case 3:
                for (int i = 255; i >= 0; i--) {
                    LATA = grayToBinary(i);
                    __delay32(1500000);
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                }
                break;
            case 4:
                for (int i = 0; i <= 99; i++) {
                    LATA = ((i / 10) << 4) | (i % 10);
                    __delay32(1500000);
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                }
                break;
            case 5:
                for (int i = 99; i >= 0; i--) {
                    LATA = ((i / 10) << 4) | (i % 10);
                    __delay32(1500000);
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                }
                break;
            case 6:
                while(1) {
                    LATA = snakePosition;
                    __delay32(500000);
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                    if (snakePosition == 0b11100000) {
                        direction = -1;
                    } else if (snakePosition == 0b00000111) {
                        direction = 1;
                    }
                    if (direction == 1) {
                        snakePosition = (snakePosition << 1); 
                    } else {
                        snakePosition = (snakePosition >> 1);
                    }
                }
                break;
            case 7:
                while(1){
                    for(int i=0;i<=7;i++){
                        if(kolejka == 255){
                            kolejka = 0;
                        } else{
                            kolejka = kolejka+pow(2,i);
                            LATA = kolejka;
                            __delay32(500000);
                        }
                        if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                    }
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)){ 
                        kolejka = 0x00;
                        break;
                    }
                }
                break;
            case 8:
                while(1){
                    for(int i=7;i>=0;i--){
                        if(kolejka == 255){
                            kolejka = 0;
                        } else{
                            kolejka = kolejka+pow(2,i);
                            LATA = kolejka;
                            __delay32(500000);
                        }
                        if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                    }
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)){ 
                        kolejka = 0x00;
                        break;
                    }
                }
                break;
            case 9:
                while(1){
                    unsigned char randomBit = getNextRandomBit();
                    LATA = lfsr; // Output random bit
                    __delay32(1500000);
                    if (BUTTON_IsPressed(BUTTON_S4) || BUTTON_IsPressed(BUTTON_S3)) break;
                }
                break;
            default:
                break;
        }
    }
    return -1;
    }
