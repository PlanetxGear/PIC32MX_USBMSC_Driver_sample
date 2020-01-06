/***************************************************************************//**
 * @file vUSBMSC_32.h
 * @brief	USB HOST MSC driver.
 *			It's a low level driver of USB module
 * @author hiroshi murakami
 * @date	20190728
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vUSBMSC_H
#define _vUSBMSC_H

#include "integer.h"

/*****************************
 * STATE
 *****************************/
enum eUSB_STATE {
    eUSB_initRegister = 0,
// Attach
    eUSB_AttachWait,
    eUSB_AttachStable,
// rest USB Bus
    eUSB_resetUsbBus,
    eUSB_resetUsbBusWait50ms,
    eUSB_waitUsbBusStable,
// setAddress,
    eUSB_SETUP_setAddress_start,
    eUSB_SETUP_setAddress_END,
// setConfig,
    eUSB_SETUP_setConfig_start,
    eUSB_SETUP_setConfig_END,

// CONTROL transfer
    eUSB_SETUP_retryAfterSOF,
        eUSB_SETUP_start,
        eUSB_SETUP_waitTransactionReturn,
        eUSB_SETUP_checkBusy_retryAfterSOF,
        eUSB_SETUP_checkBusy,
        eUSB_SETUP_waitBusy,
        eUSB_SETUP_END,

// BULK transfer
    eUSB_BULK_dataIN_retryAfterSOF,
        eUSB_BULK_dataIN_start,
        eUSB_BULK_dataIN_waitTransactionReturn,
        eUSB_BULK_dataIN_END,

    eUSB_BULK_dataOUT_retryAfterSOF,
        eUSB_BULK_dataOUT_start,
        eUSB_BULK_dataOUT_waitTransactionReturn,
        eUSB_BULK_dataOUT_END,

// IDLE or END or wait next data.
    eUSB_IDLE,

// Errors. you should initialize USB status.
    eUSB_ERRORS,
        eUSB_ERR_NODEVICE,
        eUSB_ERR_TIMEOUT,
        eUSB_ERR_STALL,
        eUSB_ERR_UnknownPID,
        eUSB_ERR_ANY,
        eUSB_ERR_END,

// when it happened, this code has bug.        
    eUSB_defaut
};

/*****************************
 * DEFINES
 *****************************/
#define setUSB_INITIALIZE()     (USBcondition.Status = eUSB_initRegister)
#define setUSB_DATAIN()         (USBcondition.Status = eUSB_BULK_dataIN_start)
#define setUSB_DATAOUT() 		(USBcondition.Status = eUSB_BULK_dataOUT_start)
#define isUSB_DETACH()          (USBcondition.Status <= eUSB_AttachWait)
#define isUSB_IDLE()            (USBcondition.Status == eUSB_IDLE)
#define isUSB_INITIALIZED()     (eUSB_SETUP_END < USBcondition.Status && USBcondition.Status <= eUSB_IDLE)
#define isUSB_NOT_INITIALIZED() ((USBcondition.Status <= eUSB_SETUP_END) || (eUSB_ERRORS <= USBcondition.Status))

// Buffer Descriptor Status Register layout  16bit.
typedef union __attribute__ ((packed)) _BD_STAT
{
    struct __attribute__ ((packed)){
        unsigned            :2;
        unsigned    BSTALL  :1;     //Buffer Stall Enable
        unsigned    DTSEN   :1;     //Data Toggle Synch Enable
        unsigned            :2;     //Reserved - write as 00
        unsigned    DATA01  :1;     //Data Toggle Packet bit Value
        unsigned    UOWN    :1;     //USB Ownership
    };
    struct __attribute__ ((packed)){
        unsigned            :2;
        unsigned    PID0    :1;
        unsigned    PID1    :1;
        unsigned    PID2    :1;
        unsigned    PID3    :1;

    };
    struct __attribute__ ((packed)){
        unsigned            :2;
        unsigned    PID     :4;         //Packet Identifier
    };
    uint16_t           Val;
} BD_STAT;


// BDT Entry Layout 32bit
typedef union __attribute__ ((packed))__BDT
{
    struct __attribute__ ((packed))
    {
        BD_STAT        STAT;
        uint16_t       CNT:10;
        uint32_t       ADR; //Buffer Address
    };
    struct __attribute__ ((packed))
    {
        uint32_t       res  :16;
        uint32_t       count:10;
    };
    uint32_t           w[2];
    uint16_t           v[4];
    uint64_t           Val;
} BDT_ENTRY;

typedef union _BDTABLE
{
    BDT_ENTRY BDT[4];
    BDT_ENTRY BDT_IO[2][2];
    struct //_BDT_LOCATION
    {
        BDT_ENTRY BDT_IN_D0;           // EP0 RX-IN EVEN Descriptor
        BDT_ENTRY BDT_IN_D1;           // EP0 RX-IN ODD Descriptor
        BDT_ENTRY BDT_OUT_D0;          // EP0 TX-OUT EVEN Descriptor
        BDT_ENTRY BDT_OUT_D1;          // EP0 TX-OUT ODD Descriptor
    };
    struct //_BDT_LOCATION
    {
        BDT_ENTRY BDT_IN[2];           // EP0 RX-IN EVEN/ODD Descriptor
        BDT_ENTRY BDT_OUT[2];          // EP0 TX-OUT EVEN/ODD Descriptor
    };
    
}BDTs_ENTRY;

//#define BDT_IN BDT[0]
//#define BDT_OUT BDT[1]

typedef struct __USB_CONDITION
{
 	enum eUSB_STATE		Status;
 	enum eUSB_STATE		Command;
	UINT16	IsLowSpeed;     //Low speed device flag. 1:Low Speed 0:Full Speed,  
	UINT16	IsAttach;       //Attache flag. 1:attach 0:detach
	UINT16	SOFCount;       // SOF freame counter
	UINT16	SOFCountEx;     // Ex SOF freame counter (prior counter value)
	UINT16	BDpid;          // BD PID save
	UINT16	BDbyteCount;    // BD transfar byte count save
	UINT16	SendBytes;      // DATA Send Bytes
	UINT32	retray_counter_AddressSet;     // for slow USBs NAK retray wait count.
	UINT32	wait_count;     // for slow USBs NAK retray wait count.
	UINT32	BufferAddress;  // DATA buffer Address
#ifdef _V_DEBUG_SCSI2
	UINT16	SOFCountSt;     // SOF freame counter save for SCSI DEBUG
#endif
} USB_CONDITION;


/*****************************
 * VARIABLES
 *****************************/
extern UINT8 UsbBufCMD64[64];	// Usb buffer for COMMAND
//extern UINT8 UsbBufDAT512[512];	// Usb buffer for DATA

extern USB_CONDITION	USBcondition;
//--extern BDT_ENTRY		BDT[4];


/*****************************
 * PROTOTYPES
 *****************************/
void USBMSC_initRegisters(void);
void USBMSC_statusControl(void);


#endif
