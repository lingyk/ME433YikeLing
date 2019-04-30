#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include "ili9341.h"
#include "i2c_master.h"
#include "imu.h"


// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
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
#pragma config WDTPS = PS1048576 // use slowest wdt    //???
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
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define MSG_LEN 100
#define DATA_LEN 14

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

    // LED pin as output
    TRISAbits.TRISA4 = 0;

    // Pushbutton pin as input
    TRISBbits.TRISB4 = 1;

    // Turn off LED
    LATAbits.LATA4 = 0;
    
    imu_init();
    LCD_init();
    LCD_clearScreen(ILI9341_BLACK);
    __builtin_enable_interrupts();
    
    unsigned char test_msg[MSG_LEN];
    unsigned char data[DATA_LEN] = {};
    unsigned char msg[MSG_LEN];
    float xacc;
    float yacc;
    
    while (1){
        _CP0_SET_COUNT(0);
        unsigned char status = imu_test();
        sprintf(test_msg,"address = %d",status);
        LCD_getstring(28,40,test_msg,ILI9341_WHITE,ILI9341_BLACK);
        
        i2c_read_multiple(IMU_ADDR, 0x20, data, DATA_LEN);
        xacc = getX(data);
        yacc = getY(data);
        sprintf(msg,"xacc = %1.3f", xacc);
        LCD_getstring(28,50,msg,ILI9341_WHITE,ILI9341_BLACK);
        sprintf(msg,"yacc = %1.3f", yacc);
        LCD_getstring(28,60,msg,ILI9341_WHITE,ILI9341_BLACK);
        
        LCD_staticbar(45,160,3,150,ILI9341_BLUE);
        LCD_staticbar(120,60,200,3,ILI9341_BLUE);
        
        LCD_xbar(120,160,3,xacc,ILI9341_WHITE,ILI9341_BLUE);
        LCD_ybar(120,160,yacc,3,ILI9341_WHITE,ILI9341_BLUE);
        
        while(_CP0_GET_COUNT()<1200000){;}
        LATAbits.LATA4 =! LATAbits.LATA4;
        
    }
}
