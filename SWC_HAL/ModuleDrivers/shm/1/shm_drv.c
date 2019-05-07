/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   shm_drv.c                                                                */
/*            This file contains Shared Memory (SHM) driver implementation    */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#include __CHIP_H_FROM_DRV()

#include "shm_drv.h"
#include "shm_regs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Module Dependencies                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined SIB_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_DRV(sib)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Bit-Mask of SHM WIN_BASE                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define SHM_WIN_BASE              0,  16
#ifdef SHM_CAPABILITY_RAM_WIN_BASE_EXTENSION
#define SHM_WIN_BASE_EXT          16,  8
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                            GLOBAL VARIABLES                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* User callback to be called from SHM interrupt                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static SHM_CALLBACK_T SHM_callback;

#ifdef ESPI_MODULE_TYPE
static SHM_ESPI_CALLBACK_T SHM_espiCallback;
#endif // ESPI_MODULE_TYPE

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                  LOCAL FUNCTIONS FORWARD DECLARATIONS                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void SHM_IntHandler     (UINT16 int_num);

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_Init                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  shm_callback - Function to be called upon shared memory interrupt                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Shared memory hardware module elements that are relevant for  */
/*                  both Shared Memory RAM windows.                                                        */
/*                  Note that flash-related initializations which are expected to be done by the boot ROM  */
/*                  are not done here (e.g. flash size, read burst size, memory protection,                */
/*                  HOSTWAIT clearing).                                                                    */
/*                                                                                                         */
/* Usage:           in order to init the shm call 4 function:                                              */
/*                   1. SHM_Init: sets the callback                                                        */
/*                   2. SHM_Config: sets SMC_CTL, unless using default values.                             */
/*                   3. SHM_ReleaseHostWait: host can start                                                */
/*                   4. SHM_HostAccessEnable(TRUE): host can start using the SHM.                          */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_Init (SHM_CALLBACK_T shm_callback)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear statuses                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(SMC_STS, REG_READ(SMC_STS));

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    REG_WRITE(SMCE_STS, REG_READ(SMCE_STS));
#endif

#ifdef SHM_CAPABILITY_OFFSET
    REG_WRITE(HOFS_STS, REG_READ(HOFS_STS));

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    REG_WRITE(HOFSE_STS, REG_READ(HOFSE_STS));
#endif
#endif /* SHM_CAPABILITY_OFFSET */

#ifdef ESPI_MODULE_TYPE
    SHM_espiCallback = NULL;
#endif // ESPI_MODULE_TYPE

    /*-----------------------------------------------------------------------------------------------------*/
    /* Save callback                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    SHM_callback = shm_callback;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure interrupt                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPT_REGISTER_AND_ENABLE(SHM_INTERRUPT_PROVIDER, SHM_INTERRUPT, SHM_IntHandler,
                                  SHM_INTERRUPT_POLARITY, SHM_INTERRUPT_PRIORITY);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_Config                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  hostErrorResponse     - Host Error Response.                                           */
/*                  bEnableIntByHostError - Enable Interrupt by Host Access Errors.                        */
/*                  bEnableIntHostAccShm  - Enable Interrupt by Host Access to Shared Memory.              */
/*                  bPrefetchEnable       - Prefetch Enable.                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configs Shared memory host handling: interrupts, error response etc.      */
/*lint -e{715}      Suppress 'bPrefetchEnable' not referenced                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_Config (   SHM_HOST_ERROR_RESPONSE_T  hostErrorResponse,
                    BOOLEAN                    bEnableIntByHostError,
                    BOOLEAN                    bEnableIntHostAccShm,
                    BOOLEAN                    bPrefetchEnable
)
{
    UINT8 smc_ctl = REG_READ(SMC_CTL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set host access shm response                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(smc_ctl, SMC_CTL_HERES,  hostErrorResponse);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable interrupts by host access error                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(smc_ctl, SMC_CTL_HERR_IE, bEnableIntByHostError);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable Host Access interrupt                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(smc_ctl, SMC_CTL_ACC_IE, bEnableIntHostAccShm);

#ifdef SHM_CAPABILITY_PREFETCH
    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure shm prefetch                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(smc_ctl, SMC_CTL_PREF_EN, bPrefetchEnable);
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set required control value (avoid setting HOSTWAIT bit at this stage)                               */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(smc_ctl, SMC_CTL_HOSTWAIT, FALSE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set Shared Memory Core Control Register                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(SMC_CTL, smc_ctl);
}

#ifdef ESPI_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EspiRegisterCallback                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  shm_espiCallback - Espi callback                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine registers callback for ESPI module interrupts                             */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EspiRegisterCallback (SHM_ESPI_CALLBACK_T shm_espiCallback)
{
    SHM_espiCallback = shm_espiCallback;
}
#endif // ESPI_MODULE_TYPE

#ifdef SHM_CAPABILITY_SEMAPHORE_SMI_GENERATION
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_HostSemaphoreConfig                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num  - Shared Memory Window to be configured.                                   */
/*                  bEnableIRQ  - TRUE:  Enable generation of IRQ on Core write to the semaphore register  */
/*                                FALSE: Disable IRQ on Core write to the semaphore register               */
/*                  bEnableSMI  - TRUE:  Enable generation of SMI on Core write to the semaphore register  */
/*                                FALSE: Disable SMI on Core write to the semaphore register               */
/*                  bEnableSemOnWindow -                                                                   */
/*                                TRUE: enable a semaphore access through the first address byte of        */
/*                                      the RAM Window. In othere words this will cuase the semaphore to   */
/*                                      be located in offset 0 of the SHM RAM window.                      */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Shared memory Semaphore behavior                              */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_HostSemaphoreConfig ( SHM_WINDOW_T window_num,
                               BOOLEAN      bEnableIRQ,
                               BOOLEAN      bEnableSMI,
                               BOOLEAN      bEnableSemOnWindow
)
{
    UINT8 sem_cts = 0;
#ifdef SHM_CAPABILITY_EXTENDED_SHM
    UINT8 seme_cts = 0;
#endif

    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Lock Host access to the appropriate device; Read appropriate semaphore status register              */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (window_num)
    {
    case SHM_WINDOW_ONE:
    case SHM_WINDOW_TWO:
        SIB_SetLock(SIB_SMEM_DEV);
        sem_cts = REG_READ(SEM_CTS);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
    case SHM_WINDOW_FOUR:
        SIB_SetLock(SIB_ESHM_DEV);
        seme_cts = REG_READ(SEME_CTS);
        break;
#endif

    default:
        break;
    }

    switch (window_num)
    {
    case SHM_WINDOW_ONE:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of IRQ                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(sem_cts, SEM_CTS_CSEM1_IE,  bEnableIRQ);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of SMI                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(sem_cts, SEM_CTS_CSEM1_SE, bEnableSMI);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable SEM on RAM window offset 0                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(sem_cts, SEM_CTS_RWIN1_SEM, bEnableSemOnWindow);
        break;

    case SHM_WINDOW_TWO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of IRQ                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(sem_cts, SEM_CTS_CSEM2_IE,  bEnableIRQ);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of SMI                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(sem_cts, SEM_CTS_CSEM2_SE, bEnableSMI);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable SEM on RAM window offset 0                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(sem_cts, SEM_CTS_RWIN2_SEM, bEnableSemOnWindow);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of IRQ                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(seme_cts, SEME_CTS_CSEM3_IE,  bEnableIRQ);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of SMI                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(seme_cts, SEME_CTS_CSEM3_SE, bEnableSMI);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable SEM on RAM window offset 0                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(seme_cts, SEME_CTS_RWIN3_SEM, bEnableSemOnWindow);
        break;

    case SHM_WINDOW_FOUR:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of IRQ                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(seme_cts, SEME_CTS_CSEM4_IE,  bEnableIRQ);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable generation of SMI                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(seme_cts, SEME_CTS_CSEM4_SE, bEnableSMI);

        /*-------------------------------------------------------------------------------------------------*/
        /* Enable/Disable SEM on RAM window offset 0                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(seme_cts, SEME_CTS_RWIN4_SEM, bEnableSemOnWindow);
        break;
#endif  /* SHM_CAPABILITY_EXTENDED_SHM */

    default:
        break;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the Host access lock; Set back the appropriate semaphore status register                      */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (window_num)
    {
    case SHM_WINDOW_ONE:
    case SHM_WINDOW_TWO:
        REG_WRITE(SEM_CTS, sem_cts);
        SIB_ClearLock(SIB_SMEM_DEV);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
    case SHM_WINDOW_FOUR:
        REG_WRITE(SEME_CTS, seme_cts);
        SIB_ClearLock(SIB_ESHM_DEV);
        break;
#endif

    default:
        break;
    }
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_WindowConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num  - Shared Memory Window to be configured.                                   */
/*                  window_base - Core base address of RAM window.                                         */
/*                  window_size - Exponent of 2 - e.g. 4 designate RAM window of 16 (2**4).                */
/*                                Valid values are 3-12.                                                   */
/*                  window_prot - bitMask of Shared RAM Window, Access Protection                          */
/*                                if a bit is set to 1 - the specific aread is protected.                  */
/*                                if a bit is set to 0 - the specific aread is not protected               */
/*                                ===========================================================              */
/*                                For uRider12 (and below)/SIO14/SIO15 the bits meaning are:               */
/*                                ===========================================================              */
/*                                - Bit 0 = Low Half of window is Read Protected                           */
/*                                - Bit 1 = Low Half of window is Write Protected                          */
/*                                - Bit 2 = High Half of window is Read Protected                          */
/*                                - Bit 3 = High Half of window is Write Protected                         */
/*                                ===========================================================              */
/*                                For MRider15 the bits meaning are:                                       */
/*                                ===========================================================              */
/*                                - Bit 0  = Write Protect the first (lower) 1/8 of the RAM window         */
/*                                - Bit 1  = Write Protect the second 1/8 of the RAM window                */
/*                                - Bit 2  = Write Protect the 3rd 1/8 of the RAM window                   */
/*                                - Bit 3  = Write Protect the 4th 1/8 of the RAM window                   */
/*                                - Bit 4  = Write Protect the 5th 1/8 of the RAM window                   */
/*                                - Bit 5  = Write Protect the 6th 1/8 of the RAM window                   */
/*                                - Bit 6  = Write Protect the 7th 1/8 of the RAM window                   */
/*                                - Bit 7  = Write Protect the 8th 1/8 of the RAM window                   */
/*                                - Bit 8  = Read Protect the first (lower) 1/8 of the RAM window          */
/*                                - Bit 9  = Read Protect the second 1/8 of the RAM window                 */
/*                                - Bit 10 = Read Protect the 3rd 1/8 of the RAM window                    */
/*                                - Bit 11 = Read Protect the 4th 1/8 of the RAM window                    */
/*                                - Bit 12 = Read Protect the 5th 1/8 of the RAM window                    */
/*                                - Bit 13 = Read Protect the 6th 1/8 of the RAM window                    */
/*                                - Bit 14 = Read Protect the 7th 1/8 of the RAM window                    */
/*                                - Bit 15 = Read Protect the 8th 1/8 of the RAM window                    */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Shared memory hardware module elements that are relevant for  */
/*                  the WCB protocol implementation.                                                       */
/*                  Note that flash-related initializations which are expected to be done by the boot ROM  */
/*                  are not done here (e.g. flash size, read burst size, memory protection,                */
/*                  HOSTWAIT clearing).                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_WindowConfig ( SHM_WINDOW_T window_num,
                        UINT32       window_base,
                        UINT8        window_size,
                        UINT16       window_prot
)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set Shared RAM window size -                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (window_num)
    {
    case SHM_WINDOW_ONE:
    case SHM_WINDOW_TWO:
        SET_REG_FIELD(WIN_SIZE, WIN_SIZE_RWIN_SIZE(window_num), window_size);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
    case SHM_WINDOW_FOUR:
        SET_REG_FIELD(WINE_SIZE, WINE_SIZE_RWIN_SIZE(window_num), window_size);
        break;
#endif

    default:
        break;
    }

#ifdef SHM_CAPABILITY_EXTENDED_8_AREA_PROTECTION
    REG_WRITE(WIN_WR_PROT(window_num), (UINT8)window_prot);
    REG_WRITE(WIN_RD_PROT(window_num), (UINT8)(window_prot >> 8));
#endif

#ifdef SHM_CAPABILITY_WIN_PROTECTION
    /*-----------------------------------------------------------------------------------------------------*/
    /* Set required Host Read/Write Protection for Window                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(WIN_PROT, WIN_PROT_RWIN(window_num), (UINT8)window_prot);
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set Shared RAM window start - Window base is the offset of the window from RAM start                */
    /*-----------------------------------------------------------------------------------------------------*/
#ifdef SHM_CAPABILITY_ABSOLUTE_WIN_BASE_ADDR
    /*-----------------------------------------------------------------------------------------------------*/
    /* Set Shared RAM window base address                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(WIN_BASE(window_num), window_base);
#else
    /*-----------------------------------------------------------------------------------------------------*/
    /* Set Shared RAM window start - Window base is the offset of the window from RAM start                */
    /*-----------------------------------------------------------------------------------------------------*/
    window_base = window_base - SHM_RAM_BASE_ADDR;
    REG_WRITE(WIN_BASE(window_num), (READ_VAR_FIELD(window_base, SHM_WIN_BASE)));
  #ifdef SHM_CAPABILITY_RAM_WIN_BASE_EXTENSION
    REG_WRITE(WIN_BA_EXT(window_num), (READ_VAR_FIELD(window_base, SHM_WIN_BASE_EXT)));
  #endif
#endif
}

#ifdef SHM_CAPABILITY_EXTENDED_SHM
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableHostEventRegisters                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window number.                                              */
/*                  enbale     - TRUE: enable, FALSE disable                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable/disable the Host Event Registers                                   */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableHostEventRegisters ( SHM_WINDOW_T window_num,
                                    BOOLEAN  enable)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    switch (window_num)
    {
    case SHM_WINDOW_THREE:
        /*-------------------------------------------------------------------------------------------------*/
        /* Host Event Registers Window 3 Enable                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(SMCE_CTL, SMCE_CTL_HEVREG3_EN, enable);
        break;

    case SHM_WINDOW_FOUR:
        /*-------------------------------------------------------------------------------------------------*/
        /* Host Event Registers Window 4 Enable                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(SMCE_CTL, SMCE_CTL_HEVREG4_EN, enable);
        break;

    case SHM_WINDOW_ONE:
    case SHM_WINDOW_TWO:
    /* Window 1 & window 2 don't support direct access. */
    default:
        break;
    }
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableSemaphoreInterrupt                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window number.                                              */
/*                  enbale     - TRUE: enable, FALSE disable                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable/disable the interrupt on host access to the Semaphore  register    */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableSemaphoreInterrupt (
    SHM_WINDOW_T    window_num,
    BOOLEAN         enable
)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    switch (window_num)
    {
    case SHM_WINDOW_ONE:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable Semaphore 1 interrupt                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(SMC_CTL, SMC_CTL_HSEM1_IE, enable);
        break;

    case SHM_WINDOW_TWO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable Semaphore 2 interrupt                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(SMC_CTL, SMC_CTL_HSEM2_IE, enable);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable Semaphore 3 interrupt                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(SMCE_CTL, SMCE_CTL_HSEM3_IE, enable);
        break;

    case SHM_WINDOW_FOUR:
        /*-------------------------------------------------------------------------------------------------*/
        /* Enable Semaphore 4 interrupt                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(SMCE_CTL, SMCE_CTL_HSEM4_IE, enable);
        break;
#endif

    default:
        break;
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SemaphoreRead                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be read.                                          */
/*                                                                                                         */
/* Returns:         Semaphore value                                                                        */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads the semaphore in a given Shared Memory Window.                      */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 SHM_SemaphoreRead (SHM_WINDOW_T window_num)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read semaphore                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    return REG_READ(SHAW_SEM(window_num));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SemaphoreWrite                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  value      - Value to be written to sempaphore.                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes to the semaphore in a given Shared Memory Window.                  */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_SemaphoreWrite (
    SHM_WINDOW_T window_num,
    UINT8        value
)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write semaphore                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(SHAW_SEM(window_num), value);
}


#if defined FIU_MODULE_TYPE && defined SHM_CAPABILITY_FLASH_ACCESS
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_FlashConfig                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  flash_size       - The flash size.                                                     */
/*                  read_burst_size  - Flash read burst size (1/16 bytes).                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the size of the connected flash for SHM operations.            */
/*                  The Flash size must be configured to the same value as in FIU_CFG register.            */
/*                  It also selects the maximum size of a read burst transaction, which is supported by    */
/*                  the flash.                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_FlashConfig (
    FIU_DEV_SIZE_T  flash_size,
    FIU_R_BURST_T   read_burst_size
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure flash size                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(FLASH_SIZE, FLASH_SIZE_FL_SIZE_P1, (flash_size + 1));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set Read burst size to four bytes if the flash supports a read burst of 16 bytes                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (read_burst_size == FIU_R_BURST_SIXTEEN_BYTE)
    {
        SET_REG_FIELD(FLASH_SIZE, FLASH_SIZE_RD_BURST, TRUE);
    }
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_HostAccessEnable                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  enable - TRUE to stall host read transactions; FALSE otherwise.                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/stalls host read transactions:                                    */
/*                  - When transactions are enabled, all transactions are served automatically by the      */
/*                    hardware, including transactions that are pending from the period when the           */
/*                    transactions were stalled.                                                           */
/*                    It is intended to be called when flash write or erase operation is completed.        */
/*                  - When transactions are stalled, no transactions are served and interrupts (notifying  */
/*                    the core of a pending transaction) are not enabled.                                  */
/*                    It is intented to be called whenever a flash write or erase operation is initiated - */
/*                    the SHM transaction are stalled until read and write operation is completed.         */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_HostAccessEnable (BOOLEAN enable)
{
#ifdef SHM_CAPABILITY_FLASH_ACCESS
    SET_REG_FIELD(FLASH_SIZE, FLASH_SIZE_STALL_HOST, (BOOLEAN)(!enable));
#else
    SET_REG_FIELD(SHM_CTL, SHM_CTL_STALL_HOST, (BOOLEAN)(!enable));
#endif
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_HostAccessState                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         TRUE if host read access is enabled; FALSE otherwise (host is stalled).                */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the host read transactions access state.                        */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN SHM_HostAccessState (void)
{
#ifdef SHM_CAPABILITY_FLASH_ACCESS
    return (BOOLEAN)(! READ_REG_FIELD(FLASH_SIZE, FLASH_SIZE_STALL_HOST));
#else
    return (BOOLEAN)(! READ_REG_FIELD(SHM_CTL, SHM_CTL_STALL_HOST));
#endif
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_ReleaseHostWait                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine releases the LPC bus stall, allowing host to communicate with the device. */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_ReleaseHostWait (void)
{
    SET_REG_FIELD(SMC_CTL, SMC_CTL_HOSTWAIT, TRUE);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_IntHandler                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num - number of triggered interrupt                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/*                  Interrupt status is not cleared -                                                      */
/*                  a. To enable callbacks to see the interrupt trigger reason.                            */
/*                  b. to enable callback to re-use interrupt (e.g. in Flash Update invocation).           */
/*                  It is the callback's responsibility to clear the status of the trigger it handles      */
/* Description:                                                                                            */
/*                  This routine is the Shared Memory interrupt handler.                                   */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_IntHandler (UINT16 int_num)
{
    UINT8 smc_sts = REG_READ(SMC_STS);

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    UINT8 smce_sts = REG_READ(SMCE_STS);
#endif

#ifdef SHM_CAPABILITY_OFFSET
    UINT8 hofs_sts = REG_READ(HOFS_STS);

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    UINT8 hofse_sts = REG_READ(HOFSE_STS);
#endif
#endif  /* SHM_CAPABILITY_OFFSET */

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the interrupt SMC_STS/SMCE_STS is W1C                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(SMC_STS, smc_sts);

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    REG_WRITE(SMCE_STS, smce_sts);
#endif

#ifdef SHM_CAPABILITY_OFFSET
    /*-----------------------------------------------------------------------------------------------------*/
    /* clear the interrupt HOFS_STS is W1C                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(HOFS_STS, hofs_sts);

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    REG_WRITE(HOFSE_STS, hofse_sts);
#endif
#endif  /* SHM_CAPABILITY_OFFSET */

    /*-----------------------------------------------------------------------------------------------------*/
    /* Call Shared Memory callback, pass SHM status mask:                                                  */
    /* LSB0: SMC_STS                                                                                       */
    /* LSB1: SMCE_STS                                                                                      */
    /* LSB2: HOFS_STS                                                                                      */
    /* LSB3: HOFSE_STS                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
#ifdef SHM_CAPABILITY_OFFSET
  #ifdef SHM_CAPABILITY_EXTENDED_SHM
    EXECUTE_FUNC(SHM_callback, MAKE32(MAKE16(smc_sts, smce_sts), MAKE16(hofs_sts, hofse_sts)));
  #else
    EXECUTE_FUNC(SHM_callback, MAKE32(MAKE16(smc_sts, 0), MAKE16(hofs_sts, 0)));
  #endif
#else
  #ifdef SHM_CAPABILITY_EXTENDED_SHM
    EXECUTE_FUNC(SHM_callback, ((UINT32)MAKE16(smc_sts, smce_sts)));
  #else
    EXECUTE_FUNC(SHM_callback, ((UINT32)smc_sts));
  #endif
#endif

#ifdef ESPI_MODULE_TYPE
    /*-----------------------------------------------------------------------------------------------------*/
    /* If it's read/write from protected area error                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(smc_sts,SMC_STS_HRERR) || READ_VAR_FIELD(smc_sts,SMC_STS_HWERR))
    {
        EXECUTE_FUNC(SHM_espiCallback, ());
    }
#endif // ESPI_MODULE_TYPE
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_ClearInterrupts                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*        window_num      - Shared Memory Window to be written.                                            */
/*        val (bit mask)  - See SHM_INT_TYPE_T enumerator description                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine clear pending Core interrupt on host access to one of the                 */
/*                  pre-configured offsets                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_ClearInterrupts (SHM_INT_TYPE_T val)
{
    UINT32 sts = (UINT32)val;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the interrupt, SMC_STS is W1C                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(SMC_STS, LSB0(sts));

#ifdef SHM_CAPABILITY_OFFSET
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the interrupt, HOFS_STS is W1C                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(HOFS_STS, LSB1(sts));
#endif

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the interrupt, SMCE_STS is W1C                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(SMCE_STS, LSB2(sts));

#ifdef SHM_CAPABILITY_OFFSET
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the interrupt, HOFSE_STS is W1C                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(HOFSE_STS, LSB3(sts));
#endif
#endif  /* SHM_CAPABILITY_EXTENDED_SHM */
}


#ifdef SHM_CAPABILITY_OFFSET
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SetCoreOffset                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  offset     -  offset from the windows base.                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Set the "Core offset" from the windows base.                                           */
/*                  A core WRITE to this offset can generate an IRQ/SMI to the host                        */
/*                  A host READ from this offset can generate an Interrupt to the core                     */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_SetCoreOffset (
    SHM_WINDOW_T  window_num,
    UINT16        offset
)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    REG_WRITE(COFS(window_num), offset);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableCoreOffsetInterrupts                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  val       -  0 (SHM_OFFSET_INT__ON_HOST_RD_CORE_OFFSET), Enable/Disable interrupt on   */
/*                                  host read from core offset.                                            */
/*                            -  1 (SHM_OFFSET_INT__ON_HOST_WR_HOST_OFFSET), Enable/Disable interrupt on   */
/*                                  host write to host offset.                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable/disable the Core interrupt on host access to one of the            */
/*                  pre-configured offsets                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableCoreOffsetInterrupts (
    SHM_WINDOW_T        window_num,
    SHM_OFFSET_INT_T    val,
    BOOLEAN             enable
)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    switch (window_num)
    {
    case SHM_WINDOW_TWO:
        /*-------------------------------------------------------------------------------------------------*/
        /* val hold the bit position                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        val += 2;
        /*lint -fallthrough */

    case SHM_WINDOW_ONE:
        if (enable)
        {
            SET_REG_BIT(HOFS_CTL, val);
        }
        else
        {
            CLEAR_REG_BIT(HOFS_CTL, val);
        }
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_FOUR:
        /*-------------------------------------------------------------------------------------------------*/
        /* val hold the bit position                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        val += 2;
        /*lint -fallthrough */

    case SHM_WINDOW_THREE:
        if (enable)
        {
            SET_REG_BIT(HOFSE_CTL, val);
        }
        else
        {
            CLEAR_REG_BIT(HOFSE_CTL, val);
        }
        break;
#endif

    default:
        break;
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SetHostOffset                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  offset     -  offset from the windows base.                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  uses the SIB to access the Host registers in order to set the "Host offset"            */
/*                  A host WRITE to this offset can generate an Interrupt to the core                      */
/*                  A core READ from this offset can generate an IRQ/SMI to the host                       */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_SetHostOffset (
    SHM_WINDOW_T  window_num,
    UINT16        offset
)
{
    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    switch (window_num)
    {
    case SHM_WINDOW_ONE:
        SIB_SetLock(SIB_SMEM_DEV);
        REG_WRITE(HOST_HOFS1L, LSB(offset));
        REG_WRITE(HOST_HOFS1H, MSB(offset));
        SIB_ClearLock(SIB_SMEM_DEV);
        break;

    case SHM_WINDOW_TWO:
        SIB_SetLock(SIB_SMEM_DEV);
        REG_WRITE(HOST_HOFS2L, LSB(offset));
        REG_WRITE(HOST_HOFS2H, MSB(offset));
        SIB_ClearLock(SIB_SMEM_DEV);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
        SIB_SetLock(SIB_ESHM_DEV);
        REG_WRITE(HOST_HOFS3L, LSB(offset));
        REG_WRITE(HOST_HOFS3H, MSB(offset));
        SIB_ClearLock(SIB_ESHM_DEV);
        break;

    case SHM_WINDOW_FOUR:
        SIB_SetLock(SIB_ESHM_DEV);
        REG_WRITE(HOST_HOFS4L, LSB(offset));
        REG_WRITE(HOST_HOFS4H, MSB(offset));
        SIB_ClearLock(SIB_ESHM_DEV);
        break;
#endif  /* SHM_CAPABILITY_EXTENDED_SHM */

    default:
        break;
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableHostOffsetInterrupts                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  mask      -  bit 0 (E_IRQ_CORE_RD_FROM_HOST_OFFSET), when set, Enable IRQ on           */
/*                                      core read from host offset.                                        */
/*                            -  bit 1 (E_IRQ_CORE_WR_TO_CORE_OFFSET), when set, Enable IRQ on             */
/*                                      core write to core offset.                                         */
/*                            -  bit 2,3 not used                                                          */
/*                            -  bit 4 (E_SMI_CORE_RD_FROM_HOST_OFFSET), when set, Enable SIM on           */
/*                                      core read from host offset.                                        */
/*                            -  bit 5 (E_SMI_CORE_WR_TO_CORE_OFFSET), when set, Enable SMI on             */
/*                                      core write to core offset.                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  uses the SIB to access the Host registers in order to enable/disable the Host          */
/*                  IRQ or SMI on core access to one of the pre-configured offsets                         */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableHostOffsetInterrupts (
    SHM_WINDOW_T  window_num,
    UINT8         mask
)
{
    UINT8 cofs_ctl = 0;
#ifdef SHM_CAPABILITY_EXTENDED_SHM
    UINT8 cofse_ctl = 0;
#endif
    UINT8 win_mask = E_IRQ_CORE_RD_FROM_HOST_OFFSET |
                     E_IRQ_CORE_WR_TO_CORE_OFFSET   |
                     E_SMI_CORE_RD_FROM_HOST_OFFSET |
                     E_SMI_CORE_WR_TO_CORE_OFFSET;

    ASSERT(window_num < SHM_NUM_OF_WINDOWS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Lock Host access to the appropriate device; Read appropriate Core Offset Control register           */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (window_num)
    {
    case SHM_WINDOW_ONE:
    case SHM_WINDOW_TWO:
        SIB_SetLock(SIB_SMEM_DEV);
        cofs_ctl = REG_READ(HOST_COFS_CTL);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
    case SHM_WINDOW_FOUR:
        SIB_SetLock(SIB_ESHM_DEV);
        cofse_ctl = REG_READ(HOST_COFSE_CTL);
        break;
#endif

    default:
        break;
    }

    switch (window_num)
    {
    case SHM_WINDOW_ONE:
        CLEAR_VAR_MASK(cofs_ctl, win_mask);
        SET_VAR_MASK(cofs_ctl, mask);
        break;

    case SHM_WINDOW_TWO:
        CLEAR_VAR_MASK(cofs_ctl, (UINT8)(win_mask << 2));
        SET_VAR_MASK(cofs_ctl, (UINT8)(mask << 2));
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
        CLEAR_VAR_MASK(cofse_ctl, win_mask);
        SET_VAR_MASK(cofse_ctl, mask);
        break;

    case SHM_WINDOW_FOUR:
        CLEAR_VAR_MASK(cofse_ctl, (UINT8)(win_mask << 2));
        SET_VAR_MASK(cofse_ctl, (UINT8)(mask << 2));
        break;
#endif  /* SHM_CAPABILITY_EXTENDED_SHM */

    default:
        break;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the Host access lock; Set back the appropriate Core Offset Control register                   */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (window_num)
    {
    case SHM_WINDOW_ONE:
    case SHM_WINDOW_TWO:
        REG_WRITE(HOST_COFS_CTL, cofs_ctl);
        SIB_ClearLock(SIB_SMEM_DEV);
        break;

#ifdef SHM_CAPABILITY_EXTENDED_SHM
    case SHM_WINDOW_THREE:
    case SHM_WINDOW_FOUR:
        REG_WRITE(HOST_COFSE_CTL, cofse_ctl);
        SIB_ClearLock(SIB_ESHM_DEV);
        break;
#endif

    default:
        break;
    }
}

#endif //SHM_CAPABILITY_OFFSET

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_PrintRegs (void)
{
    UINT i;

    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*     SHM      */\n");
    HAL_PRINT("/*--------------*/\n\n");

    HAL_PRINT("SMC_STS             = 0x%02X\n", REG_READ(SMC_STS));
    HAL_PRINT("SMC_CTL             = 0x%02X\n", REG_READ(SMC_CTL));
#ifdef SHM_CAPABILITY_FLASH_ACCESS
    HAL_PRINT("FLASH_SIZE          = 0x%02X\n", REG_READ(FLASH_SIZE));
#else
    HAL_PRINT("SHM_CTL             = 0x%02X\n", REG_READ(SHM_CTL));
#endif
#ifdef SHM_CAPABILITY_FWH
    HAL_PRINT("FWH_ID_LPC          = 0x%02X\n", REG_READ(FWH_ID_LPC));
#endif
#ifdef SHM_CAPABILITY_RAM_WIN_BASE_EXTENSION
    HAL_PRINT("WIN_BA_EXT1         = 0x%02X\n", REG_READ(WIN_BA_EXT(SHM_WINDOW_ONE)));
    HAL_PRINT("WIN_BA_EXT2         = 0x%02X\n", REG_READ(WIN_BA_EXT(SHM_WINDOW_TWO)));
#endif
#ifdef SHM_CAPABILITY_INDIRECT_MEMORY_ACCESS
    HAL_PRINT("IMA_WIN_SIZE        = 0x%02X\n", REG_READ(IMA_WIN_SIZE));
#endif
#ifdef SHM_CAPABILITY_WIN_PROTECTION
    HAL_PRINT("WIN_PROT            = 0x%02X\n", REG_READ(WIN_PROT));
#endif
    HAL_PRINT("WIN_SIZE            = 0x%02X\n", REG_READ(WIN_SIZE));

    for (i = 0; i < SHM_NUM_OF_WINDOWS; i++)
    {
        HAL_PRINT("SHAW%d_SEM           = 0x%02X\n", i, REG_READ(SHAW_SEM(i)));
    }
#ifndef SHM_CAPABILITY_ABSOLUTE_WIN_BASE_ADDR
    for (i = 0; i < SHM_NUM_OF_WINDOWS; i++)
    {
        HAL_PRINT("WIN_BASE%d           = 0x%02X\n", i, REG_READ(WIN_BASE(i)));
    }
#endif
#ifdef SHM_CAPABILITY_INDIRECT_MEMORY_ACCESS
    HAL_PRINT("IMA_SEM             = 0x%02X\n", REG_READ(IMA_SEM));
#endif
    HAL_PRINT("SHCFG               = 0x%04X\n", REG_READ(SHCFG));
#ifdef SHM_CAPABILITY_EXTENDED_8_AREA_PROTECTION
    for (i = 0; i < SHM_NUM_OF_WINDOWS; i++)
    {
        HAL_PRINT("WIN%d_WR_PROT        = 0x%02X\n", i, REG_READ(WIN_WR_PROT(i)));
        HAL_PRINT("WIN%d_RD_PROT        = 0x%02X\n", i, REG_READ(WIN_RD_PROT(i)));
    }
#endif
#ifdef SHM_CAPABILITY_INDIRECT_MEMORY_ACCESS
    HAL_PRINT("IMA_WR_PROT         = 0x%02X\n", REG_READ(IMA_WR_PROT));
    HAL_PRINT("IMA_RD_PROT         = 0x%02X\n", REG_READ(IMA_RD_PROT));
#endif
#ifdef SHM_CAPABILITY_ABSOLUTE_WIN_BASE_ADDR
    for (i = 0; i < SHM_NUM_OF_WINDOWS; i++)
    {
        HAL_PRINT("WIN_BASE%d           = 0x%08X\n", i, REG_READ(WIN_BASE(i)));
    }
#endif
#ifdef SHM_CAPABILITY_INDIRECT_MEMORY_ACCESS
    HAL_PRINT("IMA_BASE            = 0x%08X\n", REG_READ(IMA_BASE));
    HAL_PRINT("RST_CFG             = 0x%02X\n", REG_READ(RST_CFG));
#endif
#ifdef SHM_CAPABILITY_WIN_PROTECTION
    HAL_PRINT("SMCORP0             = 0x%04X\n", REG_READ(SMCORP0));
    HAL_PRINT("SMCORP1             = 0x%04X\n", REG_READ(SMCORP1));
    HAL_PRINT("SMCORP2             = 0x%04X\n", REG_READ(SMCORP2));
    HAL_PRINT("SMCORP3             = 0x%04X\n", REG_READ(SMCORP3));
    HAL_PRINT("SMCOWP0             = 0x%04X\n", REG_READ(SMCOWP0));
    HAL_PRINT("SMCOWP1             = 0x%04X\n", REG_READ(SMCOWP1));
    HAL_PRINT("SMCOWP2             = 0x%04X\n", REG_READ(SMCOWP2));
    HAL_PRINT("SMCOWP3             = 0x%04X\n", REG_READ(SMCOWP3));
    HAL_PRINT("PWIN1_BASEI         = 0x%04X\n", REG_READ(PWIN1_BASEI));
    HAL_PRINT("PWIN1_SIZEI         = 0x%04X\n", REG_READ(PWIN1_SIZEI));
    HAL_PRINT("PWIN2_BASEI         = 0x%04X\n", REG_READ(PWIN2_BASEI));
    HAL_PRINT("PWIN2_SIZEI         = 0x%04X\n", REG_READ(PWIN2_SIZEI));
    HAL_PRINT("PWIN3_BASEI         = 0x%04X\n", REG_READ(PWIN3_BASEI));
    HAL_PRINT("PWIN3_SIZEI         = 0x%04X\n", REG_READ(PWIN3_SIZEI));
    HAL_PRINT("PWIN4_BASEI         = 0x%04X\n", REG_READ(PWIN4_BASEI));
    HAL_PRINT("PWIN4_SIZEI         = 0x%04X\n", REG_READ(PWIN4_SIZEI));
    HAL_PRINT("PWIN5_BASEI         = 0x%04X\n", REG_READ(PWIN5_BASEI));
    HAL_PRINT("PWIN5_SIZEI         = 0x%04X\n", REG_READ(PWIN5_SIZEI));
    HAL_PRINT("PWIN_ULOCK          = 0x%02X\n", REG_READ(PWIN_ULOCK));
    HAL_PRINT("PROT_CFG            = 0x%02X\n", REG_READ(PROT_CFG));
#endif
#ifdef SHM_CAPABILITY_OFFSET
    HAL_PRINT("HOFS_STS            = 0x%02X\n", REG_READ(HOFS_STS));
    HAL_PRINT("HOFS_CTL            = 0x%02X\n", REG_READ(HOFS_CTL));
    for (i = 0; i < SHM_NUM_OF_WINDOWS; i++)
    {
        HAL_PRINT("COFS%d               = 0x%04X\n", i, REG_READ(COFS(i)));
    }
#endif
#ifdef SHM_CAPABILITY_HOST_OFFSET
    for (i = 0; i < SHM_NUM_OF_WINDOWS; i++)
    {
        HAL_PRINT("IHOFS%d              = 0x%04X\n", i, REG_READ(IHOFS(i)));
    }
#endif
#ifdef SHM_CAPABILITY_VERSION
    HAL_PRINT("SHM_VER             = 0x%02X\n", REG_READ(SHM_VER));
#endif
#ifdef SHM_CAPABILITY_EXTENDED_SHM
    HAL_PRINT("SMCE_STS            = 0x%02X\n", REG_READ(SMCE_STS));
    HAL_PRINT("SMCE_CTL            = 0x%02X\n", REG_READ(SMCE_CTL));
    HAL_PRINT("WINE_SIZE           = 0x%02X\n", REG_READ(WINE_SIZE));
    HAL_PRINT("HOFSE_STS           = 0x%02X\n", REG_READ(HOFSE_STS));
    HAL_PRINT("HOFSE_CTL           = 0x%02X\n", REG_READ(HOFSE_CTL));
#endif

#ifdef SHM_CAPABILITY_HW_HOST_EVENT_WIN_3_4
    for (i = 2; i < SHM_NUM_OF_WINDOWS; i++)
    {
        HAL_PRINT("WIN%d_HEVn_STS       = 0x%02X\n", i, REG_READ(HEVn_STS(i)));
        HAL_PRINT("WIN%d_HEVn_EV        = 0x%02X\n", i, REG_READ(HEVn_EV(i)));
        HAL_PRINT("WIN%d_HEVn_IEN       = 0x%02X\n", i, REG_READ(HEVn_IEN(i)));
        HAL_PRINT("WIN%d_HEVn_SEN       = 0x%02X\n", i, REG_READ(HEVn_SEN(i)));
    }
#endif

    HAL_PRINT("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_PrintVersion (void)
{
    HAL_PRINT("SHM         = %X\n", MODULE_VERSION(SHM_MODULE_TYPE));
}

