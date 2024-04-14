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
#include "adc.h"
#include "buttons.h"
#include "xc.h"
#define ALARM_THRESHOLD 512 
#define ALARM_DURATION 12 

void initializePorts() {
    TRISA = 0;
}

void turnOnAlarm() {
    while(1){
        LATA = 0xFF;
        __delay32(1500000);
        LATA = 0x00;
        __delay32(1500000);
        if(BUTTON_IsPressed(BUTTON_S4)){
            break;
        }
    }
}

void turnOffAlarm() {
    LATA = 0x00;
}

void handleAlarm(unsigned int value) {
    static unsigned int alarmOnTime = 0;
    static bool alarmActive = false;

    if (value > ALARM_THRESHOLD) {
        if (!alarmActive) {
            alarmOnTime = 0;
            alarmActive = true;
        }
    } else {
        if (alarmActive) {
            alarmActive = false;
            turnOffAlarm();
        }
    }

    if (alarmActive) {
        alarmOnTime++;

        if (alarmOnTime <= ALARM_DURATION) {
            if (alarmOnTime % 2 == 0) {
                LATA = 0x01;
                __delay32(1500000);
            } else {
                LATA = 0x00;
                __delay32(1500000);
            }
        } else {
            turnOnAlarm();
        }
    }
}

int main(void) {
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
    initializePorts();

    unsigned int value;

    while (1) {
        value = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);

        if (value == 0xFFFF) {
            continue;
        }

        handleAlarm(value);
    }

    return 0;
}