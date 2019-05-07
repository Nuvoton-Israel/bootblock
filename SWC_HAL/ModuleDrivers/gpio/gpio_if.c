/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   gpio_if.c                                                                */
/*            This file contains GPIO module driver selector                  */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#include __CHIP_H_FROM_IF()

#if defined(GPIO_MODULE_TYPE)
    #include __MODULE_DRIVER(gpio, GPIO_MODULE_TYPE)
#endif

