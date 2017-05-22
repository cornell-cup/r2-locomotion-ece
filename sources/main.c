/********************************************************************
 FileName:      main.c
 Dependencies:  See INCLUDES section
 Processor:     PIC18, PIC24, dsPIC, and PIC32 USB Microcontrollers
 Hardware:      This demo is natively intended to be used on Microchip USB demo
                boards supported by the MCHPFSUSB stack.  See release notes for
                support matrix.  This demo can be modified for use on other 
                hardware platforms.
 Complier:      Microchip C18 (for PIC18), XC16 (for PIC24/dsPIC), XC32 (for PIC32)
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Description
  ----  -----------------------------------------
  1.0   Initial release
  2.1   Updated for simplicity and to use common coding style
  2.8   Improvements to USBCBSendResume(), to make it easier to use.
        Added runtime check to avoid buffer overflow possibility if 
        the USB IN data rate is somehow slower than the UART RX rate.
  2.9b  Added support for optional hardware flow control.
  2.9f  Adding new part support   
  2.9j  Updates to support new bootloader features (ex: app version 
        fetching).
********************************************************************/

/** INCLUDES *******************************************************/
#include "../includes/usb/usb.h"
#include "../includes/usb/usb_function_cdc.h"
#include "../includes/HardwareProfile.h"

#pragma config FNOSC = PRIPLL, POSCMOD = HS, FSOSCEN = OFF, OSCIOFNC = OFF
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPBDIV = DIV_1, FPLLODIV = DIV_2
#pragma config FWDTEN = OFF, JTAGEN = OFF, ICESEL = ICS_PGx3
#pragma config UPLLIDIV = DIV_2, UPLLEN = ON

/** I N C L U D E S **********************************************************/

#include "GenericTypeDefs.h"
#include "../includes/Compiler.h"
#include "../includes/usb/usb_config.h"
#include "../includes/usb/usb_device.h"

#define _SUPPRESS_PLIB_WARNING 1
#include <plib.h>
#include "../includes/hardwareConfig.h"
#include "../includes/roboclawPacketSerial.h"
#include "../includes/R2Protocol.h"
#include <stdarg.h>

/** C O M M A N D S ********************************************************/

/** V A R I A B L E S ********************************************************/

/** P R I V A T E  P R O T O T Y P E S ***************************************/
static void InitializeSystem(void);


/******************************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *****************************************************************************/

int main(void)
{   
//    configRoboclawUART();
    
    InitializeSystem();

    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif

    configEscon();

    while(1)
    {
        #if defined(USB_POLLING)
		// Check bus status and service USB interrupts.
        USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
        				  // this function periodically.  This function will take care
        				  // of processing and responding to SETUP transactions 
        				  // (such as during the enumeration process when you first
        				  // plug in).  USB hosts require that USB devices should accept
        				  // and process SETUP packets in a timely fashion.  Therefore,
        				  // when using polling, this function should be called 
        				  // regularly (such as once every 1.8ms or faster** [see 
        				  // inline code comments in usb_device.c for explanation when
        				  // "or faster" applies])  In most cases, the USBDeviceTasks() 
        				  // function does not take very long to execute (ex: <100 
        				  // instruction cycles) before it returns.
        #endif

        OpenTimer1(T1_ON | T1_PS_1_256, 0xFFFF);
        
		// Application-specific tasks.
		// Application related code may be added here, or in the ProcessIO() function.
        struct R2ProtocolPacket packet;
        uint8_t packetData[30] = {0};
        packet.data = packetData;
        packet.data_len = 30;
        
        int result = ProcessIO(&packet);
        /* the buffers now contain relevant information;
         * they are updated if result == 1; otherwise, it's old info
         */
        
        char readBuffer[100];
        if (result){
            // new data available
            
            //print out data obtained:
            packet.data[packet.data_len] = 0;
//            sprintf(readBuffer,
//                "%s\n",packet.data);
//            putsUSBUSART(readBuffer);
//            CDCTxService();
            
            if(packet.data[0] != 'M' || !(packet.data[1] == '1' || packet.data[1] == '2')) {
                // do nothing, payload is bad
                /*
                 * Send error back to source?
                 */
            }
            else {
                /*
                 * Parse motor velocities and send velocity commands to 
                 * Roboclaw.
                 */
                char M1speed[6];
                char M2speed[6];
                memcpy(M1speed, &packet.data[2], 5);
                memcpy(M2speed, &packet.data[9], 5);
                M1speed[5] = 0;
                M2speed[5] = 0;

                int speedM1 = atoi(M1speed);
                int speedM2 = atoi(M2speed);

                driveEscon(1, speedM1);
                driveEscon(2, speedM2);
                sprintf(readBuffer, "ch1 = %d, ch2 = %d\n\r", speedM1, speedM2);
                putsUSBUSART(readBuffer);
            }
        }
        
    }//end while
}//end main


/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{

    ANSELA = 0; ANSELB = 0;
    SYSTEMConfigPerformance(40000000);

//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.    
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif
    
//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2" 
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile - (platform).h, and that an appropriate I/O pin 
//  has been mapped	to it.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;	// See HardwareProfile.h
    #endif
    
    UserInit();

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
}//end InitializeSystem


/** EOF main.c *************************************************/
