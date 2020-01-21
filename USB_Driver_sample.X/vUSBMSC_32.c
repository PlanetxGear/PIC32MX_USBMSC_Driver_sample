/***************************************************************************//**
 * @file vUSBMSC.c
 * @brief	USB HOST MSC driver.
 *			It's a low level driver of USB module
 * @author hiroshi murakami
 * @date	20190728
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vUSBMSC_C

#include <string.h>		// for memset

#include "mcc_generated_files/MCC.h"
#include "xprintf.h"

#include "v.h"
#include "vUSBMSC_32.h"
#include "vTMR1.h"


#define USB_ADDRESS_1               1           // ADDRESS = 1
#define U1TOK_PID_TOKEN_SETUP       ((UINT32)0b11010000)
#define U1TOK_PID_TOKEN_IN          ((UINT32)0b10010000)
#define U1TOK_PID_TOKEN_OUT         ((UINT32)0b00010000)
#define U1TOK_EP_EP0                ((UINT32)0x00)    // it is USB interface EP No., it isn't BDT EP.
#define BDT_STAT_UOWN_H             ((UINT16)0b10000000)    // BDTs status setting.
#define BDT_STAT_DATA0              ((UINT16)0b00000000)    // BDTs status setting.
#define BDT_STAT_DATA1              ((UINT16)0b01000000)    // BDTs status setting.

#define BDT_DATA01_POSITION         6	// useing for bit shift.
#define DIR_IN                      0	// transmission dIrection.
#define DIR_OUT                     1	// transmission dIrection.

#define ConvertToPhysicalAddress(a)   ((uint32_t)KVA_TO_PA(a))
#define KVA_TO_PA(kva) ((uint32_t)(kva) & 0x1fffffff)

BDTs_ENTRY __attribute__ ((aligned(512)))    BDTs;
BDT_ENTRY   *pBDTreserve;   //for BDT address reserve.
EP_OBJ      *pEPreserve;
UINT8       *pEvenOddreserve;
UINT8       inOut;

UINT8 UsbBufCMD64[64];	// buffer for Usb COMMAND
//UINT8 UsbBufDAT512[512];	// Usb buffer for DATA
UINT8 UsbBufDCP64[64];	// buffer for Usb DESCRIPTORs

// USB object
USB_OBJECT USBobj;

// USB setup commands
const UINT8 USBMSC_SetAddrssCommand[] = {0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};	// SET_ADDRESS contents / address = 1
const UINT8 USBMSC_SetConfigCommand[] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};	// SET_CONFIG contents  / config  = 1

const UINT8 USBMSC_GetDeviceDescriptor[]  = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00}; // Get device descriptor SETUP contents
const UINT8 USBMSC_GetConfigDescriptor[]  = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x20, 0x00}; // Get config descriptor SETUP contents
//const UINT8 USBMSC_GetReportDescriptor[]  = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0xff, 0x00}; // Get report descriptor (interface 0)
//const UINT8 USBMSC_GetReportDesc2riptor[] = {0x81, 0x06, 0x00, 0x22, 0x01, 0x00, 0xff, 0x00}; // Get report descriptor (interface 1)

//******************************************************************************
/**	
 * @brief	USB speed check & setup relative register.
 *   	    because JSTATE are set up after U1EP0 setting around PIC32MM. 
 */
void USBMSC_checkSpeedAndSetup(void)
{
            if(U1CONbits.JSTATE)	// Connected device is NOT low speed?
            {
                USBobj.IsLowSpeed = 0;	// Clear low speed flag
            }
            else 
            {
                DEBUG_PUTS("LowSpeed!\n");
                USBobj.IsLowSpeed = 1;	// Set low speed flag
            }
            U1ADDR = USBobj.IsLowSpeed? 0x80: 0x00;	// Reset USB address (Set MSB if low speed device)
            U1EP0 = USBobj.IsLowSpeed? 0xcd: 0x4d;	// Enable EP0 & Auto Retry off (Set MSB if low speed device)
}
            
//******************************************************************************
/**	
 * @brief	USB interrupts handler (Handle SOF, ATTACH, DETACH, interrupts).
 * @param[in]	U1IRbits.SOFIF
 * @param[in]	U1IRbits.ATTACHIF
 * @param[in]	U1IRbits.DETACHIF
 * @param[out]	USBobj.SOFCount
 * @param[out]	USBobj.IsAttach
 * @param[out]	USBobj.IsLowSpeed
 *   	
 */
//void __attribute__((__interrupt__,no_auto_psv)) _USB1Interrupt(void)            //pic24f
//void __attribute__ ((vector(_USB_VECTOR), interrupt(IPL1SOFT))) _USB1_ISR (  )  //pic32mm
void __ISR(_USB_1_VECTOR, IPL1AUTO) _USB1Interrupt (  )                         //pic32mx
{
    if(U1IRbits.SOFIF)    // SOF interrupts? 
    {
        USBobj.SOFCount++;      // Just increment SOF counter
    } 
    else if(U1IRbits.ATTACHIF && U1IEbits.ATTACHIE)    // Check attache flag 
    {
        U1IEbits.ATTACHIE = 0;        // Disable ATTACH interrupts
        U1IEbits.DETACHIE = 1;        // Enable DETACH interrupts
        USBobj.IsAttach = 1;    // Set attache flag
        USBobj.Status = eUSB_AttachWait;
        USBMSC_checkSpeedAndSetup();
        DEBUG_PUTS("Attached1\n");    // Show attached message
    } 
    else if(U1IRbits.DETACHIF && U1IEbits.DETACHIE)    // Detach interrupt? 
    {
        U1IEbits.ATTACHIE = 1;        // Disable ATTACH interrupts
        U1IEbits.DETACHIE = 0;        // Enable DETACH interrupts
        USBobj.IsAttach = 0;    // Set attache flag
        USBobj.Status = eUSB_initRegister;
        DEBUG_PUTS("Detached1\n");    // Show detached message
    } 
    
    // when you clearing interrupt flags, you should do it following order. 
    // U1EIR->U1IR->U1OTGIR->USB1IF
    U1EIR = 0xff;           // Clear error interrupt flag
    U1IR = 0xff;            // Clear USB status interrupt flag 
    U1OTGIR = 0xff;         // Clear OTG status interrupt flag
    IFS1bits.USBIF = 0;     // Clear USB1 interrupt flag
    
}

//******************************************************************************
/**    
 * @brief    clear PinPonBuffer Pointer.
 *       
 */
void USBMSC_clearPinPonBufferPointer(void)
{
    USBobj.EPset.EP_Data01 = 0;     //clear CONTROL transmission DATA0/1 number
    USBobj.EPin.EP_Data01 = 0;     //clear CONTROL transmission DATA0/1 number
    USBobj.EPout.EP_Data01 = 0;     //clear CONTROL transmission DATA0/1 number

    USBobj.inEvenOdd = 0;     //set pinpon buffer pointer ODD or EVEN, at first EVEN 
    USBobj.outEvenOdd = 0;    //set pinpon buffer pointer ODD or EVEN, at first EVEN

    U1CONbits.PPBRST = 1;   //pin pon buffer clear
    U1CONbits.PPBRST = 0;   //pin pon buffer clear
}

//******************************************************************************
/**    
 * @brief    clear PinPonBuffer Pointer.
 *       
 */
void USBMSC_clearEPNumber(void)
{
    USBobj.EPset.EP_No = 0;
    USBobj.EPin.EP_No = 0;
    USBobj.EPout.EP_No = 0;
}
    
//******************************************************************************
/**    
 * @brief    USB HOST CONTROL initializing.
 *       
 */
void USBMSC_initRegisters(void)
{
    DEBUG_USB1PUTS("USBMSC_initRegisters\n");
    USBobj.IsAttach = 0;      // Clear attach flag
    USBobj.IsLowSpeed = 0;    // Reset USB bus speed
    USBobj.pUSB_Descriptors = (USB_DESCRIPTORS*)&UsbBufDCP64;

    memset((void*)&BDTs, 0, 32);    // Clear BDT tables
    //KVA_TO_PA:Convert To Physical Address
    BDTs.BDT_IN_D0.ADR = (UINT32)KVA_TO_PA(&UsbBufCMD64);   // Setup IN BD buffer address (max 64 bytes). *Command buffer.
    BDTs.BDT_IN_D1.ADR = (UINT32)KVA_TO_PA(&UsbBufCMD64);   // Setup IN BD buffer address (max 64 bytes). *Command buffer.
    BDTs.BDT_OUT_D0.ADR = (UINT32)KVA_TO_PA(&UsbBufCMD64);  // Setup SETUP/OUT BD buffer address (max 64 bytes) *Command buffer.
    BDTs.BDT_OUT_D1.ADR = (UINT32)KVA_TO_PA(&UsbBufCMD64);  // Setup SETUP/OUT BD buffer address (max 64 bytes) *Command buffer.

    USBobj.Status = eUSB_initRegister;
    
    U1BDTP1 = ((DWORD)KVA_TO_PA(&BDTs) & 0x0000FF00) >> 8;
    U1BDTP2 = ((DWORD)KVA_TO_PA(&BDTs) & 0x00FF0000) >> 16;
    U1BDTP3 = ((DWORD)KVA_TO_PA(&BDTs) & 0xFF000000) >> 24;
    
    U1EIE = 0;          // Disable error interrupt
    U1IE = 0;           // Disable all interrupt
    U1OTGIE = 0;        // Disable USB bus change(OTG check) interrupt
    
    U1EIR = 0xff;       // Clear all USB error interrupts
    U1IR = 0xff;        // Clear all USB interrupts
    U1OTGIR = 0xff;     // Clear all USB interrupts
    memset((void*)&U1EP0, 0, (4*16));    // Clear all UEP register
    U1EP0 = 0X0d;

    U1PWRCbits.USBPWR = 1;    // Turn power on
    U1CON = 0x08;       // Host mode enable, SOF packet stop.
    U1CON = 0x0A;       // Host mode enable, SOF packet stop.
    U1CON = 0x08;       // Host mode enable, SOF packet stop.

    U1OTGCON = 0x30;    // Pull down D+ and D- & VBUS off

    U1ADDR = 0;         // Clear USB address register
//    U1SOF = 0xC2;     // Threth hold is 613 bits/192(DATA=64) bytes
    U1SOF = 0x4A;       // Threth hold is 64 bytes

    
    U1IEbits.ATTACHIE = 1;  // Enable ATTACH interrupt
    U1IEbits.SOFIE = 1;     // Enable SOFIE interrupt
    U1EIE = 0xbf;           // Enable all error interrupts but U1IEbits.UERRIE is off.
//    U1EIE = 0xaf;         // Enable all error interrupts but BMXEE & BTOEE & U1IEbits.UERRIE  are off.

    // USB interrupt priority
    // Priority: 1
    // SubPriority: 0
    IPC7bits.USBIP = 1;
    IPC7bits.USBIS = 0;

    IEC1bits.USBIE = 1;    // USB interrupt enable
}

//******************************************************************************
/**    
 * @brief       USB transaction 1msec wait for NAK.
 * @param[in,out]    USBobj    for next step.
 * @details
 * when NAK occur, retry transmission, but wait SOF for 1ms past.
 *       
 */
void USBMSC_wait1msForNAK(void)
{
    if(USBobj.SOFCountEx != USBobj.SOFCount)    // 1ms past?
    {
        USBobj.Status++;          //next step
    }
}

//******************************************************************************
/**    
 * @brief       USB Transaction Return.
 * @return      eUSB_STATE    is USB status for next step.
 * @param[in]   *pBDT        for checking the return Handshake packet.
 * @param[in,out]    USBobj    for update status.
 * @details    
 * wait until transaction return.
 * when it return, check U1IRbits for STALL, ERROS or compleated transmission. 
 * when transmission is done, check the return PID for ACK, NAK or other Errors.
 * if ACK then next step, if NACK then retry after SOF changing, mean go back 2 status.
 * and it also check the time out error.
 *       
 */
enum eUSB_STATE eUSBMSC_checkTransactionReturn(BDT_ENTRY *pBDT)
{
//    int i;  //////// time trap ///////////
    
    if(USBobj.SOFCountEx != USBobj.SOFCount)	// 1mS past?
    {
        USBobj.SOFCountEx = USBobj.SOFCount;	// Update new tick value
    }
// Handle Stall
    if(U1IRbits.STALLIF)	// Check STALL bit 
    {
        U1IRbits.STALLIF = 1;	// Clear STALL bit 
		USBobj.Status = eUSB_ERR_STALL;
        DEBUG_PUTS( "USB_STALL\n");
        return USBobj.Status;	// STALL return
    }
// Handle Errors
    if(U1IRbits.UERRIF)	// Check error bit 
    {
		USBobj.Status = eUSB_ERR_ANY;	// ERROR Return
        DEBUG_PRINTF( "USB_ERR_ANY. U1EIR:0x%02x\n", U1EIR);
        return USBobj.Status;
    }
// Handle data
    else if(U1IRbits.TRNIF)	// Data transfer bit set? 
    {
        U1IRbits.TRNIF = 1;	// Clear data transfer bit 
        USBobj.BDpid = pBDT->STAT.PID;	// Get PID
        USBobj.BDbyteCount = pBDT->count;	// Get data count
		DEBUG_USB1PRINTF( "PID:0x%02x\n", USBobj.BDpid);
        if(USBobj.BDpid == 0x02 || USBobj.BDpid == 0x03 || USBobj.BDpid == 0x0b)	// ACK/DATA0/DATA1 PID means success of transaction
		{
           
//            uiTMR001 = 4;		//clear set 4ms time out trap timer for USB write, it's needed! 
            USBobj.Status ++;		// Next Step
			return USBobj.Status;	// return Next Steatus
		} 
        else if(USBobj.BDpid == 0x0a)	// NAK PID means USB device is not ready
        {
            if(USBobj.RetrayCount == 0)
            {
                USBobj.Status = ++USBobj.Status;		// next
                return USBobj.Status;	// return retray with waitting SOF change for NAK
            }
            else 
            {
                USBobj.RetrayCount--;
                USBobj.Status = USBobj.Status -2;		// retray
                return USBobj.Status;	// return retray with waitting SOF change for NAK
            }
        }
        else if(USBobj.BDpid == 0x0e)	// STALL PID?
        {
			USBobj.Status = eUSB_ERR_STALL;	// Return as STALL result
            DEBUG_PUTS( "USB_STALL_PID\n");
			return USBobj.Status;
        }
        else	// Unknown pid has come
			USBobj.Status = eUSB_ERR_UnknownPID;	// Return as ERROR result
			DEBUG_PRINTF( "USB_PID_ERROR. Unknow PID:0x%02x\n", USBobj.BDpid);
			return USBobj.Status;
    }
    
    if(IS_uiTMR001_FINISH)  //time out trap
    {
        USBobj.Status = eUSB_ERR_TIMEOUT;
        DEBUG_PUTS( "USB_TIMEOUT_ERROR\n");
        return USBobj.Status;	// Time Out Error return
    }
	
    return 0;	// Wait return
}

//******************************************************************************
/**    
 * @brief    USB transaction status control.
 * @param[in,out]    USBobj.
 * @details    
 * this is main program of USB MSC module. it consists of 4 stages.
 * 1.USB module initialize, USB attach, reset USB bus
 * 2.USB set Address, set config, they are CONTROL transfer. it use EP0
 * 3.data in, data out, they are BULK transfer. they use EP1, EP2.
 * 4.Note that, when errors happen the status stops in ERRORs. 
 *   you should initialize the USB status.
 *       
 */
void USBMSC_statusControl(void)
{
    
    switch (USBobj.Status) 
    {
    case      eUSB_initRegister :
        USBMSC_initRegisters();
        USBobj.Status++;        //next step
        break;
    case      eUSB_AttachWait :
        if (USBobj.IsAttach) 
        {
            uiTMR001 = 100;        // wait timer set for **ms
            USBobj.Status++;        //next step
        }
         break;
    case      eUSB_AttachStable :
        if(IS_uiTMR001_FINISH)
        {
            USBMSC_checkSpeedAndSetup();
            
            DEBUG_USB1PUTS("Attached Stable\n");
            USBobj.Status++;        //next step
        }
         break;
    case      eUSB_resetUsbBus:
        U1CONbits.SOFEN = 0;    // Turn off SOF's, SOF packet stop
        U1CONbits.USBRST = 1;    // Invoke reset
        uiTMR001 = 50;          // wait timer set for 50ms
        USBobj.Status++;          //next step
        break;
    case      eUSB_resetUsbBusWait50ms:
        if(IS_uiTMR001_FINISH)
        {
            U1CONbits.USBRST = 0;    // Release reset
            U1CONbits.SOFEN = 1;    // Turn on SOF's, SOF packet start
            DEBUG_USB1PUTS("Reset USB Bus\n");
            uiTMR001 = 10;          // wait timer set for 10ms
            USBobj.Status++;          //next step
        }
        break;
    case      eUSB_waitUsbBusStable:
        if(IS_uiTMR001_FINISH)
        {
            USBobj.Status++;          //next step
        }
        break;


    case      eUSB_SETUP_setAddress_start:
        DEBUG_USB1PUTS("Set USB Address start\n");
        USBMSC_clearEPNumber();
        USBMSC_clearPinPonBufferPointer();
        USBobj.RetrayCount_aroundAddressSet = 0;

        memcpy(UsbBufCMD64, USBMSC_SetAddrssCommand, 8);    // Copy SET_ADDRESS request packet to send buffer, with address 1.
        USBobj.BufferAddress = (UINT32)&UsbBufCMD64;
        USBobj.TransmissionBytes = 8;

        USBobj.Command = eUSB_SETUP_setAddress_start;
        USBobj.Status = eUSB_CONTROL_OUT_start;          //SETUP!
        break;
    case      eUSB_SETUP_setAddress_BusyCheck:
        USBobj.TransmissionBytes = 0;
        USBobj.Command = eUSB_SETUP_setAddress_BusyCheck;
        USBobj.Status = eUSB_CONTROL_IN_start;          //SETUP!
        break;
    case      eUSB_SETUP_setAddress_END:
        U1ADDR = (USBobj.IsLowSpeed? 0x80: 0x00) + USB_ADDRESS_1;    // Set new address
        xprintf( "Set USB Address. U1ADDR:0x%02x\n", U1ADDR);
        DEBUG_PRINTF("retray_counter:%d\n", USBobj.RetrayCount_aroundAddressSet);
        USBobj.Status++;          //next step
        break;

    case      eUSB_SETUP_setConfig_start:
        DEBUG_USB1PUTS("Set USB Config start\n");
        USBMSC_clearEPNumber();
        USBMSC_clearPinPonBufferPointer();

        memcpy(UsbBufCMD64, USBMSC_SetConfigCommand, 8);    // Copy SET_CONFIG request packet to send buffer.
        USBobj.BufferAddress = (UINT32)&UsbBufCMD64;
        USBobj.TransmissionBytes = 8;

        USBobj.Command = eUSB_SETUP_setConfig_start;
        USBobj.Status = eUSB_CONTROL_OUT_start;          //SETUP!
        break;
    case      eUSB_SETUP_setConfig_BusyCheck:
        USBobj.TransmissionBytes = 0;
        USBobj.Command = eUSB_SETUP_setConfig_BusyCheck;
        USBobj.Status = eUSB_CONTROL_IN_start;          //SETUP!
        break;
    case      eUSB_SETUP_setConfig_END:
        xputs("Set USB Config END\n");
        USBobj.Status++;          //next step
        break;

        
    case      eUSB_GET_DeviceDescriptor_start:
        DEBUG_USB1PUTS("eUSB_GET_DeviceDescriptor_start\n");
        USBMSC_clearEPNumber();
        USBMSC_clearPinPonBufferPointer();

        memcpy(UsbBufCMD64, USBMSC_GetDeviceDescriptor, 8);    // Copy SET_CONFIG request packet to send buffer.
        USBobj.BufferAddress = (UINT32)&UsbBufCMD64;
        USBobj.TransmissionBytes = 8;

        USBobj.Command = eUSB_GET_DeviceDescriptor_start;
        USBobj.Status = eUSB_CONTROL_OUT_start;
        break;
    case      eUSB_GET_DeviceDescriptor_dataGet:
        USBobj.BufferAddress = (UINT32)&USBobj.pUSB_Descriptors->DV;
        USBobj.TransmissionBytes = 18;
        USBobj.Command = eUSB_GET_DeviceDescriptor_dataGet;
        USBobj.Status = eUSB_CONTROL_IN_start;  
        break;
    case      eUSB_GET_DeviceDescriptor_END:
        xputs("eUSB_GET_DeviceDescriptor_END\n");
        USBobj.Status++;          //next step
        break;

        
    case      eUSB_GET_ConfigDescriptor_start:
        DEBUG_USB1PUTS("eUSB_GET_ConfigDescriptor_start\n");
        USBMSC_clearEPNumber();
        USBMSC_clearPinPonBufferPointer();

        memcpy(UsbBufCMD64, USBMSC_GetConfigDescriptor, 8);    // Copy SET_CONFIG request packet to send buffer.
        USBobj.BufferAddress = (UINT32)&UsbBufCMD64;
        USBobj.TransmissionBytes = 8;

        USBobj.Command = eUSB_GET_ConfigDescriptor_start;
        USBobj.Status = eUSB_CONTROL_OUT_start;
        break;
    case      eUSB_GET_ConfigDescriptor_dataGet:
        USBobj.BufferAddress = (UINT32)&USBobj.pUSB_Descriptors->CF;
        USBobj.TransmissionBytes = 32;
        USBobj.Command = eUSB_GET_ConfigDescriptor_dataGet;
        USBobj.Status = eUSB_CONTROL_IN_start;  
        break;
    case      eUSB_GET_ConfigDescriptor_END:
        xputs("eUSB_GET_ConfigDescriptor_END\n");

        if(USBobj.pUSB_Descriptors->EPa.EP_Nos.direction == 1)
        {
            USBobj.EPin.EP_No = USBobj.pUSB_Descriptors->EPa.EP_Nos.EP_No;
            USBobj.EPout.EP_No = USBobj.pUSB_Descriptors->EPb.EP_Nos.EP_No;
        }
        else
        {
            USBobj.EPin.EP_No = USBobj.pUSB_Descriptors->EPb.EP_Nos.EP_No;
            USBobj.EPout.EP_No = USBobj.pUSB_Descriptors->EPa.EP_Nos.EP_No;
        }
        
        USBobj.Command = eUSB_IDLE;
        USBobj.Status = eUSB_IDLE;          //Read & Write command waiting
        USBMSC_clearPinPonBufferPointer();
        break;

        
    ////////////////////////////////////////////////////////////////////////////
    case      eUSB_CONTROL_OUT_start:
        DEBUG_USB1PRINTF("eUSB_CONTROL_OUT_start,SOF:%d\n",USBobj.SOFCount);

        pEPreserve = &USBobj.EPset;
        inOut = DIR_OUT;
        pEvenOddreserve = &USBobj.outEvenOdd;
        USBobj.RetrayCount = 0xffff;
        
        USBobj.Status = eUSB_DataRW_start;
        break;
        
    case      eUSB_CONTROL_IN_start:
        DEBUG_USB1PRINTF("eUSB_CONTROL_IN_start,SOF:%d\n",USBobj.SOFCount);

        pEPreserve = &USBobj.EPset;
        inOut = DIR_IN;
        pEvenOddreserve = &USBobj.inEvenOdd;
        USBobj.RetrayCount = 0xffff;
        
        USBobj.Status = eUSB_DataRW_start;
        break;

      
    case      eUSB_EPout:
        DEBUG_USB1PRINTF("eUSB_EPout,SOF:%u\n",USBobj.SOFCount);
        
        USBobj.Command = eUSB_Busy;

        pEPreserve = &USBobj.EPout;
        inOut = DIR_OUT;
        pEvenOddreserve = &USBobj.outEvenOdd;
        USBobj.RetrayCount = 0xffff;
        
        USBobj.Status = eUSB_DataRW_start;
        break;

    case      eUSB_EPin:
        DEBUG_USB1PRINTF("eUSB_EPin,SOF:%d\n",USBobj.SOFCount);
        
        USBobj.Command = eUSB_Busy;
        
        pEPreserve = &USBobj.EPin;
        inOut = DIR_IN;
        pEvenOddreserve = &USBobj.inEvenOdd;
        USBobj.RetrayCount = 0xffff;
        
        USBobj.Status = eUSB_DataRW_start;
        break;

 
    ////////////////////////////////////////////////////////////////////////////
    case      eUSB_DataRW_retryAfterSOF:
        USBMSC_wait1msForNAK();
        break;
    case      eUSB_DataRW_start:
        DEBUG_USB1PRINTF("eUSB_DataRW_start,SOF:%d\n",USBobj.SOFCount);

        pBDTreserve = &BDTs.BDT_IO[inOut][*pEvenOddreserve];
        pBDTreserve->ADR = KVA_TO_PA(USBobj.BufferAddress);    // Setup BD to buffer address
        pBDTreserve->count = USBobj.TransmissionBytes;    // Setup BD to 64 byte
        pBDTreserve->STAT.Val = BDT_STAT_UOWN_H | (pEPreserve->EP_Data01 << BDT_DATA01_POSITION);
        
        *pEvenOddreserve ^= 1;    // Flip  PinPon pointer  bit for next
        pEPreserve->EP_Data01 ^= 1;    // Flip DATA0/1 bit for next
        USBobj.Status++;        //next step
        uiTMR001 = 10;        //time out trap

        //if(pEPreserve == &USBobj.EPin)
        //if ((pBDTreserve == &BDTs.BDT_IN_D0) || (pBDTreserve == &BDTs.BDT_IN_D1))
        if(inOut == DIR_IN)
        {
            U1TOK = U1TOK_PID_TOKEN_IN | (UINT32)pEPreserve->EP_No;    //start transaction
        }
        else if(pEPreserve == &USBobj.EPout || pEPreserve == &USBobj.EPin)
        {
            
            U1TOK = U1TOK_PID_TOKEN_OUT | (UINT32)pEPreserve->EP_No;        //start transaction
        }
        else
        {
            U1TOK = U1TOK_PID_TOKEN_SETUP | (UINT32)pEPreserve->EP_No;        //start transaction
        }

        break;
    case      eUSB_DataRW_waitTransactionReturn:
        eUSBMSC_checkTransactionReturn(pBDTreserve);   //when it gets NACK, then retry.
        break;
    case      eUSB_DataRW_END:
        DEBUG_USB1PUTS("eUSB_DataRW_END\n");
//        pEPreserve->EP_Data01 ^= 1;    // Flip DATA0/1 bit for next
        USBobj.Status = ++USBobj.Command;          //return to command
        break;

        
    ////////////////////////////////////////////////////////////////////////////
    case      eUSB_Busy:
    case      eUSB_IDLE:
        USBobj.Command = eUSB_IDLE;
        break;

    ////////////////////////////////////////////////////////////////////////////
    case      eUSB_ERRORS:
        break;
    case      eUSB_ERR_TIMEOUT:
        break;
    case      eUSB_ERR_STALL:
        break;
    case    eUSB_ERR_UnknownPID:    
        break;
    case    eUSB_ERR_ANY:    
        IEC1bits.USBIE = 0;    // stop USB interrupt
        USBobj.Status = eUSB_ERR_END;
        xputs("USB_ERR_ANY\n");    // Show Error message
        xprintf( "U1OTGIR:0x%x\n", U1OTGIR);
        xprintf( "U1IR:0x%x\n", U1IR);
        xprintf( "U1EIR:0x%x\n", U1EIR);
        break;
    case    eUSB_ERR_END:    
        break;

    default:
        DEBUG_PRINTF( "USB_DEFAULT!! status:%d\n", USBobj.Status);
    }

}

