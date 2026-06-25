#include <xc.h>

#pragma config FOSC = HS
#pragma config CPUDIV = OSC1_PLL2
#pragma config PBADEN = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 20000000

#define BTN_ESQ PORTBbits.RB1   // INT1 - Seta esquerda
#define BTN_DIR PORTBbits.RB2   // INT2 - Seta direita

void delay_500ms(void){
    __delay_ms(500);
}

void main(void)
{
    unsigned char i;
    unsigned char estado_setas = 0; // 0=apagado, 1=aceso
    
    // Configuração dos pinos
    TRISB = 0b00000110;   // RB1 e RB2 como entrada
    TRISD = 0x00;         // LEDs como saída
    TRISA = 0x00;
    
    ADCON1 = 0x0F;
    INTCON2bits.nRBPU = 0;
    
    LATD = 0x00;
    
    while(1)
    {
        // SETA ESQUERDA
        if(BTN_ESQ == 0)
        {
            // Efeito sequencial de caminhamento
            for(i = 0; i < 4; i++)
            {
                LATD = 0x08 | (0x04 >> i);
                delay_500ms();
            }
            
            // Agora mantém piscando a seta completa (efeito de carro)
            while(BTN_ESQ == 0)
            {
                // Pisca a seta (liga/desliga)
                LATD = 0x0F;  // Acende todos os LEDs da seta
                delay_500ms();
                LATD = 0x00;  // Apaga todos
                delay_500ms();
            }
        }
        
        // SETA DIREITA
        if(BTN_DIR == 0)
        {
            // Efeito sequencial de caminhamento
            for(i = 0; i < 4; i++)
            {
                LATD = 0x10 | (0x20 << i);
                delay_500ms();
            }
            
            // Agora mantém piscando a seta completa (efeito de carro)
            while(BTN_DIR == 0)
            {
                // Pisca a seta (liga/desliga)
                LATD = 0xF0;  // Acende todos os LEDs da seta
                delay_500ms();
                LATD = 0x00;  // Apaga todos
                delay_500ms();
            }
        }
        
        // Se nenhum botão estiver pressionado, apaga tudo
        LATD = 0x00;
    }
}