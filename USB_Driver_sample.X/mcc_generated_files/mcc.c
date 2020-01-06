/**
  @Generated PIC32MX MCUs Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    mcc.c

  @Summary:
    This is the mcc.c file generated using PIC32MX MCUs

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC32MX MCUs - pic32mx : v1.35
        Device            :  PIC32MX250F128B
        Driver Version    :  1.02
    The generated drivers are tested against the following:
        Compiler          :  XC32 1.42
        MPLAB             :  MPLAB X 3.55
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

// Configuration bits: selected in the GUI

// DEVCFG3
#pragma config PMDL1WAY = ON    // Peripheral Module Disable Configuration->Allow only one reconfiguration
#pragma config IOL1WAY = ON    // Peripheral Pin Select Configuration->Allow only one reconfiguration
#pragma config FUSBIDIO = OFF    // USB USID Selection->Controlled by Port Function
#pragma config FVBUSONIO = OFF    // USB VBUS ON Selection->Controlled by Port Function

// DEVCFG2
#pragma config FPLLIDIV = DIV_1    // PLL Input Divider->1x Divider
#pragma config FPLLMUL = MUL_24    // PLL Multiplier->24x Multiplier
#pragma config UPLLIDIV = DIV_1    // USB PLL Input Divider->1x Divider
#pragma config UPLLEN = ON    // USB PLL Enable->Enabled
#pragma config FPLLODIV = DIV_2    // System PLL Output Clock Divider->PLL Divide by 2

// DEVCFG1
#pragma config FNOSC = PRIPLL    // Oscillator Selection Bits->Primary Osc w/PLL (XT+,HS+,EC+PLL)
#pragma config FSOSCEN = ON    // Secondary Oscillator Enable->Enabled
#pragma config IESO = OFF    // Internal/External Switch Over->Disabled
#pragma config POSCMOD = XT    // Primary Oscillator Configuration->XT osc mode
#pragma config OSCIOFNC = OFF    // CLKO Output Signal Active on the OSCO Pin->Disabled
#pragma config FPBDIV = DIV_1    // Peripheral Clock Divisor->Pb_Clk is Sys_Clk/1
#pragma config FCKSM = CSDCMD    // Clock Switching and Monitor Selection->Clock Switch Disable, FSCM Disabled
#pragma config WDTPS = PS1048576    // Watchdog Timer Postscaler->1:1048576
#pragma config WINDIS = OFF    // Watchdog Timer Window Enable->Watchdog Timer is in Non-Window Mode
#pragma config FWDTEN = OFF    // Watchdog Timer Enable->WDT Disabled (SWDTEN Bit Controls)
#pragma config FWDTWINSZ = WINSZ_25    // Watchdog Timer Window Size->Window Size is 25%

// DEVCFG0
#pragma config DEBUG = OFF    // Background Debugger Enable->Debugger is Disabled
#pragma config JTAGEN = OFF    // JTAG Enable->JTAG Disabled
#pragma config ICESEL = ICS_PGx1    // ICE/ICD Comm Channel Select->Communicate on PGEC1/PGED1
#pragma config PWP = OFF    // Program Flash Write Protect->Disable
#pragma config BWP = OFF    // Boot Flash Write Protect bit->Protection Disabled
#pragma config CP = OFF    // Code Protect->Protection Disabled

#include "mcc.h"

/**
  @Summary
    Indicates the exception cause. 

  @Description
    This array identifies the cause for exception.
 */

static char *cause[] = 
{
    "Interrupt", "Undefined", "Undefined", "Undefined",
    "Load/fetch address error", "Store address error",
    "Instruction bus error", "Data bus error", "Syscall", 
    "Breakpoint", "Reserved instruction", "Coprocessor unusable", 
    "Arithmetic overflow", "Trap", "Reserved", "Reserved", 
    "Reserved", "Reserved", "Reserved"
};

void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    UART2_Initialize();
    TMR1_Initialize();
    INTERRUPT_Initialize();

}

void SYSTEM_RegUnlock(void)
{
    SYSKEY = 0x12345678;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
}

void SYSTEM_RegLock(void)
{
    SYSKEY = 0x00000000; 
}

void OSCILLATOR_Initialize(void)
{
    SYSTEM_RegUnlock(); 
    // CF no clock failure; COSC PRIPLL; PLLODIV DIV_2; PBDIVRDY disabled; SLOCK in lock; FRCDIV FRC/1; SLPEN Idle on WAIT instruction; NOSC PRIPLL; PLLMULT MUL_24; SOSCEN disabled; PBDIV DIV_1; CLKLOCK locked; OSWEN Switch is Complete; SOSCRDY disabled; 
    OSCCON = 0x80733A0;
    SYSTEM_RegLock();
    // TUN Center Frequency; 
    OSCTUN = 0x0;
    // DIVSWEN disabled; RSLP disabled; ACTIVE Active; ROSEL SYSCLK; OE Not Driven out on REFCLKO pin; SIDL disabled; RODIV 0; ON disabled; 
    REFOCON = 0x100;
    // ROTRIM 0; 
    REFOTRIM = 0x0;
}

void _general_exception_handler ()
{
    /* Mask off the ExcCode Field from the Cause Register
    Refer to the MIPs Software User's manual */
    uint8_t _excep_code;
    uint8_t _excep_addr;
    uint8_t *_cause_str;
    _excep_code = (_CP0_GET_CAUSE() & 0x0000007C) >> 2;
    _excep_addr = _CP0_GET_EPC();
    _cause_str  = cause[_excep_code];

    while(1)
    {
      ;
    }
}

/**
 End of File
*/