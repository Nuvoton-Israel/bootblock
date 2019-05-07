/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   shm_regs.h                                                               */
/*            This file contains Shared Memory (SHM) module registers         */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _SHM_REGS_H
#define _SHM_REGS_H

#include __CHIP_H_FROM_DRV()

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                    Shared Memory (SHM) Core Registers                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#define SMC_STS                 (SHM_BASE_ADDR + 0x000), SHM_ACCESS, 8
#define SMC_CTL                 (SHM_BASE_ADDR + 0x001), SHM_ACCESS, 8
#ifdef SHM_CAPABILITY_FLASH_ACCESS
#define FLASH_SIZE              (SHM_BASE_ADDR + 0x002), SHM_ACCESS, 8
#endif
#ifdef SHM_CAPABILITY_FWH
#define FWH_ID_LPC              (SHM_BASE_ADDR + 0x003), SHM_ACCESS, 8
#endif
#ifdef SHM_CAPABILITY_RAM_WIN_BASE_EXTENSION
#define WIN_BA_EXT(win)         (SHM_BASE_ADDR + 0x004 + (win)), SHM_ACCESS, 8
#endif
#ifdef SHM_CAPABILITY_WIN_PROTECTION
#define WIN_PROT                (SHM_BASE_ADDR + 0x006), SHM_ACCESS, 8
#endif
#define WIN_SIZE                (SHM_BASE_ADDR + 0x007), SHM_ACCESS, 8
#define SHAW_SEM(win)           (SHM_BASE_ADDR + 0x008 + (win)), SHM_ACCESS, 8
#define WIN_BASE(win)           (SHM_BASE_ADDR + 0x00A + (win * 2L)), SHM_ACCESS, 16
#define SHCFG                   (SHM_BASE_ADDR + 0x00E), SHM_ACCESS, 16
#ifdef SHM_CAPABILITY_WIN_PROTECTION
#define SMCORP0                 (SHM_BASE_ADDR + 0x010), SHM_ACCESS, 16
#define SMCORP1                 (SHM_BASE_ADDR + 0x012), SHM_ACCESS, 16
#define SMCORP2                 (SHM_BASE_ADDR + 0x014), SHM_ACCESS, 16
#define SMCORP3                 (SHM_BASE_ADDR + 0x016), SHM_ACCESS, 16
#define SMCOWP0                 (SHM_BASE_ADDR + 0x018), SHM_ACCESS, 16
#define SMCOWP1                 (SHM_BASE_ADDR + 0x01A), SHM_ACCESS, 16
#define SMCOWP2                 (SHM_BASE_ADDR + 0x01C), SHM_ACCESS, 16
#define SMCOWP3                 (SHM_BASE_ADDR + 0x01E), SHM_ACCESS, 16
#define PWIN1_BASEI             (SHM_BASE_ADDR + 0x020), SHM_ACCESS, 16
#define PWIN1_SIZEI             (SHM_BASE_ADDR + 0x022), SHM_ACCESS, 16
#define PWIN2_BASEI             (SHM_BASE_ADDR + 0x024), SHM_ACCESS, 16
#define PWIN2_SIZEI             (SHM_BASE_ADDR + 0x026), SHM_ACCESS, 16
#define PWIN3_BASEI             (SHM_BASE_ADDR + 0x028), SHM_ACCESS, 16
#define PWIN3_SIZEI             (SHM_BASE_ADDR + 0x02A), SHM_ACCESS, 16
#define PWIN4_BASEI             (SHM_BASE_ADDR + 0x02C), SHM_ACCESS, 16
#define PWIN4_SIZEI             (SHM_BASE_ADDR + 0x02E), SHM_ACCESS, 16
#define PWIN5_BASEI             (SHM_BASE_ADDR + 0x030), SHM_ACCESS, 16
#define PWIN5_SIZEI             (SHM_BASE_ADDR + 0x032), SHM_ACCESS, 16
#define PWIN_ULOCK              (SHM_BASE_ADDR + 0x038), SHM_ACCESS, 8
#define ULOCK_OPEN              (SHM_BASE_ADDR + 0x039), SHM_ACCESS, 8
#define PROT_CFG                (SHM_BASE_ADDR + 0x03A), SHM_ACCESS, 8
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                    Shared Memory (SHM) Host Registers                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#define SEM_CTS                 SIB_ADDRESS(SIB_SMEM_DEV, 0x0A), SHM_HOST_ACCESS, 8


/*---------------------------------------------------------------------------------------------------------*/
/* SMC_STS register fields                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define SMC_STS_HRERR           0,  1
#define SMC_STS_HWERR           1,  1
#define SMC_STS_HSEM1W          4,  1
#define SMC_STS_HSEM2W          5,  1
#define SMC_STS_SHM_ACC         6,  1

/*---------------------------------------------------------------------------------------------------------*/
/* SHCFG register fields                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define SHCFG_SEMW1_DIS         7,  1
#define SHCFG_SEMW2_DIS         6,  1
#define SHCFG_FL_SIZE_EX        4,  1
#define SHCFG_ABTDIS            1,  1
#define SHCFG_IMAEN             0,  1

/*---------------------------------------------------------------------------------------------------------*/
/* SMC_CTL register fields                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define SMC_CTL_HERES           0,  2   /* Bits 0-1: Host Error Response.                                  */
#define SMC_CTL_HERR_IE         2,  1   /* Bit    2: Enable Interrupt by Host Access Errors.               */
#define SMC_CTL_HSEM1_IE        3,  1   /* Bit    3: Enable Interrupt by Host Semaphore 1 Written.         */
#define SMC_CTL_HSEM2_IE        4,  1   /* Bit    4: Enable Interrupt by Host Semaphore 2 Written.         */
#define SMC_CTL_ACC_IE          5,  1   /* Bit    5: Enable Interrupt by Host Access to Shared Memory.     */
#ifdef SHM_CAPABILITY_PREFETCH
#define SMC_CTL_PREF_EN         6,  1   /* Bit    6: Prefetch Enable.                                      */
#endif
#define SMC_CTL_CONFIG          0,  7
#define SMC_CTL_HOSTWAIT        7,  1

/*---------------------------------------------------------------------------------------------------------*/
/* HERES field values                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define HERES_NO_SYNC           0x00
#define HERES_IGNORE_WRITES     0x01
#define HERES_LONG_WAIT_SYNC    0x02

#ifdef SHM_CAPABILITY_FLASH_ACCESS
/*---------------------------------------------------------------------------------------------------------*/
/* FLASH_SIZE register values                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define FLASH_SIZE_FL_SIZE_P1   0,  6
#define FLASH_SIZE_STALL_HOST   6,  1
#define FLASH_SIZE_RD_BURST     7,  1
#endif

#ifdef SHM_CAPABILITY_WIN_PROTECTION
/*---------------------------------------------------------------------------------------------------------*/
/* WIN_PROT register values                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define WIN_PROT_RWIN1          0,  4
#define WIN_PROT_RWIN2          4,  4
#define WIN_PROT_RWIN(win_num) (win_num * 4), 4

#define WIN_PROT_RW1L_RP        0,  1
#define WIN_PROT_RW1L_WP        1,  1
#define WIN_PROT_RW1H_RP        2,  1
#define WIN_PROT_RW1H_WP        3,  1
#define WIN_PROT_RW2L_RP        4,  1
#define WIN_PROT_RW2L_WP        5,  1
#define WIN_PROT_RW2H_RP        6,  1
#define WIN_PROT_RW2H_WP        7,  1
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* WIN_SIZE register values                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define WIN_SIZE_RWIN1_SIZE     0,  4
#define WIN_SIZE_RWIN2_SIZE     4,  4
#define WIN_SIZE_RWIN_SIZE(win_num) (win_num * 4), 4

/*---------------------------------------------------------------------------------------------------------*/
/* WIN_SIZE field values                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define WIN_SIZE_16_BYTES       0x4
#define WIN_SIZE_256_BYTES      0x8
#define WIN_SIZE_512_BYTES      0x9

/*---------------------------------------------------------------------------------------------------------*/
/* SHAW1_SEM, SHAW2_SEM register values                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define SHAWx_SEM_HSEM          0,  4
#define SHAWx_SEM_CSEM          4,  4

/*---------------------------------------------------------------------------------------------------------*/
/* SEM_CTS register values                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define SEM_CTS_CSEM1W          0,  1
#define SEM_CTS_CSEM2W          1,  1
#ifdef SHM_CAPABILITY_SEMAPHORE_SMI_GENERATION
#define SEM_CTS_CSEM1_IE        2,  1
#define SEM_CTS_CSEM2_IE        3,  1
#define SEM_CTS_CSEM1_SE        4,  1
#define SEM_CTS_CSEM2_SE        5,  1
#endif
#define SEM_CTS_RWIN1_SEM       6,  1
#define SEM_CTS_RWIN2_SEM       7,  1

#endif /* _SHM_REGS_H */

