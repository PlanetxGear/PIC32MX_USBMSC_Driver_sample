/***************************************************************************//**
 * @file vFIFO.c
 * @brief	FIFO buffer for serial data
 * @author hiroshi murakami
 * @date	20160827
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vFIFO_C

#include "mcc_generated_files/mcc.h"
#include "string.h"
#include "vFIFO.h"

//******************************************************************************
// Get a byte from FIFO *return a data from buffer
//******************************************************************************
BYTE bFIFO_getc (
	FIFO_STRUCT* Fifo
)
{
   	BYTE data;
	int index;

	Fifo->status = eFIFO_bufferStored;
	if (!Fifo->dataCounter) {						//when FIFO buffer empty */
		Fifo->status = eFIFO_bufferEmpty;
		return 0x00;
	}

	///////////////////////////////////////////////////////////////////
	//IEC0bits.T3IE = 0;						//for wave Interrupt_TMR3
	///////////////////////////////////////////////////////////////////
	
	index = Fifo->readIndex;							// Get a byte from FIFO
	data = Fifo->buffer[index++];
	Fifo->dataCounter--;
	Fifo->readIndex = index % Fifo->buffer_size;
	
	///////////////////////////////////////////////////////////////////
	//IEC0bits.T3IE = 1;						//for wave Interrupt_TMR3
	///////////////////////////////////////////////////////////////////

	return data;								//return a data
}

//******************************************************************************
// Put a byte into FIFO *return a status
//******************************************************************************
FIFO_STATUS eFIFO_putc (
	FIFO_STRUCT* Fifo,
	BYTE d
)
{
	int index;

	if (iFIFO_full(Fifo)) {					//  Return when FIFO is full
		Fifo->status = eFIFO_bufferFull;
		return  eFIFO_bufferFull;
	}

	Fifo->status = 0;
	index = Fifo->writeIndex;							// Put a data into the FIFO
	Fifo->buffer[index++] = d;
	Fifo->dataCounter++;
	Fifo->writeIndex = index % Fifo->buffer_size;
	return  eFIFO_bufferStored;

}

//******************************************************************************
// Initialize FIFO instance
//******************************************************************************
void vFIFO_init (
	FIFO_STRUCT* Fifo,
	int buffer_size,
	BYTE* buffer
)
{
	/* Clear FIFOs */
	Fifo->readIndex = 0;
	Fifo->writeIndex = 0;
	Fifo->dataCounter = 0;
	Fifo->status = eFIFO_bufferEmpty;
	Fifo->buffer_size = buffer_size;
	Fifo->buffer = buffer;
	memset( (Fifo->buffer) , 0x00 , Fifo->buffer_size );
}


