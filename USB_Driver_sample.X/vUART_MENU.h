/***************************************************************************//**
 * @file vUART_MENU.h
 * @brief	UART Moniter Menu.
 * @author hiroshi murakami
 * @date	20190813
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vUART_MENU_H
#define _vUART_MENU_H

/*****************************
 * VARIABLES
 *****************************/
enum eUART_MENU
{
	eUART_MENU_menu_msg1,					//Start message
	eUART_MENU_prompt,
	eUART_MENU_getCMND,
	eUART_MENU_execCommand,					//command branching
	eUART_MENU_commandError,
	eUART_MENU_end
};

#if defined _vUART_MENU_C
#else
	extern enum eUART_MENU eUART_MENU_status;
#endif


/*****************************
 * PROTOTYPES
 *****************************/
void vUART_MENU_init(void);
void vUART_MENU_control(void);		//serial menu control.

void vCommand_Help(void);

void vXputs_prompt(void);
void vXputs_commandError(char* str);
void vXputs_FatFsCommandError(void);
void vXputs_FormatError(void);

void vCommand_CheckAndTest(void);
void vUART_commandUSB(void);


#endif
