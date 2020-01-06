/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC32MX MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC32MX MCUs - pic32mx : v1.35
        Device            :  PIC32MX250F128B
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC32 1.42
        MPLAB             :  MPLAB X 3.55
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"

#include "integer.h"
#include "xprintf.h"

#include "vTMR1.h"
#include "vUSBMSC_32.h"
#include "vSCSI_32.h"
#include "vUART_CMND.h"
#include "vUART_MENU.h"

//  Enable Global Interrupts 
#define INTERRUPT_GlobalEnable()   __builtin_mtc0(12,0,(__builtin_mfc0(12,0) | 0x0001));


/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    vUART_CMND_init();
    vUART_MENU_init(); 
	USBMSC_initRegisters();
//    SCSI_init();
    
    INTERRUPT_GlobalEnable();

    while (1)
    {
        // Add your application code
        vUART_MENU_control();

        if(USBcondition.Status <= eUSB_IDLE )
        {
            USBMSC_statusControl();
        }

        if(IS_uiTMR007_FINISH)
        {
            uiTMR007 = 500;     //wait 500msec
            LED1_Toggle();
        }
    }
    
    return -1;
}
/**
 End of File
*/