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
#include "buttons.h"
#include "lcd.h"

volatile int player1Time = 8;  // Czas dla gracza 1 w sekundach
volatile int player2Time = 8;  // Czas dla gracza 2 w sekundach
volatile int *currentPlayerTime = &player1Time;
volatile int *waitingPlayerTime = &player2Time;
volatile bool isGameActive = false;

void system_init(void) {
    LCD_Initialize();
    BUTTON_Enable(BUTTON_S3);
    BUTTON_Enable(BUTTON_S4);
    BUTTON_Enable(BUTTON_S5);
    BUTTON_Enable(BUTTON_S6);
}

void LCD_PutNumber(uint16_t number) {
    char leading = 1; // Zmienna do ?ledzenia, czy to pocz?tek liczby

    if (number >= 10000) {
        LCD_PutChar('0' + number / 10000 % 10);
        leading = 0;
    }
    if (number >= 1000 || !leading) {
        LCD_PutChar('0' + number / 1000 % 10);
        leading = 0;
    }
    if (number >= 100 || !leading) {
        LCD_PutChar('0' + number / 100 % 10);
        leading = 0;
    }
    if (number >= 10 || !leading) {
        LCD_PutChar('0' + number / 10 % 10);
    }
    LCD_PutChar('0' + number % 10);
}

void updateDisplay() {
    LCD_ClearScreen();
    LCD_PutString("Player 1: ", 10);
    LCD_PutNumber(player1Time);
    LCD_PutChar('\n');
    LCD_PutString("Player 2: ", 10);
    LCD_PutNumber(player2Time);
}

void switchPlayers() {
    volatile int *temp = currentPlayerTime;
    currentPlayerTime = waitingPlayerTime;
    waitingPlayerTime = temp;
    updateDisplay();
}

void endGame(const char* message) {
    isGameActive = false;
    LCD_ClearScreen();
    LCD_PutString(message, 16);
    __delay32(12000000); 
    mainMenu();
}

void checkButtons() {
    if (BUTTON_IsPressed(BUTTON_S3) && currentPlayerTime == &player2Time) {
        switchPlayers();
    } else if (BUTTON_IsPressed(BUTTON_S4) && currentPlayerTime == &player1Time) {
        switchPlayers();
    }

    if (BUTTON_IsPressed(BUTTON_S5)) {
        endGame(currentPlayerTime == &player1Time ? "MAT: P1" : "MAT: P2");
        __delay32(12000000); 
    }

    if (BUTTON_IsPressed(BUTTON_S6)) {
        endGame("Game Over: Draw");
        __delay32(12000000); 
    }
}

void mainMenu() {
    LCD_ClearScreen();
    LCD_PutString("Chess Clock", 11);
    LCD_PutChar('\n');
    LCD_PutString("Pres S3 to start", 17);
    while (!BUTTON_IsPressed(BUTTON_S3));
    isGameActive = true;
    player1Time = 8;  // Reset time for player 1
    player2Time = 8;  // Reset time for player 2
    currentPlayerTime = &player1Time;
    waitingPlayerTime = &player2Time;
    updateDisplay();
}

void gamePlay() {
    currentPlayerTime = &player1Time;
    waitingPlayerTime = &player2Time;
    updateDisplay();

    while (isGameActive) {
        checkButtons();
        __delay32(4000000);  // Opó?nienie o 1 sekund?
         if (--(*currentPlayerTime) <= 0) {
            endGame(currentPlayerTime == &player1Time ? "Game Over: P1" : "Game Over: P2");
            break;
        }  // Dekrementacja czasu aktualnego gracza
        updateDisplay();
    }
}

int main(void) {
    system_init();
    mainMenu();
    while (1) {
        gamePlay();
    }
    return 0;
}