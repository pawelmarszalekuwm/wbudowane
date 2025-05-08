
// PIC24FJ128GA010 Configuration Bit Settings

// 'C' source line config statements

// CONFIG2
#pragma config POSCMOD = XT             // Primary Oscillator Select (XT Oscillator mode selected)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRI              // Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = ON              // Watchdog Timer Enable (Watchdog Timer is enabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>
#include "buttons.h"
#include "lcd.h"
#include "string.h"

void system_init(void){
    
    LCD_Initialize();
    
    BUTTON_Enable(BUTTON_S3);
    BUTTON_Enable(BUTTON_S4);
    BUTTON_Enable(BUTTON_S5);
    BUTTON_Enable(BUTTON_S6);    
}

void display_start_screen(void){
    
    LCD_ClearScreen();
    LCD_PutString("DZIEN DOBRY", 11);
    LCD_PutChar('\n');
    LCD_PutString("EKRAN LCD", 9);    
}

void flashLEDs(){

    AD1PCFG = 0xFFFF;
    TRISA = 0x0000;
    LATA = 0xFFFF;
}

void displayText(void){
    
    const char* dni[] = {"czwartek", "sroda"};
    int dzienIndex = 0;
    
    LCD_ClearScreen();
    LCD_PutString("Wybierz dzien", 13);
    LCD_PutChar('\n');
    LCD_PutString("dzien: ",7);
    LCD_PutString(dni[dzienIndex], strlen(dni[dzienIndex]));
    
    __delay32(1000000);
    while(1){
        
        if(BUTTON_IsPressed(BUTTON_S5)){
            mainMenu();
            return;
        }
        if(BUTTON_IsPressed(BUTTON_S4)){
            dzienIndex = (dzienIndex+1)%2;
            
            LCD_ClearScreen();
            LCD_PutString("Wybierz dzien", 13);
            LCD_PutChar('\n');
            LCD_PutString("dzien: ", 7);
            LCD_PutString(dni[dzienIndex], strlen(dni[dzienIndex]));
            
            __delay32(400000);
            while(BUTTON_IsPressed(BUTTON_S4));
            
        }
        
        if (BUTTON_IsPressed(BUTTON_S6)){
            LCD_ClearScreen();
            LCD_PutString("Wybrano:", 8);
            LCD_PutString(dni[dzienIndex], strlen(dni[dzienIndex]));
            
            flashLEDs();
            __delay32(500000);
            
        }
        if(BUTTON_IsPressed(BUTTON_S3)){
            LATA = 0x0000;
            LCD_ClearScreen();
            LCD_PutString("Wyjscie",7);
            __delay32(1000000);
            return;
        }
        
        
    }
    
}

void mainMenu(void){
    display_start_screen();
    
    while (1){
        
        __delay32(1400000);
        if(BUTTON_IsPressed(BUTTON_S5)){
            displayText();
            return;
        }else if(BUTTON_IsPressed(BUTTON_S4)){
            displayText();
            return;
        }else if (BUTTON_IsPressed(BUTTON_S3)){
            break;
        }
    }
}


int main(void){
    system_init();
    mainMenu();
    while(1);
    return 0;
}
