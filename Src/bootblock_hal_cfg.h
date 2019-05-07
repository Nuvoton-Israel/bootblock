/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*       bootblock_hal_cfg.h                                                  */
/*            Poleg BMC BB Firmware main functions                            */
/* Project:   Poleg BMC Bootblock                                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             UNUSED MODULES                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* #define NPCM750  1                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Module Drivers exclusion  : SWC_HAL includes many driver. Drivers which are not in used on bootblock are*/
/*                             undef-ed at this file.                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#undef GIC_MODULE_TYPE
//#undef FUSE_MODULE_TYPE
//#undef STRP_MODULE_TYPE


// #undef FIU_MODULE_TYPE


// #undef  GPIO_MODULE_TYPE
#undef  PM_CHAN_MODULE_TYPE
//#undef  OTP_MODULE_TYPE
#undef  SHA_MODULE_TYPE
#undef  PKA_MODULE_TYPE
#undef  EWOC_MODULE_TYPE
#undef  TWD_MODULE_TYPE
#undef  ICU_MODULE_TYPE
#undef  NVIC_MODULE_TYPE
#undef  SHI_MODULE_TYPE
#undef  MFT_MODULE_TYPE
#undef  DCU_MODULE_TYPE
#undef  ITIM8_MODULE_TYPE
#undef  PWM_MODULE_TYPE
#undef  SWC_MODULE_TYPE
#undef  HFCG_MODULE_TYPE
#undef  STI_MODULE_TYPE
#undef  KBC_HOST_MODULE_TYPE
#undef  SMB_MODULE_TYPE
#undef  ADC_MODULE_TYPE
#undef  PMC_MODULE_TYPE
#undef  SIO_MODULE_TYPE
#undef  GDMA_MODULE_TYPE
#undef  LFCG_MODULE_TYPE
// #undef  SHM_MODULE_TYPE
#undef  SCFG_MODULE_TYPE
#undef  CR_UART_MODULE_TYPE
#undef  MTC_MODULE_TYPE
#undef  DAC_MODULE_TYPE
#undef  KBSCAN_MODULE_TYPE
#undef  SYST_MODULE_TYPE
#undef  MIWU_MODULE_TYPE
#undef  RNG_MODULE_TYPE
#undef  SIB_MODULE_TYPE
#undef  CPS_MODULE_TYPE
#undef  MSWC_MODULE_TYPE
#undef  PS2_MODULE_TYPE
#undef  PECI_MODULE_TYPE
#undef  SCS_MODULE_TYPE
#undef  DES_MODULE_TYPE
#undef  EMC_MODULE_TYPE
#undef  VCD_MODULE_TYPE

#undef  AES_MODULE_TYPE

#undef  SPI_MODULE_TYPE
//#undef  UART_MODULE_TYPE

#undef  SD_MODULE_TYPE
//#undef  MC_MODULE_TYPE
#undef  ECE_MODULE_TYPE
#undef  EMC_MODULE_TYPE
#undef  GMAC_MODULE_TYPE
// #undef  ESPI_MODULE_TYPE
// #undef  TMC_MODULE_TYPE



/*---------------------------------------------------------------------------------------------------------*/
/* Logical Drivers exclusion                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#undef  BM_MODULE_TYPE
#undef  RSA_MODULE_TYPE
#undef  CIRLED_MODULE_TYPE
#undef  FLASH_DEV_MODULE_TYPE
#undef  TACHO_MODULE_TYPE
#undef  ACPI_MODULE_TYPE
//#undef  TIMER_MODULE_TYPE
#undef  CIR_MODULE_TYPE
#undef  FLASH_MODULE_TYPE
#undef  RTC_MODULE_TYPE

/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOGICAL DRIVERS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Flash SPI Module                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
//#define FLASH_DEV_MODULE_TYPE               hw_fiu
//#define FLASH_DEV_WRITE_WITH_ERASE_EN
//#define FLASH_DEV_USE_READ_WITH_MEMCPY
//#define FLASH_DEV_SECURITY_UNSUPPORTED



/*---------------------------------------------------------------------------------------------------------*/
/* RSA Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
// #define RSA_MODULE_TYPE                    hw_pka

/*---------------------------------------------------------------------------------------------------------*/
/* Big math module                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
// #define BM_MODULE_TYPE                     sw_external_hook

/*---------------------------------------------------------------------------------------------------------*/
/* Timer logical layer                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
//#define TIMER_MODULE_TYPE                  hw_multi

/*---------------------------------------------------------------------------------------------------------*/
/* RTC LED Flash Module                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
//#define RTC_MODULE_TYPE                    hw_mtc


/*---------------------------------------------------------------------------------------------------------*/
/* I2C Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
//#ifndef I2C_MODULE_TYPE
//#define I2C_MODULE_TYPE                     hw_smb
//#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           PERIPHERAL DRIVERS                                            */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
//#undef  ETH_PHY_MODULE_TYPE
//#undef  SPI_FLASH_MODULE_TYPE

/*---------------------------------------------------------------------------------------------------------*/
/* PHY assignment per module                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
//#define BOARD_ETH0_PHY_OPS              DP83848C_Ops
//#define BOARD_ETH1_PHY_OPS              DP83848C_Ops

/*---------------------------------------------------------------------------------------------------------*/
/* SPI Flash                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/




