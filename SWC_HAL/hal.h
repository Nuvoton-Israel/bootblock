/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   hal.h                                                                    */
/*            This file contains the HAL chip definitions and full API        */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _HAL_H
#define _HAL_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             CHIP DEPENDENT                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#if !defined SWC_HAL_FLAT
#include "Chips/chip.h"
#else
#include "chip.h"
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              MODULE DRIVERS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* SCS Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SCS_MODULE_TYPE
#include __MODULE_IF_HEADER(scs)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DWT Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DWT_MODULE_TYPE
#include __MODULE_IF_HEADER(dwt)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* HFCG Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined HFCG_MODULE_TYPE
#include __MODULE_IF_HEADER(hfcg)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FIU Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FIU_MODULE_TYPE || defined FIU_MODULE_TYPE_EXT
#include __MODULE_IF_HEADER(fiu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Keyboard scan module Driver                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#if defined KBSCAN_MODULE_TYPE
#include __MODULE_IF_HEADER(kbscan)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ICU Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ICU_MODULE_TYPE
#include __MODULE_IF_HEADER(icu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* NVIC Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined NVIC_MODULE_TYPE
#include __MODULE_IF_HEADER(nvic)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MIWU Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MIWU_MODULE_TYPE
#include __MODULE_IF_HEADER(miwu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TWD Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TWD_MODULE_TYPE
#include __MODULE_IF_HEADER(twd)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SYST Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SYST_MODULE_TYPE
#include __MODULE_IF_HEADER(syst)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MFT Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MFT_MODULE_TYPE
#include __MODULE_IF_HEADER(mft)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PWM Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PWM_MODULE_TYPE
#include __MODULE_IF_HEADER(pwm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CLK (bmc) module Driver                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CLK_MODULE_TYPE
#include __MODULE_IF_HEADER(clk)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*  DES Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DES_MODULE_TYPE
#include   __MODULE_IF_HEADER(des)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SMB Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SMB_MODULE_TYPE
#include __MODULE_IF_HEADER(smb)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PECI_MODULE_TYPE
#include __MODULE_IF_HEADER(peci)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SHM Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SHM_MODULE_TYPE
#include __MODULE_IF_HEADER(shm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DP80 Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DP80_MODULE_TYPE
#include __MODULE_IF_HEADER(dp80)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GPIO_MODULE_TYPE
#include __MODULE_IF_HEADER(gpio)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SIB Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SIB_MODULE_TYPE
#include __MODULE_IF_HEADER(sib)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ADC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ADC_MODULE_TYPE
#include __MODULE_IF_HEADER(adc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DCU Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DCU_MODULE_TYPE
#include __MODULE_IF_HEADER(dcu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* KBC HOST Module Driver                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#if defined KBC_HOST_MODULE_TYPE
#include __MODULE_IF_HEADER(kbc_host)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* KBC core Module Driver                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#if defined KBC_MODULE_TYPE
#include __MODULE_IF_HEADER(kbc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SIO Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SIO_MODULE_TYPE
#include __MODULE_IF_HEADER(sio)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* EWOC Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined EWOC_MODULE_TYPE
#include __MODULE_IF_HEADER(ewoc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MSWC Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MSWC_MODULE_TYPE
#include __MODULE_IF_HEADER(mswc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SWC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SWC_MODULE_TYPE
#include __MODULE_IF_HEADER(swc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SPI Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SPI_MODULE_TYPE
#include __MODULE_IF_HEADER(spi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PS/2 module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PS2_MODULE_TYPE
#include __MODULE_IF_HEADER(ps2)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ITIM8 Module Driver                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ITIM8_MODULE_TYPE
#include __MODULE_IF_HEADER(itim8)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ITIM32 Module Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ITIM32_MODULE_TYPE
#include __MODULE_IF_HEADER(itim32)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PM Channel Module Driver                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PM_CHAN_MODULE_TYPE
#include __MODULE_IF_HEADER(pm_chan)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PMC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PMC_MODULE_TYPE
#include __MODULE_IF_HEADER(pmc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DAC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DAC_MODULE_TYPE
#include __MODULE_IF_HEADER(dac)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CPS Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CPS_MODULE_TYPE
#include __MODULE_IF_HEADER(cps)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* STI Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined STI_MODULE_TYPE
#include __MODULE_IF_HEADER(sti)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* OTP Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined OTP_MODULE_TYPE
#include __MODULE_IF_HEADER(otp)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* OTPI Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined OTPI_MODULE_TYPE
#include __MODULE_IF_HEADER(otpi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SHI Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SHI_MODULE_TYPE
#include __MODULE_IF_HEADER(shi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* LFCG Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined LFCG_MODULE_TYPE
#include __MODULE_IF_HEADER(lfcg)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SHA Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SHA_MODULE_TYPE
#include __MODULE_IF_HEADER(sha)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GDMA Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GDMA_MODULE_TYPE
#include __MODULE_IF_HEADER(gdma)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PKA Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PKA_MODULE_TYPE
#include __MODULE_IF_HEADER(pka)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MTC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MTC_MODULE_TYPE
#include __MODULE_IF_HEADER(mtc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CR UART Module Driver                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CR_UART_MODULE_TYPE
#include __MODULE_IF_HEADER(cr_uart)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* BBRM Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined BBRM_MODULE_TYPE
#include __MODULE_IF_HEADER(bbrm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* RNG Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined RNG_MODULE_TYPE
#include __MODULE_IF_HEADER(rng)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* AES module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined AES_MODULE_TYPE
#include __MODULE_IF_HEADER(aes)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MRC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MRC_MODULE_TYPE
#include __MODULE_IF_HEADER(mrc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ESPI_MODULE_TYPE
#include __MODULE_IF_HEADER(espi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SSL Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SSL_MODULE_TYPE
#include __MODULE_IF_HEADER(ssl)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SDP Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SDP_MODULE_TYPE
#include __MODULE_IF_HEADER(sdp)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GIC module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GIC_MODULE_TYPE
#include __MODULE_IF_HEADER(gic)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SD module Driver                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SD_MODULE_TYPE
#include __MODULE_IF_HEADER(sd)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* STRP module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined STRP_MODULE_TYPE
#include __MODULE_IF_HEADER(strp)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* UART module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined UART_MODULE_TYPE
#include __MODULE_IF_HEADER(uart)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* VCD module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined VCD_MODULE_TYPE
#include __MODULE_IF_HEADER(vcd)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GMAC module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GMAC_MODULE_TYPE
#include __MODULE_IF_HEADER(gmac)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FUSE module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FUSE_MODULE_TYPE
#include __MODULE_IF_HEADER(fuse)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TMC module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TMC_MODULE_TYPE
#include __MODULE_IF_HEADER(tmc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MC module Driver                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MC_MODULE_TYPE
#include __MODULE_IF_HEADER(mc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* LCT module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined LCT_MODULE_TYPE
#include __MODULE_IF_HEADER(lct)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOGICAL DRIVERS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* TACHO Logical Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TACHO_MODULE_TYPE
#include __LOGICAL_IF_HEADER(tacho)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CIR Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CIR_MODULE_TYPE
#include __LOGICAL_IF_HEADER(cir)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CIR LED flash Logical Driver                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CIRLED_MODULE_TYPE
#include __LOGICAL_IF_HEADER(cirled)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FLASH DEV Logical Driver                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FLASH_DEV_MODULE_TYPE || defined FLASH_DEV_MODULE_TYPE_EXT
#include __LOGICAL_IF_HEADER(flash_dev)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FLASH Logical Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FLASH_MODULE_TYPE
#include __LOGICAL_IF_HEADER(flash)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* NV MEM Logical Driver                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined NV_MEM_MODULE_TYPE
#include __LOGICAL_IF_HEADER(nv_mem)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TIMER Logical Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TIMER_MODULE_TYPE
#include __LOGICAL_IF_HEADER(timer)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* BM Logical Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined BM_MODULE_TYPE
#include __LOGICAL_IF_HEADER(bm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* RSA Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined RSA_MODULE_TYPE
#include __LOGICAL_IF_HEADER(rsa)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* HMAC Logical Driver                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined HMAC_MODULE_TYPE
#include __LOGICAL_IF_HEADER(hmac)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* I2C Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined I2C_MODULE_TYPE
#include __LOGICAL_IF_HEADER(i2c)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* RTC Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined RTC_MODULE_TYPE
#include __LOGICAL_IF_HEADER(rtc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ACPI Logical Driver                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ACPI_MODULE_TYPE
#include __LOGICAL_IF_HEADER(acpi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           PERIPHERAL DRIVERS                                            */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* ALS Peripheral Driver                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ALS_MODULE_TYPE
#include __PERIPHERAL_IF_HEADER(als)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SPI_FLASH Module Driver                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SPI_FLASH_MODULE_TYPE
#include __PERIPHERAL_IF_HEADER(spi_flash)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*  ETH_PHY Module Driver                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ETH_PHY_MODULE_TYPE
#include __PERIPHERAL_IF_HEADER(eth_phy)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HAL_Version                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         HAL version                                                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the HAL's version                                               */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 HAL_Version (void);

#endif /* _HAL_H */
