

/*
 
 
 #include <xc.h>
#include <stdio.h>
#include "nxlcd.h"

#pragma config FOSC = HS
#pragma config CPUDIV = OSC1_PLL2
#pragma config PBADEN = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 20000000

void main() {

    unsigned int q;
    long v;
    unsigned int temp;

    char ADC[6];
    char tensao[8];
    char temp_str[6];

    // ADC config
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001110;
    ADCON2 = 0b10111101;

    TRISA = 0b00000001;

    TRISD = 0x00;
    TRISE = 0x00;

    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01);
    __delay_ms(16);

    while (1) {

        ADCON0bits.GO_DONE = 1;
        while (ADCON0bits.GO_DONE);

        q = (ADRESH << 8) | ADRESL;

        v = (5000L * q) / 1023;

        temp = v / 10;

        sprintf(ADC, "%u", q);
        sprintf(tensao, "%ld", v);
        sprintf(temp_str, "%u", temp);

        // Linha 1: ADC
        WriteCmdXLCD(0x80);
        putsXLCD("ADC:");
        putsXLCD(ADC);

        // Linha 2: temperatura
        WriteCmdXLCD(0xC0);
        putsXLCD("T:");
        putsXLCD(temp_str);
        putrsXLCD("C ");

        __delay_ms(200);
    }
}
 
 */


#include <xc.h>
#include <stdio.h>
#include "nxlcd.h"

#pragma config FOSC = HS
#pragma config CPUDIV = OSC1_PLL2
#pragma config PBADEN = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config CCP2MX = ON

#define _XTAL_FREQ 20000000

#define PASSO 51

unsigned int valor = 512;

void ajusta_dc(unsigned int dc)
{
    CCPR2L = dc >> 2;
    CCP2CONbits.DC2B1 = (dc >> 1) & 1;
    CCP2CONbits.DC2B0 = dc & 1;
}
void UART_Write_Text(char *txt)
{
    while(*txt)
    {
        while(!PIR1bits.TXIF);
        TXREG = *txt++;
    }
}
void __interrupt(high_priority) ISR(void)
{
    if (INTCON3bits.INT1IF)
    {
        valor += PASSO;
        if (valor > 1023) valor = 1023;
        INTCON3bits.INT1IF = 0;
    }

    if (INTCON3bits.INT2IF)
    {
        if (valor > PASSO) valor -= PASSO;
        else valor = 0;

        INTCON3bits.INT2IF = 0;
    }
}

void main()
{
    unsigned int q;
    long v;
    unsigned int temp;

    char ADC[6];
    char temp_str[6];

    TRISA = 0b00000001;
    TRISB = 0b00000110; // RB1, RB2 entradas
    
    TXSTA = 0b00100100;   // TXEN + BRGH
    RCSTA = 0b10000000;   // serial enable
    SPBRG = 129;          // 9600 baud

    TRISCbits.TRISC1 = 0;

    // ADC
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001110;
    ADCON2 = 0b10111101;

    // PWM 6250 Hz
    PR2 = 199;
    T2CON = 0b00000101;
    CCP2CON = 0b00001100;

    // INT1 / INT2
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT2IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT2IF = 0;

    INTCON2bits.INTEDG1 = 1;
    INTCON2bits.INTEDG2 = 1;

    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01);
    __delay_ms(16);

    /*
     * while (1)
    {
        ajusta_dc(valor);

        ADCON0bits.GO_DONE = 1;
        while (ADCON0bits.GO_DONE);

        q = (ADRESH << 8) | ADRESL;

        v = (5000L * q) / 1023;

        temp = v / 10;

        sprintf(ADC, "%u", q);
        sprintf(temp_str, "%u", temp);

        WriteCmdXLCD(0x80);
        putsXLCD("PWM Temp");

        WriteCmdXLCD(0xC0);
        putsXLCD(temp_str);
        putrsXLCD("C ");

        __delay_ms(200);
    }*/
    
        while (1)
    {
        ajusta_dc(valor);

        ADCON0bits.GO_DONE = 1;
        while (ADCON0bits.GO_DONE);

        q = (ADRESH << 8) | ADRESL;

        v = (5000L * q) / 1023;

        temp = v / 10;

        sprintf(temp_str, "%u", temp);

        // LCD
        WriteCmdXLCD(0x80);
        putsXLCD("Temp:");
        putsXLCD(temp_str);
        putrsXLCD("C ");

        // UART ? MATLAB
        UART_Write_Text(temp_str);
        UART_Write_Text("\r\n");

        __delay_ms(500);
    }
}