/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   shared_defs.h                                                            */
/* This file contains general definitions shared between Booter and Rom Code  */
/*  Project:                                                                  */
/*            Poleg Boot Block and ROM code                                   */
/*----------------------------------------------------------------------------*/
#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

#include "../SWC_HAL/Chips/chip.h"

#if  defined(DEBUG_LOG) || defined(DEV_LOG)
#include "Apps/serial_printf/serial_printf.h"
#else
#define         serial_printf(x,...)                   (void)0
#define         serial_printf_debug(x,...)             (void)0
#endif



/*---------------------------------------------------------------------------------------------------------*/
/* Global Definitions                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define SIGNATURE_SIZE          256 // bytes

#define MAX_CODE_SIZE           0x01000000L     // 16MB

#define FLASH_START_ADDR        FIU0_BASE_ADDR + 0x50

#define FLASH_BOOT_ALTERNATIVES 2


/*---------------------------------------------------------------------------------------------------------*/
/* Boot module exported definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define START_TAG_SIZE            8     // bytes
//                                       0xAA550750        T     O     O     B
#define START_TAG_ARR_BOOTBLOCK {0x50, 0x07, 0x55, 0xAA, 0x54, 0x4F, 0x4F, 0x42}
//                                 U     B     O     O     T     B     L     K
#define START_TAG_ARR_UBOOT     {0x55, 0x42, 0x4F, 0x4F, 0x54, 0x42, 0x4C, 0x4B}




#endif /* SHARED_DEFS_H */

