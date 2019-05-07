/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   tmc_if.h                                                                 */
/*            This file contains TMC module interface                         */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _TMC_IF_H
#define _TMC_IF_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_IF()

#include "hal.h"


/*---------------------------------------------------------------------------------------------------------*/
/* TMC modules enum                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum TMC_MODULE_T_tag
{
    TMC_MODULE_0      = 0,
    TMC_MODULE_1      = 1,
    TMC_MODULE_2      = 2
} TMC_MODULE_T;



/*---------------------------------------------------------------------------------------------------------*/
/* Timer channel  (                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if !defined(TIMER_MODULE_TYPE)   /* If not using logical timer layer */
typedef enum TIMER_CHANNEL_tag
{
    /* TMC_MODULE_0 : */
    TIMER_CHANNEL_0 = 0,  /* do not change */
    TIMER_CHANNEL_1,
    TIMER_CHANNEL_2,
    TIMER_CHANNEL_3,
    TIMER_CHANNEL_4,

    /* TMC_MODULE_1 : */
    TIMER_CHANNEL_5,
    TIMER_CHANNEL_6,
    TIMER_CHANNEL_7,
    TIMER_CHANNEL_8,
    TIMER_CHANNEL_9,

    /* TMC_MODULE_2 : */
    TIMER_CHANNEL_10,
    TIMER_CHANNEL_11,
    TIMER_CHANNEL_12,
    TIMER_CHANNEL_13,
    TIMER_CHANNEL_14,

    TIMER_CHANNEL_NONE,
} TIMER_CHANNEL_T;
#endif



#ifdef GIC_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_StartPeriodic                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum        - Timer number                                                         */
/*                  ticksPerSec     - Timer frequency in hertz                                             */
/*                  tickHanlder     - Handler for Timer tick event. If NULL is given, internal event       */
/*                                    handler is used                                                      */
/*                  tickHandlerArg  - Argument to the handler                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialization of the timer to perform periodic event.                    */
/*                  If 'tickHandler' is given it is called every 'tickPerSec' tick.                        */
/*                  Otherwise internal handler is increasing internal tick counter for the given timer.    */
/*                  Its value can be read using TMC_GetTick function                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TMC_StartPeriodic (TIMER_CHANNEL_T timerNum, UINT32 ticksPerSec, HANDLER_T tickHanlder, void* tickHandlerArg);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_stopPeriodic                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine stops the timer started by startPeriodic routine                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TMC_StopPeriodic (TIMER_CHANNEL_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_startOneShot                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSec -    number of micro seconds the timer will count (to zero)                   */
/*                  timerNum -    the timer number to use                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Activate timer in OneShot mode, no use of interrupts                                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TMC_StartOneShot (TIMER_CHANNEL_T timerNum, UINT32 microSec);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_oneShotRunning                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum -  number of timer to check                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Checks whether the specified timer is still running (counting)                         */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN TMC_OneShotRunning (TIMER_CHANNEL_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_reset                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs TIMER reset                                                      */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TMC_Reset (TIMER_CHANNEL_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_getTick                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns TIMER value                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 TMC_GetTick (TIMER_CHANNEL_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_getHWTick                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum -                                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine return the content of the HW ticking register                             */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 TMC_GetHWTick (TIMER_CHANNEL_T timerNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_watchdogRest                                                                       */
/*                                                                                                         */
/* Parameters:      wd - one per timer module                                                              */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs watchdog reset                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void TMC_WatchdogReset (TMC_MODULE_T wd);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_WatchdogTouch                                                                      */
/*                                                                                                         */
/* Parameters:      wd - one per timer module                                                              */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                   This routine performs watchdog touch for a running WD                                 */
/*---------------------------------------------------------------------------------------------------------*/
void TMC_WatchdogTouch (TMC_MODULE_T wd);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_StartWatchDog                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  wd    -   watchdog number (one per module)                                             */
/*                  timeSec - todo!  currently it's 22 sec only.                                           */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine starts the wd                                                             */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TMC_StartWatchDog (TMC_MODULE_T wd, UINT32 timeSec);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_StopWatchDog                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  wd    -   watchdog number (one per module)                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine stops the wd and clear all pending interrupts                             */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TMC_StopWatchDog (TMC_MODULE_T wd);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void TMC_PrintRegs (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_PrintModuleRegs                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  wd - The watchdog number (module) to be printed.                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module instance registers                                      */
/*---------------------------------------------------------------------------------------------------------*/
void TMC_PrintModuleRegs (TMC_MODULE_T wd);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TMC_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void TMC_PrintVersion (void);


#endif // _TMC_IF_H

