#include <xc.h>

#pragma config FOSC=INTRC_NOCLKOUT
#pragma config WDTE=OFF
#pragma config IESO=OFF
#pragma config FCMEN=OFF

void configTimer(void) {
    OSCCONbits.IRCF2 = 0; //De 4MHz a 250kHz
    OPTION_REGbits.PS1 = 0; //Factor de escala de 256 a 64
    OPTION_REGbits.PSA = 0; //Usar preescalador
    OPTION_REGbits.T0CS = 0; //Activar el contar pulsos de oscilador principal
    INTCONbits.T0IF = 0;
    TMR0 = 0;
}

void configADC(void) {
    ADCON0bits.ADON = 1; // Convertidor Enable 
    ADCON0bits.CHS3 = 1; // Pin B2
    ADCON0bits.ADCS1 = 0; // Iteracion de la conversion a FOSC/32
    ADCON1bits.ADFM = 0; // Justificado a la izquierda
    ANSEL= 0;
    ANSELH = 0;
    ADRESL = 0;
}

void configUART(void) {
    RCSTAbits.SPEN = 1;
    RCSTAbits.CREN = 1;
    TXSTAbits.BRGH = 1;
    TXSTAbits.TXEN = 1;
    PIR1bits.ADIF=0;
}

void waitTimer(void) {
    while(INTCONbits.T0IF != 1)
        ;
    TMR0 = 0;
    INTCONbits.T0IF = 0;
}

void runADC(void) {
    ADCON0bits.GO = 1;
    while(PIR1bits.ADIF != 1)
        ;
    PIR1bits.ADIF = 0;
}

void sendADC(void) {
    unsigned char parteB, parteA, aux;
    parteA = ADRESH;
    parteB = ADRESL;
        
    aux = parteA & 0x07; //Se guardan los 3 bits menos signifcativos de parte alta
    aux = aux << 2; //Se guarda espacio para los 2 bits de la parte baja
        
    parteA = parteA >> 3; //Se dejan solo 5 bits en parte alta
    parteA = parteA | 0x80; //Se enciende bandera de que es parte alta
        
    parteB = parteB >> 6; //Se recorren bits al lado derecho
    parteB = parteB | aux; //Se junta parte baja con los 3 bits de parte alta
        
    TXREG = parteB;
    TXREG = parteA;
}

void main(void) {    
    TRISD=0X00;
    TRISB=0XFF;
    
    configTimer();
    configADC();
    configUART();
    
    PORTD=0x0F;
    
    while(1) {
        waitTimer();
        runADC();
        waitTimer();
        sendADC();
    }
}
