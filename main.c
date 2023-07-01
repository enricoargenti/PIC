#pragma config FOSC  = HS       // Oscillator Selection bits (XT oscillator)
#pragma config WDTE  = OFF      // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP   = OFF     // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD   = OFF      // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT   = OFF      // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP    = OFF      // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 8000000

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

//#define RE_DE    RE2
//#define LED_DATA PORTD
//#define LED_CTRL PORTA
#define COMMAND 0
#define DATA 1
#define LCD_EN PORTEbits.RE1
#define LCD_RS PORTEbits.RE2
#define LCDPORT PORTD
#define LCDPORT_DIR TRISD
#define LCD_EN_DIR TRISEbits.TRISE1
#define LCD_RS_DIR TRISEbits.TRISE2
#define L_ON 0x0F
#define L_OFF 0x08
#define L_CLR 0x01
#define L_L1 0x80
#define L_L2 0xC0
#define L_CR 0x0F
#define L_NCR 0x0C
#define L_CFG 0x38
#define L_CUR 0x0E
#define RS485_TX_EN     RC0     // Transmit enable pin
#define RS485_RX_EN     RC1     // Receive enable pin



void __interrupt() ISR();
void UART_Init(void);
void RS485_TxEnable(void);
void RS485_RxEnable(void);
void UART_Write(char data);
char UART_Read1(void);
char UART_Read2(void);

void Timer0_Init(void);
unsigned char GenerateRandomNumber(void);

unsigned char num1,num2,num3,num4,num5,num21,num22,num23,num24,num25;

unsigned char myUniqueId = 0x09;
unsigned char messageType;

// Buffer to receive messages from the serial port
char buffer[50];
char received;
int bufferIndex;

void initLCD(void);
void lcdSend(char, char);
void lcdPrint(char *);
char KeyPadReader(void);
//void KeyPressed(unsigned char, char* codice);
void intToString(int, char *);
char potenza(char, char);

const unsigned char colMask[3] ={
    //76543210 posizione del bit
    0b11111110, // Colonna 1 => RB0 a massa
    0b11111101, // Colonna 2 => RB1 a massa
    0b11111011 // Colonna 3 => RB2 a massa
};
const unsigned char rowMask[4] ={
    0b00000001, // Riga 1
    0b00000010, // Riga 2
    0b00000100, // Riga 3
    0b00001000 // Riga 4
};
// valore dei tasti coincidenti con il keypressed
const unsigned char keys[] = {'#', '7', '4', '1', '*', '8', '5', '2', '0', '9', '6', '3'};
unsigned char keypressed = 0; // peso numerico del pulsante premuto
char keyok = 0; // flag di pulsante premuto

// inizializzazione degli indici dei cicli
unsigned char colScan = 0;
unsigned char rowScan = 0;
static __bit old_btn;
char stato = 0;
char old_stato = 0;

char codice[6];
char countdown = 60;
char print_countdown[3];
char data[6];
char stringa[16];
int stringPosition = 0;
unsigned char restart = 0;
int flag = 0;
int index;
unsigned char randomNum;
unsigned int count = 0;

void main()
{
    TRISA=0x00;	   
	TRISB=0x00;
	TRISC=0x00;
	TRISD=0x00;
    TRISE=0X00;
    UART_Init();
    initLCD();
    Timer0_Init();
    while(1)
    {
        lcdSend(L_CLR, COMMAND);
        lcdPrint("Premi *");
        num1 = GenerateRandomNumber();
        num2 = GenerateRandomNumber();
        num3 = GenerateRandomNumber();
        num4 = GenerateRandomNumber();
        num5 = GenerateRandomNumber();
        while (codice[0] == '\0')
        {   
            KeyPadReader();
            if (keys[keypressed] == '*'){
                break;
            }
        }
        lcdSend(L_CLR, COMMAND);
        lcdPrint("Attendere");
        while(num1 != 0x30 && num1 != 0x31 && num1 != 0x32 && num1 != 0x33 &&
              num1 != 0x34 && num1 != 0x35 && num1 != 0x36 && num1 != 0x37 &&
              num1 != 0x38 && num1 != 0x39){
        num1 = GenerateRandomNumber();
        }
        lcdSend(L_CLR, COMMAND);
        lcdPrint("Attendere.");
        while(num2 != 0x30 && num2 != 0x31 && num2 != 0x32 && num2 != 0x33 &&
              num2 != 0x34 && num2 != 0x35 && num2 != 0x36 && num2 != 0x37 &&
              num2 != 0x38 && num2 != 0x39){
        num2 = GenerateRandomNumber();
        }
        lcdSend(L_CLR, COMMAND);
        lcdPrint("Attendere..");
        while(num3 != 0x30 && num3 != 0x31 && num3 != 0x32 && num3 != 0x33 &&
              num3 != 0x34 && num3 != 0x35 && num3 != 0x36 && num3 != 0x37 &&
              num3 != 0x38 && num3 != 0x39){
        num3 = GenerateRandomNumber();
        }
        lcdSend(L_CLR, COMMAND);
        lcdPrint("Attendere...");
        while(num4 != 0x30 && num4 != 0x31 && num4 != 0x32 && num4 != 0x33 &&
              num4 != 0x34 && num4 != 0x35 && num4 != 0x36 && num4 != 0x37 &&
              num4 != 0x38 && num4 != 0x39){
        num4 = GenerateRandomNumber();
        }
        lcdSend(L_CLR, COMMAND);
        lcdPrint("Attendere....");
        while(num5 != 0x30 && num5 != 0x31 && num5 != 0x32 && num5 != 0x33 &&
              num5 != 0x34 && num5 != 0x35 && num5 != 0x36 && num5 != 0x37 &&
              num5 != 0x38 && num5 != 0x39){
        num5 = GenerateRandomNumber();
        }
        
        RS485_TxEnable();
        UART_Write(myUniqueId); // Print my own unique identifier
        messageType = 0x01;     // Here the message is type 1 (first message)
        UART_Write(messageType); 
        UART_Write(num1);
        UART_Write(num2);
        UART_Write(num3);
        UART_Write(num4);
        UART_Write(num5);
        UART_Write('*');    // Stopping symbol
        lcdSend(L_CLR, COMMAND);
        lcdSend(num1, DATA);
        lcdSend(num2, DATA);
        lcdSend(num3, DATA);
        lcdSend(num4, DATA);
        lcdSend(num5, DATA);
        
        keypressed = 0;
        RS485_RxEnable();
        flag = 1;
        UART_Read1();
        
        if(received)
        {
            lcdSend(L_CLR, COMMAND);
            lcdPrint("Dato arrivato");
            bufferIndex = 0;
            received = 0;
        }
        //UART_Read2(); //legge char lentamente
    }
}

void UART_Init() {
    // Configure UART module
    TRISC6 = 0;     // TX pin is output
    TRISC7 = 1;     // RX pin is input

    // Set baud rate to 9600
    BRGH = 1;       // High speed baud rate
    SPBRG = 51;     // Value for 9600 baud rate (Fosc = 8MHz)

    // Enable asynchronous serial port
    SYNC = 0;       // Asynchronous mode
    SPEN = 1;       // Serial port enabled

    // Enable transmission and reception
    TXEN = 1;       // Transmission enabled
    CREN = 1;       // Reception enabled
}

void RS485_TxEnable() {
    RS485_TX_EN = 1;
    RS485_RX_EN = 0;
}

void RS485_RxEnable() {
    RS485_TX_EN = 0;
    RS485_RX_EN = 1;
}

void UART_Write(char data) {
    while (!TXIF) // Wait for the previous transmission to finish
        continue;
    TXREG = data; // Transmit data
}

char UART_Read2() {
    char trash;
    while (countdown != 0){
        if (!RCIF){
            lcdSend(L_CLR, COMMAND);
            lcdPrint("Attendere");
            intToString(countdown, print_countdown);
            lcdSend(L_L2, COMMAND);
            lcdPrint(print_countdown);
            countdown --;
            __delay_ms(60);
        }        
        else{
            while (countdown != 0){
                intToString(countdown, print_countdown);
                lcdSend(L_L2, COMMAND);
                lcdPrint(print_countdown);
                countdown --;
                __delay_ms(60);
                if(RCREG == myUniqueId){
                    lcdSend(L_CLR, COMMAND);
                    lcdSend(RCREG, DATA);
                    RCIF = 0;
                    trash = RCREG;
                    while (countdown != 0){
                        if (!RCIF){ // Wait for data to be received
                            intToString(countdown, print_countdown);
                            lcdSend(L_L2, COMMAND);
                            lcdPrint(print_countdown);
                            countdown --;
                            __delay_ms(60);
                        }
                        else{
                            while (countdown != 0){
                                intToString(countdown, print_countdown);
                                lcdSend(L_L2, COMMAND);
                                lcdPrint(print_countdown);
                                countdown --;
                                __delay_ms(60);
                                if(RCREG == 0x31){
                                    lcdSend(0x80 + 1, COMMAND);
                                    lcdSend(RCREG, DATA);                
                                    RCIF = 0;
                                    trash = RCREG;
                                    countdown = 60;        
                                    lcdSend(L_L2, COMMAND);
                                    lcdPrint("APRI PORTA");
                                    __delay_ms(200);
                                    return RCREG; // Return received data
                                }
                                                                            
                                RCIF = 0;
                                trash = RCREG;
                            }
                        }
                    }
                }
                RCIF = 0;
                trash = RCREG;
            }      
        }
    }
    lcdSend(L_CLR, COMMAND);
    lcdPrint("Tempo Scaduto");
    RCIF = 0;
    countdown = 60;
    __delay_ms(200);
    return RCREG;
        /*while (!RCIF) // Wait for data to be received
        continue;*/
}

char UART_Read1() {
    char count = 0;
    countdown = 60;
    while (countdown != 0){   //!RCIF
        if (count != 5){
            if(keypressed != 0 && count == 0){
                lcdSend(L_CLR, COMMAND);
                lcdSend(keys[keypressed], DATA);
                num21 = keys[keypressed];
                keypressed = 0;
                count ++;
            }
            if(keypressed != 0 && count == 1){
                lcdSend(0x80 + count, COMMAND);
                lcdSend(keys[keypressed], DATA);
                num22 = keys[keypressed];
                keypressed = 0;
                count ++;
            }
            if(keypressed != 0 && count == 2){
                lcdSend(0x80 + count, COMMAND);
                lcdSend(keys[keypressed], DATA);
                num23 = keys[keypressed];
                keypressed = 0;
                count ++;
            }
            if(keypressed != 0 && count == 3){
                lcdSend(0x80 + count, COMMAND);
                lcdSend(keys[keypressed], DATA);
                num24 = keys[keypressed];
                keypressed = 0;
                count ++;
            }
            if(keypressed != 0 && count == 4){
                lcdSend(0x80 + count, COMMAND);
                lcdSend(keys[keypressed], DATA);
                num25 = keys[keypressed];
                keypressed = 0;
                count ++;
            }
            intToString(countdown, print_countdown);
            lcdSend(L_L2, COMMAND);
            lcdPrint(print_countdown);
            countdown --;
            //__delay_ms(80);
        }        
        else{
            RS485_TxEnable();
            UART_Write(myUniqueId);
            messageType = 0x02;
            UART_Write(messageType);  // This time the type is the second one
            UART_Write(num1);
            UART_Write(num2);
            UART_Write(num3);
            UART_Write(num4);
            UART_Write(num5);
            UART_Write(num21);
            UART_Write(num22);
            UART_Write(num23);
            UART_Write(num24);
            UART_Write(num25);
            UART_Write('*');
            RS485_RxEnable();
            lcdSend(L_CLR, COMMAND);
            lcdPrint("Sent!");
            RCIF = 0;
            countdown = 60;
            //__delay_ms(100);
            return RCREG; // Return received data
        }
    }
    lcdSend(L_CLR, COMMAND);
    lcdPrint("Codice Scaduto");
    RCIF = 0;
    countdown = 60;
    //__delay_ms(200);
    return RCREG;
        /*while (!RCIF) // Wait for data to be received
        continue;*/
}

void initLCD() {
    LCD_RS = 0;
    LCD_EN = 0;
    __delay_ms(20);
    LCD_EN = 1;
    lcdSend(L_CFG, COMMAND); // mando il comando di configurazione CFG
    __delay_ms(5);
    lcdSend(L_CFG, COMMAND); // rimando il comando di configurazione CFG
    __delay_ms(1);
    lcdSend(L_CFG, COMMAND);
    lcdSend(L_OFF, COMMAND);
    lcdSend(L_ON, COMMAND);
    lcdSend(L_CLR, COMMAND);
    lcdSend(L_NCR, COMMAND); //Niente cursore: non lo voglio per bellezza
    lcdSend(L_L1, COMMAND); // Se scrivo L2 stamperà sulla seconda riga  
}

void lcdSend(char dato, char tipo) {
    LCDPORT_DIR = 0x00;
    LCD_EN_DIR = 0;
    LCD_RS_DIR = 0;

    LCD_EN = 1;
    LCDPORT = dato;
    LCD_RS = tipo;
    LCD_EN = 0;
    LCD_EN = 1;
}

void lcdPrint(char *str) {
    int n = 0;
    while (str[n] != '\0') {
        lcdSend(str[n++], DATA);
    }
}

void __interrupt() ISR() {
    
    if(RCIF)
    {
        buffer[bufferIndex++] = RCREG;
        // missing
        RCIF = 0;
    }
        
    TMR0IF = 0;
    if(flag == 1){
        TRISD |= 0x0F;
        for (colScan = 0; colScan < 3; colScan++) {
            PORTB = PORTB | 0x07;
            PORTB &= colMask[colScan];

            for (rowScan = 0; rowScan < 4; rowScan++) {
                if (!(PORTD & rowMask[rowScan]) && (old_btn)) {
                    old_btn = 0;
                    stato++;
                }
                if ((PORTD & rowMask[rowScan]) && (!old_btn)) {
                    if ((PORTD & rowMask[rowScan]) && (!old_btn)) {
                        old_btn = 1;
                    }
                }
                if (stato != old_stato) {
                    keypressed = rowScan + (4 * colScan);
                    // chiamo la funzione
                    //KeyPressed(keypressed, codice);
                    old_stato = stato;
                }
            }
        }
        TRISD |= 0x00;
    }
}

void Timer0_Init() {
    // Configure Timer0
    T0CS = 0;    // Use internal instruction clock
    PSA = 0;     // Assign prescaler to Timer0
    PS2 = 1;     // Set prescaler to 1:256
    PS1 = 1;
    PS0 = 1;

    // Enable Timer0
    TMR0 = 6;    // Clear Timer0
    TMR0IF = 0;  // Clear Timer0 overflow flag
    TMR0IE = 1;  // Disable Timer0 interrupt
    GIE = 1;
}

unsigned char GenerateRandomNumber() {
    unsigned int count = 0;
    while (count != 186){
        count++;
    }
    unsigned char randomNum = TMR0;
    TMR0IF = 0;
    return randomNum;
}

char KeyPadReader() {
    TRISD |= 0x0F;
    //do
    // porto a massa una colonna alla volta
    for (colScan = 0; colScan < 3; colScan++) {
        PORTB = PORTB | 0x07; // porto tutte le colonne a 1
        PORTB &= colMask[colScan]; // porto a zero la colonna attuale

        // ciclo sulle righe
        for (rowScan = 0; rowScan < 4; rowScan++) {
            if (!(PORTD & rowMask[rowScan]) && (old_btn)) {
                old_btn = 0;
                stato++;
            }
            if ((PORTD & rowMask[rowScan]) && (!old_btn)) {
                __delay_ms(10);
                if ((PORTD & rowMask[rowScan]) && (!old_btn)) {
                    old_btn = 1;
                }
            }
            if (stato != old_stato) {
                keypressed = rowScan + (4 * colScan); // numero di pulsante premuto
                // chiamo la funzione
                //KeyPressed(keypressed, codice);
                old_stato = stato;
                return keypressed;
            }
        }
    }
    //while(1);
}

/*void KeyPressed(unsigned char keypressed, char* codice) {
    // Se l'utente preme l'asterisco, un codice di cinque numeri 
    // viene generato randomicamente
    if (keys[keypressed] == '*') {
        // Use current time as
        // seed for random generator
        char upper = 9;
        char lower = 0;
        int stringPosition = 0;

        char numero[5];

        int i;
        int count = 5;
        //int codice[5];

        for (i = 0; i < count; i++) {
            char randNumber = '0' + ((rand() % (upper - lower + 1)) + lower);

            //intToString(randNumber, numero);    // passo n da convertire e stringa in cui salvarlo


            codice[i] = randNumber;


        }
        codice[5] = '\0';
        lcdSend(L_L1, COMMAND);
        flag = 1;

        // Test per capire se i numeri contenuti nell'array codice siano giusti
        for (i = 0; i < count; i++) {
            intToString(codice[i], numero);    // passo n da convertire e stringa in cui salvarlo
            lcdSend(L_L1+(stringPosition++), COMMAND);
            lcdPrint(numero);
        }
    }

    __delay_ms(100);

    // rimango in un ciclo continuo fino a che
    // il pulsante non viene rilasciato
    PORTD = PORTD | 0x0F;
    while ((PORTD & 0x0F) != 0x0F) {
        PORTD = PORTD | 0x0F;
        continue;
    }

}*/

char potenza(char b, char e) {
    char n = 1;
    for (int i = 0; i < e; i++) {
        n = n * b; // che è uguale a n *= b;
    }
    return n;
}

void intToString(int n, char *str) {
    // calcolo il numero di cifre di cui è composto il numero

    char cifre = 1;
    char i = 0;
    // fintantoché la divisione dà un numero diverso da zero è vera, incremento le cifre
    while (n / potenza(10, cifre)) cifre++;

    for (i = 0; i < cifre; i++) {
        char t = potenza(10, cifre - 1 - i);
        str[i] = '0' + (char) (n / t);
        n = n % t;
    }

    str[i] = '\0';
}
