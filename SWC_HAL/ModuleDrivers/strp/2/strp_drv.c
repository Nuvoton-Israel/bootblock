/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   strp_drv.c                                                               */
/*            This file contains straps and fuse straps access routines       */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#define STRAPS_C

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "hal.h"

#include "strp_drv.h"

#include "../../fuse/3/fuse_regs.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Register access macros                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

#define USE_FUSESTARP()        (READ_REG_FIELD(FUSTRAP, FUSTRAP_USEFUSTRAP))         // Use FUSSTRAP register for startup configuration

#define SECBOOT_ON()           (READ_REG_FIELD(FUSTRAP, FUSTRAP_oSECBOOT))           // Perform a secure boot
#define ECC_IS_ON(straps)      (READ_VAR_FIELD(STRP_Straps, FUSTRAP))                     // Enable ECC for DDR
#define HALT_ON_FAILURE()      (READ_REG_FIELD(FUSTRAP, FUSTRAP_oHLTOF))             // Halt execution of Image verification error
#define KEY_ACCESS_LOCK_ON()   (READ_REG_FIELD(FUSTRAP, FUSTRAP_oAESKEYACCLK))       // instructs to FW to lock keys OTP on sec fail
#define SIG_KEY_SIZE()         (READ_REG_FIELD(FKEYIND, FKEYIND_KSIZE) ? AES_KEY_128 : AES_256) // Retrieves the signature key size from a fuse strap
#define SECEN_ON()             (!READ_REG_FIELD(PWRON, PWRON_STRAP8))

#define SECURE_BOOT_ON()       (SECBOOT_ON())                                        // Boot with security checks


#define SKIP_INIT_MODE()       ((STRP_CLKFRQ_T)READ_REG_FIELD(PWRON, PWRON_STRAP3_1) == STRP_CLKFRQ_OPTION1)

/*---------------------------------------------------------------------------------------------------------*/
/* Module local variables                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  STRP_Straps       = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Function implementation                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_Select                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine get the calculated strap values (either from the fuses or directly        */
/*                  from the straps. This is a singleton.                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void STRP_Select (void)
{

    STRP_Straps = 0;
    if (USE_FUSESTARP())
    {
        // Read startup configuration from FUSTRAP register:
        STRP_Straps = REG_READ(FUSTRAP);  // take from fusetrap: CP_SKIPINIT, CKFRQ

        // Flip all the bits (all configuration is according to FUSTRAP bit polarity)
        STRP_Straps = ~STRP_Straps;

        // set straps values that are not in fuses ( on fusetrap they are marked as reseved):
        SET_VAR_FIELD(STRP_Straps, PWRON_STRAP4    , READ_REG_FIELD(PWRON, PWRON_STRAP4));   // J2EN
        SET_VAR_FIELD(STRP_Straps, PWRON_STRAP6    , READ_REG_FIELD(PWRON, PWRON_STRAP6));   // ECC
        SET_VAR_FIELD(STRP_Straps, PWRON_STRAP7    , READ_REG_FIELD(PWRON, PWRON_STRAP7));   // HIZ
        SET_VAR_FIELD(STRP_Straps, PWRON_STRAP8    , READ_REG_FIELD(PWRON, PWRON_STRAP8));   // SECEN
        SET_VAR_FIELD(STRP_Straps, PWRON_STRAP10_9 , READ_REG_FIELD(PWRON, PWRON_STRAP10_9));// FUP
        SET_VAR_FIELD(STRP_Straps, PWRON_STRAP12   , READ_REG_FIELD(PWRON, PWRON_STRAP12));  // SFAB
        SET_VAR_FIELD(STRP_Straps, PWRON_STRAP13   , READ_REG_FIELD(PWRON, PWRON_STRAP13));  // SPI0F18

    }
    else
    {
        // Read startup configuration from PWRON register:
        STRP_Straps = REG_READ(PWRON);
    }

    // If ROM code mode is set to secureboot (oSecBoot ) but straps are set to skip init mode
    // the ROM ignores clk fuse and set the clk to default. skip init in secire mode means igore fuse clock configuration.
    if ( STRP_SecureBoot_On () == 1 )
    {
        // if it's skipinit - set clk to default (800MHz) regardless of what fusestrap says:
        if (STRP_SkipInit_On())
        {
            SET_VAR_FIELD(STRP_Straps, FUSTRAP_FUSTRAP3_1 , STRP_CLKFRQ_OPTION7);
        }
    }

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_SecureBoot_On                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine  Read the SB strap and oSecboot, as is. without init, which will be done  */
/*                  later on                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN STRP_SecureBoot_On()
{
    return (SECBOOT_ON() );

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_SkipInit_On                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets the skip init mode. It is derived from the clk configuration register*/
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN STRP_SkipInit_On (void)
{
    /* Read the clk strap, as is. without init, which will be done later on */
    return SKIP_INIT_MODE();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_Ecc_On                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets strap 6 (ECC)  value . Used by boot block                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN STRP_Ecc_On (void)
{
    return READ_VAR_FIELD(STRP_Straps, PWRON_STRAP6); //  ECC_IS_ON(STRP_Straps);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_FUP_mode                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets the FUP mode ( Flash update mode)                                    */
/*---------------------------------------------------------------------------------------------------------*/
STRP_FUP_T STRP_FUP_mode (void)
{
    return (STRP_FUP_T)READ_VAR_FIELD(STRP_Straps, PWRON_STRAP10_9); // Specifies Core UART connection to external I/F SI2
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_Ckfrq_Get                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets the clk strap configuration from the pwrn register                   */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 STRP_Ckfrq_Get (void)
{
    return READ_VAR_FIELD(STRP_Straps, PWRON_STRAP3_1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_MemSpeed_Get                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets the memory speed (STRAP6 and GPIO169)                                */
/*---------------------------------------------------------------------------------------------------------*/
STRP_MEM_SPEED_T         STRP_MemSpeed_Get (void)
{

// ECC (strap 6) : GPIO_61

#define BOARD_DRB
#ifdef BOARD_DRB
#define STRP_MEM_SPEED_LOW_GPIO   169

#else  // EB.
//#define STRP_MEM_SPEED_LOW_GPIO   123   /* EB: J23.4  */
#endif


    volatile UINT  highBit = 0;
    volatile UINT  lowBit  = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /*  T.P.: Use strap 6 (ECC) to get the speed. I'm sorry, not my call.                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    highBit = STRP_Ecc_On();

    /*-----------------------------------------------------------------------------------------------------*/
    /*  Config GPIO169 as input and sample it                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_Init(STRP_MEM_SPEED_LOW_GPIO, GPIO_DIR_INPUT, GPIO_PULL_UP, GPIO_OTYPE_PUSH_PULL, FALSE);
    CLK_Delay_MicroSec(10);
    lowBit = GPIO_Read(STRP_MEM_SPEED_LOW_GPIO);


    /*-----------------------------------------------------------------------------------------------------*/
    /*  return the combined value                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    return (STRP_MEM_SPEED_T)((lowBit & (UINT)0x01)  | ((highBit << 1)  & (UINT)0x02)) ;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_get                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  strp    -  strp the to get . Value is not reverted or anything like that               */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets the actual strap value (from the pin strap, not from the fuse        */
/*---------------------------------------------------------------------------------------------------------*/
UINT           STRP_get (STRP_TYPE_T strp)
{
    UINT retVal = 0;
    switch (strp)
    {
        case STRP_1_3_CKFRQ:
            retVal = (STRP_CLKFRQ_T)READ_REG_FIELD(PWRON, PWRON_STRAP3_1);
            break;

        case STRP_4_J2EN:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP4);
            break;

        case STRP_5:
            ASSERT(FALSE) ; // strp 5 is reserved
            break;

        case STRP_6_ECC:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP6);
            break;

        case STRP_7_HIZ:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP7);
            break;

        case STRP_8_SECEN:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP8);
            break;

        case STRP_9_10_FUP:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP10_9);
            break;

        case STRP_11_BSPA:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP11);
            break;

        case STRP_12_SFAB:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP12);
            break;

        case STRP_13_SPI0F18:
            retVal = READ_REG_FIELD(PWRON, PWRON_STRAP13);
            break;
        default:
            ASSERT(FALSE);
            break;
    }

    return retVal;
}


#undef STRAPS_C

