/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   uart_regs.h                                                              */
/*            This file contains definitions of UART registers                */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _UART_REGS_H
#define _UART_REGS_H


#include __CHIP_H_FROM_DRV()


/**************************************************************************************************************************/
/*   Receive Buffer Register (RBR)                                                                                        */
/**************************************************************************************************************************/
#define  RBR(n)                            (UART_BASE_ADDR(n) + 0x0000) , UART_ACCESS, 8        /* Offset: UART_BA + 0000h */
#define  RBR_Transmit                    0 , 8               /* Transmit Holding Register (THR)                                                                                       */

/**************************************************************************************************************************/
/*   Transmit Holding Register (THR)                                                                                      */
/**************************************************************************************************************************/
#define  THR(n)                            (UART_BASE_ADDR(n) + 0x0000) , UART_ACCESS, 8        /* Offset: UART_BA + 0000h */
#define  THR_through                     0 , 8               /* through the SOUT pin (LSB first).                                                                                     */

/**************************************************************************************************************************/
/*   Interrupt Enable Register (IER)                                                                                      */
/**************************************************************************************************************************/
#define  IER(n)                            (UART_BASE_ADDR(n) + 0x0004) , UART_ACCESS, 8        /* Offset: UART_BA + 0004h */
#define  IER_nDBGACK_EN                  4 , 1               /* 4 nDBGACK_EN (ICE Debug Mode Acknowledge Enable).                                                                     */
#define  IER_MSIE                        3 , 1               /* 3 MSIE (Modem Status Interrupt (Irpt_MOS) Enable).                                                                    */
#define  IER_RLSIE                       2 , 1               /* 2 RLSIE (Receive Line Status Interrupt (Irpt_RLS) Enable).                                                            */
#define  IER_THREIE                      1 , 1               /* 1 THREIE (Transmit Holding Register Empty Interrupt (Irpt_THRE) Enable).                                              */
#define  IER_RDAIE                       0 , 1               /* 0 RDAIE (Receive Data Available Interrupt (Irpt_RDA) Enable and Timeout Interrupt (Irpt_TOUT) Enable).                */

/**************************************************************************************************************************/
/*   Divisor Latch (Low Byte) Register (DLL)                                                                              */
/**************************************************************************************************************************/
#define  DLL(n)                        (UART_BASE_ADDR(n) + 0x0000) , UART_ACCESS, 8        /* Offset: UART_BA + 0000h */

/**************************************************************************************************************************/
/*   Divisor Latch (High Byte) Register (DLM)                                                                             */
/**************************************************************************************************************************/
#define  DLM(n)                       (UART_BASE_ADDR(n) + 0x0004) , UART_ACCESS, 8         /* Offset: UART_BA + 0004h */
#define  DLM_Baud                   0 , 8                    /* 7-0 Baud Rate Divisor (High Byte). The high byte of the baud rate divisor.                                            */

/**************************************************************************************************************************/
/*   Interrupt Identification Register (IIR)                                                                              */
/**************************************************************************************************************************/
#define  IIR(n)                            (UART_BASE_ADDR(n) + 0x0008) , UART_ACCESS, 8        /* Offset: UART_BA + 0008h */
#define  IIR_FMES                        7 , 1               /* 7 FMES (FIFO Mode Enable Status). Indicates whether FIFO mode is enabled or not. Since FIFO mode is                   */
#define  IIR_RFTLS                       5 , 2               /* 6-5 RFTLS (RxFIFO Threshold Level Status). Shows the current setting of the receiver FIFO threshold level             */
#define  IIR_DMS                         4 , 1               /* 4 DMS (DMA Mode Select). The DMA function is not implemented in this version. When reading IIR, the DMS               */
#define  IIR_IID                         1 , 3               /* 3-1 IID (Interrupt Identification). IID together with NIP indicates the current interrupt request from the UART.      */
#define  IIR_NIP                         0 , 1               /* 0 NIP (No Interrupt Pending).                                                                                         */

/**************************************************************************************************************************/
/*   FIFO Control Register (FCR)                                                                                          */
/**************************************************************************************************************************/
#define  FCR(n)                            (UART_BASE_ADDR(n) + 0x0008) , UART_ACCESS, 8        /* Offset: UART_BA + 0008h */
#define  FCR_RFITL                       4 , 4               /* 7-4 RFITL (RxFIFO Interrupt (Irpt_RDA) Trigger Level).                                                                */
#define  FCR_DMS                         3 , 1               /* 3 DMS (DMA Mode Select). The DMA function is not implemented in this version.                                         */
#define  FCR_TFR                         2 , 1               /* 2 TFR (TxFIFO Reset). Setting this bit generates a reset to the TxFIFO. The TxFIFO becomes empty (Tx pointer          */
#define  FCR_RFR                         1 , 1               /* 1 RFR (RxFIFO Reset). Setting this bit generates an OSC cycle reset pulse to reset the RxFIFO. The RxFIFO             */
#define  FCR_FME                         0 , 1               /* 0 FME (FIFO Mode Enable). The UART always operates in FIFO mode; therefore, writing this bit has no effect            */

/**************************************************************************************************************************/
/*   Line Control Register (LCR)                                                                                          */
/**************************************************************************************************************************/
#define  LCR(n)                            (UART_BASE_ADDR(n) + 0x000C) , UART_ACCESS, 8        /* Offset: UART_BA + 000Ch */
#define  LCR_DLAB                        7 , 1               /* 7 DLAB (Divisor Latch Access Bit).                                                                                    */
#define  LCR_BCB                         6 , 1               /* 6 BCB (Break Control Bit). When this bit is set to logic 1, the serial data output (SOUT) is forced to the Spacing    */
#define  LCR_SPE                         5 , 1               /* 5 SPE (Stick Parity Enable).                                                                                          */
#define  LCR_EPE                         4 , 1               /* 4 EPE (Even Parity Enable).                                                                                           */
#define  LCR_PBE                         3 , 1               /* 3 PBE (Parity Bit Enable).                                                                                            */
#define  LCR_NSB                         2 , 1               /* 2 NSB (Number of "STOP Bits").                                                                                        */
#define  LCR_WLS                         0 , 2               /* 1-0 WLS (Word Length Select).                                                                                         */
#define  LCR_Word                        10 , 1              /* 10 Word Length                                                                                                        */

/**************************************************************************************************************************/
/*   Modem Control Register (MCR)                                                                                         */
/**************************************************************************************************************************/
#define  MCR(n)                            (UART_BASE_ADDR(n) + 0x0010) , UART_ACCESS, 8        /* Offset: UART_BA + 0010h */
#define  MCR_LBME                        4 , 1               /* 4 LBME (Loopback Mode Enable).                                                                                        */
#define  MCR_OUT2                        3 , 1               /* 3 OUT2. Used in loopback mode to drive DCD input.                                                                     */
#define  MCR_RTS                         1 , 1               /* 1 RTS (Request to Send Signal). Complement version of Request to Send (RTS) signal.                                   */
#define  MCR_DTR                         0 , 1               /* 0 DTR (Data Terminal Ready Signal). Complement version of Data Terminal Ready (DTR) signal.                           */

/**************************************************************************************************************************/
/*   Line Status Control Register (LSR)                                                                                   */
/**************************************************************************************************************************/
#define  LSR(n)                            (UART_BASE_ADDR(n) + 0x0014) , UART_ACCESS, 8        /* Offset: UART_BA + 0014h */
#define  LSR_ERR_Rx                      7 , 1               /* 7 ERR_Rx (RxFIFO Error).                                                                                              */
#define  LSR_TE                          6 , 1               /* 6 TE (Transmitter Empty).                                                                                             */
#define  LSR_THRE                        5 , 1               /* 5 THRE (Transmitter Holding Register Empty).                                                                          */
#define  LSR_BII                         4 , 1               /* 4 BII (Break Interrupt Indicator). Is set to a logic 1 when the received data input is held in the "spacing state"    */
#define  LSR_FEI                         3 , 1               /* 3 FEI (Framing Error Indicator). Is set to logic 1 when the received character does not have a valid "stop bit"       */
#define  LSR_PEI                         2 , 1               /* 2 PEI (Parity Error Indicator). This bit is set to logic 1 when the received character does not have a valid "parity  */
#define  LSR_OEI                         1 , 1               /* 1 OEI (Overrun Error Indicator). An overrun error occurs only after the RxFIFO is full and the next character has     */
#define  LSR_RFDR                        0 , 1               /* 0 RFDR (RxFIFO Data Ready).                                                                                           */

/**************************************************************************************************************************/
/*   Modem Status Register (MSR)                                                                                          */
/**************************************************************************************************************************/
#define  MSR(n)                            (UART_BASE_ADDR(n) + 0x0018) , UART_ACCESS, 8        /* Offset: UART_BA + 0018h */
#define  MSR_DCD                         7 , 1               /* 7 DCD. (Data Carrier Detect). Complement version of Data Carrier Detect (DCD) input.                                  */
#define  MSR_RI                          6 , 1               /* 6 RI. (Ring Indicator) Complement version of Ring Indicator (RI) input.                                               */
#define  MSR_DSR                         5 , 1               /* 5 DSR (Data Set Ready). Complement version of Data Set Ready (DSR) input.                                             */
#define  MSR_CTS                         4 , 1               /* 4 CTS (Clear to Send). Complement version of Clear To Send (CTS) input).                                              */
#define  MSR_DDCD                        3 , 1               /* 3 DDCD (DCD State Change). Is set when DCD input changes state; it is reset if the CPU reads the MSR. When            */
#define  MSR_DRI                         2 , 1               /* 2 DRI (RI State Change). It is set when RI input changes state to asserted; it is reset if the CPU reads the MSR.     */
#define  MSR_DDSR                        1 , 1               /* 1 DDSR (DSR State Change). It is set when DSR input changes state; it is reset if the CPU reads the MSR.              */
#define  MSR_DCTS                        0 , 1               /* 0 DCTS (CTS State Change). It is set when CTS input changes state; it is reset if the CPU reads the MSR.              */

/**************************************************************************************************************************/
/*   Timeout Register (TOR)                                                                                               */
/**************************************************************************************************************************/
#define  TOR(n)                            (UART_BASE_ADDR(n) + 0x001C) , UART_ACCESS, 8        /* Offset: UART_BA + 001Ch */
#define  TOR_TOIE                        7 , 1               /* 7 TOIE (Timeout Interrupt Enable). Enabled only when this bit is set and IER register bit 0 is set.                   */
#define  TOR_TOIC                        0 , 7               /* 6-0 TOIC (Timeout Interrupt Comparator). The timeout counter resets and starts counting (the counting clock =         */



enum FCR_RFITL_type
{
    FCR_RFITL_1B    = 0x0,
    FCR_RFITL_4B    = 0x4,
    FCR_RFITL_8B    = 0x8,
    FCR_RFITL_14B   = 0xC,
};



enum LCR_WLS_type
{
    LCR_WLS_5bit    = 0x0,
    LCR_WLS_6bit    = 0x1,
    LCR_WLS_7bit    = 0x2,
    LCR_WLS_8bit    = 0x3,
};


enum IIR_IID_type
{
  IIR_IID_MODEM = 0x0,
  IIR_IID_THRE  = 0x1,
  IIR_IID_TOUT  = 0x5,
  IIR_IID_RDA   = 0x2,
  IIR_IID_RLS   = 0x3,
};


#endif // _UART_REGS_H
