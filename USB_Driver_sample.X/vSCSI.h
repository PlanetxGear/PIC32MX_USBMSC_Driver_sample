/***************************************************************************//**
 * @file vSCSI.h
 * @brief	SCSI COMAND CONTROLLER.
 *			it's designed to work in concert with vUSBMSC.
 * @author hiroshi murakami
 * @date	20190728
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vSCSI_H
#define _vSCSI_H

#include "integer.h"

/*****************************
 * STATE
 *****************************/
enum eSCSI_COMMAND {
//command stage.
	eSCSI_requestSense = 0,
	eSCSI_readCapacity,
	eSCSI_read,
	eSCSI_write
};

enum eSCSI_STATE {
//SCSI transaction stage.
	eSCSI_CBW,
	eSCSI_CBW_start,
	eSCSI_CBW_wait,
    eSCSI_getputBranch,
    
	eSCSI_getData_start,
	eSCSI_getData_wait,
	eSCSI_getData_next,
	
	eSCSI_putData_start,
	eSCSI_putData_wait,
	eSCSI_putData_next,
	
//    eSCSI_CSW,
	eSCSI_CSW_start,
	eSCSI_CSW_wait,
    eSCSI_check_CSW_return,
	
// IDLE or END or WAIT next data.
	eSCSI_IDLE,

// Errors. you should initialize USB status.
    eSCSI_ERRORS,
        eSCSI_ERR_dataSize, //data size error
        eSCSI_ERR_signature,//signature error
        eSCSI_ERR_ANY,      //some error (01 cmd err 02 phase error)
        eSCSI_ERR_NOT_INITIALIZED,   //USB isn't initialized
        eSCSI_ERR_END,
        
// when it happened, this code has bug.        
    eSCSI_defaut
};

/*****************************
 * DEFINES
 *****************************/
#define SECTOR_LENGTH512	512
#define setSCSI_NOT_INITIALIZED()   (SCSIcondition.Status = eSCSI_ERR_NOT_INITIALIZED)
#define isSCSI_ERROR()              (SCSIcondition.Status >= eSCSI_ERRORS)


typedef struct __SCSI_CONDITION
{
	enum eSCSI_STATE        Status;
	enum eSCSI_COMMAND		Command;
	enum eSCSI_STATE        ErrorCode;
    UINT32					MscTotal;		// SCSI final sector number
    UINT32					DataLength;		// transmission data length
    UINT8*					UsbBuffAddr;	// transmission data buffer address
} SCSI_CONDITION;

/*****************************
 * VARIABLES
 *****************************/
extern SCSI_CONDITION SCSIcondition;
//extern UINT8 UsbBufDAT512[512];	// Usb buffer for DATA


/*****************************
 * PROTOTYPES
 *****************************/
void SCSI_init(void);
void SCSI_statusControl(void);

void SCSI_requestSense(
UINT8* 	buffAddr	// transmission data buffer address
);

void SCSI_readCapacity(
UINT8* 	buffAddr	// working buffer, which is putted in transmission data.
);

void SCSI_read(
UINT8* 	buffAddr,	// transmission data buffer address
UINT32	sectorNo		// Sector address
);

void SCSI_write(
UINT8* 	buffAddr,	// transmission data buffer address
UINT32	sectorNo		// Sector address
);


#endif
