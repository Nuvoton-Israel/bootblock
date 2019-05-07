/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   serial_printf.h                                                          */
/*            This file contains interface to serial printf module            */
/*  Project:                                                                  */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef __SERIAL_PRINTF_H
#define __SERIAL_PRINTF_H

#include "stdarg.h"



#define KBOLD_ON    "\x1b[1m"
#define KBOLD_OFF   "\x1b[22m"


#define KNRM  "\x1B[0m" KBOLD_OFF
#define KRED  "\x1B[31m" KBOLD_ON
#define KGRN  "\x1B[32m" KBOLD_ON
#define KYEL  "\x1B[33m" KBOLD_ON
#define KBLU  "\x1B[34m" KBOLD_ON
#define KMAG  "\x1B[35m" KBOLD_ON
#define KCYN  "\x1B[36m" KBOLD_ON
#define KWHT  "\x1B[37m" KBOLD_ON



/*---------------------------------------------------------------------------------------------------------*/
/* Full debug. Used for Nuvoton.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#if   defined(DEBUG_LOG)
int             serial_printf(const char *fmt, ...);
#define         serial_printf_debug             serial_printf

/*---------------------------------------------------------------------------------------------------------*/
/* Partial prints, print only the highlites                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined(DEV_LOG)
int             serial_printf(const char *fmt, ...);
#define         serial_printf_debug(x,...)             (void)0


/*---------------------------------------------------------------------------------------------------------*/
/* Production. no printfs.                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#else
#define         serial_printf(x,...)                   (void)0
#define         serial_printf_debug(x,...)             (void)0


#endif // DEBUG_LOG

int str_printf (char *buf, const char *fmt, ...);


#endif //__SERIAL_PRINTF_H

