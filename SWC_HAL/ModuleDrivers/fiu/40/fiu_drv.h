/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   fiu_drv.h                                                                */
/*            This file contains FIU driver interface                         */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _FIU_DRV_H
#define _FIU_DRV_H

#define FIU_CAPABILITY_QUAD_READ
#define FIU_CAPABILITY_SUPPORT_128MB_FLASH
#define FIU_CAPABILITY_CHIP_SELECT
#define FIU_CAPABILITY_SPI_X
#define FIU_CAPABILITY_4B_ADDRESS

#define FIU_MAX_UMA_DATA_SIZE 16


//#define FIU_CAPABILITY_CONFIG_ADDR_SIZE


// not supported:
//#define FIU_CAPABILITY_FIU_TEST_QUAD_EN
//#define FIU_CAPABILITY_RESP_CFG_QUAD_EN
//#define FIU_CAPABILITY_SEC_CHIP_SELECT


#define    FIU_READ_STATUS_REG_PERIOD        1     /* Usec TODO update  */
#define    FIU_USEC_TO_LOOP_COUNT(time)      (time / FIU_READ_STATUS_REG_PERIOD)
#define    BITS_7_0                          0xFF
#define    BITS_15_8                         0xFF00
#define    BITS_23_16                        0xFF0000


typedef enum
{
    FIU_TRANS_STATUS_DONE        = 0,
    FIU_TRANS_STATUS_IN_PROG     = 1
} FIU_TRANS_STATUS_T;

#include __MODULE_IF_HEADER_FROM_DRV(fiu)

#endif  /* _FIU_DRV_H */

