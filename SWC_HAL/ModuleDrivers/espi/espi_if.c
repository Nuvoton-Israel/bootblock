/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   espi_if.c                                                                */
/* This file contains Enhanced Serial Peripheral Interface (eSPI) module driver selector */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#include __CHIP_H_FROM_IF()

#if defined(ESPI_MODULE_TYPE)
    #include __MODULE_DRIVER(espi, ESPI_MODULE_TYPE)
#endif

