#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h>
#define CS LATAbits.LATA0 
#define CONFIGA 0b01110000
#define CONFIGB 0b11110000

void init_SPI();
void setvoltage(char,short int);
char SPI1_I0(char write);
// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF// no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0x5877 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module


int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    init_SPI();
    __builtin_enable_interrupts();
    
    float valA,valB,delta = 0.0;
    int tri_count = 0, s = 1, hz = 10;
    float m = 10.23;
    
    while (1){
        _CP0_SET_COUNT(0);
        valA = 512.0 + 511.0*sin(delta*2*3.14*hz);      // Function for 10Hz sin wave
        delta = delta + .001;                           // Increment time step
        setvoltage(CONFIGA, valA);                       // Send voltage to DACa
        
        valB = s*m*tri_count;                           // Function for 5Hz triangle wave
        tri_count++;
        if (tri_count <= 100) {                         // Positive slope for half the function
            s = 1;
        }
        else if (tri_count <= 200) {                    // Negative slope for the other half
            s = -1;
        }
        else {                                          // Reset counter after one period has passed
            tri_count = 0;
        }
        setvoltage(CONFIGB, valB);                       // Send voltage to DACb
        
        while (_CP0_GET_COUNT() < 24000) {;}            // Wait until 1ms has passed ((48M/2)*.001sec)
    }
    
    return 0;
}

char SPI1_I0(char write){
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF){;}
    return SPI1BUF;
}

void init_SPI(){
    SDI1Rbits.SDI1R = 0b0100;
    TRISAbits.TRISA1 = 0;               // Make RPA1 an output pin
    RPA1Rbits.RPA1R = 0b0011;           // Set RPA1 to SDO1 (pin 3 -> SDI pin 5 on DAC)
    TRISAbits.TRISA0 = 0;               // Set A0 as an output pin (CS)
    SS1Rbits.SS1R = 0b0000;             // Set RPA0 to SS1 (pin 2 -> CS pin 3 on DAC)
    CS = 1;
    SPI1CON = 0;
    SPI1BUF;
    SPI1BRG = 0x1;
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;   
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1;
    CS = 0;
    //SPI1_I0(0x01);
    //SPI1_I0(0x41);
    CS = 1;
}

void setvoltage(char config_bits,short int data){
    char first = data >>6;
    char bit1 = config_bits | first;
    char bit2 = data << 2;
    CS = 0;
    SPI1_I0(bit1);
    SPI1_I0(bit2);
    CS = 1;
}
        
    
