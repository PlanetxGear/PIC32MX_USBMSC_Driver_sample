/***************************************************************************//**
 * @file vSCSI.c
 * @brief	SCSI COMAND CONTROLLER.
 *			it's designed to work in concert with vUSBMSC.
 * @author hiroshi murakami
 * @date	20190728
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vSCSI_C

#include <string.h>		// for memset

#include "mcc_generated_files/mcc.h"
#include "xprintf.h"

#include "v.h"
#include "vSCSI.h"
#include "vUSBMSC.h"
//#include "vTMR1.h"


// SCSI condition
SCSI_CONDITION SCSIcondition;


// SCSI command
const UINT8 SCSIrequestSense[] = {	// REQUEST SENSE command contents(SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x00, 0x80, 0x00, 0x0c, 0x03,
	0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT8 SCSIreadCapacity[] = {	// READ CAPACITY command contents(SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x80, 0x00, 0x0c, 0x25,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT8 SCSIreadCmd[] = {	// READ command contents (SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x0c, 0x28,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT8 SCSIwriteCmd[] = {	// WRITE command contents (SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x2a,
	0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//xxxconst UINT16 ScsiTable[] =	// SCSI command table
//xxx{(UINT16)SCSIrequestSense, (UINT16)SCSIreadCapacity, (UINT16)SCSIreadCmd, (UINT16)SCSIwriteCmd};// 0,1,2,3

// LOCAL PROTOTYPES
void SCSI_command_go(void);

//******************************************************************************
/**	
 * @brief	SCSI CONTROL initializing .
 * @param[in,out]	SCSIcondition.Status.
 * @details
 * just setup SCSIcondition.Status to IDLE, mean waiting this module.
 *   	
 */
void SCSI_init(void)
{
	setUSB_INITIALIZE();
    while(USBcondition.Status < eUSB_IDLE )
    {
        USBMSC_statusControl();
    }
    if(USBcondition.Status >= eUSB_ERRORS )
    {
        SCSIcondition.Status = eSCSI_ERR_NOT_INITIALIZED;
    }
    else 
    {
        SCSIcondition.Status = eSCSI_IDLE;
    }
    
}

//******************************************************************************
/**	
 * @brief	SCSI transaction return checking.
 * @param[in]	USBcondition.Status     is used in vUSBMSC.c.
 * @param[out]	SCSIcondition.Status    is used this module.
 * @details
 * check the Status of ehe execution result of the USBMSC transaction.
 * if successful, return to the next step status, 
 * otherwise return the Error status.
 *   	
 */
void SCSI_checkTransactionReturn(void)
{
    if (USBcondition.Status == eUSB_IDLE) 
    {
        SCSIcondition.Status++;		//next step
    }
    else if((USBcondition.Status >= eUSB_ERRORS))
    {
        DEBUG_PUTS("SCSI USB error\n");
        SCSIcondition.Status = eSCSI_ERR_ANY;
    }
    else if((USBcondition.Status <= eUSB_SETUP_END))
    {
        DEBUG_PUTS("SCSI/USB setup didn't done.\n");
        SCSIcondition.Status = eSCSI_ERR_NOT_INITIALIZED;
    }

}

//******************************************************************************
/**	
 * @brief	Set sector number into SCSI read/write command contents.
 * @param[in]	secterNumber    is used in vUSBMSC.c.
 * @param[out]	buf             is included SCSI command.
 * @details
 * Set sector number into SCSI command contents.
 * SCSI command sequence is "big endian", so it is necessary to change the order.
 * *pic is "little endian"
 *   	
 */
void SetSector(UINT32 secterNumber, UINT8 buf[])
{
	buf[17] = secterNumber >> 24;	// Set sector number 1
	buf[18] = secterNumber >> 16;	// Set sector number 2
	buf[19] = secterNumber >> 8;	// Set sector number 3
	buf[20] = secterNumber & 255;	// Set sector number 4
}

//******************************************************************************
/**	
 * @brief   check CSW phrase.
 * @return      eSCSI_STATE
 * @param[in]   dataSize    is CSW data size.
 * @param[in]   buf[]       is command buffer, it's included CSW return phrase.
 * @details
 * check CSW return phrase, whether it is correct or not.
 *   	
 */
enum eSCSI_STATE SCSI_isErrInCSW(INT16 dataSize, UINT8 buf[])
{
	if(dataSize != 13)	// Check size first
    {
        DEBUG_PUTS( "SCSI_DATA_SIZE_ERROR\n");
		return eSCSI_ERR_dataSize;
    }
    
	if(buf[0] != 'U' || buf[1] != 'S' || buf[2] != 'B'|| buf[3] != 'S')
    {
        DEBUG_PUTS( "SCSI_SIGNATURE_ERROR\n");
		return eSCSI_ERR_signature;
    }
    
	if(buf[12])	// Some error (01 cmd err 02 phase error) found?
    {
        DEBUG_PUTS( "SCSI_ANY_ERROR\n");
		return eSCSI_ERR_ANY;
    }
    return 0; // Normal return
}

//******************************************************************************
/**	
 * @brief	SCSI transaction status control.
 * @param[in,out]	SCSIcondition.
 * @details	
 * this is main program of SCSI module. it consists of 3 stages.
 * 1.SCSI transaction stage.
 *  2.1.CBW
 *  2.2.getData
 *  2.3.putData
 *  2.4.CSW
 * 2.IDLE stage, it wait commands.
 * 3.Errors, when errors happen the status stops in ERRORs. 
 *   maybe, you should initialize the USB status & SCSI status.
 *   	
 */
void SCSI_statusControl(void)
{
	switch (SCSIcondition.Status) 
    {
	case  	eSCSI_CBW_start :
        DEBUG_SCSI1PUTS("SCSI CBW\n");
        SCSIcondition.ErrorCode = 0;
		BDT_OUT.ADR = (UINT32)UsbBufCMD64;	// the command buffer address to BDT_OUT buffer address.
        BDT_OUT.STAT.UOWN = 1;
        BDT_OUT.count = 31;	// Setup BD to send 31 byte. It must be appropriate data size.
//        BDT_OUT.UOWN_COUNT = 0x801f;	// Setup BD to send 31 byte. It must be appropriate data size. 
		setUSB_DATAOUT();
		SCSIcondition.Status++;		//next step
		break;
	case  	eSCSI_CBW_wait :
        SCSI_checkTransactionReturn();
		break;

	case  	eSCSI_getputBranch :
        if(SCSIcondition.Command == eSCSI_write)
        {
            SCSIcondition.Status = eSCSI_putData_start;
        }
        else
        {
            SCSIcondition.Status = eSCSI_getData_start;
        }
		break;

	case  	eSCSI_getData_start :
        DEBUG_SCSI1PUTS("SCSI getDATA Start\n");
		BDT_IN.ADR = (UINT32)SCSIcondition.UsbBuffAddr;	// the data buffer address to BDT_IN buffer address.
		setUSB_DATAIN();
		SCSIcondition.Status++;		//next step
		break;
	case  	eSCSI_getData_wait :
        SCSI_checkTransactionReturn();
		break;
	case  	eSCSI_getData_next :
        DEBUG_SCSI1PRINTF("SCSI getDATA DataLength:%d, BufAddr:0x%04x \n",(int)SCSIcondition.DataLength ,(int)SCSIcondition.UsbBuffAddr);
		SCSIcondition.DataLength = SCSIcondition.DataLength - 64;
		if (SCSIcondition.DataLength > 0)
		{
			SCSIcondition.UsbBuffAddr = SCSIcondition.UsbBuffAddr + 64;
			SCSIcondition.Status = eSCSI_getData_start;	//next data..
		}
        else
		{
            DEBUG_SCSI1PUTS("SCSI getDATA End\n");
			SCSIcondition.Status = eSCSI_CSW_start;		//exit data getting.
		}
		break;
		
    case  	eSCSI_putData_start :
        DEBUG_SCSI1PUTS("SCSI putDATA Start\n");
		BDT_OUT.ADR = (UINT32)SCSIcondition.UsbBuffAddr;	// the data buffer address to BDT_OUT buffer address.
        BDT_OUT.STAT.UOWN = 1;
        BDT_OUT.count = 64;	// Setup BD to send 64 byte. It must be appropriate data size. 
//        BDT_OUT.UOWN_COUNT = 0x8040;	// Setup BD to send 64 byte. It must be appropriate data size. 
		setUSB_DATAOUT();
		SCSIcondition.Status++;		//next step
		break;
	case  	eSCSI_putData_wait :
        SCSI_checkTransactionReturn();
		break;
	case  	eSCSI_putData_next :
        DEBUG_SCSI1PRINTF("SCSI putDATA. DataLength:%ld, BufAddr:0x%08lx \n",SCSIcondition.DataLength ,SCSIcondition.UsbBuffAddr);
		SCSIcondition.DataLength = SCSIcondition.DataLength - 64;
		if (SCSIcondition.DataLength > 0)
		{
			SCSIcondition.UsbBuffAddr = SCSIcondition.UsbBuffAddr + 64;
			SCSIcondition.Status = eSCSI_putData_start;	//next data..
		}
        else
		{
            DEBUG_SCSI1PUTS("SCSI putDATA End\n");
			SCSIcondition.Status = eSCSI_CSW_start;		//exit data putting.
		}
		break;

	case  	eSCSI_CSW_start :
        DEBUG_SCSI1PUTS("SCSI CSW\n");
		memset(UsbBufCMD64, 0, 64);		// clear the command buffer
		BDT_IN.ADR = (UINT32)UsbBufCMD64;	// the command buffer address to BDT_IN buffer address.
		setUSB_DATAIN();
		SCSIcondition.Status++;		//next step
		break;
	case  	eSCSI_CSW_wait :
        SCSI_checkTransactionReturn();
		break;
	case	eSCSI_check_CSW_return:
        SCSIcondition.ErrorCode = SCSI_isErrInCSW(USBcondition.BDbyteCount, UsbBufCMD64);
        if (SCSIcondition.ErrorCode)	//if error..
        {
            SCSIcondition.Status = SCSIcondition.ErrorCode;
		}
        else
        {
            SCSIcondition.Status = eSCSI_IDLE ;	//Nomal return to Idle..
        }
		
		break;

	////////////////////////////////////////////////////////////////////////////
	case	eSCSI_IDLE:
		SCSIcondition.Command = 0;  // Clear
		break;

	////////////////////////////////////////////////////////////////////////////
	case  	eSCSI_ERRORS:
		break;
	case  	eSCSI_ERR_dataSize:
		break;
	case  	eSCSI_ERR_signature:
		break;
	case  	eSCSI_ERR_ANY:      //some error (01 cmd err 02 phase error)
		break;
	case  	eSCSI_ERR_END:
		break;
 
    default:
		DEBUG_PRINTF( "SCSI_DEFAULT!! status:%d\n", SCSIcondition.Status);
	}

}

//******************************************************************************
/**	
 * @brief   go action SCSI_statusControl & USBMSC_statusControl.
 * @param[in, out]  SCSIcondition.Status        go CBW process
 *   	
 */
void SCSI_command_go(void)
{
    USBcondition.SOFCountEx = USBcondition.SOFCount;    //for wait new SOF
    SCSIcondition.Status = eSCSI_CBW_start;		//go CBW process
    while(SCSIcondition.Status < eSCSI_IDLE )
    {
        USBMSC_statusControl();
        SCSI_statusControl();
    }
}

//******************************************************************************
/**	
 * @brief   execute SCSI request sense command. it examines the rsult of the last instruction.
 * @param[in] buffAddr		working buffer, transmission data buffer address
 * @details
 *   	
 */
void SCSI_requestSense(
UINT8* 	buffAddr	// transmission data buffer address
)
{
    DEBUG_SCSI1PUTS("SCSI requestSense command\n");
    memcpy(UsbBufCMD64, SCSIrequestSense, 31);	// SCSIrequestSense command.
        
    SCSIcondition.DataLength = 64;		//data 64 byte. *over write DataLength.
    SCSIcondition.UsbBuffAddr = buffAddr;   //data buffer address.
    SCSIcondition.Command = eSCSI_requestSense;

    SCSI_command_go();
	
    DEBUG_SCSI2PUTS("SCSI requestSense commandEND\n");
}

//******************************************************************************
/**	
 * @brief   execute SCSI read capacyty command.
 * @param[in]  buffAddr		is working buffer which is putted in transmission data.
 * @details
 * put max secter No. into the SCSIcondition.MscTotal.
 *   	
 */
void SCSI_readCapacity(
UINT8*	buffAddr	// working buffer, transmission data buffer address
)
{
int	i;
    DEBUG_SCSI1PUTS("SCSI readCapacity command\n");
    memcpy(UsbBufCMD64, SCSIreadCapacity, 31);	// SCSI readCapacity command.
        
    SCSIcondition.DataLength = 64;		//data 64 byte. *over write DataLength.
    SCSIcondition.UsbBuffAddr = buffAddr;   //data buffer address.
    SCSIcondition.Command = eSCSI_readCapacity;

    SCSI_command_go();

    DEBUG_SCSI2PUTS("SCSI readCapacity END\n");
    // Device returns 4 byte length capacity information(max sector NO.) & sector length bytes.
    SCSIcondition.MscTotal = 0;
    for(i = 0 ; i < 4; i++)	// Get 4 byte value. max sector NO.
    {
        SCSIcondition.MscTotal = (SCSIcondition.MscTotal << 8) + *buffAddr;	// Get 32bit number
        buffAddr++;
    }
    SCSIcondition.DataLength = 0;
    for(i = 4 ; i < 8; i++)	// Get 4 byte value, one sector length byte. may be 512 bytes.
    {
        SCSIcondition.DataLength = (SCSIcondition.DataLength << 8) + *buffAddr;	// Get 32bit number
        buffAddr++;
    }
    DEBUG_SCSI2PRINTF("LastSecter:0x%08lx DataLength:0x%08lxbyte\n",SCSIcondition.MscTotal, SCSIcondition.DataLength);
	
}

//******************************************************************************
/**	
 * @brief   execute SCSI read command.
 * @param[in]  buffAddr		is buffer Address which is putted in transmission data.
 * @param[in]  sectorNo		is target sectorNo in USB memory.
 * @details
 * got USB data into UsbBuffer.
 *   	
 */
void SCSI_read(
UINT8* 	buffAddr,	// transmission data buffer address
UINT32	sectorNo		// Sector address
)
{
	DEBUG_SCSI1PUTS("SCSI Read command\n");
	DEBUG_SCSI1PRINTF("SOF START:%u\n", USBcondition.SOFCount);
	#ifdef _V_DEBUG_SCSI2
		USBcondition.SOFCountSt = USBcondition.SOFCount;
	#endif
	memcpy(UsbBufCMD64, SCSIreadCmd, 31);	//SCSI read command.
	SetSector(sectorNo, UsbBufCMD64);		//set final sector number to command.
	
    SCSIcondition.DataLength = SECTOR_LENGTH512;             //data 512 byte.
    SCSIcondition.UsbBuffAddr = buffAddr;   //data buffer address.
    SCSIcondition.Command = eSCSI_read;

    SCSI_command_go();
	
    DEBUG_SCSI1PRINTF("SOF END:%u=>%d \n", USBcondition.SOFCount,(USBcondition.SOFCount - USBcondition.SOFCountSt));
    DEBUG_SCSI1PUTS("SCSI Read END\n");
}

//******************************************************************************
/**	
 * @brief   execute SCSI write command.
 * @param[in]  buffAddr		is buffer Address which include transmission data.
 * @param[in]  sectorNo		is target sectorNo in USB memory.
 * @details
 * write to specific sector in USB memory.
 *   	
 */
void SCSI_write(
UINT8* 	buffAddr,	// transmission data buffer address
UINT32	sectorNo		// Sector address
)
{
    DEBUG_SCSI1PUTS("SCSI Write command\n");
    DEBUG_SCSI1PRINTF("SOF START:%u\n", USBcondition.SOFCount);
    #ifdef _V_DEBUG_SCSI2
        USBcondition.SOFCountSt = USBcondition.SOFCount;
    #endif
    memcpy(UsbBufCMD64, SCSIwriteCmd, 31);	//SCSI write command.
	SetSector(sectorNo, UsbBufCMD64);		//set final sector number to command.
	
    SCSIcondition.DataLength = SECTOR_LENGTH512;             //data 512 byte.
    SCSIcondition.UsbBuffAddr = buffAddr;   //data buffer address.

    SCSIcondition.Command = eSCSI_write;

    SCSI_command_go();
	
    DEBUG_SCSI1PRINTF("SOF END:%u=>%d \n", USBcondition.SOFCount,(USBcondition.SOFCount - USBcondition.SOFCountSt));
    DEBUG_SCSI1PUTS("SCSI Read END\n");
}


