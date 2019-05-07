/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   hal_print.c                                                              */
/*            This file contains Common HAL Print functions                   */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "hal.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                         FUNCTION IMPLEMENTATION                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HAL_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints all modules registers                                              */
/*---------------------------------------------------------------------------------------------------------*/
void HAL_PrintRegs (void)
{
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*   REGISTERS  */\n");
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*--------------*/\n\n");

#if defined CHIP_NAME
    CHIP_PrintRegs();
#endif

#if defined ADC_MODULE_TYPE
    ADC_PrintRegs();
#endif

#if defined AES_MODULE_TYPE
    AES_PrintRegs();
#endif

#if defined BBRM_MODULE_TYPE
    BBRM_PrintRegs();
#endif

#if defined CLK_MODULE_TYPE
    CLK_PrintRegs();
#endif

#if defined CR_UART_MODULE_TYPE
    CR_UART_PrintRegs();
#endif

#if defined DAC_MODULE_TYPE
    DAC_PrintRegs();
#endif

#if defined DCU_MODULE_TYPE
    DCU_PrintRegs();
#endif

#if defined DES_MODULE_TYPE
    DES_PrintRegs();
#endif

#if defined ECE_MODULE_TYPE
    ECE_PrintRegs();
#endif

#if defined EMC_MODULE_TYPE
    EMC_PrintRegs();
#endif

#if defined ESPI_MODULE_TYPE
    ESPI_PrintRegs();
#endif

#if defined FIU_MODULE_TYPE
    FIU_PrintRegs();
#endif

#if defined FUSE_MODULE_TYPE
    FUSE_PrintRegs();
#endif

#if defined GDMA_MODULE_TYPE
    GDMA_PrintRegs();
#endif

#if defined GIC_MODULE_TYPE
    GIC_PrintRegs();
#endif

#if defined GPIO_MODULE_TYPE
    GPIO_PrintRegs();
#endif

#if defined HFCG_MODULE_TYPE
    HFCG_PrintRegs();
#endif

#if defined ICU_MODULE_TYPE
    ICU_PrintRegs();
#endif

#if defined ITIM8_MODULE_TYPE
    ITIM8_PrintRegs();
#endif

#if defined ITIM32_MODULE_TYPE
    ITIM32_PrintRegs();
#endif

#if defined KBC_MODULE_TYPE
    KBC_PrintRegs();
#endif

#if defined KBSCAN_MODULE_TYPE
    KBSCAN_PrintRegs();
#endif

#if defined LFCG_MODULE_TYPE
    LFCG_PrintRegs();
#endif

#if defined MC_MODULE_TYPE
    MC_PrintRegs();
#endif

#if defined MFT_MODULE_TYPE
    MFT_PrintRegs();
#endif

#if defined MIWU_MODULE_TYPE
    MIWU_PrintRegs();
#endif

#if defined MRC_MODULE_TYPE
    MRC_PrintRegs();
#endif

#if defined MSWC_MODULE_TYPE
    MSWC_PrintRegs();
#endif

#if defined MTC_MODULE_TYPE
    MTC_PrintRegs();
#endif

#if defined NVIC_MODULE_TYPE
    NVIC_PrintRegs();
#endif

#if defined OTP_MODULE_TYPE
    OTP_PrintRegs();
#endif

#if defined OTPI_MODULE_TYPE
    OTPI_PrintRegs();
#endif

#if defined PECI_MODULE_TYPE
    PECI_PrintRegs();
#endif

#if defined PKA_MODULE_TYPE
    PKA_PrintRegs();
#endif

#if defined PM_CHAN_MODULE_TYPE
    PM_CHAN_PrintRegs();
#endif

#if defined PMC_MODULE_TYPE
    PMC_PrintRegs();
#endif

#if defined PS2_MODULE_TYPE
    PS2_PrintRegs();
#endif

#if defined PWM_MODULE_TYPE
    PWM_PrintRegs();
#endif

#if defined RNG_MODULE_TYPE
    RNG_PrintRegs();
#endif

#if defined SCS_MODULE_TYPE
    SCS_PrintRegs();
#endif

#if defined DWT_MODULE_TYPE
    DWT_PrintRegs();
#endif

#if defined SD_MODULE_TYPE
    SD_PrintRegs();
#endif

#if defined SHA_MODULE_TYPE
    SHA_PrintRegs();
#endif

#if defined SHI_MODULE_TYPE
    SHI_PrintRegs();
#endif

#if defined SHM_MODULE_TYPE
    SHM_PrintRegs();
#endif

#if defined DP80_MODULE_TYPE
    DP80_PrintRegs();
#endif

#if defined SIB_MODULE_TYPE
    SIB_PrintRegs();
#endif

#if defined SMB_MODULE_TYPE
    SMB_PrintRegs();
#endif

#if defined SPI_MODULE_TYPE
    SPI_PrintRegs();
#endif

#if defined STI_MODULE_TYPE
    STI_PrintRegs();
#endif

#if defined SYST_MODULE_TYPE
    SYST_PrintRegs();
#endif

#if defined TMC_MODULE_TYPE
    TMC_PrintRegs();
#endif

#if defined TWD_MODULE_TYPE
    TWD_PrintRegs();
#endif

#if defined UART_MODULE_TYPE
    UART_PrintRegs();
#endif

#if defined VCD_MODULE_TYPE
    VCD_PrintRegs();
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HAL_PrintVars                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints all modules variables                                              */
/*---------------------------------------------------------------------------------------------------------*/
void HAL_PrintVars (void)
{
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*   VARIABLES  */\n");
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*--------------*/\n\n");

#if defined I2C_MODULE_TYPE
    I2C_PrintVars();
#endif

#if defined MFT_MODULE_TYPE
    MFT_PrintVars();
#endif

#if defined PECI_MODULE_TYPE
    PECI_PrintVars();
#endif

#if defined SMB_MODULE_TYPE
    SMB_PrintVars();
#endif

#if defined TACHO_MODULE_TYPE
    TACHO_PrintVars();
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HAL_PrintVersions                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints all modules versions                                               */
/*---------------------------------------------------------------------------------------------------------*/
void HAL_PrintVersions (void)
{
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*   VERSIONS   */\n");
    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*--------------*/\n\n");

#if defined ADC_MODULE_TYPE
    ADC_PrintVersion();
#endif

#if defined AES_MODULE_TYPE
    AES_PrintVersion();
#endif

#if defined BBRM_MODULE_TYPE
    BBRM_PrintVersion();
#endif

#if defined CLK_MODULE_TYPE
    CLK_PrintVersion();
#endif

#if defined CR_UART_MODULE_TYPE
    CR_UART_PrintVersion();
#endif

#if defined DAC_MODULE_TYPE
    DAC_PrintVersion();
#endif

#if defined DCU_MODULE_TYPE
    DCU_PrintVersion();
#endif

#if defined DES_MODULE_TYPE
    DES_PrintVersion();
#endif

#if defined ECE_MODULE_TYPE
    ECE_PrintVersion();
#endif

#if defined EMC_MODULE_TYPE
    EMC_PrintVersion();
#endif

#if defined ESPI_MODULE_TYPE
    ESPI_PrintVersion();
#endif

#if defined FIU_MODULE_TYPE
    FIU_PrintVersion();
#endif

#if defined FUSE_MODULE_TYPE
    FUSE_PrintVersion();
#endif

#if defined GDMA_MODULE_TYPE
    GDMA_PrintVersion();
#endif

#if defined GIC_MODULE_TYPE
    GIC_PrintVersion();
#endif

#if defined GPIO_MODULE_TYPE
    GPIO_PrintVersion();
#endif

#if defined HFCG_MODULE_TYPE
    HFCG_PrintVersion();
#endif

#if defined ICU_MODULE_TYPE
    ICU_PrintVersion();
#endif

#if defined ITIM8_MODULE_TYPE
    ITIM8_PrintVersion();
#endif

#if defined KBC_MODULE_TYPE
    KBC_PrintVersion();
#endif

#if defined KBSCAN_MODULE_TYPE
    KBSCAN_PrintVersion();
#endif

#if defined LFCG_MODULE_TYPE
    LFCG_PrintVersion();
#endif

#if defined MC_MODULE_TYPE
    MC_PrintVersion();
#endif

#if defined MFT_MODULE_TYPE
    MFT_PrintVersion();
#endif

#if defined MIWU_MODULE_TYPE
    MIWU_PrintVersion();
#endif

#if defined MRC_MODULE_TYPE
    MRC_PrintVersion();
#endif

#if defined MSWC_MODULE_TYPE
    MSWC_PrintVersion();
#endif

#if defined MTC_MODULE_TYPE
    MTC_PrintVersion();
#endif

#if defined NVIC_MODULE_TYPE
    NVIC_PrintVersion();
#endif

#if defined OTP_MODULE_TYPE
    OTP_PrintVersion();
#endif

#if defined OTPI_MODULE_TYPE
    OTPI_PrintVersion();
#endif

#if defined PECI_MODULE_TYPE
    PECI_PrintVersion();
#endif

#if defined PKA_MODULE_TYPE
    PKA_PrintVersion();
#endif

#if defined PM_CHAN_MODULE_TYPE
    PM_CHAN_PrintVersion();
#endif

#if defined PMC_MODULE_TYPE
    PMC_PrintVersion();
#endif

#if defined PS2_MODULE_TYPE
    PS2_PrintVersion();
#endif

#if defined PWM_MODULE_TYPE
    PWM_PrintVersion();
#endif

#if defined RNG_MODULE_TYPE
    RNG_PrintVersion();
#endif

#if defined SCS_MODULE_TYPE
    SCS_PrintVersion();
#endif

#if defined DWT_MODULE_TYPE
    DWT_PrintVersion();
#endif

#if defined SD_MODULE_TYPE
    SD_PrintVersion();
#endif

#if defined SHA_MODULE_TYPE
    SHA_PrintVersion();
#endif

#if defined SHI_MODULE_TYPE
    SHI_PrintVersion();
#endif

#if defined SHM_MODULE_TYPE
    SHM_PrintVersion();
#endif

#if defined DP80_MODULE_TYPE
    DP80_PrintVersion();
#endif

#if defined SIB_MODULE_TYPE
    SIB_PrintVersion();
#endif

#if defined SMB_MODULE_TYPE
    SMB_PrintVersion();
#endif

#if defined SPI_MODULE_TYPE
    SPI_PrintVersion();
#endif

#if defined STI_MODULE_TYPE
    STI_PrintVersion();
#endif

#if defined SYST_MODULE_TYPE
    SYST_PrintVersion();
#endif

#if defined TMC_MODULE_TYPE
    TMC_PrintVersion();
#endif

#if defined TWD_MODULE_TYPE
    TWD_PrintVersion();
#endif

#if defined UART_MODULE_TYPE
    UART_PrintVersion();
#endif

#if defined VCD_MODULE_TYPE
    VCD_PrintVersion();
#endif
}

