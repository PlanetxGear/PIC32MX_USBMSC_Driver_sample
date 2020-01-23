/***************************************************************************//**
 * @file vUSBMSC_32.h
 * @brief	USB HOST MSC driver.
 *			It's a low level driver of USB module
 * @author hiroshi murakami
 * @date	20200123
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
    eUSB_SETUP_setAddress_BusyCheck,
    eUSB_SETUP_setAddress_END,
// setConfig,
    eUSB_SETUP_setConfig_start,
    eUSB_SETUP_setConfig_BusyCheck,
    eUSB_SETUP_setConfig_END,

// getDeviceDiscriptor
    eUSB_GET_DeviceDescriptor_start,
    eUSB_GET_DeviceDescriptor_dataGet,
    eUSB_GET_DeviceDescriptor_END,
// getConfigDiscriptor
    eUSB_GET_ConfigDescriptor_start,
    eUSB_GET_ConfigDescriptor_dataGet,
    eUSB_GET_ConfigDescriptor_END,
    
    eUSB_SETUP_END,
    
// CONTROL transfer 
    eUSB_CONTROL_OUT_start,
    eUSB_CONTROL_IN_start,

// BULK transfer 
    eUSB_EPout,
    eUSB_EPin,
    
// DATA Read Write        
    eUSB_DataRW_retryAfterSOF,
        eUSB_DataRW_start,
        eUSB_DataRW_waitTransactionReturn,
        eUSB_DataRW_END,

        
// "eUSB_Busy" is temporarily set as a command variable during the "DATA Read Write" status.
    eUSB_Busy,
// IDLE or END or Wait next data.
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
#define setUSB_INITIALIZE()     (USBobj.Status = eUSB_initRegister)
#define setUSB_EPin()           (USBobj.Status = eUSB_EPin)
#define setUSB_EPout() 		    (USBobj.Status = eUSB_EPout)
#define isUSB_DETACH()          (USBobj.Status <= eUSB_AttachWait)
#define isUSB_IDLE()            (USBobj.Status == eUSB_IDLE)
#define isUSB_INITIALIZED()     (eUSB_SETUP_END < USBobj.Status && USBobj.Status <= eUSB_IDLE)
#define isUSB_NOT_INITIALIZED() ((USBobj.Status <= eUSB_SETUP_END) || (eUSB_ERRORS <= USBobj.Status))


// *****************************************************************************
/* BDT Entry Layout
*/

// Buffer Descriptor Status Register layout  16bit.
typedef union __attribute__ ((packed)) _BD_STAT
{
    struct __attribute__ ((packed)){
        UINT8            :2;
        UINT8    BSTALL  :1;     //Buffer Stall Enable
        UINT8    DTSEN   :1;     //Data Toggle Synch Enable
        UINT8            :2;     //Reserved - write as 00
        UINT8    DATA01  :1;     //Data Toggle Packet bit Value
        UINT8    UOWN    :1;     //USB Ownership
    };
    struct __attribute__ ((packed)){
        UINT8            :2;
        UINT8    PID0    :1;
        UINT8    PID1    :1;
        UINT8    PID2    :1;
        UINT8    PID3    :1;

    };
    struct __attribute__ ((packed)){
        UINT8            :2;
        UINT8    PID     :4;         //Packet Identifier
    };
    UINT16           Val;
} BD_STAT;


// BDT Entry Layout 32bit
typedef union __attribute__ ((packed))__BDT
{
    struct __attribute__ ((packed))
    {
        BD_STAT        STAT;
        UINT16       CNT:10;
        UINT32       ADR; //Buffer Address
    };
    struct __attribute__ ((packed))
    {
        UINT32       res  :16;
        UINT32       count:10;
    };
    UINT32           w[2];
    UINT16           v[4];
    UINT64           Val;
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


// *****************************************************************************
/* USB Device Descriptor Structure
*/
typedef struct __attribute__ ((packed)) _USB_DEVICE_DESCRIPTOR
{
    UINT8 bLength;               // Length of this descriptor.
    UINT8 bDescriptorType;       // DEVICE descriptor type (USB_DESCRIPTOR_DEVICE).
    UINT16 bcdUSB;               // USB Spec Release Number (BCD).
    UINT8 bDeviceClass;          // Class code (assigned by the USB-IF). 0xFF-Vendor specific.
    UINT8 bDeviceSubClass;       // Subclass code (assigned by the USB-IF).
    UINT8 bDeviceProtocol;       // Protocol code (assigned by the USB-IF). 0xFF-Vendor specific.
    UINT8 bMaxPacketSize0;       // Maximum packet size for endpoint 0.
    UINT16 idVendor;             // Vendor ID (assigned by the USB-IF).
    UINT16 idProduct;            // Product ID (assigned by the manufacturer).
    UINT16 bcdDevice;            // Device release number (BCD).
    UINT8 iManufacturer;         // Index of String Descriptor describing the manufacturer.
    UINT8 iProduct;              // Index of String Descriptor describing the product.
    UINT8 iSerialNumber;         // Index of String Descriptor with the device's serial number.
    UINT8 bNumConfigurations;    // Number of possible configurations.
} USB_DEVICE_DESCRIPTOR;

// *****************************************************************************
/* USB Configuration Descriptor Structure
*/
typedef struct __attribute__ ((packed)) _USB_CONFIGURATION_DESCRIPTOR
{
    UINT8 bLength;               // Length of this descriptor.
    UINT8 bDescriptorType;       // CONFIGURATION descriptor type (USB_DESCRIPTOR_CONFIGURATION).
    UINT16 wTotalLength;         // Total length of all descriptors for this configuration.
    UINT8 bNumInterfaces;        // Number of interfaces in this configuration.
    UINT8 bConfigurationValue;   // Value of this configuration (1 based).
    UINT8 iConfiguration;        // Index of String Descriptor describing the configuration.
    UINT8 bmAttributes;          // Configuration characteristics.
    UINT8 bMaxPower;             // Maximum power consumed by this configuration.
} USB_CONFIGURATION_DESCRIPTOR;

// *****************************************************************************
/* USB Interface Descriptor Structure
*/
typedef struct __attribute__ ((packed)) _USB_INTERFACE_DESCRIPTOR
{
    UINT8 bLength;               // Length of this descriptor.
    UINT8 bDescriptorType;       // INTERFACE descriptor type (USB_DESCRIPTOR_INTERFACE).
    UINT8 bInterfaceNumber;      // Number of this interface (0 based).
    UINT8 bAlternateSetting;     // Value of this alternate interface setting.
    UINT8 bNumEndpoints;         // Number of endpoints in this interface.
    UINT8 bInterfaceClass;       // Class code (assigned by the USB-IF).  0xFF-Vendor specific.
    UINT8 bInterfaceSubClass;    // Subclass code (assigned by the USB-IF).
    UINT8 bInterfaceProtocol;    // Protocol code (assigned by the USB-IF).  0xFF-Vendor specific.
    UINT8 iInterface;            // Index of String Descriptor describing the interface.
} USB_INTERFACE_DESCRIPTOR;

// *****************************************************************************
/* USB Endpoint Descriptor Structure
*/
typedef struct __attribute__ ((packed)) _EP_Nos
{
    UINT8 EP_No:4;
    UINT8 :3;
    UINT8 direction: 1;
} EP_Nos;

typedef struct __attribute__ ((packed)) _USB_ENDPOINT_DESCRIPTOR
{
    UINT8 bLength;               // Length of this descriptor.
    UINT8 bDescriptorType;       // ENDPOINT descriptor type (USB_DESCRIPTOR_ENDPOINT).
//    UINT8 bEndpointAddress;      // Endpoint address. Bit 7 indicates direction (0=OUT, 1=IN).
    union
    {   
        EP_Nos EP_Nos;
        UINT8 bEndpointAddress;
    }; 
    UINT8 bmAttributes;          // Endpoint transfer type.
    UINT16 wMaxPacketSize;        // Maximum packet size.
    UINT8 bInterval;             // Polling interval in frames.
} USB_ENDPOINT_DESCRIPTOR;

// *****************************************************************************
/* USB Descriptors
*/
typedef struct __attribute__ ((packed)) _USB_DESCRIPTORS
{
    USB_DEVICE_DESCRIPTOR           DV;
    USB_CONFIGURATION_DESCRIPTOR    CF;
    USB_INTERFACE_DESCRIPTOR        IF;
    USB_ENDPOINT_DESCRIPTOR         EPa;
    USB_ENDPOINT_DESCRIPTOR         EPb;
} USB_DESCRIPTORS;




// *****************************************************************************
/* controller for SIE(Serial Interface Engine).
*/
typedef struct _EP_OBJECT
{
    UINT8     Data01;
    UINT8     EP_No;
} EP_OBJECT;

typedef struct _SIE_OBJECT      // controller for SIE(Serial Interface Engine).
{
    EP_OBJECT   EPset;
    EP_OBJECT   EPin;
    EP_OBJECT   EPout;
    
    UINT8       BDbyteCount;      // BD transfar byte count save
    UINT8       BDpid;            // BD PID save
    
    UINT8       inOut;      // transmission diraction. 0:/1:
    UINT8       EvenOdd[2]; // BDT PinPon buffer Pointer. 0:Rx(In)/1:Tx(Out)
    
} SIE_OBJECT;

// *****************************************************************************
/* interface for vUSBMSC module
*/
typedef struct __USB_OBJECT    // interface for vUSBMSC module
{
    UINT32    BufferAddress;  // DATA buffer Address
    enum eUSB_STATE        Status;
    enum eUSB_STATE        Command;
    UINT16    SOFCount;       // SOF freame counter
    UINT16    SOFCountEx;     // Ex SOF freame counter (prior counter value)
    UINT16    RetrayCount_aroundAddressSet;     // for slow USBs NAK retray wait count.
    UINT8     IsLowSpeed;     //Low speed device flag. 1:Low Speed 0:Full Speed,  
    UINT8     IsAttach;       //Attache flag. 1:attach 0:detach

    UINT8     TransmissionBytes;      // DATA Send Bytes
    
    USB_DESCRIPTORS *pUSB_Descriptors;    
#ifdef _V_DEBUG_SCSI2
	UINT16	SOFCountSt;     // SOF freame counter save for SCSI DEBUG
#endif
} USB_OBJECT;





/*****************************
 * VARIABLES
 *****************************/
extern UINT8 UsbBufCMD64[64];	// Usb buffer for COMMAND
//extern UINT8 UsbBufDAT512[512];	// Usb buffer for DATA

extern USB_OBJECT	USBobj;


/*****************************
 * PROTOTYPES
 *****************************/
void USBMSC_initRegisters(void);
void USBMSC_statusControl(void);


#endif
