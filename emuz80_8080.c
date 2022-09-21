/*!
 * PIC18F47Q84/PIC18F47Q83 ROM RAM and UART emulation firmware
 * This single source file contains all code
 *
 * Target: EMUZ80 with 8085
 * Compiler: MPLAB XC8 v2.36
 * 
 * Modified by Satoshi Okue https://twitter.com/S_Okue
 * Version 0.1 2022/9/21
 */

/*
 *  PIC18F47Q84 ROM RAM and UART emulation firmware
 *  This single source file contains all code
 * 
 *  Original source Written by Tetsuya Suzuki
 *  https://github.com/vintagechips/emuz80
 *  Target: EMUZ80 - The computer with only Z80 and PIC18F47Q43
 *  Compiler: MPLAB XC8 v2.36  
 *
 *  Modified by  Gazelle https://twitter.com/Gazelle8087
 *  1st release 2022.7.17
 * 
 *  Target: PIC18F47Q84
 *  IDE: MPLAB X v6.0
 *  Compiler: XC8 v2.36
 *
 *  References: source code written by yyhayami
 *  for the CLC configuration and memory mapping/access procedure.
 *  https://github.com/yyhayami/emuz80_hayami/tree/main/emuz80_clc.X 
*/


// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator Selection (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_64MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG2
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCKED bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#ifdef _18F47Q84
#pragma config JTAGEN = OFF
#pragma config FCMENP = OFF
#pragma config FCMENS = OFF
#endif

// CONFIG3
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCKED bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (Low-Power BOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG4
#pragma config BORV = VBOR_1P9  // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 1.9V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD module is disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = OFF    // PPSLOCK bit One-Way Set Enable bit (PPSLOCKED bit can be set and cleared repeatedly (subject to the unlock sequence))
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG5
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled; SWDTEN is ignored)

// CONFIG6
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG7
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#ifndef _18F47Q84
#pragma config DEBUG = OFF  
#endif

// CONFIG8
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not Write protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers not Write protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not Write protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG10
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>

#define CLK_8085 1000000UL  // 8085 clock frequency(Max 1.1MHz)

#define ROM_SIZE 0x4000     //16K bytes
#define RAM_SIZE 0x2000
#define ROM_TOP 0x0000      //ROM top address
#define RAM_TOP 0x8000      //RAM top address

#define UART_DREG 0x00      //Data REG
#define UART_CREG 0x01      //Control REG

#define _XTAL_FREQ 64000000UL

//8085 ROM equivalent, see end of this file
extern const unsigned char rom[];

//8085 RAM equivalent
volatile unsigned char ram[RAM_SIZE];

//Address Bus
union {
    unsigned int w; //16 bits Address
    struct {
        unsigned char l; //Address low
        unsigned char h; //Address high
    };
} ab;

unsigned char USARTC;

/*
// UART3 Transmit
void putch(char c) {
    while(!U3TXIF); // Wait or Tx interrupt flag set
    U3TXB = c; // Write data
}

// UART3 Recive
char getch(void) {
    while(!U3RXIF); // Wait for Rx interrupt flag set
    return U3RXB; // Read data
}
//*/

// Never called, logically
void __interrupt(irq(default),base(8)) Default_ISR(){}


// Called at ALE raising edge
void __interrupt(irq(CLC1),base(8)) CLC1_ISR(){
    CLC1IF = 0;         // Clear the CLC1 interrupt flag
    ab.l = PORTB;       // Read address low
    ab.h = PORTD;       // Read address high
}

// Called at /RD falling edge
void __interrupt(irq(CLC3),base(8)) CLC3_ISR(){
    CLC3IF = 0;                         // Clear the CLC3 interrupt flag
	asm("clrf	TRISB,c		\n");       // Set AD7-AD0 bus as output
    if (!RA2) {                         // IO/M = Memory area
        if(CLC6OUT) {                   // ROM area
            LATB = rom[ab.w - ROM_TOP]; // Out ROM data
        } else if(CLC5OUT) {            // RAM area
            LATB = ram[ab.w - RAM_TOP]; // Out RAM data
        } else
            LATB = 0xff;                // Invalid data
    } else {                            // IO/M = IO area
        if(PORTD == UART_CREG) {        // PIR9
            USARTC = PIR9 & 0x03;       // SWAP bit1,bit0
            if (USARTC == 0x02)
                LATB = 0x01;
            else if (USARTC == 0x01)
                LATB = 0x02;
            else
            LATB = USARTC;              // Out PIR9
        } else if (PORTD == UART_DREG) { // U3RXB
            LATB = U3RXB;               // Out U3RXB
            LATE1 = 0;
        } else {
            LATB = 0x00;
        }
    }
    while(!RA0);                        // Wait raising edge
    asm("setf	TRISB,c		\n");       // Set AD7-AD0 bus as input
}

// Called at /WT falling edge
void __interrupt(irq(CLC4),base(8)) CLC4_ISR(){
    CLC4IF = 0;                         // Clear the CLC4 interrupt flag
    if (!RA2) {                         // IO/M = Memory area
        if(CLC5OUT) {                   // RAM area
            ram[ab.w - RAM_TOP] = PORTB;// Write into RAM
        }
    } else {
        if(PORTD == UART_DREG) {        // U3TXB
            U3TXB = PORTB;              // Write into U3TXB
        }
    }
}

// Called at INTR falling edge
void __interrupt(irq(CLC2),base(8)) CLC2_ISR(){
    CLC2IF = 0;                         // Clear the CLC3 interrupt flag
	asm("clrf	TRISB,c		\n");       // Set AD7-AD0 bus as output
        LATB = 0xff;                    // CALL INST.
    while(!RC6);                        // Wait raising edge
    asm("setf	TRISB,c		\n");       // Set AD7-AD0 bus as input
}

// main routine
void main(void) {

    int i;

    // System initialize
    OSCFRQ = 0x08;      // 64MHz internal OSC

    // Address bus A15-A8 pin
    ANSELD = 0x00;      // Disable analog function
    WPUD = 0xff;        // Week pull up
    TRISD = 0xff;       // Set as input

    // Address bus AD7-AD0 pin
    ANSELB = 0x00;      // Disable analog function
    WPUB = 0xff;        // Week pull up
    TRISB = 0xff;       // Set as input

    // RESETIN output pin
    ANSELE2 = 0;        // Disable analog function
    LATE2 = 0;          // Reset=Low
    TRISE2 = 0;         // Set as output

    // RD (RA0)  input pin
    ANSELA0 = 0;        // Disable analog function
    WPUA0 = 1;          // Week pull up
    TRISA0 = 1;         // Set as intput

    // WT (RA1)  input pin
    ANSELA1 = 0;        // Disable analog function
    WPUA1 = 1;          // Week pull up
    TRISA1 = 1;         // Set as intput

    // IO/M (RA2)  input pin
    ANSELA2 = 0;        // Disable analog function
    WPUA2 = 1;          // Week pull up
    TRISA2 = 1;         // Set as intput

    // ALE (RA3)  input pin
    ANSELA3 = 0;        // Disable analog function
    WPUA3 = 1;          // Week pull up
    TRISA3 = 1;         // Set as intput

    // 8085 clock(RA5) by NCO FDC mode
    RA5PPS = 0x3f;      // RA5 assign NCO1
    ANSELA5 = 0;        // Disable analog function
    TRISA5 = 0;         // NCO output pin
    NCO1INC = CLK_8085 * 2 / 61;
    NCO1CLK = 0x00;     // Clock source Fosc
    NCO1PFM = 0;        // FDC mode
    NCO1OUT = 1;        // NCO output enable
    NCO1EN = 1;         // NCO enable

    // S0 (RC0)  input pin
    ANSELC0 = 0;        // Disable analog function
    WPUC0 = 1;          // Week pull up
    TRISC0 = 1;         // Set as intput

    // S1 (RC1)  input pin
    ANSELC1 = 0;        // Disable analog function
    WPUC1 = 1;          // Week pull up
    TRISC1 = 1;         // Set as intput

    // INTR (RE1) output pin
    ANSELE1 = 0;        // Disable analog function
    LATE1 = 0; 
    TRISE1 = 0;         // Set as output

    // INTA (RC6)  input pin
    ANSELC6 = 0;        // Disable analog function
    WPUC6 = 1;          // Week pull up
    TRISC6 = 1;         // Set as intput

    // HOLD (RC7) output pin
    ANSELC7 = 0;        // Disable analog function
    LATC7 = 0;
    TRISC7 = 0;         // Set as output

    // HLDA (RE0)  input pin
    ANSELE0 = 0;        // Disable analog function
    WPUE0 = 1;          // Week pull up
    TRISE0 = 1;         // Set as intput

    // READY (RA4) output pin
    ANSELA4 = 0;        // Disable analog function
    LATA4 = 1;
    TRISA4 = 0;         // Set as output

    // TRAP (RC2) output pin
    ANSELC2 = 0;        // Disable analog function
    LATC2 = 0;
    TRISC2 = 0;         // Set as output

    // RST5.5 (RC3) output pin
    ANSELC3 = 0;        // Disable analog function
    LATC3 = 0;
    TRISC3 = 0;         // Set as output

    // RST6.5 (RC4) output pin
    ANSELC4 = 0;        // Disable analog function
    LATC4 = 0;
    TRISC4 = 0;         // Set as output

    // RST7.5 (RC5) output pin
    ANSELC5 = 0;        // Disable analog function
    LATC5 = 0;
    TRISC5 = 0;         // Set as output


    // UART3 initialize
    U3BRG = 416;        // 9600bps @ 64MHz
    U3RXEN = 1;         // Receiver enable
    U3TXEN = 1;         // Transmitter enable

    // UART3 Receiver
    ANSELA7 = 0;        // Disable analog function
    TRISA7 = 1;         // RX set as input
    U3RXPPS = 0x07;     // RA7->UART3:RX3;

    // UART3 Transmitter
    ANSELA6 = 0;        // Disable analog function
    LATA6 = 1;          // Default level
    TRISA6 = 0;         // TX set as output
    RA6PPS = 0x26;      // RA6->UART3:TX3;

    U3ON = 1;           // Serial port enable

    //========== CLC pin assign ===========    
    CLCIN0PPS = 0x03;   // RA3 <- ALE
    CLCIN1PPS = 0x16;   // RC6 <- INTA
    CLCIN2PPS = 0x1f;   // RD7 <- A15
    CLCIN3PPS = 0x1e;   // RD6 <- A14
    CLCIN4PPS = 0x00;	// RA0 <- /RD
    CLCIN5PPS = 0x01;   // RA1 <- /WT
    CLCIN6PPS = 0x1d;   // RD5 <- A13
    CLCIN7PPS = 0x1c;   // RD4 <- A12

    //========== CLC1 ALE interrupt raising edge ==========
    CLCSELECT = 0;      // select CLC1  

    CLCnSEL0 = 0;       // CLCIN0PPS <- ALE
    CLCnSEL1 = 127;     // NC 
    CLCnSEL2 = 127;     // NC
    CLCnSEL3 = 127;     // NC

    CLCnGLS0 = 0x02;    // ALE noninverted
    CLCnGLS1 = 0x04;    // 1(0 inverted) for AND gate
    CLCnGLS2 = 0x10;    // 1(0 inverted) for AND gate
    CLCnGLS3 = 0x40;    // 1(0 inverted) for AND gate

    CLCnPOL = 0x00;     // Not inverted
    CLCnCON = 0x92;     // Positive Edge / 4 input AND

    //========== CLC2 INTR interrupt falling edge ==========
    CLCSELECT = 1;      // select CLC2  

    CLCnSEL0 = 1;       // CLCIN1PPS <- INTR
    CLCnSEL1 = 127;     // NC 
    CLCnSEL2 = 127;     // NC
    CLCnSEL3 = 127;     // NC

    CLCnGLS0 = 0x02;    // INTR noninverted
    CLCnGLS1 = 0x04;    // 1(0 inverted) for AND gate
    CLCnGLS2 = 0x10;    // 1(0 inverted) for AND gate
    CLCnGLS3 = 0x40;    // 1(0 inverted) for AND gate

    CLCnPOL = 0x00;     // Not inverted
    CLCnCON = 0x8A;     // Negative Edge / 4 input AND

    //========== CLC3 /RD interrupt falling edge ==========
    CLCSELECT = 2;      // select CLC3  

    CLCnSEL0 = 4;       // CLCIN4PPS <- /RD
    CLCnSEL1 = 127;     // NC 
    CLCnSEL2 = 127;     // NC
    CLCnSEL3 = 127;     // NC

    CLCnGLS0 = 0x02;    // /RD noninverted
    CLCnGLS1 = 0x04;    // 1(0 inverted) for AND gate
    CLCnGLS2 = 0x10;    // 1(0 inverted) for AND gate
    CLCnGLS3 = 0x40;    // 1(0 inverted) for AND gate

    CLCnPOL = 0x00;     // Not inverted
    CLCnCON = 0x8A;     // Negative Edge / 4 input AND

    //========== CLC4 /WT interrupt falling edge ==========
    CLCSELECT = 3;      // select CLC4  

    CLCnSEL0 = 5;       // CLCIN5PPS <- /WT
    CLCnSEL1 = 127;     // NC 
    CLCnSEL2 = 127;     // NC
    CLCnSEL3 = 127;     // NC

    CLCnGLS0 = 0x02;    // /WT noninverted
    CLCnGLS1 = 0x04;    // 1(0 inverted) for AND gate
    CLCnGLS2 = 0x10;    // 1(0 inverted) for AND gate
    CLCnGLS3 = 0x40;    // 1(0 inverted) for AND gate

    CLCnPOL = 0x00;     // Not inverted
    CLCnCON = 0x8A;     // Negative Edge / 4 input AND

    //========== CLC5 RAM address decoder 0x8000 - 0x8FFF ==========
    CLCSELECT = 4;      // CLC5 select

    CLCnSEL0 = 2;       // CLCIN2PPS <- A15
    CLCnSEL1 = 3;       // CLCIN3PPS <- A14
    CLCnSEL2 = 6;       // CLCIN6PPS <- A13
#ifndef _18F47Q43
    CLCnSEL3 = 127;     // NC
#else
    CLCnSEL3 = 7;       // CLCIN7PPS <- A12
#endif

    CLCnGLS0 = 0x02;    // A15 noninverted
    CLCnGLS1 = 0x04;    // A14 inverted
    CLCnGLS2 = 0x10;    // A13 inverted
    CLCnGLS3 = 0x40;    // A12 inverted

    CLCnPOL = 0x00;     // Not inverted the CLC2 output
    CLCnCON = 0x82;     // 4 input AND

    //========== CLC6 ROM address decoder 0x0000 - 0x3FFF ==========
    CLCSELECT = 5;      // CLC6 select

    CLCnSEL0 = 2;       // CLCIN2PPS <- A15
    CLCnSEL1 = 3;       // CLCIN3PPS <- A14
    CLCnSEL2 = 127;     // NC
    CLCnSEL3 = 127;     // NC
 
    CLCnGLS0 = 0x01;    // A15 inverted
    CLCnGLS1 = 0x04;    // A14 inverted
    CLCnGLS2 = 0x10;    // 1(0 inverted) for AND gate
    CLCnGLS3 = 0x40;    // 1(0 inverted) for AND gate

    CLCnPOL = 0x00;     // Not inverted the CLC1 output
    CLCnCON = 0x82;     // 4 input AND


    // RAM clear
    for(i = 0; i < RAM_SIZE; i++) {
        ram[i] = 0;
    }

    // Unlock IVT
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0x00;

    // Default IVT base address
    IVTBASE = 0x000008;

    // Lock IVT
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0x01;

    // CLC VI enable
    CLC1IF = 0;		// Clear the CLC1 interrupt flag
    CLC2IF = 0;		// Clear the CLC3 interrupt flag
    CLC3IF = 0;		// Clear the CLC3 interrupt flag
    CLC4IF = 0;		// Clear the CLC4 interrupt flag
    CLC1IE = 1;     // Enabling CLC1 interrupt ALE falling
    CLC2IE = 1;     // Enabling CLC3 interrupt /RD raising
    CLC3IE = 1;     // Enabling CLC3 interrupt /RD raising
    CLC4IE = 1;		// Enabling CLC4 interrupt /WT raising

    GIE = 1;        // Global interrupt enable
    LATE2 = 1;      // Release reset

    // 8085 start

    while(1) {
        while (U3RXIF)
            LATE1 = 1;
        while (!U3RXIF)
            LATE1 = 0;
    }
}

const unsigned char rom[ROM_SIZE] = {
};