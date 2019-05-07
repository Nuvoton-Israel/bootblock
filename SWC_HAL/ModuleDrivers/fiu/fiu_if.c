/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   fiu_if.c                                                                 */
/*            This file contains FIU module driver selector                   */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#include __CHIP_H_FROM_IF()

#if defined (FIU_MODULE_TYPE)
    #include __MODULE_DRIVER(fiu, FIU_MODULE_TYPE)
#endif

