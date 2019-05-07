/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   uart_if.c                                                                */
/*            This file contains UART module driver selector                  */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#include __CHIP_H_FROM_IF()

#include "hal.h"

#if defined(UART_MODULE_TYPE)
    #include __MODULE_DRIVER(uart, UART_MODULE_TYPE)
#endif

