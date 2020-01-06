/***************************************************************************//**
 * @file vFIFO.h
 * @brief	FIFO buffer for serial data
 * @author hiroshi murakami
 * @date	20160827
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vFIFO_H
#define _vFIFO_H

#include "integer.h"

/*****************************
 * VARIABLES
 *****************************/
typedef enum {
	eFIFO_bufferStored = 0,
	eFIFO_bufferEmpty,
	eFIFO_busy,
	eFIFO_bufferFull
} FIFO_STATUS;

typedef volatile struct {
	int     readIndex, writeIndex, dataCounter;             // Read index, Write index, Data counter
	int     buffer_size;            // buffer_size
	BYTE    *buffer; // FIFO buffer
	FIFO_STATUS     status;         // status -1:empty, -9:full
    
} FIFO_STRUCT;

/*****************************
 * PROTOTYPES
 *****************************/
void vFIFO_init (FIFO_STRUCT* Fifo, int buffer_size, BYTE* buffer);
FIFO_STATUS	eFIFO_putc (FIFO_STRUCT* Fifo, BYTE d);    // Put a byte into FIFO. *return a status
BYTE bFIFO_getc (FIFO_STRUCT* Fifo);            // Get a byte from FIFO. *return a data from buffer
#define iFIFO_remains(Fifop)  ((Fifop)->dataCounter)      // return remaining bytes of data existing of FIFO. if zero there are no data.
#define iFIFO_full(Fifop)  ((Fifop)->dataCounter == (Fifop)->buffer_size)
#define iFIFO_space(Fifop)  ((Fifop)->buffer_size - (Fifop)->dataCounter)

#endif

