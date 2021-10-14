/*
 * File: Postlab9.c
 * Autor: Katherine Rac
 * Creado: 11/10/2021
 * 
 * Descripción: pot en RA0, RA1 y RA2, servo en RC2 y RC1, led en RB0
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



/*
 * ---------------------------- Constantes -----------------------------------
 */
#define _XTAL_FREQ 4000000  //8MHz
#define _tmr0_value 250 ///20 MS

/*
 * ----------------------------- Variables ----------------------------------
 */
char bandera;                   // Para interrupciones
char pot_count;                 //Tercer potenciometro
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
        //Casos de interrupción 
        if (bandera == 0)
        {
            CCPR1L = (ADRESH>>1)+124;       //Se le da valor al CCPR1l
            CCP1CONbits.DC1B = (ADRESH & 0b01);     //Se verifiva los valores
            CCP1CONbits.DC1B0 = (ADRESL>>7);    //más signifitvias y los menos
            PIR1bits.ADIF = 0;
        }
        else if (bandera == 1)
        {
            CCPR2L = (ADRESH>>1)+124;   //Se le da valor al CCPR1l
            CCP1CONbits.DC1B = (ADRESH & 0b01);  //Se verifiva los valores
            CCP1CONbits.DC1B0 = (ADRESL>>7);    //más signifitvias y los menos
            PIR1bits.ADIF = 0;
        }
        
        
        else if (bandera == 2)
        {
            pot_count = ADRESH;     //Se envía el adresh a una nueva variable 
            PIR1bits.ADIF = 0;
        }
    }
    
    if (INTCONbits.T0IF == 1)
        {
            //tmr0
            PORTB++;       //Contado en el puerto B
            INTCONbits.T0IF = 0;
            TMR0 = _tmr0_value;

            if (PORTB < pot_count){     //Se compara 
                PORTDbits.RD0  = 1;
            }
            else
                PORTDbits.RD0 =0;
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
            if (ADCON0bits.CHS == 0)
            {
                bandera = 0;
                ADCON0bits.CHS = 1;
            }
            else if (ADCON0bits.CHS == 1)
            {
                bandera = 1;
                ADCON0bits.CHS = 2;
            }
            else if (ADCON0bits.CHS == 2)
            {
                bandera = 2;
                ADCON0bits.CHS = 0;
            }
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
    ANSEL = 0b00000111;
    ANSELH = 0;
    
    TRISA = 0b11111111;  //Entrada
    TRISD = 0;          //Salida
    TRISB = 0;          //Salida
    
    PORTB = 0;          //Valores iniciales
    PORTD = 0;
 
    //---------------------- Configuración del oscilador ----------------------
    OSCCONbits.IRCF = 0b0111;   //4MHz
    OSCCONbits.SCS = 1;
    
    //----------------------- Configuración del ADC ---------------------------
    ADCON1bits.ADFM = 0;        //Justificaión izquierda
    ADCON1bits.VCFG0 = 0;       //Vref en VSS Y VDD
    ADCON1bits.VCFG1 = 0;       
    
    ADCON0bits.ADCS = 0b01;     //FOSC/8
    ADCON0bits.CHS = 0;
    //ADCON0bits.CHS = 1;
    //ADCON0bits.CHS = 2; 
    ADCON0bits.ADON = 1;
     __delay_us(50);
     
    //----------------------- Configuración del PWM ---------------------------
     TRISCbits.TRISC2 = 1;          //RC2 /CCP1 entrada
     TRISCbits.TRISC1 = 1;
     PR2 = 255;                     //Cnfig periodo
     
     CCP1CONbits.P1M = 0;           //Config modo PWM 1
     CCP1CONbits.CCP1M = 0b1100;    
     
     CCP2CONbits.CCP2M = 0;
     CCP2CONbits.CCP2M = 0b1100;
     
     CCPR1L = 0X0f;                 //Ciclo de trabajo inicial
     CCPR2L = 0X0f;                 //Ciclo de trabajo inicial
     CCP1CONbits.DC1B = 00;
     CCP2CONbits.DC2B0 = 0;
     CCP2CONbits.DC2B1 = 0;
     
     //------------------ Configuracipon del tmr0 ------------------------
    //Timer0 Registers Prescaler= 256 
    //TRP - TMR0 Preset = 217 - Freq = 50.08 Hz - Period = 0.019968 seconds
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit
    OPTION_REGbits.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 
    OPTION_REGbits.PSA = 0;   // bit 3  Prescaler Assignment bit
    OPTION_REGbits.PS2 = 0;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 1;
    
    TMR0 = _tmr0_value;             // preset for timer register
    
    INTCONbits.T0IF = 0;        //Deshabilitar la bandera
    INTCONbits.T0IE = 1;        //Activar interrupcon Timer 0
    INTCONbits.GIE = 1; 
    
     //------------------------------- TMR2 ----------------------------------
    PIR1bits.TMR2IF = 0;           //apagamos la bandera
    T2CONbits.T2CKPS = 0b11;       //Prescaler 1:16
    T2CONbits.TMR2ON = 1;

    while (PIR1bits.TMR2IF == 0);
    PIR1bits.TMR2IF = 0;           //Esperamos un ciclo del tmr2

    TRISCbits.TRISC2 = 0;          //Salida del PWM
    TRISCbits.TRISC1 = 0;
     
    //------------------ Configuración de las interrupciones  -----------------
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    return;

}



