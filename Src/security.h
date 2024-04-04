/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   security.h                                                               */
/*            This file contains API of security routines for ROM code        */
/*  Project:                                                                  */
/*            ROM Code                                                        */
/*----------------------------------------------------------------------------*/
#ifndef SECURITY_H
#define SECURITY_H

#include "defs.h"
#include "boot.h"
#include "shared_defs.h"


DEFS_STATUS         BOOTBLOCK_CheckUbootHeader_l   (BOOT_HEADER_T *uBootHeader, UINT8 image, unsigned int *destAddr, unsigned int *codeSize);
DEFS_STATUS         BOOTBLOCK_CheckBBHeader_l      (BOOT_HEADER_T *bbHeader, UINT8 image);


#endif /* SECURITY_H */

