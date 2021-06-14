/***********************************************************/
/*   ____ ___ _   _ ___ ___     ____ _                     */
/*  |  _ \_ _| | | |_ _/ _ \   / ___| | ___  _ __   ___    */
/*  | |_) | || | | || | | | | | |   | |/ _ \| '_ \ / _ \   */
/*  |  __/| || |_| || | |_| | | |___| | (_) | | | |  __/   */
/*  |_|  |___|\___/|___\___/   \____|_|\___/|_| |_|\___|   */
/*                                                         */
/*  By: Lucas Teske                                        */
/***********************************************************/
/*     Basicly this is an PIUIO Clone with an ATMEGA328    */
/*     It uses V-USB for the USB Interface and few chips   */
/***********************************************************/
/*           This is main code from PIUIO Clone            */
/*  This is the simple branch. This is for users that want */
/*           just to connect to your DIY pads              */
/*       The lights arent available on this one.           */
/***********************************************************/
/*                    License is GPLv3                     */
/*  Please consult https://github.com/racerxdl/piuio_clone */
/***********************************************************/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h> 
#include <util/delay.h> 

#include <avr/pgmspace.h>   
#include "usbdrv.h"
#include "usbconfig.h"

//    Some Macros to help

#define GETBIT(port,bit) ((port) & (0x01 << (bit)))     //    Get Byte bit
#define SETBIT(port,bit) ((port) |= (0x01 << (bit)))    //    Set Byte bit
#define CLRBIT(port,bit) ((port) &= ~(0x01 << (bit)))   //    Clr Byte bit

//    PIUIO Bytes

// This is for Byte0 of the player short

#define SENSOR_1     0x01
#define SENSOR_2     0x02
#define SENSOR_3     0x04
#define SENSOR_4     0x08
#define SENSOR_5     0x10
#define SENSOR_6     0x20
#define SENSOR_7     0x40
#define SENSOR_8     0x80

//    Clone Port Map

#define GET_BUTTON_1 GETBIT(PINC,0)         // C0: (Pin A0) P1 Option
#define GET_BUTTON_2 GETBIT(PINC,1)         // C1: (Pin A1) P1 Menu Left
#define GET_BUTTON_3 GETBIT(PINC,2)         // C2: (Pin A2) P1 Menu Right
#define GET_BUTTON_4 GETBIT(PINC,3)         // C3: (Pin A3) P1 Start
#define GET_BUTTON_5 GETBIT(PINC,4)         // C4: (Pin A4) P2 Option
#define GET_BUTTON_6 GETBIT(PINC,5)         // C5: (Pin A5) P2 Menu Left
#define GET_BUTTON_7 GETBIT(PINB,3)         // B3: (Pin D11) P2 Menu Right
#define GET_BUTTON_8 GETBIT(PINB,4)         // B4: (Pin D12) P2 Start


//    Some Vars to help
static unsigned char LampData[8];       //    The LampData buffer received
static unsigned char InputData[8];      //    The InputData buffer to send
static unsigned char datareceived = 0;  //    How many bytes we received
static unsigned char dataLength = 0;    //    Total to receive

//static unsigned char Input[2];          //    The actual 16 bits Input data
static unsigned char Output[2];         //    The actual 16 bits Output data

USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {
    //    This function will be only triggered when game writes to the lamps output.
    //    I will keep this part of code, but we wont use to write to outputs. Because lack of IO
    unsigned char i;              
    for(i = 0; datareceived < 8 && i < len; i++, datareceived++)
     LampData[datareceived] = data[i];    
    if(datareceived == dataLength)    {    //    Time to set OUTPUT
        Output[0] = LampData[0];           //    The AM use unsigned short for those. 
        Output[1] = LampData[2];           //    So we just skip one byte
                                           //    The other bytes are just 0xFF junk
    }    
    return (datareceived == dataLength);   // 1 if we received it all, 0 if not
}

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
    usbRequest_t *rq = (void *)data;
    if(rq->bRequest == 0xAE)    {                               //    Access Game IO
        switch(rq->bmRequestType)    {
            case 0x40:                                          //    Writing data to outputs
            datareceived = 0;
            dataLength = (unsigned char)rq->wLength.word;
                return USB_NO_MSG;                              //    Just tell we want a callback to usbFunctionWrite
                break;
            case 0xC0:                                          //    Reading input data
                usbMsgPtr = InputData;                          //    Just point to the buffer, and 
                return 8;                                       //    saying to send 8 bytes to the PC
                break;
            }
        }
    return 0;                                                   //    Ops, it cant get here
}

void pollInputOutput()    {
    //  This will get the inputs.
    //  PIUIO actually sends inverted data.
    InputData[0] = 0xFF;
    InputData[1] = 0xFF;
    InputData[2] = 0xFF;
    InputData[3] = 0xFF;  

    // Handle button inputs (LS, someone DO THIS BETTER. LOL)

    if(GET_BUTTON_1)   { InputData[0] |= SENSOR_1;    }else{ InputData[0] &= ~(SENSOR_1);    };
    if(GET_BUTTON_2)   { InputData[0] |= SENSOR_2;    }else{ InputData[0] &= ~(SENSOR_2);    };
    if(GET_BUTTON_3)   { InputData[0] |= SENSOR_3;    }else{ InputData[0] &= ~(SENSOR_3);    };
    if(GET_BUTTON_4)   { InputData[0] |= SENSOR_4;    }else{ InputData[0] &= ~(SENSOR_4);    };
    if(GET_BUTTON_5)   { InputData[0] |= SENSOR_5;    }else{ InputData[0] &= ~(SENSOR_5);    };
    if(GET_BUTTON_6)   { InputData[0] |= SENSOR_6;    }else{ InputData[0] &= ~(SENSOR_6);    };
    if(GET_BUTTON_7)   { InputData[0] |= SENSOR_7;    }else{ InputData[0] &= ~(SENSOR_7);    };
    if(GET_BUTTON_8)   { InputData[0] |= SENSOR_8;    }else{ InputData[0] &= ~(SENSOR_8);    };
    
    // Handle lamp outputs
    
    if(GETBIT(Output[0], 7)) { SETBIT(PORTB, 2); }else{ CLRBIT(PORTB, 2); }; //P1 Option
    if(GETBIT(Output[0], 6)) { SETBIT(PORTB, 1); }else{ CLRBIT(PORTB, 1); }; //P1 Menu Left
    if(GETBIT(Output[0], 5)) { SETBIT(PORTB, 0); }else{ CLRBIT(PORTB, 0); }; //P1 Menu Right
    if(GETBIT(Output[0], 4)) { SETBIT(PORTD, 7); }else{ CLRBIT(PORTD, 7); }; //P1 Start
    if(GETBIT(Output[0], 3)) { SETBIT(PORTD, 6); }else{ CLRBIT(PORTD, 6); }; //P2 Option
    if(GETBIT(Output[0], 2)) { SETBIT(PORTD, 5); }else{ CLRBIT(PORTD, 5); }; //P2 Menu left
    if(GETBIT(Output[0], 1)) { SETBIT(PORTD, 4); }else{ CLRBIT(PORTD, 4); }; //P2 Menu Right
    if(GETBIT(Output[0], 0)) { SETBIT(PORTD, 3); }else{ CLRBIT(PORTD, 3); }; //P2 Start
    

}

int main() {
    unsigned char i;
    wdt_enable(WDTO_1S);

    DDRB = 0x18; //Set port pins B0-2 as outputs, 3-4 as inputs
    DDRC = 0; //PortC should be all inputs
    DDRD = 0xF8; //Leave port pins D0-2 untouched (for serial/USB), pins D3-7 as outputs
    
	// make it simpler and use internal pullups
	PORTB =	0x18; //Enable pullups on only port pins B3-4
	PORTC = 0x3F; //Enable pullups on port pins C0-5
	
    for(i=0;i<8;i++)
        InputData[i] = 0xFF;
    
    usbInit();
    usbDeviceDisconnect();                      // enforce re-enumeration
    for(i = 0; i<250; i++) {                    // wait 500 ms
        wdt_reset();                            // keep the watchdog happy
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();                                      // Enable interrupts after re-enumeration
    while(1) {
        wdt_reset();                            // keep the watchdog happy
        usbPoll();
        pollInputOutput();
    }

    return 0;
}
