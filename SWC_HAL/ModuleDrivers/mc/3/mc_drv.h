/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   mc_drv.h                                                                 */
/*            This file contains MC driver interface                          */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef MC_DRV_H
#define MC_DRV_H

#include __MODULE_IF_HEADER_FROM_DRV(mc)

#include "MC_Cadence_Define_Register_CTL_C.h"   // MC field definition
#if 0
#include "cds_mem_CTL_regconfig_C.h"            // MC configuration for Cadenc DDR device (JDEC)
#else
#include "mic_mem_CTL_regconfig_C.h"            // MC configuration for Micron DDR device
#endif
#include "MC_Priority_Settings.h"               // Override configuration for priority settings


/*-------------------------------------------------------------------------------------*/
/* Bit Fields to be used by mc_config  (Values returned from BOOTBLOCK_Get_MC_config)  */
/*-------------------------------------------------------------------------------------*/
#define MC_DISABLE_CAPABILITY_INPUT_DQS_ENHANCE_TRAINING (0x01)
#define MC_CAPABILITY_IGNORE_ECC_DEVICE          (0x02)

#endif //MC_DRV_H

