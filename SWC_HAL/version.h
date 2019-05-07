/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   version.h                                                                */
/*            This file contains the version number of SWC HAL                */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#define SWC_HAL_MAJOR   2
#define SWC_HAL_MINOR   5
#define SWC_HAL_REV     9
#define SWC_HAL_PATCH   0

#define SWC_HAL_VERSION         MAKE32(MAKE16(SWC_HAL_PATCH, SWC_HAL_REV), MAKE16(SWC_HAL_MINOR, SWC_HAL_MAJOR))
#define SWC_HAL_VERSION_STR     STRINGX(SWC_HAL_MAJOR) "." STRINGX(SWC_HAL_MINOR) "." STRINGX(SWC_HAL_REV) "." STRINGX(SWC_HAL_PATCH)


