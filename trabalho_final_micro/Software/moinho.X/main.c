/*
 * Trabalho: Altomação No processo do moinho de ceramica
 * Author: Miguel Garcia Silvestre 17206714
 *
 */

#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>


#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config FOSC = HS
#pragma config BOREN = ON

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7


#define _XTAL_FREQ 4000000

#define ESCOTILHA PORTCbits.RC4
#define LED_MOTOR PORTCbits.RC1
#define PORTAO PORTCbits.RC2
#define LED_AGUA PORTCbits.RC3
#define MOTOR PORTCbits.RC0
#define BAGUA PORTCbits.RC5
#define ON PORTBbits.RB2



int valor = 0; // var para guardar valor da convers?o
int B, SP, SE, SA;
int conta=0, paradas = 0;   //vari?vel auxiliar conta. Inicia com 0

void __interrupt() TrataInt()
{
    if(INTF)
    {
        INTCONbits.INTF = 0;
        ESCOTILHA = 0;
        __delay_ms(100);
        LED_MOTOR = 0;
        __delay_ms(100);
        PORTAO = 0;
        __delay_ms(100);
        LED_AGUA = 0;
        __delay_ms(100);
        MOTOR = 0;
        __delay_ms(100);
        BAGUA = 0;
        __delay_ms(100);
        valor = 0;
    }
    if (ADIF)  //foi a interrup??o de final de convers?o AD?
    {  
        PIR1bits.ADIF = 0; //reseta o flag da interrup??o
           
        if (B != 127) // balança
        { 
            valor = 0;
            LED_MOTOR = 0;
            __delay_ms(100);
            MOTOR = 0;
            __delay_ms(100); 
            LED_AGUA = 0;
            __delay_ms(100);
            BAGUA = 0;
            __delay_ms(100);
        }
            
        if(SP != 127) // sensor portão
        {
            valor = 0;
            LED_MOTOR = 0;
            __delay_ms(100);
            LED_AGUA = 0;
            __delay_ms(100);
            MOTOR = 0;
            __delay_ms(100);
            BAGUA = 0;
            __delay_ms(100);
            PORTAO = 1;
            __delay_ms(100);
        }
        else if(SP == 127)
        {
            PORTAO = 0;
            __delay_ms(100);
        }
            
        if(SE > 127) //sensor escotilha
        {
            valor = 0;
            ESCOTILHA = 1;
            __delay_ms(100);
            LED_MOTOR = 0;
            __delay_ms(100);
            MOTOR = 0;
            __delay_ms(100);
        }
        else if(SE == 127){
            ESCOTILHA = 0;
            __delay_ms(100);
        }
        if(SA != 127 && B != 127 && SE != 127)
        {
            valor = 0;
            LED_MOTOR = 0;
            __delay_ms(50);
            MOTOR = 0;
            __delay_ms(50);
            
            if(SA > 127 && B > 127)
            {
                LED_AGUA = 1;
                __delay_ms(100);
                LED_AGUA = 0;
                __delay_ms(100);
                BAGUA = 0;
                __delay_ms(100);
            }
            else if(SA < 127 && B < 127)
            {
                LED_AGUA = 1;
                __delay_ms(100);
                BAGUA = 1;
                __delay_ms(100);
            }
            else if(SA > 127 && B < 127)
            {
                LED_AGUA = 1;
                __delay_ms(50);
                LED_AGUA = 0;
                __delay_ms(50);
                LED_AGUA = 1;
                __delay_ms(50);
                LED_AGUA = 0;
                __delay_ms(50);
                LED_AGUA = 1;
                __delay_ms(50);
                LED_AGUA = 0;
                __delay_ms(50);
            }
            else if(SA < 127 && B > 127)
            {
                LED_AGUA = 1;
                __delay_ms(50);
                LED_AGUA = 0;
                __delay_ms(50);
                LED_AGUA = 1;
                __delay_ms(50);
                LED_AGUA = 0;               
            }
        }    
        else if(SA == 127)
        {
            LED_AGUA = 0;
            __delay_ms(100);
            BAGUA = 0;
            __delay_ms(100);
        }
    }
    
    if (TMR1IF)  //foi a interrup??o de estouro do timer1 que chamou a int?
     {  
        PIR1bits.TMR1IF = 0; //reseta o flag da interrup??o
        TMR1L = 0xDC;        //reinicia contagem com 3036
        TMR1H = 0x0B;        
        
        //comandos pra tratar a interrup??o
        if (conta == 4 && valor == 1){
            ESCOTILHA = 0;
            __delay_ms(100);
            LED_MOTOR = 0;
            __delay_ms(100);
            PORTAO = 0;
            __delay_ms(100);
            LED_AGUA = 0;
            __delay_ms(100);
            MOTOR = 0;
            __delay_ms(100);
            BAGUA = 0;
            __delay_ms(100);      //Passa valor invertido para o pino f?sico
            conta = 0;
            paradas++;
        }
        else if(conta != 4 && valor == 1)
        {
            LED_MOTOR = 1;
            __delay_ms(100);
             MOTOR = 1;
            __delay_ms(100);
        }
        
    }
    
    return;
}

#include "lcd.h"

int main(void)
{
//    OPTION_REG = 0b01111111;
    OPTION_REGbits.nRBPU = 0; //Ativa resistores de pull-ups

    TRISB = 0b0001111;
    TRISC = 0b0000000;
    TRISA = 0b11111111;       //configura pinos de entrada(1)e sa?da (0)
    TRISD = 0x00;  //configura PORTD como sa?da. Local onde deve estar o LCD

     
    ESCOTILHA = 0;
    __delay_ms(50);
    LED_MOTOR = 0;
    __delay_ms(50);
    PORTAO = 0;
    __delay_ms(50);
    LED_AGUA = 0;
    __delay_ms(50);
    MOTOR = 0;
    __delay_ms(50);
    BAGUA = 0;
    __delay_ms(50);
    
    Lcd_Init();    //necess?rio para o LCD iniciar

    OPTION_REGbits.INTEDG = 1;

    INTCONbits.GIE = 1;
    INTCONbits.INTE = 1;
    
    //*** configura o conversor anal?gico/digital*******************************
    //ficar? somente com AN0 como entrada anal?gica. Tem tabela no datasheet
    ADCON1bits.PCFG0   = 1;  //configura as entradas anal?gicas
    ADCON1bits.PCFG1   = 0;  //configura as entradas anal?gicas
    ADCON1bits.PCFG2   = 0;  //configura as entradas anal?gicas
    ADCON1bits.PCFG3   = 1;  //configura as entradas anal?gicas

    //define o clock de convers?o
    ADCON0bits.ADCS0 = 0  ;   //confirmando default Fosc/2
    ADCON0bits.ADCS1 = 0  ;   //confirmando default Fosc/2
    ADCON1bits.ADFM = 0   ;   //Default ? 0. Pra mostra que pegar? os dados em 8 bits 
                              //no ADRESH, pois est? justifica ? esquerda
                              // Passando pra 1 pode pegar os dados em 10 bits 
                              //usando os dois registradores ADRESHL justificado ?
                              //direita

    //inicializa registradores do AD
    ADRESL = 0x00;          //inicializar valor anal?gico com 0
    ADRESH = 0x00;          

    ADCON0bits.ADON = 1;     //Liga AD 
   
        
        
    //** configurando interrup??es ***********************************
    INTCONbits.GIE=1;       //Habiliita a int global
    INTCONbits.PEIE = 1;    //Habilita a int dos perif?ricos
    PIE1bits.TMR1IE = 1;    //Habilita int do timer 1

    //*** configura o timer 1 *****************************************
    T1CONbits.TMR1CS = 0;   //Define timer 1 como temporizador (Fosc/4)
    T1CONbits.T1CKPS0 = 1;  //bit pra configurar pre-escaler, nesta caso 1:8
    T1CONbits.T1CKPS1 = 1;  //bit pra configurar pre-escaler, nesta caso 1:8

    TMR1L = 0xDC;          //carga do valor inicial no contador (65536-62500)
    TMR1H = 0x0B;          //3036. Quando estourar contou 62500, passou 0,5s   

    T1CONbits.TMR1ON = 1;   //Liga o timer    
        
        
    while(1)
    {      
        int botao_precionado = 0, i = 0;
        //Seleciona canal de entrada 0 como entrada anal?gica
        ADCON0bits.CHS0 = 0; //configura canal 0 como entrada anal?gica
        ADCON0bits.CHS1 = 0; //configura canal 0 como entrada anal?gica
        ADCON0bits.CHS2 = 0; //configura canal 0 como entrada anal?gica
        __delay_us(10);     //tempo de convers?o
        ADCON0bits.GO = 1;  //converte
        __delay_us(10);     //tempo de convers?o
        B = ADRESH;     // passa valores convertido do reg para a vari?vel

        ADCON0bits.CHS0 = 1; //configura canal 1 como entrada anal?gica
        ADCON0bits.CHS1 = 0; //configura canal 1 como entrada anal?gica
        ADCON0bits.CHS2 = 0; //configura canal 1 como entrada anal?gica
        __delay_us(10);     //tempo de convers?o
        ADCON0bits.GO = 1;  //converte
        __delay_us(10);     //tempo de convers?o
        SP = ADRESH; 

        ADCON0bits.CHS0 = 0; //configura canal 2 como entrada anal?gica
        ADCON0bits.CHS1 = 1; //configura canal 2 como entrada anal?gica
        ADCON0bits.CHS2 = 0; //configura canal 2 como entrada anal?gica
        __delay_us(10);     //tempo de convers?o
        ADCON0bits.GO = 1;  //converte
        __delay_us(10);     //tempo de convers?o
        SE = ADRESH; 

        ADCON0bits.CHS0 = 1; //configura canal 3 como entrada anal?gica
        ADCON0bits.CHS1 = 1; //configura canal 3 como entrada anal?gica
        ADCON0bits.CHS2 = 0; //configura canal 3 como entrada anal?gica
        __delay_us(10);     //tempo de convers?o

        ADCON0bits.GO = 1;  //converte
        __delay_us(10);     //tempo de convers?o
        SA = ADRESH; 

        if(ON == 0)
        {
//            Lcd_Clear();                    //limpa LCD
//            LED_MOTOR = 1;
//            __delay_ms(100);
//            MOTOR = 1;
//            __delay_ms(200);
            valor = 1;
        }
        
       
        
        
        
        if (B != 127) // balança
        { 
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);            //P?e curso linha 1 coluna 1
                Lcd_Write_String("PESO DO MOINHO");  //escreve string
                __delay_ms(100);
            if(B > 127){
                    Lcd_Set_Cursor(2,1);             //linha 2 coluna 1
                    Lcd_Write_String("ALTO"); //escreve string]
                    __delay_ms(1000);
                    
            }
            else if(B < 127)
            {
                    Lcd_Set_Cursor(2,1);             //linha 2 coluna 1
                    Lcd_Write_String("BAIXO"); //escreve string]
                    __delay_ms(1000);
                   
            }
            CLRWDT ();
        }
        else if(B == 127) //sensor escotilha
        {
            Lcd_Clear();     
        }

        if(SP != 127) // sensor portão
        {
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);             //linha 2 coluna 1
                Lcd_Write_String("ESTADO DO PORTAO"); //escreve string]
                __delay_ms(1000);
                Lcd_Set_Cursor(2,1);             //linha 2 coluna 1
                Lcd_Write_String("ABERTO"); //escreve string]
                __delay_ms(1000);
                CLRWDT ();
        }
        else if(SP == 127) //sensor escotilha
        {
            Lcd_Clear();     
        }

        if(SE != 127) //sensor escotilha
        {
                Lcd_Clear();  
                Lcd_Set_Cursor(1,1);             //linha 2 coluna 1
                Lcd_Write_String("ESTADO DA ESCOTILHA"); //escreve string]
                __delay_ms(100);
                if(SE > 127){
                    

                    Lcd_Set_Cursor(2,1);             //linha 2 coluna 1
                    Lcd_Write_String("ESCOTILHA: ABERTA"); //escreve string]
                    __delay_ms(1000);
                  
                }
                CLRWDT ();
        }
        else if(SE == 127) //sensor escotilha
        {
            Lcd_Clear();     
        }

        if(SA != 127 && B != 127)
        {
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);             //linha 2 coluna 1
            Lcd_Write_String("NIVEL DA AGUA"); //escreve string]
            __delay_ms(100);
            if(SA > 127)
            {
               
                Lcd_Set_Cursor(2,1);             //linha 2 coluna 1
                Lcd_Write_String("NIVEL: ALTO"); //escreve string]
                __delay_ms(1000);
            }
            else if(SA < 127)
            {
                Lcd_Set_Cursor(2,1);             //linha 2 coluna 1
                Lcd_Write_String("NIVEL: BAIXO"); //escreve string]
                __delay_ms(1000);
            }
            CLRWDT ();
        }
        else if(SA == 127){
            Lcd_Clear();
        }
        CLRWDT ();
    }
        
}

