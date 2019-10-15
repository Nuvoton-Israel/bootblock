/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   hal.c                                                                    */
/*            This file contains all modules of the HAL                       */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#define HAL_C

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "version.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             CHIP DEPENDENT                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#if !defined SWC_HAL_FLAT
#include "Chips/chip.c"
#else
#include "chip.c"
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              MODULE DRIVERS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* ADC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ADC_MODULE_TYPE
#include __MODULE_IF(adc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* AES Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined AES_MODULE_TYPE
#include __MODULE_IF(aes)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* BBRM Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined BBRM_MODULE_TYPE
#include __MODULE_IF(bbrm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CLK Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CLK_MODULE_TYPE
#include __MODULE_IF(clk)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CPS Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CPS_MODULE_TYPE
#include __MODULE_IF(cps)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CR UART Module Driver                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CR_UART_MODULE_TYPE
#include __MODULE_IF(cr_uart)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DAC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DAC_MODULE_TYPE
#include __MODULE_IF(dac)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DCU Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DCU_MODULE_TYPE
#include __MODULE_IF(dcu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DES Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DES_MODULE_TYPE
#include __MODULE_IF(des)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DP80 Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DP80_MODULE_TYPE
#include __MODULE_IF(dp80)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DWT Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DWT_MODULE_TYPE
#include __MODULE_IF(dwt)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ECE Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ECE_MODULE_TYPE
#include __MODULE_IF(ece)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* EMC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined EMC_MODULE_TYPE
#include __MODULE_IF(emc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ESPI_MODULE_TYPE
#include __MODULE_IF(espi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* EWOC Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined EWOC_MODULE_TYPE
#include __MODULE_IF(ewoc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FIU Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FIU_MODULE_TYPE || defined FIU_MODULE_TYPE_EXT
#include __MODULE_IF(fiu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FUSE Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FUSE_MODULE_TYPE
#include __MODULE_IF(fuse)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GDMA Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GDMA_MODULE_TYPE
#include __MODULE_IF(gdma)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GIC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GIC_MODULE_TYPE
#include __MODULE_IF(gic)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GMAC Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GMAC_MODULE_TYPE
#include __MODULE_IF(gmac)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined GPIO_MODULE_TYPE
#include __MODULE_IF(gpio)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* HFCG Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined HFCG_MODULE_TYPE
#include __MODULE_IF(hfcg)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ICU Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ICU_MODULE_TYPE
#include __MODULE_IF(icu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ITIM8 Module Driver                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ITIM8_MODULE_TYPE
#include __MODULE_IF(itim8)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ITIM32 Module Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ITIM32_MODULE_TYPE
#include __MODULE_IF(itim32)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ITIM64 Module Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ITIM64_MODULE_TYPE
#include __MODULE_IF(itim64)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* KBC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined KBC_MODULE_TYPE
#include __MODULE_IF(kbc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* KBC HOST Module Driver                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#if defined KBC_HOST_MODULE_TYPE
#include __MODULE_IF(kbc_host)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Keyboard scan module Driver                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#if defined KBSCAN_MODULE_TYPE
#include __MODULE_IF(kbscan)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* LCT Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined LCT_MODULE_TYPE
#include __MODULE_IF(lct)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* LFCG Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined LFCG_MODULE_TYPE
#include __MODULE_IF(lfcg)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MC Module Driver                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MC_MODULE_TYPE
#include __MODULE_IF(mc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MFT Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MFT_MODULE_TYPE
#include __MODULE_IF(mft)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MIWU Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MIWU_MODULE_TYPE
#include __MODULE_IF(miwu)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MRC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MRC_MODULE_TYPE
#include __MODULE_IF(mrc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MSWC Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MSWC_MODULE_TYPE
#include __MODULE_IF(mswc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MTC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined MTC_MODULE_TYPE
#include __MODULE_IF(mtc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* NVIC Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined NVIC_MODULE_TYPE
#include __MODULE_IF(nvic)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* OTP Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined OTP_MODULE_TYPE
#include __MODULE_IF(otp)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* OTPI Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined OTPI_MODULE_TYPE
#include __MODULE_IF(otpi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PECI_MODULE_TYPE
#include __MODULE_IF(peci)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PKA Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PKA_MODULE_TYPE
#include __MODULE_IF(pka)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PM Channel Module Driver                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PM_CHAN_MODULE_TYPE
#include __MODULE_IF(pm_chan)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PMC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PMC_MODULE_TYPE
#include __MODULE_IF(pmc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PS/2 module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PS2_MODULE_TYPE
#include __MODULE_IF(ps2)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PWM Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined PWM_MODULE_TYPE
#include __MODULE_IF(pwm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* RNG Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined RNG_MODULE_TYPE
#include __MODULE_IF(rng)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SCS Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SCS_MODULE_TYPE
#include __MODULE_IF(scs)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SD Module Driver                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SD_MODULE_TYPE
#include __MODULE_IF(sd)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SDP Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SDP_MODULE_TYPE
#include __MODULE_IF(sdp)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SHA Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SHA_MODULE_TYPE
#include __MODULE_IF(sha)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SHI Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SHI_MODULE_TYPE
#include __MODULE_IF(shi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SHM Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SHM_MODULE_TYPE
#include __MODULE_IF(shm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SIB Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SIB_MODULE_TYPE
#include __MODULE_IF(sib)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SIO Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SIO_MODULE_TYPE
#include __MODULE_IF(sio)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SMB Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SMB_MODULE_TYPE
#include __MODULE_IF(smb)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SPI Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SPI_MODULE_TYPE
#include __MODULE_IF(spi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SSL Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SSL_MODULE_TYPE
#include __MODULE_IF(ssl)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* STI Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined STI_MODULE_TYPE
#include __MODULE_IF(sti)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* STRP Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined STRP_MODULE_TYPE
#include __MODULE_IF(strp)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SWC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SWC_MODULE_TYPE
#include __MODULE_IF(swc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SYST Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SYST_MODULE_TYPE
#include __MODULE_IF(syst)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TMC Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TMC_MODULE_TYPE
#include __MODULE_IF(tmc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TWD Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TWD_MODULE_TYPE
#include __MODULE_IF(twd)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* UART Module Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined UART_MODULE_TYPE
#include __MODULE_IF(uart)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* VCD Module Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined VCD_MODULE_TYPE
#include __MODULE_IF(vcd)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOGICAL DRIVERS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* ACPI Logical Driver                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ACPI_MODULE_TYPE
#include __LOGICAL_IF(acpi)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* BM Logical Driver                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined BM_MODULE_TYPE
#include __LOGICAL_IF(bm)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CIR Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CIR_MODULE_TYPE
#include __LOGICAL_IF(cir)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CIRLED Logical Driver                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined CIRLED_MODULE_TYPE
#include __LOGICAL_IF(cirled)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ECC Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ECC_MODULE_TYPE
#include __LOGICAL_IF(ecc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FLASH DEV Logical Driver                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FLASH_DEV_MODULE_TYPE || defined FLASH_DEV_MODULE_TYPE_EXT
#include __LOGICAL_IF(flash_dev)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FLASH Logical Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined FLASH_MODULE_TYPE
#include __LOGICAL_IF(flash)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* HMAC Logical Driver                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined HMAC_MODULE_TYPE
#include __LOGICAL_IF(hmac)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* I2C Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined I2C_MODULE_TYPE
#include __LOGICAL_IF(i2c)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* NV MEM Logical Driver                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined NV_MEM_MODULE_TYPE
#include __LOGICAL_IF(nv_mem)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* RSA Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined RSA_MODULE_TYPE
#include __LOGICAL_IF(rsa)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* RTC Logical Driver                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined RTC_MODULE_TYPE
#include __LOGICAL_IF(rtc)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TACHO Logical Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TACHO_MODULE_TYPE
#include __LOGICAL_IF(tacho)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TIMER Logical Driver                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined TIMER_MODULE_TYPE
#include __LOGICAL_IF(timer)
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
#include __PERIPHERAL_IF(als)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ETH_PHY Module Driver                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ETH_PHY_MODULE_TYPE
#include __PERIPHERAL_IF(eth_phy)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SPI_FLASH Module Driver                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SPI_FLASH_MODULE_TYPE
#include __PERIPHERAL_IF(spi_flash)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                         FUNCTION IMPLEMENTATION                                         */
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
UINT32 HAL_Version (void)
{
    return SWC_HAL_VERSION;
}

#undef HAL_C

