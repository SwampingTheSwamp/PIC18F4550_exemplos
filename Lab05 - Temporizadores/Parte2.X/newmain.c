#include <xc.h>
#include <string.h>
#pragma config FOSC  = HS
#pragma config CPUDIV= OSC1_PLL2
#pragma config WDT   = ON
#pragma config WDTPS = 512
#pragma config PBADEN= OFF
#pragma config LVP   = OFF

#define _XTAL_FREQ 20000000

const unsigned char SEG7[10] = {
    0x3F, // 0: a b c d e f
    0x06, // 1:   b c
    0x5B, // 2: a b   d e   g
    0x4F, // 3: a b c d     g
    0x66, // 4:   b c     f g
    0x6D, // 5: a   c d   f g
    0x7D, // 6: a   c d e f g
    0x07, // 7: a b c
    0x7F, // 8: a b c d e f g
    0x6F  // 9: a b c d   f g
};

volatile unsigned int  contagem  = 9999; // valor inicial: 999.9s
volatile unsigned char pausado   = 0;
volatile unsigned char digito    = 0;    // digito atual na varredura (0-3)

void interrupt ISR_Alta(void) {

    // ---------- TIMER1: varredura 5ms ----------
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;
        TMR1H = 0x9E;   // recarga 40536 = 0x9E58
        TMR1L = 0x58;

        // Apaga todos os displays (evita ghosting)
        LATAbits.LATA5 = 0;   // DISP1
        LATAbits.LATA2 = 0;   // DISP2
        LATEbits.LATE0 = 0;   // DISP3
        LATEbits.LATE2 = 0;   // DISP4

        // Calcula os 4 dígitos a partir de 'contagem'
        // contagem = D3 D2 D1 . D0
        // D3=centenas-seg, D2=dezenas-seg, D1=unidades-seg, D0=décimos
        unsigned int  tmp = contagem;
        unsigned char d0  = tmp % 10; tmp /= 10; // décimos
        unsigned char d1  = tmp % 10; tmp /= 10; // unidades seg
        unsigned char d2  = tmp % 10; tmp /= 10; // dezenas seg
        unsigned char d3  = tmp % 10;             // centenas seg

        unsigned char val;
        switch (digito) {
            case 0:                          // DISP1 ? centenas de segundos
                val = SEG7[d3];
                LATD = val;
                LATAbits.LATA5 = 1;
                break;
            case 1:                          // DISP2 ? dezenas de segundos
                val = SEG7[d2];
                LATD = val;
                LATAbits.LATA2 = 1;
                break;
            case 2:                          // DISP3 ? unidades de segundos
                // ponto decimal entre segundos e décimos
                val = SEG7[d1] | 0x80;      // liga dp
                LATD = val;
                LATEbits.LATE0 = 1;
                break;
            case 3:                          // DISP4 ? décimos de segundo
                val = SEG7[d0];
                LATD = val;
                LATEbits.LATE2 = 1;
                break;
        }
        digito++;
        if (digito == 4) digito = 0;
    }

    // ---------- TIMER0: 100ms ? decrementa contagem ----------
    if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        TMR0H = 0xF8;   // recarga 63583 = 0xF85F
        TMR0L = 0x5F;

        if (!pausado && contagem > 0)
            contagem--;
    }
}

// -------------------------------------------------------
// ISR BAIXA PRIORIDADE ? INT1, INT2, UART RX
// -------------------------------------------------------
void interrupt low_priority ISR_Baixa(void) {

    // INT1 ? pausa/retoma alternado
    if (INTCON3bits.INT1IF) {
        INTCON3bits.INT1IF = 0;
        pausado ^= 1;
    }

    // INT2 ? reset da contagem
    if (INTCON3bits.INT2IF) {
        INTCON3bits.INT2IF = 0;
        contagem = 9999;
        pausado  = 0;
    }

    // UART RX ? comando vindo do PC
    if (PIR1bits.RCIF) {
        unsigned char c = RCREG;   // lê e limpa RCIF automaticamente
        if      (c == 'z') { contagem = 9999; pausado = 0; }
        else if (c == 'p') { pausado = 1; }
        else if (c == 'r') { pausado = 0; }
    }
}


void uart_puts(const char *s) {
    while (*s) {
        while (!TXSTAbits.TRMT); // aguarda buffer vazio
        TXREG = *s++;
    }
}


void envia_contagem(void) {
    unsigned int  tmp = contagem;
    char buf[10];
    unsigned char d0 = tmp % 10; tmp /= 10;
    unsigned char d1 = tmp % 10; tmp /= 10;
    unsigned char d2 = tmp % 10; tmp /= 10;
    unsigned char d3 = tmp % 10;
    buf[0] = '0' + d3;
    buf[1] = '0' + d2;
    buf[2] = '0' + d1;
    buf[3] = '.';
    buf[4] = '0' + d0;
    buf[5] = '\r';
    buf[6] = '\n';
    buf[7] = '\0';
    uart_puts(buf);
}

// -------------------------------------------------------
// MAIN
// -------------------------------------------------------
void main(void) {

    
    ADCON1 = 0x0F;          // todos pinos digitais
    TRISD = 0x00;           
    LATD  = 0x00;

    TRISAbits.TRISA5 = 0;   // DISP1
    TRISAbits.TRISA2 = 0;   // DISP2
    TRISEbits.TRISE0 = 0;   // DISP3
    TRISEbits.TRISE2 = 0;   // DISP4

    LATAbits.LATA5 = 0;
    LATAbits.LATA2 = 0;
    LATEbits.LATE0 = 0;
    LATEbits.LATE2 = 0;

    TRISBbits.TRISB1 = 1;   // INT1 entrada
    TRISBbits.TRISB2 = 1;   // INT2 entrada
    INTCON2bits.nRBPU = 0;  // pull-ups internos PORTB

    // --- UART 57600 bps @ 20MHz ---
    // SPBRG = Fosc / (16 * baud) - 1  (BRGH=1, SYNC=0)
    // = 20000000 / (16 * 57600) - 1 = 20,7 ? 21
    TRISCbits.TRISC6 = 0;   // TX saída
    TRISCbits.TRISC7 = 1;   // RX entrada
    SPBRG  = 21;
    TXSTAbits.BRGH = 1;     // alta velocidade
    TXSTAbits.SYNC = 0;     // assíncrono
    RCSTAbits.SPEN = 1;     // habilita serial
    TXSTAbits.TXEN = 1;     // habilita TX
    RCSTAbits.CREN = 1;     // habilita RX contínuo

    // --- Timer1: varredura 5ms ---
    T1CON  = 0b00000001;    // TMR1ON, Fosc/4, prescaler 1:1
    TMR1H  = 0x9E;
    TMR1L  = 0x58;
    PIE1bits.TMR1IE  = 1;
    PIR1bits.TMR1IF  = 0;
    IPR1bits.TMR1IP  = 1;   // alta prioridade

    // --- Timer0: contagem 100ms ---
    T0CON  = 0b10000111;    // TMR0ON, 16-bit, CLKO, prescaler 1:256
    TMR0H  = 0xF8;
    TMR0L  = 0x5F;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP= 1;  // alta prioridade

    // --- Interrupções externas ---
    RCONbits.IPEN = 1;

    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IP = 0; // baixa prioridade

    INTCON3bits.INT2IE = 1;
    INTCON3bits.INT2IF = 0;
    INTCON3bits.INT2IP = 0; // baixa prioridade

  
    PIE1bits.RCIE  = 1;
    PIR1bits.RCIF  = 0;
    IPR1bits.RCIP  = 0;     // baixa prioridade

    INTCONbits.GIEL = 1;    // habilita baixa prioridade
    INTCONbits.GIEH = 1;    // habilita alta prioridade

    
    unsigned int ultima = contagem;

    while (1) {
        CLRWDT();
        if (contagem != ultima) {
            ultima = contagem;
            envia_contagem();
        }
    }
}