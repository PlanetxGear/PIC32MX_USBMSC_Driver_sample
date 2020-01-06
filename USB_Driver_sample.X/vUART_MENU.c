/***************************************************************************//**
 * @file vUART_MENU.c
 * @brief	UART Moniter Menu.
 * @author hiroshi murakami
 * @date	20190813
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vUART_MENU_C

#include "mcc_generated_files/mcc.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "integer.h"
#include "xprintf.h"

#include "v.h"
#include "vUART_CMND.h"
#include "vUART_MENU.h"
#include "vUSBMSC_32.h"
#include "vSCSI_32.h"

enum eUART_MENU eUART_MENU_status;
unsigned char *ucPtr1, *ucPtr2;			// Pointer for Byte data

DWORD AccSize;			/* Work register for fs command */
WORD AccFiles, AccDirs;
BYTE*  pBuff;
volatile UINT uTimer;	/* 1kHz increment timer */

UINT8 UsbBufDAT512[512];	// Usb buffer for DATA

const BYTE bMsg01[] = {
//	"+Input your command!\n"		
	"+(DEL)->Command clear\n"		
	"+(ESC)->exit or clear\n"
	"+Thhmmss->Time setting\n"
	"+ua->initializ USB & set USB Address as 1, Config as 1\n"				
	"+ub->command SCSI inquiry\n"
	"+uc->command SCSI readCapacity & get LastSecter No.\n"
	"+ue->command SCSI requestSense\n"
	"+uf->command SCSI readFormatCapacity & get LastSecter No.\n"
	"+ur->command SCSI read & get data in the LastSecter\n"				
	"+uw <No.>->command SCSI write & write a No. into the LastSecter\n"				
	"?a ALL help.\n"
	"\0"
};

//******************************************************************************
//* initialization
//******************************************************************************
void vUART_MENU_init(void)
{
	eUART_MENU_status = eUART_MENU_prompt ;
}

//******************************************************************************
//* UART Menu status control
//******************************************************************************
void vUART_MENU_control(void)
{
//	BYTE *ptr;
//	ptr = &cCmdBuf[0];

	switch (eUART_MENU_status) 
	{
	case  	eUART_MENU_menu_msg1 :
		xputs((char *)bMsg01);		//start message output
		eUART_MENU_status++;		//next status
		break;

	case  	eUART_MENU_prompt :
		vXputs_prompt();		//output "LF" and ">"
		eUART_MENU_status++;		//next status
		break;

	case  	eUART_MENU_getCMND :
		switch(eUART_CMND_Getc())		//get command strings and input them into command buffer
		{
		case eUART_CMND_Wait:
			break;
		case eUART_CMND_Exec:
			eUART_MENU_status = eUART_MENU_execCommand;
			break;
		case eUART_CMND_Esc:
			eUART_MENU_status = eUART_MENU_menu_msg1;
			break;
		case eUART_CMND_Del:
			eUART_MENU_status = eUART_MENU_prompt;
			break;
		case eUART_CMND_Err:
			eUART_MENU_status = eUART_MENU_prompt;
			break;
		default:		//waiting until finishing command input & ENTER
			break;
		}
		break;

	case  	eUART_MENU_execCommand:		//Command selection
		switch(cCmdBuf[0]) 
		{
		case 'U':		//USB setting command
		case 'u':		//USB setting command
			vUART_commandUSB();
			eUART_MENU_status = eUART_MENU_prompt;
            break;
			
#ifdef _V_DEBUG
		case 'C':								//other Check & Test
		case 'c':								//
			vCommand_CheckAndTest();
			eUART_MENU_status = eUART_MENU_prompt;
			break;
#endif
			
		case '?':	//out put menu
			vCommand_Help();
			eUART_MENU_status = eUART_MENU_prompt;
			break;
		case '\n':	//LF
		case '\0':								//NULL
			eUART_MENU_status = eUART_MENU_prompt;
			break;

		default:								// command error
			eUART_MENU_status = eUART_MENU_commandError;
			break;
		}
		break;

	case  	eUART_MENU_commandError:
		xputs("? Unrecognized command format\n");	
		eUART_MENU_status = eUART_MENU_prompt;
		break;

	default:
		eUART_MENU_status = eUART_MENU_prompt;
	}

}

//******************************************************************************
//* Command USB test
//******************************************************************************
void vUART_commandUSB(void)
{
char*	pCmdbuf;
BYTE*	ptr;
long	num1, p1;
int		i;
long    ofs;
UINT    cnt;
	
    pCmdbuf = (char*)&cCmdBuf[1];
	p1 = 0;
    for (ptr=UsbBufDAT512, i = 0; i < 512; ptr += 1, i += 1)
    {
        *ptr = 0;
    }

	switch(cCmdBuf[1]) {
	case 'A':
	case 'a':
		SCSI_init();
		break;

	case 'B':
	case 'b':
//        SCSI_requestSense(UsbBufDAT512);
        SCSI_inquiry(UsbBufDAT512);
        for (ptr=&UsbBufDAT512[p1], ofs = p1, cnt = 4; cnt; cnt--, ptr += 16, ofs += 16)
            put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);

        // default value for debug
        SCSIcondition.MscTotal = 0x79dfff;
        SCSIcondition.DataLength = 0x200;

		break;

	case 'E':
	case 'e':
        SCSI_requestSense(UsbBufDAT512);
        for (ptr=&UsbBufDAT512[p1], ofs = p1, cnt = 4; cnt; cnt--, ptr += 16, ofs += 16)
            put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);
		break;

	case 'C':
	case 'c':
        SCSI_readCapacity(UsbBufDAT512);
        for (ptr=&UsbBufDAT512[p1], ofs = p1, cnt = 4; cnt; cnt--, ptr += 16, ofs += 16)
            put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);
		break;
        
	case 'F':
	case 'f':
        SCSI_readFormatCapacity(UsbBufDAT512);
        for (ptr=&UsbBufDAT512[p1], ofs = p1, cnt = 4; cnt; cnt--, ptr += 16, ofs += 16)
            put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);
		break;
        
    case 'D' :	/* bd <ofs> - Dump R/W buffer */
    case 'd' :	/* bd <ofs> - Dump R/W buffer */
		pCmdbuf = (char*)&cCmdBuf[3];   //Get a offset
		if (!xatoi(&pCmdbuf, &p1))
		{
			p1 = 0;
		}
        for (ptr=&UsbBufDAT512[p1], ofs = p1, cnt = 32; cnt; cnt--, ptr += 16, ofs += 16)
            put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);
        break;

	case 'R':
	case 'r':
        // read data buffer.
        SCSI_read(UsbBufDAT512, SCSIcondition.MscTotal);
        for (ptr=(BYTE*)UsbBufDAT512, ofs = 0; ofs < 512; ptr += 16, ofs += 16)
        {
            put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);
        }
		break;
		
	case 'S':
	case 's':
        xprintf("SCSIstatus:%d USBstatus:%d \n",SCSIcondition.Status, USBcondition.Status);
		break;
		
	case 'W':
	case 'w':	/* uw <No.> - Fill the last sector with a No. */
		pCmdbuf = (char*)&cCmdBuf[3];   //Get a No.
		if (!xatoi(&pCmdbuf, &num1))
		{
			vXputs_FormatError();
		} else if (SCSIcondition.MscTotal == 0)
        {
            	xputs("You should get Max sector No. by 'UC' command.\n");
        } else
		{
			// set data buffer by input Number
			for (ptr=UsbBufDAT512, i = 0; i < 512; ptr += 1, i += 1)
			{
				*ptr = num1;
			}
            SCSI_write(UsbBufDAT512, SCSIcondition.MscTotal);
		}
		break;
		
	default:
//		xputs("?Format Error.\n");	//“ü—ÍƒGƒ‰[
		vXputs_FormatError();
		break;
	}

}


#ifdef _V_DEBUG
//******************************************************************************
//* Other Check & Test 
//******************************************************************************
void vCommand_CheckAndTest(void)
{
/*
	switch(cCmdBuf[0]) {
	case 'C':
	case 'c':
		if(cCmdBuf[1]=='d'){
			vLCD_menu12_close();
			vLCD_task11_close();
			xprintf( "LCD DisEN\n");
		}else{
			vLCD_task11_open();
			vLCD_menu12_open();
			xprintf( "LCD EN & open\n");
		}
		break;

	default:
		vXputs_FormatError();
		break;
	}
*/
}
#endif


//******************************************************************************
//* Command Help output
//******************************************************************************
void vCommand_Help(void)
{
//	BYTE *ptr;
//	ptr = &cCmdBuf[0];
	switch (cCmdBuf[0]) {
	case '?' :
		switch (cCmdBuf[1]) {
		case 'a' :	// ALL help.
			xputs((char *)bMsg01);
			break;

		default:	// general message
			xputs((char *)bMsg01);
			break;
		}		
		break;
	}
		
}


//******************************************************************************
//* output "command prompt"
//******************************************************************************
void vXputs_prompt(void)
{
	xputs(">");	
}

//******************************************************************************
//* output "Format Error"
//******************************************************************************
void vXputs_FormatError(void)
{
	xputs("? Format Error.\n");	
}

//******************************************************************************
//* output "command Error"
//******************************************************************************
void vXputs_commandError(	char* str)
{
	xputs("? ");
	xputs(str);
	xputs(" Command Error.\n");
}

