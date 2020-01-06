/***************************************************************************//**
 * @file v.h
 * @brief	include files, debug print, etc...
 * @author hiroshi murakami
 * @date	20170519
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/

#ifndef _V_H    /* Guard against multiple inclusion */
#define _V_H

#define _V_DEBUG		// when it's debugging, define _V_DEBUG_USB1. 
#define _V_DEBUG_USB1		// when it's debugging, define _V_DEBUG_USB1. 
#define _V_DEBUG_SCSI1		// when it's debugging, define _V_DEBUG_SCSI1. 
#define _V_DEBUG_SCSI2		// when it's debugging, define _V_DEBUG_SCSI2. 

#ifdef _V_DEBUG
	#define	DEBUG_PUTS(str1)	xputs(str1)
	#define	DEBUG_PRINTF(fmt1, ...)	xprintf(fmt1, __VA_ARGS__)
#else
	#define	DEBUG_PUTS(str1)
	#define	DEBUG_PRINTF(fmt1, ...)
#endif

#ifdef _V_DEBUG_USB1
	#define	DEBUG_USB1PUTS(str1)	xputs(str1)
	#define	DEBUG_USB1PRINTF(fmt1, ...)	xprintf(fmt1, __VA_ARGS__)
#else
	#define	DEBUG_USB1PUTS(str1)
	#define	DEBUG_USB1PRINTF(fmt1, ...)
#endif

#ifdef _V_DEBUG_SCSI1
	#define	DEBUG_SCSI1PUTS(str1)	xputs(str1)
	#define	DEBUG_SCSI1PRINTF(fmt1, ...)	xprintf(fmt1, __VA_ARGS__)
#else
	#define	DEBUG_SCSI1PUTS(str1)
	#define	DEBUG_SCSI1PRINTF(fmt1, ...)
#endif

#ifdef _V_DEBUG_SCSI2
	#define	DEBUG_SCSI2PUTS(str1)	xputs(str1)
	#define	DEBUG_SCSI2PRINTF(fmt1, ...)	xprintf(fmt1, __VA_ARGS__)
#else
	#define	DEBUG_SCSI2PUTS(str1)
	#define	DEBUG_SCSI2PRINTF(fmt1, ...)
#endif

//#define vFLAG_ON  	1
//#define vFLAG_OFF  	0

#endif

