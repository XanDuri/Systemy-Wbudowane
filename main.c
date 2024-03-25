
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
#include <stdlib.h> // Dla funkcji rand()

// Konfiguracja wej??/wyj?? dla przycisków i diod LED
#define BUTTON_FORWARD PORTBbits.RB0 // Przycisk do prze??czania w przód
#define BUTTON_BACKWARD PORTBbits.RB1 // Przycisk do prze??czania w ty?
#define LEDS PORTA // Diody LED

// Funkcja opó?nienia (mo?esz dostosowa? dla swoich potrzeb)
void delay() {
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 100; j++);
    }
}

// Funkcja wy?wietlaj?ca warto?? na diodach LED
void display(unsigned char value) {
    LEDS = value;
}

// Zadanie 1: 8 bitowy licznik binarny zliczaj?cy w gór? (0?255)
void counter_up() {
    unsigned char count = 0;
    while(1) {
        display(count);
        if(BUTTON_FORWARD == 0) {
            while(BUTTON_FORWARD == 0);
            count++;
            if(count == 256) count = 0;
        }
        delay();
    }
}

// Zadanie 2: 8 bitowy licznik binarny zliczaj?cy w dó? (255?0)
void counter_down() {
    unsigned char count = 255;
    while(1) {
        display(count);
        if(BUTTON_BACKWARD == 0) {
            while(BUTTON_BACKWARD == 0);
            if(count == 0) count = 255;
            else count--;
        }
        delay();
    }
}

// Zadanie 3: 8 bitowy licznik w kodzie Graya zliczaj?cy w gór? (repr. 0?255)
void gray_counter_up() {
    unsigned char count = 0;
    while(1) {
        display(count ^ (count >> 1));
        if(BUTTON_FORWARD == 0) {
            while(BUTTON_FORWARD == 0);
            count++;
            if(count == 256) count = 0;
        }
        delay();
    }
}

// Zadanie 4: 8 bitowy licznik w kodzie Graya zliczaj?cy w dó? (repr. 255?0)
void gray_counter_down() {
    unsigned char count = 255;
    while(1) {
        display(count ^ (count >> 1));
        if(BUTTON_BACKWARD == 0) {
            while(BUTTON_BACKWARD == 0);
            if(count == 0) count = 255;
            else count--;
        }
        delay();
    }
}

// Zadanie 5: 2x4 bitowy licznik w kodzie BCD zliczaj?cy w gór? (0?99)
void bcd_counter_up() {
    unsigned char tens = 0;
    unsigned char ones = 0;
    while(1) {
        display((tens << 4) | ones);
        if(BUTTON_FORWARD == 0) {
            while(BUTTON_FORWARD == 0);
            ones++;
            if(ones == 10) {
                ones = 0;
                tens++;
                if(tens == 10) tens = 0;
            }
        }
        delay();
    }
}

// Zadanie 6: 2x4 bitowy licznik w kodzie BCD zliczaj?cy w dó? (99?0)
void bcd_counter_down() {
    unsigned char tens = 9;
    unsigned char ones = 9;
    while(1) {
        display((tens << 4) | ones);
        if(BUTTON_BACKWARD == 0) {
            while(BUTTON_BACKWARD == 0);
            if(ones == 0) {
                ones = 9;
                if(tens == 0) tens = 9;
                else tens--;
            } else {
                ones--;
            }
        }
        delay();
    }
}

// Zadanie 7: 3 bitowy w??yk poruszaj?cy si? lewo-prawo
void snake() {
    unsigned char pattern = 7; // Warto?? pocz?tkowa dla w??a
    unsigned char direction = 1; // Kierunek ruchu (1 - w prawo, 0 - w lewo)
    while(1) {
        display(pattern);
        delay();
        if(direction == 1) {
            pattern <<= 1;
            if(pattern == 14) direction = 0;
        } else {
            pattern >>= 1;
            if(pattern == 7) direction = 1;
        }
    }
}

// Zadanie 8: Kolejka
void queue() {
    unsigned char pattern = 1;
    while(1) {
        display(pattern);
        delay();
        pattern <<= 1;
        if(pattern == 255) break;
    }
}

// Zadanie 9: 6 bitowy generator liczb pseudolosowych oparty o konfiguracj? 1110011
unsigned char pseudo_random() {
    static unsigned char seed = 0x73; // Seed zgodny z konfiguracj? 1110011
    seed ^= (seed << 5);
    seed ^= (seed >> 7);
    seed ^= (seed << 3);
    return seed;
}

int main(void) {
    // Konfiguracja wej??/wyj??
    TRISA = 0x00; // Ustawienie portu A jako wyj?cie (diod LED)
    TRISB = 0xFF; // Ustawienie portu B jako wej?cie (przyciski)

    // Zmienna przechowuj?ca stan przycisku poprzedniego
    unsigned char previous_button_state = 1;
    // Numer aktualnie wybranego podprogramu
    unsigned char program_number = 0;

    // P?tla niesko?czona
    while(1) {
        // Obs?uga prze??czania mi?dzy podprogramami
        if(BUTTON_FORWARD == 0 && previous_button_state != 0) {
            program_number++;
            if(program_number > 8) program_number = 0; // Zwró? uwag?, ?e tutaj jest 8, poniewa? mamy 9 podprogramów
            while(BUTTON_FORWARD == 0);
        }
        else if(BUTTON_BACKWARD == 0 && previous_button_state != 0) {
            if(program_number == 0) program_number = 8;
            else program_number--;
            while(BUTTON_BACKWARD == 0);
        }
        previous_button_state = BUTTON_FORWARD;

        // Wybór zadania do uruchomienia
        switch(program_number) {
            case 0:
                counter_up();
                break;
            case 1:
                counter_down();
                break;
            case 2:
                gray_counter_up();
                break;
            case 3:
                gray_counter_down();
                break;
            case 4:
                bcd_counter_up();
                break;
            case 5:
                bcd_counter_down();
                break;
            case 6:
                snake();
                break;
            case 7:
                queue();
                break;
            case 8:
                /*unsigned char random_value = pseudo_random();
                display(random_value);
                delay();*/
                break;
        }
    }
    return 0;
}