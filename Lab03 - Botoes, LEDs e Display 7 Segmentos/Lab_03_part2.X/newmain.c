//Pratica 3 - Etapa 2: Contador de Numeros Primos com Display de 7 segmentos
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
#define BT_INC  PORTBbits.RB1    // Botao incrementa (proximo primo) - INT1
#define BT_DEC  PORTBbits.RB2    // Botao decrementa (primo anterior) - INT2
#define BT_DISP PORTBbits.RB0    // Botao troca de display - INT0
void main(void)
{
   unsigned char indice = 0, dispAtivo = 0;
   // Vetor com os valores em binario p/ os segmentos do display (0 a 9)
    unsigned char const disp7seg[] = {
                      0b00111111,    //0
                      0b00000110,    //1
                      0b01011011,    //2
                      0b01001111,    //3
                      0b01100110,    //4
                      0b01101101,    //5
                      0b01111101,    //6
                      0b00000111,    //7
                      0b01111111,    //8
                      0b01100111};   //9
   // Vetor com os numeros primos de 1 digito
   unsigned char const primos[] = {2, 3, 5, 7};

   TRISB = 0b00000111;              // PORTB,0 / PORTB,1 / PORTB,2 como entrada
   TRISD = 0;                       // Toda a PORTD como saida (segmentos)
   TRISA = 0;                       // Toda a PORTA como saida (selecao display)
   TRISEbits.TRISE0 = 0;            // RE0 como saida (selecao display 3)
   TRISEbits.TRISE2 = 0;            // RE2 como saida (selecao display 4)
   ADCON1 = 0x0F;                   // Pinos analogicos desligados
   INTCON2bits.nRBPU = 0;           // Habilita Pull-ups do PORTB

   LATD = disp7seg[primos[indice]];  // Mostra o primeiro primo (2)
   LATA = 0x00;
   LATEbits.LATE0 = 0;
   LATEbits.LATE2 = 0;

   while(1){
        // Mostra o numero primo atual no display selecionado
        LATD = disp7seg[primos[indice]];

        LATAbits.LATA5 = 0;       // Desliga DISP1
        LATAbits.LATA2 = 0;       // Desliga DISP2
        LATEbits.LATE0 = 0;       // Desliga DISP3
        LATEbits.LATE2 = 0;       // Desliga DISP4

        if(dispAtivo == 0)
            LATAbits.LATA5 = 1;   // Liga DISP1
        else if(dispAtivo == 1)
            LATAbits.LATA2 = 1;   // Liga DISP2
        else if(dispAtivo == 2)
            LATEbits.LATE0 = 1;   // Liga DISP3
        else
            LATEbits.LATE2 = 1;   // Liga DISP4

        // Botao RB1: avanca para o proximo numero primo
        if(BT_INC == 0){
            __delay_ms(25);
            if(BT_INC == 0){
                if(indice == 3)
                    indice = 0;
                else
                    indice += 1;
                while(BT_INC == 0);   // Espera soltar o botao
            }
        }

        // Botao RB2: volta para o numero primo anterior
        if(BT_DEC == 0){
            __delay_ms(25);
            if(BT_DEC == 0){
                if(indice == 0)
                    indice = 3;
                else
                    indice -= 1;
                while(BT_DEC == 0);   // Espera soltar o botao
            }
        }

        // Botao RB0: troca o display ativo (DISP1->DISP2->DISP3->DISP4->DISP1)
        if(BT_DISP == 0){
            __delay_ms(25);
            if(BT_DISP == 0){
                if(dispAtivo == 3)
                    dispAtivo = 0;
                else
                    dispAtivo += 1;
                while(BT_DISP == 0);  // Espera soltar o botao
            }
        }
   }
}