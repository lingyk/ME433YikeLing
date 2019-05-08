#include "app.h"
#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include "i2c_master.h"
#include "imu.h"
#include "ili9341.h"

APP_DATA appData;
#define MSG_LEN 100
#define DATA_LEN 14


void APP_Initialize ( void ){
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
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

        case APP_STATE_SERVICE_TASKS:{
            
            unsigned char test_msg[MSG_LEN];
            unsigned char data[DATA_LEN] = {};
            unsigned char msg[MSG_LEN];
            float xacc;
            float yacc;
            
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
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
