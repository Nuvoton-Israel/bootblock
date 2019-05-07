/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   fuse_drv.h                                                               */
/*            This file contains API of FUSE module routines for ROM code     */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef FUSE_DRV_H
#define FUSE_DRV_H

#include "hal.h"
#include __MODULE_IF_HEADER_FROM_DRV(fuse)

#if defined (SHA_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_DRV(sha)
#endif

#if defined (AES_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_DRV(aes)
#endif

#endif /* FUSE_DRV_H */

