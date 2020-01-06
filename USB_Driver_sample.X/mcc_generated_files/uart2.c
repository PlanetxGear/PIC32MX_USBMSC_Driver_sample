/**
  UART2 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart2.c

  @Summary
    This is the generated source file for the UART2 driver using PIC32MX MCUs

  @Description
    This source file provides APIs for driver for UART2. 
    Generation Information : 
        Product Revision  :  PIC32MX MCUs - pic32mx : v1.35
        Device            :  PIC32MX250F128B
        Driver Version    :  0.5
    The generated drivers are tested against the following:
        Compiler          :  XC32 1.42
        MPLAB 	          :  MPLAB X 3.55
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include "uart2.h"

/**
  Section: Data Type Definitions
*/

/** UART Driver Queue Status

  @Summary
    Defines the object required for the status of the queue.
*/

typedef union
{
    struct
    {
            uint8_t full:1;
            uint8_t empty:1;
            uint8_t reserved:6;
    }s;
    uint8_t status;
}

UART_BYTEQ_STATUS;

/** UART Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

*/
typedef struct
{
    /* RX Byte Q */
    uint8_t                                      *rxTail ;

    uint8_t                                      *rxHead ;

    /* TX Byte Q */
    uint8_t                                      *txTail ;

    uint8_t                                      *txHead ;

    UART_BYTEQ_STATUS                        rxStatus ;

    UART_BYTEQ_STATUS                        txStatus ;

} UART_OBJECT ;

static UART_OBJECT uart2_obj ;

/** UART Driver Queue Length

  @Summary
    Defines the length of the Transmit and Receive Buffers

*/

#define UART2_CONFIG_TX_BYTEQ_LENGTH 64
#define UART2_CONFIG_RX_BYTEQ_LENGTH 64


/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

*/

static uint8_t uart2_txByteQ[UART2_CONFIG_TX_BYTEQ_LENGTH] ;
static uint8_t uart2_rxByteQ[UART2_CONFIG_RX_BYTEQ_LENGTH] ;

/** UART Hardware FIFO Buffer Length

  @Summary
    Defines the length of the Transmit and Receive FIFOs
 
*/

#define UART2_TX_FIFO_LENGTH 1
#define UART2_RX_FIFO_LENGTH 1 

/**
  Section: Driver Interface
*/


void UART2_Initialize (void)
{
   // STSEL 1S; IREN disabled; PDSEL 8N; RTSMD disabled; RXINV disabled; SIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; UEN TX_RX; ON enabled; 
   U2MODE = 0x8008;
   // UTXISEL TX_ONE_CHAR; UTXINV disabled; ADDR 0; URXEN disabled; OERR disabled; ADM_EN disabled; URXISEL RX_ONE_CHAR; UTXBRK disabled; UTXEN disabled; ADDEN disabled; 
   U2STA = 0x0;
   // U2TXREG 0; 
   U2TXREG = 0x0;
   // BaudRate = 115200; Frequency = 48000000 Hz; BRG 103; 
   U2BRG = 0x67;

   IEC1bits.U2RXIE = 1;

   U2STAbits.UTXEN = 1;
   U2STAbits.URXEN = 1;

   //Enabling UART
   U2MODEbits.ON = 1;

   uart2_obj.txHead = uart2_txByteQ;
   uart2_obj.txTail = uart2_txByteQ;
   uart2_obj.rxHead = uart2_rxByteQ;
   uart2_obj.rxTail = uart2_rxByteQ;
   uart2_obj.rxStatus.s.empty = true;
   uart2_obj.txStatus.s.empty = true;
   uart2_obj.txStatus.s.full = false;
   uart2_obj.rxStatus.s.full = false;
}




/**
    Maintains the driver's transmitter/receiver/error state machine and implements its ISR
*/
void __ISR(_UART_2_VECTOR, IPL1AUTO) _UART_2 ( void )
{ 
    if(IFS1bits.U2RXIF)
    {
        int count = 0;

        while((count < UART2_RX_FIFO_LENGTH) && (U2STAbits.URXDA == 1))
        {
            count++;

            *uart2_obj.rxTail = U2RXREG;

            uart2_obj.rxTail++;

            if(uart2_obj.rxTail == (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH))
            {
                uart2_obj.rxTail = uart2_rxByteQ;
            }

            uart2_obj.rxStatus.s.empty = false;
        
            if(uart2_obj.rxTail == uart2_obj.rxHead)
            {
                //Sets the flag RX full
                uart2_obj.rxStatus.s.full = true;
                break;
            }
        }
        IFS1CLR= 1 << _IFS1_U2RXIF_POSITION;
    }
    else if (IFS1bits.U2TXIF)
    {
        if(uart2_obj.txStatus.s.empty)
        {
            IEC1bits.U2TXIE = false;
            return;
        }

        IFS1CLR= 1 << _IFS1_U2TXIF_POSITION;

        int count = 0;
        while((count < UART2_TX_FIFO_LENGTH)&& !(U2STAbits.UTXBF == 1))
        {
            count++;

            U2TXREG = *uart2_obj.txHead;

            uart2_obj.txHead++;

            if(uart2_obj.txHead == (uart2_txByteQ + UART2_CONFIG_TX_BYTEQ_LENGTH))
            {
                uart2_obj.txHead = uart2_txByteQ;
            }

            uart2_obj.txStatus.s.full = false;

            if(uart2_obj.txHead == uart2_obj.txTail)
            {
                uart2_obj.txStatus.s.empty = true;
                break;
            }
        }
    }
    else
    {
        if ((U2STAbits.OERR == 1))
        {
            U2STAbits.OERR = 0;
        }

        IFS1CLR= 1 << _IFS1_U2EIF_POSITION;
    }  
}

/**
  Section: UART Driver Client Routines
*/

uint8_t UART2_Read( void)
{
    uint8_t data = 0;

    data = *uart2_obj.rxHead;

    uart2_obj.rxHead++;

    if (uart2_obj.rxHead == (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH))
    {
        uart2_obj.rxHead = uart2_rxByteQ;
    }

    if (uart2_obj.rxHead == uart2_obj.rxTail)
    {
        uart2_obj.rxStatus.s.empty = true;
    }

    uart2_obj.rxStatus.s.full = false;

    return data;
}


unsigned int UART2_ReadBuffer( uint8_t *buffer, const unsigned int bufLen)
{
    unsigned int numBytesRead = 0 ;
    while ( numBytesRead < ( bufLen ))
    {
        if( uart2_obj.rxStatus.s.empty)
        {
            break;
        }
        else
        {
            buffer[numBytesRead++] = UART2_Read () ;
        }
    }

    return numBytesRead ;
}



void UART2_Write( const uint8_t byte)
{
    IEC1bits.U2TXIE = false;
    
    *uart2_obj.txTail = byte;

    uart2_obj.txTail++;
    
    if (uart2_obj.txTail == (uart2_txByteQ + UART2_CONFIG_TX_BYTEQ_LENGTH))
    {
        uart2_obj.txTail = uart2_txByteQ;
    }

    uart2_obj.txStatus.s.empty = false;

    if (uart2_obj.txHead == uart2_obj.txTail)
    {
        uart2_obj.txStatus.s.full = true;
    }

    IEC1bits.U2TXIE = true ;
	
}


unsigned int UART2_WriteBuffer( const uint8_t *buffer , const unsigned int bufLen )
{
    unsigned int numBytesWritten = 0 ;

    while ( numBytesWritten < ( bufLen ))
    {
        if((uart2_obj.txStatus.s.full))
        {
            break;
        }
        else
        {
            UART2_Write (buffer[numBytesWritten++] ) ;
        }
    }

    return numBytesWritten ;

}


UART2_TRANSFER_STATUS UART2_TransferStatusGet (void )
{
    UART2_TRANSFER_STATUS status = 0;

    if(uart2_obj.txStatus.s.full)
    {
        status |= UART2_TRANSFER_STATUS_TX_FULL;
    }

    if(uart2_obj.txStatus.s.empty)
    {
        status |= UART2_TRANSFER_STATUS_TX_EMPTY;
    }

    if(uart2_obj.rxStatus.s.full)
    {
        status |= UART2_TRANSFER_STATUS_RX_FULL;
    }

    if(uart2_obj.rxStatus.s.empty)
    {
        status |= UART2_TRANSFER_STATUS_RX_EMPTY;
    }
    else
    {
        status |= UART2_TRANSFER_STATUS_RX_DATA_PRESENT;
    }
    return status;
}


uint8_t UART2_Peek(uint16_t offset)
{
    if( (uart2_obj.rxHead + offset) > (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH))
    {
      return uart2_rxByteQ[offset - (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH - uart2_obj.rxHead)];
    }
    else
    {
      return *(uart2_obj.rxHead + offset);
    }
}


unsigned int UART2_ReceiveBufferSizeGet(void)
{
    if(!uart2_obj.rxStatus.s.full)
    {
        if(uart2_obj.rxHead > uart2_obj.rxTail)
        {
            return(uart2_obj.rxHead - uart2_obj.rxTail);
        }
        else
        {
            return(UART2_CONFIG_RX_BYTEQ_LENGTH - (uart2_obj.rxTail - uart2_obj.rxHead));
        } 
    }
    return 0;
}


unsigned int UART2_TransmitBufferSizeGet(void)
{
    if(!uart2_obj.txStatus.s.full)
    { 
        if(uart2_obj.txHead > uart2_obj.txTail)
        {
            return(uart2_obj.txHead - uart2_obj.txTail);
        }
        else
        {
            return(UART2_CONFIG_TX_BYTEQ_LENGTH - (uart2_obj.txTail - uart2_obj.txHead));
        }
    }
    return 0;
}


bool UART2_ReceiveBufferIsEmpty (void)
{
    return(uart2_obj.rxStatus.s.empty);
}


bool UART2_TransmitBufferIsFull(void)
{
    return(uart2_obj.txStatus.s.full);
}


UART2_STATUS UART2_StatusGet (void)
{
    return U2STA;
}



/**
  End of File
*/
