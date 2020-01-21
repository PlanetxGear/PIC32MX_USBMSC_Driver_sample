/***************************************************************************//**
 * @file vTMR1.c
 * @brief	wait timer for any process.
 *			1000Hz 1msec interval.
 *			use TMR1.
 * @author hiroshi murakami
 * @date	20161225
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vTMR1_C

#include "mcc_generated_files/mcc.h"
#include "vTMR1.h"

//    static volatile
volatile unsigned int uiTMR001;             // used for USB status
volatile unsigned int uiTMR002;             // used for I2C wait of Lcdtask11
volatile unsigned int uiTMR007;             // used for LED Blink

//******************************************************************************
//	Function Name :Intrupt TIMER1
//	Outline		:ƒ^ƒCƒ}1Š„‚èž‚Ýˆ—  1000Hz 1msec interval
//	Input		:
//	Output		:
//******************************************************************************
void TMR1_CallBack(void)
//void Intrupt_TMR1(void)
{
	if (uiTMR001 > 0) --uiTMR001; // 1000Hz decrement timer with zero stopped
	if (uiTMR002 > 0) --uiTMR002; // 1000Hz decrement timer with zero stopped
//	if (uiTMR003 > 0) --uiTMR003; // 1000Hz decrement timer with zero stopped
//	if (uiTMR004 > 0) --uiTMR004; // 1000Hz decrement timer with zero stopped
//	if (uiTMR005 > 0) --uiTMR005; // 1000Hz decrement timer with zero stopped
//	if (uiTMR006 > 0) --uiTMR006; // 1000Hz decrement timer with zero stopped
	if (uiTMR007 > 0) --uiTMR007; // 1000Hz decrement timer with zero stopped
}

void vTMR1_init(void)
{
    uiTMR001 = 0;
    uiTMR002 = 0;
//    uiTMR003 = 0;
//    uiTMR004 = 0;
//    uiTMR005 = 0;
//    uiTMR006 = 0;
    uiTMR007 = 0;
}


