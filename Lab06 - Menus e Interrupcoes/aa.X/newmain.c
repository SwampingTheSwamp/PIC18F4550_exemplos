#include <xc.h>
#include "nxlcd.h"
#define _XTAL_FREQ 20000000



#pragma config FOSC = HS // Fosc = 20MHz -> Tcy = 200ns
#pragma config CPUDIV = OSC1_PLL2
#pragma config WDT    = ON
#pragma config WDTPS  = 512
#pragma config PBADEN = OFF
#pragma config LVP    = OFF

unsigned char h = 23, m = 59, s = 50;
unsigned char modo = 0;
signed char menu=1;


void escreve2dig(unsigned char v);
void atualiza_display() ;

// Rotina de Serviço de Interrupção (ISR) de alta prioridade
void interrupt ISR_Alta_Prioridade(void){
 
            extern signed char menu;
            
    switch (menu){
            case 1:
    WriteCmdXLCD(0x01);
    __delay_ms(2);
    WriteCmdXLCD(0x0C);
    INTCONbits.INT0IF = 0;

    while (1) {
        // Exibe hora atual
        WriteCmdXLCD(0x80);
        putrsXLCD("Relogio:        ");
        WriteCmdXLCD(0xC0);
        escreve2dig(h);
        putcXLCD(':');
        escreve2dig(m);
        putcXLCD(':');
        escreve2dig(s);
        putrsXLCD("  ");

        unsigned char fatia;
        for (fatia = 0; fatia < 100; fatia++) {
            __delay_ms(10);
            CLRWDT();

            // Verifica botao RB0 para entrar no modo ajuste
            if (PORTBbits.RB0 == 0) {
                __delay_ms(20); // debounce
                if (PORTBbits.RB0 == 0) {
                    while (PORTBbits.RB0 == 0); // espera soltar

                    // === MODO AJUSTE ===
                    unsigned char campo = 0; // 0=H, 1=M, 2=S
                    unsigned char ajustando = 1;

                    while (ajustando) {
                        CLRWDT();

                        // Exibe com campo selecionado destacado
                        WriteCmdXLCD(0x80);
                        switch (campo) {
                            case 0: putrsXLCD("Ajuste: [H] M S "); break;
                            case 1: putrsXLCD("Ajuste:  H [M] S"); break;
                            case 2: putrsXLCD("Ajuste:  H  M [S]"); break;
                        }
                        WriteCmdXLCD(0xC0);
                        escreve2dig(h);
                        putcXLCD(':');
                        escreve2dig(m);
                        putcXLCD(':');
                        escreve2dig(s);
                        putrsXLCD("  ");

                        
                        if (PORTBbits.RB1 == 0) {
                            __delay_ms(20);
                            if (PORTBbits.RB1 == 0) {
                                while (PORTBbits.RB1 == 0);
                                switch (campo) {
                                    case 0: h++; if (h >= 24) h = 0; break;
                                    case 1: m++; if (m >= 60) m = 0; break;
                                    case 2: s++; if (s >= 60) s = 0; break;
                                }
                            }
                        }

                        
                        if (PORTBbits.RB2 == 0) {
                            __delay_ms(20);
                            if (PORTBbits.RB2 == 0) {
                                while (PORTBbits.RB2 == 0);
                                switch (campo) {
                                    case 0: if (h == 0) h = 23; else h--; break;
                                    case 1: if (m == 0) m = 59; else m--; break;
                                    case 2: if (s == 0) s = 59; else s--; break;
                                }
                            }
                        }

                       
                        if (PORTBbits.RB0 == 0) {
                            __delay_ms(20);
                            if (PORTBbits.RB0 == 0) {
                                while (PORTBbits.RB0 == 0);
                                campo++;
                                if (campo > 2) {
                                    ajustando = 0; 
                                }
                            }
                        }
                    }

                   
                    WriteCmdXLCD(0x01);
                    __delay_ms(2);
                    WriteCmdXLCD(0x80);
                    putrsXLCD("Hora ajustada!  ");
                    WriteCmdXLCD(0xC0);
                    escreve2dig(h);
                    putcXLCD(':');
                    escreve2dig(m);
                    putcXLCD(':');
                    escreve2dig(s);
                    __delay_ms(1500);

                  
                    WriteCmdXLCD(0x01);
                    __delay_ms(2);
                    WriteCmdXLCD(0x80);
                    putrsXLCD("Opcoes do Menu:");
                    WriteCmdXLCD(0x0C);
                    INTCONbits.INT0IF = 0;
                    return; 
                }
            }
        }

        s++;
        if (s == 60) { s = 0; m++; }
        if (m == 60) { m = 0; h++; }
        if (h == 24) { h = 0; }
    }
    break;
            case 2:
                    LATCbits.LATC1 ^= 1;    

                WriteCmdXLCD(0x01);
                __delay_ms(2);
                WriteCmdXLCD(0x80);
                putrsXLCD("Lampada:        ");
                WriteCmdXLCD(0xC0);
                if (LATCbits.LATC1)
                    putrsXLCD("LIGADA          ");
                else
                    putrsXLCD("DESLIGADA       ");

                __delay_ms(1500);

                WriteCmdXLCD(0x01);
                __delay_ms(2);
                WriteCmdXLCD(0x80);
                putrsXLCD("Opcoes do Menu:");
                WriteCmdXLCD(0x0C);
                break;
            case 3:
                 WriteCmdXLCD(0x01);
                __delay_ms(2);
                WriteCmdXLCD(0x80);
                putrsXLCD("Buzzer:         ");
                WriteCmdXLCD(0xC0);
                putrsXLCD("TOCANDO...      ");

                unsigned int ciclo;
                for (ciclo = 0; ciclo < 400; ciclo++) {
                    LATCbits.LATC2 = 1;
                    __delay_us(2500);    // meio periodo alto (2,5ms)
                    LATCbits.LATC2 = 0;
                    __delay_us(2500);    // meio periodo baixo (2,5ms)

                    // RB0 para o buzzer antecipadamente
                    if (PORTBbits.RB0 == 0) {
                        __delay_ms(20);
                        if (PORTBbits.RB0 == 0) {
                            while (PORTBbits.RB0 == 0);
                            break;
                        }
                    }
                }

                LATCbits.LATC2 = 0;     // garante nivel baixo ao finalizar

                WriteCmdXLCD(0xC0);
                putrsXLCD("PARADO          ");
                __delay_ms(1000);

                WriteCmdXLCD(0x01);
                __delay_ms(2);
                WriteCmdXLCD(0x80);
                putrsXLCD("Opcoes do Menu:");
                WriteCmdXLCD(0x0C);
                break;
            case 4:
                WriteCmdXLCD(0x01);
                __delay_ms(2);
                WriteCmdXLCD(0x80);
                putrsXLCD("Modo Sleep      ");
                __delay_ms(500);

                LATAbits.LATA3 = 0; // LED vermelho indica sleep
                LATAbits.LATA5 = 1;          

              
                INTCONbits.INT0IF  = 0;
                INTCON3bits.INT1IF = 0;
                INTCON3bits.INT2IF = 0;

                Sleep();                 // PIC entra em modo sleep
                NOP();                   // executado logo apos acordar

                
                INTCONbits.INT0IF  = 0;
                INTCON3bits.INT1IF = 0;
                INTCON3bits.INT2IF = 0;

                LATAbits.LATA3 = 1;   // LED verde: funcionamento normal
                LATAbits.LATA5 = 0;             

                
                OpenXLCD(FOUR_BIT & LINES_5X7);
                WriteCmdXLCD(0x01);
                __delay_ms(2);
                WriteCmdXLCD(0x80);
                putrsXLCD("Opcoes do Menu:");
                WriteCmdXLCD(0x0C);
                break;
    }
                INTCONbits.INT0IF = 0;
 
}

// Rotina de Serviço de Interrupção (ISR) de baixa prioridade
void interrupt low_priority ISR_Baixa_Prioridade(void){
    extern signed char menu;
        // Ocorr?ncia de INT1
        if (INTCON3bits.INT1IF){
        menu++;
        if (menu == 5)
        menu = 1;
        INTCON3bits.INT1IF = 0;
        }
        // Ocorr?ncia de INT2
        else if (INTCON3bits.INT2F){
        menu--;
        if (menu == 0)
            menu = 4;
            INTCON3bits.INT2IF = 0;
        }
   
}

void escreve2dig(unsigned char v) {
    putcXLCD((v / 10) + '0');
    putcXLCD((v % 10) + '0');
}

void atualiza_display() {
    WriteCmdXLCD(0x80);
    putrsXLCD("Hora ");
    escreve2dig(h);
    putcXLCD(':');
    escreve2dig(m);
    putcXLCD(':');
    escreve2dig(s);

    WriteCmdXLCD(0xC0);
    if (modo)
        putrsXLCD("AJUSTE      ");
    else
        putrsXLCD("NORMAL      ");
}





void main() {
        char atraso;
        ADCON1 = 0x0F;
        TRISBbits.TRISB0 = 1;
        TRISBbits.TRISB1 = 1;
        TRISBbits.TRISB2 = 1;
        TRISAbits.TRISA5 = 0;
        TRISAbits.TRISA3 = 0;
        TRISCbits.TRISC1 = 0;
        TRISCbits.TRISC2 = 0;
        INTCON2bits.nRBPU = 0;
   
        //Configura??o das interrup??es
        RCONbits.IPEN = 1; // habilita prioridade

        // Configura??o de INT0
        INTCONbits.INT0IE = 1; // habilita INT0
        INTCONbits.INT0IF = 0; // zera flag da INT0
        // INT0 ? sempre de alta prioridade
        // Configura??o de INT1
        INTCON3bits.INT1IE = 1; // habilita INT1
        INTCON3bits.INT1IF = 0; // zera flag da INT1
        INTCON3bits.INT1IP = 0; // interrup??o INT1 ? de baixa prioridade

            // Configura??o de INT2
        INTCON3bits.INT2IE = 1; // habilita INT2
        INTCON3bits.INT2IF = 0; // zera flag da INT2
        INTCON3bits.INT2IP = 0; // interrup??o INT2 ? de baixa prioridade

        INTCONbits.GIEL = 1; // habilita interrup??es de baixa prioridade
        INTCONbits.GIEH = 1; // habilita interrup??es de alta prioridade
   

        OpenXLCD( FOUR_BIT & LINES_5X7 );
        WriteCmdXLCD(0x01);
        __delay_ms(16);

        WriteCmdXLCD(0x80);
        putrsXLCD("Opcoes do Menu:");
        WriteCmdXLCD(0x0C);

while (1) {
        CLRWDT();
        PORTAbits.RA5 = 0;
        PORTAbits.RA3 = 1;
        WriteCmdXLCD(0xC0);
        switch (menu){
            case 1:
            putrsXLCD("Ajusta horário  ");
            break;

            case 2:
            putrsXLCD("1/0 lâmpada     ");
            break;

            case 3:
            putrsXLCD("1/0 buzzer      ");
            break;
            case 4:
            putrsXLCD("Entra em sleep  ");
            break;
        }
       for (atraso=0; atraso<50; atraso++)
            __delay_ms(10);
    }
}