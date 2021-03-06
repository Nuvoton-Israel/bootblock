/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   hal_common.h                                                             */
/*            This file contains common HAL macros                            */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _HAL_COMMON_H_
#define _HAL_COMMON_H_


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDE                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "defs.h"


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              Common Macros                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Busy Wait with Timeout                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define BUSY_WAIT_TIMEOUT(busy_cond, timeout)       \
{                                                   \
    UINT32 __time = timeout;                        \
                                                    \
    do                                              \
    {                                               \
        if (__time-- == 0)                          \
        {                                           \
            return DEFS_STATUS_RESPONSE_TIMEOUT;    \
        }                                           \
    } while (busy_cond);                            \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Empty itteration delay loop                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define DELAY_LOOP(count)                           \
{                                                   \
    volatile UINT16 __i;                            \
    for (__i=0; __i<(count); ++__i) {}              \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Microsecond delay loop                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define DELAY_USEC(microSecDelay, coreClk)                                                      \
{                                                                                               \
    volatile UINT32 __i;                                                                        \
    volatile UINT32 __iterations = USEC_TO_ITERATIONS(microSecDelay, coreClk);                  \
    for (__i=0; __i<__iterations; ++__i) {}                                                     \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Calculate number of iterations:                                                                         */
/*                                                                                                         */
/*  iterations =    coreClk        microSecDelay                                                           */
/*               -------------  X  -------------                                                           */
/*              CyclesPerIter      _1SEC_IN_USEC_                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define USEC_TO_ITERATIONS(microSecDelay, coreClk)                                              \
    (UINT32)(((coreClk / _1SEC_IN_USEC_) * (microSecDelay)) / HFCG_NUM_OF_CYCLES_PER_ITERATION)

/*---------------------------------------------------------------------------------------------------------*/
/* Print function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef HAL_PRINT
#define HAL_PRINT_CAPABILITY
#else
#define HAL_PRINT(ARGS...)
#endif

#endif  /* _HAL_COMMON_H_ */

