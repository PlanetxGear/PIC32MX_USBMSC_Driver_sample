/***************************************************************************//**
 * @file vTMR1.h
 * @brief	wait timer for any process.
 *			1000Hz 1msec interval.
 *			use TMR1.
 * @author hiroshi murakami
 * @date	20161225
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vTMR1_H
#define _vTMR1_H

/*****************************
 * DEFINES
 *****************************/
#define IS_uiTMR001_FINISH (uiTMR001==0)
//#define IS_uiTMR002_FINISH (uiTMR002==0)
//#define IS_uiTMR003_FINISH (uiTMR003==0)
//#define IS_uiTMR004_FINISH (uiTMR004==0)
//#define IS_uiTMR005_FINISH (uiTMR005==0)
//#define IS_uiTMR006_FINISH (uiTMR006==0)
#define IS_uiTMR007_FINISH (uiTMR007==0)

/*****************************
 * VARIABLES
 *****************************/
#if defined _vTMR1_C
#else
volatile extern unsigned int
		uiTMR001,
//		uiTMR002,
//		uiTMR003,
//		uiTMR004,
//		uiTMR005,
//		uiTMR006,
		uiTMR007;
#endif

/*****************************
 * PROTOTYPES
 *****************************/
void vTMR1_init(void);

void TMR1_CallBack(void);
//void Intrupt_TMR1(void);    //decrease timer parameter step by 1msec.

#endif