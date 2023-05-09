/* 
 * File:   Prelab_10.c
 * Author: pablo
 *
 * Created on 7 de mayo de 2023, 10:03 p.m.
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF          // Code Protection bit (Program memory code protection is enabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


/**********LIBRERIAS**********/

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic16f887.h>

/**********DEFINIR CONSTANTES**********/

#define _XTAL_FREQ 8000000
unsigned int numero;
unsigned int centena; // Almacena las centenas en  ASCII
unsigned int decena; 
unsigned int unidad; 
uint8_t contador;


/**********VARIABLES GLOBALES**********/

/**********PROTOTIPOS**********/
void setup(void);
void initUART(void);
void enviar_numero_decimal(unsigned int numero);

/**********INTERRUPCIONES**********/

void __interrupt() isr(void) {
    
    if (RBIF) { // interrupción en PORTB 
      
        if (RB0 == 0 && RB1 == 1) {  // Si solo RB0 se presiono 
            contador++; // Incrementar el contador
        }
        
        if (RB0 == 1 && RB1 == 0) { // Si solo RB1 se presiono
            contador--; // Decrementar el contador
        }
        
        centena = (contador/100);       // Calcular las centenas 
        decena = ((contador/10)%10);    // Calcular las decenas 
        unidad = (contador%10);         // Calcular las unidades 
        

        RBIF = 0;        // Limpiar la bandera de interrupción de PORTB 
    }
     __delay_ms(5);

}


/**********CÓDIGO PRINCIPAL**********/


/*
 * 
 */
void main(void) 
{
    setup();
    initUART();
    while(1)   //loop principal
    { 
        TXREG = centena + 48; // Enviar el valor de las centenas
        __delay_ms(5);        
        TXREG = decena + 48; // Enviar el valor de las decenas
        __delay_ms(5);                 
        TXREG = unidad + 48; // Enviar el valor de las unidades
        __delay_ms(5);     
        TXREG = '\n'; // Enviar nueva línea
        __delay_ms(5);
        TXREG = '\r'; // Enviar retorno 
        __delay_ms(5);

        //Si se prende la bandera de recepción de datos
        if(RCIF == 1){
            PORTD = RCREG;
            PIR1bits.RCIF = 0; // Borrar el indicador

        }  
    }
}



/**********FUNCIONES**********/


void initUART(void){
    /*
     * 12.1.1.6 Asynchronous Transmission Set-up:
    1. Initialize the SPBRGH, SPBRG register pair and
    the BRGH and BRG16 bits to achieve the desired
    baud rate (see Section 12.3 ?EUSART Baud
    Rate Generator (BRG)?).
    2. Enable the asynchronous serial port by clearing
    the SYNC bit and setting the SPEN bit.
    3. If 9-bit transmission is desired, set the TX9 control bit. A set ninth data bit will indicate that the 8
    Least Significant data bits are an address when
    the receiver is set for address detection.
    4. Enable the transmission by setting the TXEN
    control bit. This will cause the TXIF interrupt bit
    to be set.
    5. If interrupts are desired, set the TXIE interrupt
    enable bit. An interrupt will occur immediately
    provided that the GIE and PEIE bits of the
    INTCON register are also set.
    6. If 9-bit transmission is selected, the ninth bit
    should be loaded into the TX9D data bit.
    7. Load 8-bit data into the TXREG register. This
    will start the transmission.
     */
    
    //paso1
    SPBRG = 12;     //SPBRGH:SPBRG = (8MHz/9600)/64 -1 = 12 ---> Real 9615.38
    SPBRGH = 0;     //%error = (9615.38-9600)/9600*100 = 0.16%
    BRGH = 0;
    BRG16 = 0;
    
    //paso2
    SYNC = 0;   //Modo asincronico habilitado
    SPEN = 1;   //Habilitación del modulo UART
    
    //paso3, habilitacion de los 9 bits.
    
    
    //paso4 Habilitacion de la transmicion
    TXEN = 1; //Habilitar transmision & TXIF = 1
    TXIF = 0; //Apagar la bandera TX
    // Asynchronous Reception Set-up
    CREN = 1;   //habilita la recepcion
    RCIF = 0;   //limpia bandera de interrupcion de datos recibidos
    
    //Paso 5> Interrupciones
    
    //Paso 6> cargar el 9no bit.
    
    //Paso7> cargar los 8 bits
    
    
    
}    
   
void setup(void)
{
    //configuracion de entradas y salidas
    ANSELH = 0;
    
    TRISB = 0b11;  // Input los primeros 2 bits del PORTB
    TRISD = 0;      // Puertos como salida
    TRISE = 0;
    
    PORTA = 0;   //Se inicializan todos los puertos en 0
    PORTD = 0;
    PORTE = 0;
        
    //Configuracion PULL-UP
    OPTION_REGbits.nRBPU = 0;   //0 = PORTB pull-ups are enabled by individual PORT latch values
    WPUB = 0b11;   // Pull-up Enable para los primeros 2 bits del PORTB
    IOCB = 0b11; //Interrupt-on-change enabled para los primeros 2 bits del PORTB

    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //Utilización del oscilador Interno
    
    //Configuración de las interrupciones
    INTCONbits.GIE = 1;  // Usualmente la global se enciende de [ultimo]
    INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    INTCONbits.RBIF = 0; // Limpiar la bandera de interrupcion del PORTB
    INTCONbits.RBIE = 1; // Habilitar la interrupcion del PORTB  
   
    return;
}