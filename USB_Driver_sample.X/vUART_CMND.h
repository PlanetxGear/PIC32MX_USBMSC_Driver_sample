/***************************************************************************//**
 * @file vUART_CMND.h
 * @brief	serial COMMAND input.
 *			use UART2
 * @author hiroshi murakami
 * @date	20161225
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vUART_CMND_H
#define _vUART_CMND_H

#include "vFIFO.h"

/*****************************
 * VARIABLES
 *****************************/

enum eUART_CMND {
	eUART_CMND_Wait = 0,
	eUART_CMND_Exec,
	eUART_CMND_Esc,
	eUART_CMND_Del,
	eUART_CMND_END,
	eUART_CMND_Err
}	;

#if defined _vUART_CMND_C
#else
	extern unsigned char cCmdBuf[];
#endif

/*****************************
 * PROTOTYPES
 *****************************/

void vUART_CMND_init(void);				//
enum eUART_CMND eUART_CMND_Getc(void);	//get command and store to command buffer. 

void vPutc_to_TxFifo (unsigned char d);			//for xprintf
void vUART_TxPutc(void);					//FIFO byte data stream output to UART TX


#endif
