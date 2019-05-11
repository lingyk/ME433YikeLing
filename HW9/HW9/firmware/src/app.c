
#include "app.h"
#include "ili9341.h"
#include "i2c_master.h"
#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>

#define buttonx 120
#define buttony1 80
#define buttony2 200
#define L 80.9
#define H 50
#define BL 30
#define BH 10

char letter[100];
int count;
char x_value;
char y_value;
char z_value;


APP_DATA appData;


void APP_Initialize ( void )
{
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    __builtin_enable_interrupts();
    i2c_master_setup();
    LCD_init();
    SPI1_init();
    LCD_clearScreen(ILI9341_BLACK);
    LCD_button_plus(buttonx,buttony1,L,H,BL,BH,ILI9341_WHITE, ILI9341_BLACK); // plus button
    LCD_button_minus(buttonx,buttony2,L,H,BL,BH, ILI9341_WHITE,ILI9341_BLACK); //minus button
    count = 0;
    
    

}




void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {   
            _CP0_SET_COUNT(0);
            
            CS_T = 0;
            int z_raw;
            unsigned short x;
            unsigned short y;
            unsigned short t;
            int z1;
            int z2;

            XPT2046_read(&x, &y, &z1, &z2);
            z_raw=z1-z2+4095;

            unsigned short x_actual;
            unsigned short y_actual;
            x_actual=((x-245)/15.125);
            y_actual=((3900-y)/11.15);

//print value to lcd
            CS_T=1;
            sprintf(letter,"X Raw : %d      ",x); //print x value
            LCD_getstring(30,40,letter,ILI9341_BLUE,ILI9341_WHITE);  
            sprintf(letter,"Y Raw : %d      ",y); //print y value
            LCD_getstring(30,55,letter,ILI9341_BLUE,ILI9341_WHITE);
            sprintf(letter,"X : %d      ",x_actual); //print x value
            LCD_getstring(30,70,letter,ILI9341_BLUE,ILI9341_WHITE);   
            sprintf(letter,"Y : %d     ",y_actual); //print y value
            LCD_getstring(30,85,letter,ILI9341_BLUE,ILI9341_WHITE);
            sprintf(letter,"Z : %d   ",z_raw); // print z value
            LCD_getstring(30,100,letter,ILI9341_BLUE,ILI9341_WHITE);
// if in + button , count +1
            if (z_raw>800)
                { 
            if(buttonx<=x_actual && x_actual<=(buttonx+L) && buttony1<y_actual && y_actual<(buttony1+H)){
                count=count+1;
                }
            }
       
// if in - button, count -1
            if (z_raw>800){
                if(buttonx<=x_actual && x_actual<=(buttonx+L) && buttony2<y_actual && y_actual<(buttony2+H)){
                count=count-1;
                }
                }
            sprintf(letter,"C O U N T :%d",count);
            LCD_getstring(135, 45,letter,ILI9341_BLUE,ILI9341_WHITE); 
                }
                    break;
                }

        /* TODO: implement your application state machine.*/
        

    }

 

