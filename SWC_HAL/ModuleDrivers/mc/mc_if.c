/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   mc_if.c                                                                  */
/*            This file contains MC module driver selector                    */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#include "hal.h"
#include __CHIP_H_FROM_IF()

#if defined(MC_MODULE_TYPE)
    #include __MODULE_DRIVER(mc, MC_MODULE_TYPE)
#endif

