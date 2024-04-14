// PIC24FJ128GA010 Configuration Bit Settings
// For more on Configuration Bits, see Section 1.1
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
#include <xc.h>
#include "lcd.h"

#define LCD_ROWS 2
#define LCD_COLUMNS 16

int main(void) {
    LCD_Initialize();
    
    while(1) {
        char animText[] = "PROMOCJA!";
        int textLength = sizeof(animText) - 1;
        
        for (int i = LCD_COLUMNS; i >= -textLength; i--) {
            LCD_ClearScreen();
            if (i < LCD_COLUMNS && i >= -textLength) {
                LCD_PutString(animText + (i < 0 ? -i : 0), textLength + i);
            }
            __delay32(500000);
        }
        
        char animText2[] = "KANAPKA Z SZYNKA - 3.99";
        int textLength2 = sizeof(animText2) - 1;
        
        for (int i = LCD_COLUMNS; i >= -textLength2; i--) {
            LCD_ClearScreen();
            if (i < LCD_COLUMNS && i >= -textLength2) {
                LCD_PutString(animText2 + (i < 0 ? -i : 0), textLength2 + i);
            }
            __delay32(500000);
        }
    }
    return 0;
}