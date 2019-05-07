/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   tmc_regs.h                                                               */
/*            This file contains TMC module registers                         */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _TMC_REGS_H
#define _TMC_REGS_H

#include __CHIP_H_FROM_DRV()

/*---------------------------------------------------------------------------------------------------------*/
/* • Timer Module 0 (TIM0) is used as timers 0-4 interrupts and for internal watchdog 0. Watchdog reset    */
/* output from this module is connected to the internal Watchdog reset 0 (WDR0) and resets the modules     */
/* defined in WD0RCR registers. Timers 0-4 in the rest of the chapter relate to NPCM750 timers 0-4.        */
/* • Timer Module 1 (TIM1) is used as timers 5-9 interrupts and for internal watchdog 1. Watchdog reset    */
/* output                                                                                                  */
/* from this module is connected to the internal Watchdog reset 1 (WDR1) and resets the modules defined in */
/* WD1RCR registers. Timers 0-4 in the rest of the chapter relate to NPCM750 timers 5-9. The reset signal  */
/* from this module watchdog is available externally on the nWDO1 pin.                                     */
/* • Timer Module 2 (TIM2) is used as timers 10-14 interrupts and for internal watchdog 2. Watchdog reset  */
/* output  from this module is connected to the internal Watchdog reset 2 (WDR2) and resets the modules    */
/*  defined in WD2RCR registers. Timers 0-4 in the rest of the chapter relate to NPCM750 timers 10-14.     */
/*---------------------------------------------------------------------------------------------------------*/


/**************************************************************************************************************************/
/*   Timer Control and Status Register 0-4 (TCR0-TCR4)                                                                    */
/**************************************************************************************************************************/
#define  TCR0(n)                           (TMC_BASE_ADDR(n) + 0x0000) , TMC_ACCESS, 32         /* TCR0: TMC_BA + 0000h  */
#define  TCR1(n)                           (TMC_BASE_ADDR(n) + 0x0004) , TMC_ACCESS, 32         /* TCR1: TMC_BA + 0004h  */
#define  TCR2(n)                           (TMC_BASE_ADDR(n) + 0x0020) , TMC_ACCESS, 32         /* TCR2: TMC_BA + 0020h  */
#define  TCR3(n)                           (TMC_BASE_ADDR(n) + 0x0024) , TMC_ACCESS, 32         /* TCR3: TMC_BA + 0024h  */
#define  TCR4(n)                           (TMC_BASE_ADDR(n) + 0x0040) , TMC_ACCESS, 32         /* TCR4: TMC_BA + 0040h  */
#define  TCR_FREEZE_EN                  31 , 1              /* 31 FREEZE_EN (ICE Debug Mode Acknowledge (nDBGACK_EN) Enable).                                                        */
#define  TCR_CEN                        30 , 1              /* 30 CEN (Counter Enable).                                                                                              */
#define  TCR_IE                         29 , 1              /* 29 IE (Interrupt Enable).                                                                                             */
#define  TCR_MODE                       27 , 2             /* 28-27 MODE (Timer Operating Mode).                                                                                    */
#define  TCR_CRST                       26 , 1              /* 26 CRST (Counter Reset). Setting this bit resets the TIMER counter and also forces CEN bit to 0.                      */
#define  TCR_CACT                       25 , 1              /* 25 CACT (Timer is in Active). Read-only. Indicates the counter status of the timer. It is the reflected value of      */
#define  TCR_PRESCALE                   0 , 8              /* 7-0 PRESCALE (Clock Prescale Divide Count). Clock input is divided by PRESCALE + 1 before it is fed                   */

/**************************************************************************************************************************/
/*   Timer Initial Count Register 0-4 (TICR0-TICR4)                                                                       */
/**************************************************************************************************************************/
#define  TICR0(n)                          (TMC_BASE_ADDR(n) + 0x0008) , TMC_ACCESS, 32         /* TICR0: TMC_BA + 0008h  */
#define  TICR1(n)                          (TMC_BASE_ADDR(n) + 0x000C) , TMC_ACCESS, 32         /* TICR1: TMC_BA + 000Ch  */
#define  TICR2(n)                          (TMC_BASE_ADDR(n) + 0x0028) , TMC_ACCESS, 32         /* TICR2: TMC_BA + 0028h  */
#define  TICR3(n)                          (TMC_BASE_ADDR(n) + 0x002C) , TMC_ACCESS, 32         /* TICR3: TMC_BA + 002Ch  */
#define  TICR4(n)                          (TMC_BASE_ADDR(n) + 0x0048) , TMC_ACCESS, 32         /* TICR4: TMC_BA + 0048h  */
#define  TICR_TIC                       0 , 24             /* 23-0 TIC (Timer Initial Count). A 24-bit value representing the initial count. The timer reloads this value when the  */

/**************************************************************************************************************************/
/*   Timer Data Register 0-4 (TDR0-TDR4)                                                                                  */
/**************************************************************************************************************************/
#define  TDR0(n)                           (TMC_BASE_ADDR(n) + 0x0010) , TMC_ACCESS, 32         /* TDR0: TMC_BA + 0010h  */
#define  TDR1(n)                           (TMC_BASE_ADDR(n) + 0x0014) , TMC_ACCESS, 32         /* TDR1: TMC_BA + 0014h  */
#define  TDR2(n)                           (TMC_BASE_ADDR(n) + 0x0030) , TMC_ACCESS, 32         /* TDR2: TMC_BA + 0030h  */
#define  TDR3(n)                           (TMC_BASE_ADDR(n) + 0x0034) , TMC_ACCESS, 32         /* TDR3: TMC_BA + 0034h  */
#define  TDR4(n)                           (TMC_BASE_ADDR(n) + 0x0050) , TMC_ACCESS, 32         /* TDR4: TMC_BA + 0050h  */
#define  TDR_TDR                        0 , 24             /* 23-0 TDR (Timer Data Register). The current count is registered in this 24-bit value.                                 */

/**************************************************************************************************************************/
/*   Timer Interrupt Status Register (TISR)                                                                               */
/**************************************************************************************************************************/
#define  TISR(n)                           (TMC_BASE_ADDR(n) + 0x0018) , TMC_ACCESS, 32         /* Offset: TMC_BA + 0018h */
#define  TISR_TIF4                       4 , 1               /* 4 TIF4 (Timer Interrupt Flag 4).                                                                                      */
#define  TISR_TIF3                       3 , 1               /* 3 TIF3 (Timer Interrupt Flag 3).                                                                                      */
#define  TISR_TIF2                       2 , 1               /* 2 TIF2 (Timer Interrupt Flag 2).                                                                                      */
#define  TISR_TIF1                       1 , 1               /* 1 TIF1 (Timer Interrupt Flag 1).                                                                                      */
#define  TISR_TIF0                       0 , 1               /* 0 TIF0 (Timer Interrupt Flag 0).                                                                                      */

/**************************************************************************************************************************/
/*   Watchdog Timer Control Register (WTCR)                                                                               */
/**************************************************************************************************************************/
#define  WTCR(n)                           (TMC_BASE_ADDR(n) + 0x001C) , TMC_ACCESS, 32         /* Offset: TMC_BA + 001Ch */
#define  WTCR_WTCLK                      10 , 2             /* 11-10 WTCLK (Watchdog Timer Clock Divide). Determines whether the watchdog timer clock input is                       */
#define  WTCR_FREEZE_EN                  9 , 1               /* 9 FREEZE_EN (ICE Debug Mode Acknowledge Enable - nDBGACK_EN).                                                         */
#define  WTCR_WTTME                      8 , 1               /* 8 WTTME (Watchdog Timer Test Mode Enable). For efficiency, the 26-bit up counter within the                           */
#define  WTCR_WTE                        7 , 1               /* 7 WTE (Watchdog Timer Enable).                                                                                        */
#define  WTCR_WTIE                       6 , 1               /* 6 WTIE (Watchdog Timer Interrupt Enable).                                                                             */
#define  WTCR_WTIS                       4 , 2              /* 5-4 WTIS (Watchdog Timer Interval Select). Selects the watchdog timer interval. No matter which interval              */
#define  WTCR_WTIF                       3 , 1               /* 3 WTIF (Watchdog Timer Interrupt Flag). If watchdog interrupt is enabled, the hardware sets this bit to               */
#define  WTCR_WTRF                       2 , 1               /* 2 WTRF (Watchdog Timer Reset Flag). When the watchdog timer initiates a reset, the hardware sets                      */
#define  WTCR_WTRE                       1 , 1               /* 1 WTRE (Watchdog Timer Reset Enable). Setting this bit enables the Watchdog Timer reset function.                     */
#define  WTCR_WTR                        0 , 1               /* 0 WTR (Watchdog Timer Reset). Brings the watchdog timer into a known state. It resets the watchdog                    */



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              TMC Register                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define TMC_PORTS_PER_MODULE          5
#define TMC_PORT_TO_MODULE(port)      ((port) / TMC_PORTS_PER_MODULE)
#define TMC_PORT_IN_MODULE(port)      ((port) % TMC_PORTS_PER_MODULE)


#define  TCR(channel)               CONCAT3(TCR,channel,(TMC_PORT_TO_MODULE(channel)))
#define  TICR(channel)              CONCAT3(TICR,channel,(TMC_PORT_TO_MODULE(channel)))
#define  TDR(channel)               CONCAT3(TDR,channel,(TMC_PORT_TO_MODULE(channel)))



//#define TMC_PORT_BASE_FUNCTION(port)  ((((port)>>1)<<5) + (((port)&1)<<2))
//#define TMC_PORT_BASE_ADDR(port)      (TMC_BASE_ADDR(TMC_PORT_TO_MODULE(port)) + TMC_PORT_BASE_FUNCTION(TMC_PORT_IN_MODULE(port)))

/*---------------------------------------------------------------------------------------------------------*/
/* Multiple per module                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
//#define TMC_TCSR(port)    (TMC_PORT_BASE_ADDR(port) + 0x00), TMC_ACCESS, 32     // R/W Timer Control and Status Register 0 0000_0005h
//#define TMC_TICR(port)    (TMC_PORT_BASE_ADDR(port) + 0x08), TMC_ACCESS, 32     // R/W Timer Initial Control Register 0 0000_0000h
//#define TMC_TDR(port)     (TMC_PORT_BASE_ADDR(port) + 0x10), TMC_ACCESS, 32     // RO Timer Data Register 0 0000_0000h

/*---------------------------------------------------------------------------------------------------------*/
/* Single per module                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
//#define TMC_TISR(port)    (TMC_BASE_ADDR(TMC_PORT_TO_MODULE(port))+ 0x18), TMC_ACCESS, 32     // R/W1C Timer Interrupt Status Register 0000_0000h
//#define TMC_WTCR          (TMC_BASE_ADDR(0)                         + 0x1C), TMC_ACCESS, 32     // R/W Watchdog Timer Control Register 0000_0400h




typedef enum TCR_MODE_type
{
    TCR_MODE_ONESHOT   = 0x0,
    TCR_MODE_PERIODIC  = 0x1,
    TCR_MODE_TOGGLE    = 0x2,
} TCR_MODE_T;



/*---------------------------------------------------------------------------------------------------------*/
/* WTIS (Watchdog Timer Interval Select). Selects the watchdog timer interval. No matter which interval    */
/* is chosen, the reset timeout always occurs 1024 prescale clocks later than the interrupt on timeout.    */
/*---------------------------------------------------------------------------------------------------------*/
                        /* Bits     Interrupt TO    Reset Timeout          Real Time Interval    Real Time Interval   Real Time Interval   */
                        /* 5 4                                             CLK = 25 MHz / 256    CLK = 25 MHz /2048   CLK = 25 MHz / 65536 */
#define  WTIS_MODE_0      0x00  /* 2^14 clocks      2^14 + 1024 clocks     0.17 sec              1.42606336 sec       43 sec  */
#define  WTIS_MODE_1      0x01  /* 2^16 clocks      2^16 + 1024 clocks     0.67 sec              5.4525952 sec        172 sec   */
#define  WTIS_MODE_2      0x02  /* 2^18 clocks      2^18 + 1024 clocks     2.7 sec               21.55872256 sec      687 sec   */
#define  WTIS_MODE_3      0x03  /* 2^20 clocks      2^20 + 1024 clocks     10.7 sec              85.983232 sec        2750 sec  */


/*---------------------------------------------------------------------------------------------------------*/
/* WTCLK (Watchdog Timer Clock Divide). Determines whether the watchdog timer clock input is               */
/* divided. The clock source of the watchdog timer is the UART clock (24 MHz).                             */
/* When WTTME = 1, setting this field has no effect on the watchdog timer clock (using original clk input).*/
/*---------------------------------------------------------------------------------------------------------*/
#define    WTCLK_DIV_1             0x00   /*  0 0: Using original clock input     */
#define    WTCLK_DIV_256           0x01   /*  0 1: Clock input divided by 256     */
#define    WTCLK_DIV_2048          0x02   /*  1 0: Clock input divided by 2048    */
#define    WTCLK_DIV_65536         0x03   /*  1 1: Clock input divided by 65536   */





#endif // _TMC_REGS_H
