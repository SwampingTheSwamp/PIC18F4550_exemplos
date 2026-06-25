//Pratica 3 - Etapa 1: Acionamento Sequencial de LEDs (Seta de Carro)
// Baseado no Exemplo 6 (PICSimLab)
#include<xc.h>
// Configuracoes
#pragma config  FOSC = HS               // Fosc = 20MHz, Tcy = 200ns (PLL n?o habilitado)
#pragma config  CPUDIV = OSC1_PLL2      // sinal de clk do sistema ? o oscliador prim?rio
#pragma config  PBADEN = OFF            // Pinos do PORTB come?am como digitais
#pragma config  WDT = OFF               // Watchdog desativado
#pragma config  LVP = OFF               // Desabilita grava??o em baixa tens?o
#define _XTAL_FREQ 20000000             // define o clock para 20 MHz (fosc)
// Definicoes de hardware
#define BT_ESQ PORTBbits.RB1     // Botao seta esquerda (INT1)
#define BT_DIR PORTBbits.RB2     // Botao seta direita (INT2)
void main(void)
{
   unsigned char atraso = 0;
   // Vetor com os valores para os LEDs da seta esquerda (D4,D3,D2,D1 em PORTD)
    unsigned char const setaEsq[] = {
                      0b00001000,    // D4
                      0b00001100,    // D4,D3
                      0b00001110,    // D4,D3,D2
                      0b00001111};   // D4,D3,D2,D1
   // Vetor com os valores para os LEDs da seta direita (D5,D6,D7,D8 em PORTD)
    unsigned char const setaDir[] = {
                      0b00010000,    // D5
                      0b00110000,    // D5,D6
                      0b01110000,    // D5,D6,D7
                      0b11110000};   // D5,D6,D7,D8
   unsigned char i;
   TRISB=0b00000110;                // PORTB,1 e PORTB,2 como entrada
   TRISD=0;                         // Toda a PORTD como saida
   ADCON1 = 0x0F;                   // Pinos anal?gicos desligados
   INTCON2bits.nRBPU = 0;           // Habilita Pull-ups do PORTB
   LATD = 0x00;
   while(1){
        if(BT_ESQ == 0){
            for (i = 0;i<4;i++){
                LATD = setaEsq[i];
                while (atraso < 50){
                    __delay_ms(10);
                    atraso += 1;
                }
                atraso = 0;
            }
            LATD = 0x00;
        }
        else if(BT_DIR == 0){
            for (i = 0;i<4;i++){
                LATD = setaDir[i];
                while (atraso < 50){
                    __delay_ms(10);
                    atraso += 1;
                }
                atraso = 0;
            }
            LATD = 0x00;
        }
        else
            LATD = 0x00;          // Apaga os LEDs
   }
}