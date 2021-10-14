/*
 * File: Prelab9.c
 * Autor: Katherine Rac
 * Creado: 11/10/2021
 * 
 * Descripción: pot en RA0 y servo en RC2     
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

/*
 * --------------------------- Configracion de bits --------------------------
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillador Interno sin salidas, XT
#pragma config WDTE = OFF       // WDT disabled (reinicio repetitivo del pic)
#pragma config PWRTE = OFF     // PWRT enabled  (espera de 72ms al iniciar)
#pragma config MCLRE = OFF      // El pin de MCLR se utiliza como I/O
#pragma config CP = OFF         // Sin protección de código
#pragma config CPD = OFF        // Sin protección de datos
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit 
#pragma config LVP = OFF         // Low Voltage Programming Enable bit 

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit 
#pragma config WRT = OFF        // Flash Program Memory Self Write 

/*
 * --------------------------- Librerias --------------------------
 */

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000  //8MHz

/*
 * ---------------------------- Constantes -----------------------------------
 */

/*
 * ----------------------------- Variables ----------------------------------
 */
                  // Para interrupciones

/*
 * ------------------------------- Tabla ------------------------------------
 */


/*
 * -------------------------- Prototipos de funciones -----------------------
 */
void setup (void);
/*
 * ------------------------------ Interrupción -------------------------------
 */
void __interrupt () isr (void)
{
    if (PIR1bits.ADIF)   // Interrupcion ADC
    {
        CCPR1L = (ADRESH>>1)+124;
        CCP1CONbits.DC1B = (ADRESH & 0b01);
        CCP1CONbits.DC1B0 = (ADRESL>>7);
        PIR1bits.ADIF = 0;
        
    }
}

/*
 * ----------------------------- MAIN CONFIGURACION --------------------------- 
 */

void main (void)
{
    setup();
    while(1)
    {
        if (ADCON0bits.GO == 0)
        {
            __delay_us(50);
            ADCON0bits.GO = 1;
            
        }
    }
}
/*
 * -------------------------------- Funciones --------------------------------
 */

void setup(void)
{
    //--------------- Configuración de entradas y salidas --------------
    ANSEL = 0b00000001;
    ANSELH = 0;
    
    TRISA = 0b11111111;  //Entrada
 
    //---------------------- Configuración del oscilador ----------------------
    OSCCONbits.IRCF = 0b0111;   //8MHz
    OSCCONbits.SCS = 1;
    
    //----------------------- Configuración del ADC ---------------------------
    ADCON1bits.ADFM = 0;        //Justificaión izquierda
    ADCON1bits.VCFG0 = 0;       //Vref en VSS Y VDD
    ADCON1bits.VCFG1 = 0;       
    
    ADCON0bits.ADCS = 0b01;     //FOSC/8
    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;
     __delay_us(50);
     
    //----------------------- Configuración del PWM ---------------------------
     TRISCbits.TRISC2 = 1;          //RC2 /CCP1 entrada
     PR2 = 255;                     //Cnfig periodo
     CCP1CONbits.P1M = 0;           //Config modo PWM
     CCP1CONbits.CCP1M = 0b1100;    
     
     CCPR1L = 0X0f;                 //Ciclo de trabajo inicial
     CCP1CONbits.DC1B = 00;
     
     //------------------------------- TMR2 ----------------------------------
     PIR1bits.TMR2IF = 0;           //apagamos la bandera
     T2CONbits.T2CKPS = 0b11;       //Prescaler 1:16
     T2CONbits.TMR2ON = 1;
     
     while (PIR1bits.TMR2IF == 0);
     PIR1bits.TMR2IF = 0;           //Esperamos un ciclo del tmr2
     
     TRISCbits.TRISC2 = 0;          //Salida del PWM
     
    //------------------ Configuración de las interrupciones  -----------------
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    return;

}

