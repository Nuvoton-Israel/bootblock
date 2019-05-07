/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   uart_if.h                                                                */
/*            This file contains interface of UART driver                     */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _UART_IF_H
#define _UART_IF_H

#include "hal.h"
//#include "../../Common/hal_error.h"


/*---------------------------------------------------------------------------------------------------------*/
/* UART ports definition                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    UART0_DEV = 0,  // UART0 is a general UART block without modem-I/O-control connection to external signals.
    UART1_DEV = 1,  // UART1-3 are each a general UART with modem-I/O-control connection to external signals.
    UART2_DEV = 2,
    UART3_DEV = 3
} UART_DEV_T;

#if defined NPCM650
/*---------------------------------------------------------------------------------------------------------*/
/* Uart Mux modes definitions                                                                              */
/*---------------------------------------------------------------------------------------------------------*/

typedef enum
{
    UART_MUX_CORE_SNOOP        = 0,
    UART_MUX_CORE_TAKEOVER     = 1,
    UART_MUX_CORE_SP2__SP1_SI1 = 2,
    UART_MUX_CORE_SP2__SP1_SI2 = 3,
    UART_MUX_SKIP_CONFIG       = 4,
} UART_MUX_T;

#elif (defined NPCM750 || defined NPCM750_CP || defined NPCM850 )

/*---------------------------------------------------------------------------------------------------------*/
/* Uart Mux modes definitions. These numbers match the register field value. Do not change !               */
/* s == snoop                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum UART_MUX_tag
{
    UART_MUX_MODE1_HSP1_SI2____HSP2_UART2__UART1_s_HSP1__UART3_s_SI2                = 0,       // 0 0 0: Mode 1 - HSP1 connected to SI2  , HSP2 connected to UART2 ,UART1 snoops HSP1, UART3 snoops SI2
    UART_MUX_MODE2_HSP1_UART1__HSP2_SI2____UART2_s_HSP2__UART3_s_SI2                = 1,       // 0 0 1: Mode 2 - HSP1 connected to UART1, HSP2 connected to SI2   ,UART2 snoops HSP2, UART3 snoops SI2
    UART_MUX_MODE3_HSP1_UART1__HSP2_UART2__UART3_SI2                                = 2,       // 0 1 0: Mode 3 - HSP1 connected to UART1, HSP2 connected to UART2 ,UART3 connected to SI2
    UART_MUX_MODE4_HSP1_SI1____HSP2_SI2____UART1_s_SI1___UART3_s_SI2__UART2_s_HSP1  = 3,       // 0 1 1: Mode 4 - HSP1 connected to SI1  , HSP2 connected to SI2   ,UART1 snoops SI1,  UART3 snoops SI2,   UART2 snoops HSP1 (default)
    UART_MUX_MODE5_HSP1_SI1____HSP2_UART2__UART1_s_HSP1__UART3_s_SI1                = 4,       // 1 0 0: Mode 5 - HSP1 connected to SI1  , HSP2 connected to UART2 ,UART1 snoops HSP1, UART3 snoops SI1
    UART_MUX_MODE6_HSP1_SI1____HSP2_SI2____UART1_s_SI1___UART3_s_SI2__UART2_s_HSP2  = 5,       // 1 0 1: Mode 6 - HSP1 connected to SI1  , HSP2 connected to SI2   ,UART1 snoops SI1,  UART3 snoops SI2,   UART2 snoops HSP2
    UART_MUX_MODE7_HSP1_SI1____HSP2_UART2__UART1_s_HSP1__UART3_SI2                  = 6,       // 1 1 0: Mode 7 - HSP1 connected to SI1  , HSP2 connected to UART2 ,UART1 snoops HSP1, UART3 connected to SI2
    UART_MUX_RESERVED                                                               = 7,       // skip uart mode configuration.
    UART_MUX_SKIP_CONFIG                                                            = 8        // this is a SW option to allow config of UART without touching the UART mux.
} UART_MUX_T;

#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Common baudrate definitions                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    UART_BAUDRATE_110       = 110,
    UART_BAUDRATE_300       = 300,
    UART_BAUDRATE_600       = 600,
    UART_BAUDRATE_1200      = 1200,
    UART_BAUDRATE_2400      = 2400,
    UART_BAUDRATE_4800      = 4800,
    UART_BAUDRATE_9600      = 9600,
    UART_BAUDRATE_14400     = 14400,
    UART_BAUDRATE_19200     = 19200,
    UART_BAUDRATE_38400     = 38400,
    UART_BAUDRATE_57600     = 57600,
    UART_BAUDRATE_115200    = 115200,
    UART_BAUDRATE_230400    = 230400,
    UART_BAUDRATE_380400    = 380400,
    UART_BAUDRATE_460800    = 460800,
} UART_BAUDRATE_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Uart Rx Fifo Trigger level definitions                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    UART_RXFIFO_TRIGGER_1B      = 0x0,
    UART_RXFIFO_TRIGGER_4B      = 0x1,
    UART_RXFIFO_TRIGGER_8B      = 0x2,
    UART_RXFIFO_TRIGGER_14B     = 0x3,
} UART_RXFIFO_TRIGGER_T;


/*---------------------------------------------------------------------------------------------------------*/
/* UART parity types                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    UART_PARITY_NONE    = 0x00,
    UART_PARITY_EVEN    = 0x01,
    UART_PARITY_ODD     = 0x02,
} UART_PARITY_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Uart stop bits                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    UART_STOPBIT_1          = 0x00,
    UART_STOPBIT_DYNAMIC    = 0x01,
} UART_STOPBIT_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Callback functions for UART IRQ handler                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*UART_irq_callback_t)(UINT8 devNum, void *args);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_Init                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs UART initialization                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_Init (UART_DEV_T devNum, UART_MUX_T muxMode, UART_BAUDRATE_T baudRate);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PutC                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART                                                 */
/*                  Note that the function is blocking till char can be send                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_PutC (UART_DEV_T devNum, const UINT8 c );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_GetC                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads char from UART                                                      */
/*                  Note that the function is blocking till char is available                              */
/*---------------------------------------------------------------------------------------------------------*/
 UINT8 UART_GetC (UART_DEV_T devNum );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PutC_NB                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK if the char was written or error if it couldn't be written              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART in NON-Blocking manner                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_PutC_NB (UART_DEV_T devNum, const UINT8 c );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_GetC_NB                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         DEFS_STATUS_OK if char was read or error if it no char was available                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads char from UART in NON-Blocking manner                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_GetC_NB ( UART_DEV_T devNum, UINT8* c );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_TestRX                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine test if there is a char in RX fifo                                        */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN UART_TestRX ( UART_DEV_T devNum );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_TestTX                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine test if there is a char in TX fifo                                        */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN UART_TestTX ( UART_DEV_T devNum );



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_ResetFIFOs                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  rxFifo - if TRUE RX fifo is reseted                                                    */
/*                  txFifo - if TRUE TX fifo is reseted                                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs FIFO reset                                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_ResetFIFOs (UART_DEV_T devNum, BOOLEAN txFifo, BOOLEAN rxFifo);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetTxIrqState                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  On -                                                                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables Tx Interrupt                                             */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetTxIrqState (UART_DEV_T devNum, BOOLEAN On);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetRxIrqState                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  On -                                                                                   */
/*                  timeout -                                                                              */
/*                  triggerLevel -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Rx interrupt enable/disable                                      */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetRxIrqState (UART_DEV_T devNum, BOOLEAN On);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetRxConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timeout -                                                                              */
/*                  triggerLevel -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Rx irq configurations                                            */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetRxConfig (UART_DEV_T devNum, UINT8 timeout, UART_RXFIFO_TRIGGER_T triggerLevel);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetParity                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  parity -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets parity configuration                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetParity (UART_DEV_T devNum, UART_PARITY_T parity);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBitsPerChar                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  bits -                                                                                 */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set bits per char                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetBitsPerChar (UART_DEV_T devNum, UINT32 bits);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBaudrate                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  baudrate -                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets new baudrate                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetBaudrate (UART_DEV_T devNum, UART_BAUDRATE_T baudrate);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetStopBit                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  stopbit -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets number of stopbits                                                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetStopBit (UART_DEV_T devNum, UART_STOPBIT_T stopbit);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBreak                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets break on the given UART                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetBreak (UART_DEV_T devNum, BOOLEAN state);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_Isr                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  rxCallback -                                                                           */
/*                  rxParam -                                                                              */
/*                  txCallback -                                                                           */
/*                  txParam -                                                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs IRQ handling                                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_Isr (UART_DEV_T devNum,  UART_irq_callback_t rxCallback, void* rxParam,
                                          UART_irq_callback_t txCallback, void* txParam);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART_PrintRegs (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PrintModuleRegs                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - module to be printed.                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module instance registers                                      */
/*---------------------------------------------------------------------------------------------------------*/
void UART_PrintModuleRegs (UART_DEV_T devNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PrintVersion                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void UART_PrintVersion (void);

#endif //_UART_DRV_H

