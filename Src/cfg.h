/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   cfg.h                                                                    */
/*            This file contains API of configuration routines for ROM code   */
/*  Project:                                                                  */
/*            Poleg Bootblock and ROM Code (shared header)                    */
/*----------------------------------------------------------------------------*/

#ifndef CFG_H
#define CFG_H

#include "hal.h"


/*---------------------------------------------------------------------------------------------------------*/
/* CFG module exported functions                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void 	CFG_HandleResetIndication (void);
void 	CFG_HandleWatchdog (void);
void    CFG_MiscConfigurations (void);
void    CFG_SHM_ReleaseHostWait (void);
void    CFG_PrintResetType (char *str, BOOLEAN bPrint);
UINT    CFG_GetResetNum (void);
void    CFG_HeartBeatToggle (int iCnt);


/*---------------------------------------------------------------------------------------------------------*/
/* CFG general utilities                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void    Tick (void);
void    Tock (void);


#endif /* CFG_H */
