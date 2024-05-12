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
#include <xc.h>
#include "adc.h"
#include "buttons.h"
#include "lcd.h"
void system_init(void) {
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);

    LCD_Initialize();

    BUTTON_Enable(BUTTON_S3);
    BUTTON_Enable(BUTTON_S4);
    BUTTON_Enable(BUTTON_S6);
    BUTTON_Enable(BUTTON_S5);
}

void display_start_screen(void) {
    LCD_ClearScreen();
    LCD_PutString("   MIKROFALA", 12);
    LCD_PutChar('\n');
    LCD_PutString(" MOC CZAS EXIT", 14);
}


void displayPowerControl(void) {
    uint16_t powerLevel;

    LCD_ClearScreen();
    LCD_PutString("Wybierz moc", 11);
    LCD_PutChar('\n');
    __delay32(1000000);
    while (1) {
        if (BUTTON_IsPressed(BUTTON_S5)) {
            mainMenu(); // Powrót do ekranu startowego
            return;  // Wyj?cie z funkcji
        }

        if (BUTTON_IsPressed(BUTTON_S4)) {
            powerLevel = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);  // Odczytaj warto?? z potencjometru
            LCD_ClearScreen();
            LCD_PutString("Wybierz moc", 11);
            LCD_PutChar('\n');
            LCD_PutString("Moc: ", 5);
            LCD_PutNumber(powerLevel);  // Za?o?enie, ?e funkcja wy?wietla liczb?
            LCD_PutChar('%');
            __delay32(400000);  // Opó?nienie o 100 ms

            while (BUTTON_IsPressed(BUTTON_S4)); // Debouncing przycisku
        }
    }
}

void LCD_PutNumber(uint16_t number) {
    if (number >= 1000) {
        LCD_PutChar('0' + number / 1000 % 10);
    }
    if (number >= 100) {
        LCD_PutChar('0' + number / 100 % 10);
    }
    if (number >= 10) {
        LCD_PutChar('0' + number / 10 % 10);
    }
    LCD_PutChar('0' + number % 10);
}

void displayTimeControl(void) {
    uint16_t time;
    uint8_t clickCount = 0;  // Licznik klikni?? przycisku S4
    
    LCD_ClearScreen();
    LCD_PutString("Ustaw czas", 10);
    LCD_PutChar('\n');
    __delay32(1000000);
    while (1) {
        if (BUTTON_IsPressed(BUTTON_S5)) {
            mainMenu();  // Powrót do menu g?ównego
            return;
        }

        if (BUTTON_IsPressed(BUTTON_S4)) {
            __delay32(400000);  // Debounce delay
            clickCount++;  // Zwi?ksz licznik klikni??
            if (clickCount == 1) {
                time = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);  // Odczytaj warto?? z potencjometru
                LCD_ClearScreen();
                LCD_PutString("Czas ustawiony: ", 15);
                LCD_PutNumber(time);
                LCD_PutString("ms", 2);
            } else if (clickCount == 2) {
                startTimer(time);  // Rozpocznij odliczanie czasu
                break;
            }
            while (BUTTON_IsPressed(BUTTON_S4));  // Debouncing przycisku
        }
    }
}

void startTimer(uint16_t duration) {
    uint16_t remainingTime = duration;
    uint16_t time;  // Dodanie deklaracji zmiennej time

    while (remainingTime > 0) {
        if (BUTTON_IsPressed(BUTTON_S5)) {  // Dodaj czas
            time = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
            remainingTime += time;
            LCD_ClearScreen();
            LCD_PutString("Czas dodany: ", 13);
            LCD_PutNumber(remainingTime);
            LCD_PutString("ms", 2);
            continue;
        }

        if (BUTTON_IsPressed(BUTTON_S6)) {  // Natychmiastowe zatrzymanie i migotanie ?wiate?
            flashLEDs();
            mainMenu();
            return;
        }

        LCD_ClearScreen();
        LCD_PutString("Pozostaly czas: ", 16);
        LCD_PutNumber(remainingTime);
        LCD_PutString("ms", 2);
        __delay32(400000);  // Opó?nienie o 100 ms
        remainingTime--;
    }

    flashLEDs();  // Migotanie diod po zako?czeniu odliczania
    mainMenu();
}

void flashLEDs() {
    AD1PCFG = 0xFFFF; // set to digital I/O (not analog)
    TRISA = 0x0000; // set all port bits to be output
    for (int i = 0; i < 10; i++) {
        LATA = 0xFFFF; // Zak?adaj?c, ?e LATx steruje diodami LED
        __delay32(2000000);  // 50 ms w??czone
        LATA = 0x0000;
        __delay32(2000000);  // 50 ms wy??czone
    }
}

void mainMenu(void) {
    display_start_screen();

    while (1) {
        __delay32(1400000);
        if (BUTTON_IsPressed(BUTTON_S6)) {
            displayPowerControl();
        } else if (BUTTON_IsPressed(BUTTON_S5)) {
            displayTimeControl();
        } else if (BUTTON_IsPressed(BUTTON_S4)) {
            break;  // EXIT
        }
    }
}

int main(void) {
    system_init();
    mainMenu();

    while (1) {
        // Potentially low-power sleep
    }

    return 0;
}