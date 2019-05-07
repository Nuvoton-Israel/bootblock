/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*                         fuse_wrapper.c                                     */
/* This file contains fuse wrapper implementation. it wraps all access to the otp */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/


#include "Apps/serial_printf/serial_printf.h"
#include "fuse_wrapper.h"

/*---------------------------------------------------------------------------------------------------------*/
/* This global array is used to host the full encloded key                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define        FUSE_ARRAY_MAX_SIZE    512
static UINT8   fuse_encoded[FUSE_ARRAY_MAX_SIZE] = {0};



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_WRPR_get                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fuse_address    -   address in the fuse\key array.                                     */
/*                  fuse_length     -   length in bytes inside the fuse array (before encoding)            */
/*                  fuse_ecc        -   nible parity\majority\none                                         */
/*                  array           -   key array\fuse array                                               */
/*                  value           -                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine read a value from the fuses.                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length,   FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value)
{
    UINT16 iCnt = 0;

    DEFS_STATUS status = DEFS_STATUS_OK;

    DEFS_STATUS_COND_CHECK(fuse_length <= FUSE_ARRAY_MAX_SIZE, DEFS_STATUS_INVALID_PARAMETER);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read the fuses                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    // if no parity
    if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_NONE )
    {
        for (iCnt = 0; iCnt < fuse_length ; iCnt++)
        {
            FUSE_Read((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , &value[iCnt]);
        }
    }
    else
    {
        for (iCnt = 0; iCnt < fuse_length ; iCnt++)
        {
            FUSE_Read((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , &fuse_encoded[iCnt]);
        }

    }



    /*-----------------------------------------------------------------------------------------------------*/
    /* Either encode the data or read it as is                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_NIBBLE_PARITY)
    {
        status = FUSE_NibParEccDecode(fuse_encoded, value, fuse_length);
    }

    if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_MAJORITY)
    {
        status = FUSE_MajRulEccDecode(fuse_encoded, value, fuse_length);
    }


    return status;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_WRPR_get_CP_Fustrap                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fuse_address    -                                                                      */
/*                  fuse_length     -   length in bytes inside the fuse array (before encoding)            */
/*                  fuse_ecc        -   nible parity\majority\none                                         */
/*                  array           -   key array\fuse array                                               */
/*                  value           -                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine read a value from the fuses. It's for a field that is ten bit :(          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get_CP_Fustrap (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length,   FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value)
{

    /* Due to architecture bug , CP_FUSTRAP is only 10 bits. In order not to change the rest of the FUSE_Wrapper, this code is set seperetly */

    UINT16 iCnt = 0;
    UINT  bit;
    UINT8 E1, E2, E3;


    DEFS_STATUS_COND_CHECK(fuse_length <= FUSE_ARRAY_MAX_SIZE, DEFS_STATUS_INVALID_PARAMETER);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read the fuses                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    for (iCnt = 0; iCnt < fuse_length ; iCnt++)
    {
        FUSE_Read((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , &fuse_encoded[iCnt]);
    }


    for (bit = 0; bit < 10; bit++)
    {
        E1 = READ_VAR_BIT(fuse_encoded[(( 0 + bit ) / 8)], (( 0 + bit ) % 8));
        E2 = READ_VAR_BIT(fuse_encoded[((10 + bit ) / 8)], ((10 + bit ) % 8));
        E3 = READ_VAR_BIT(fuse_encoded[((20 + bit ) / 8)], ((20 + bit ) % 8));
        if ((E1+E2+E3) >= 2)
        {
            SET_VAR_BIT(value[(bit / 8)], (bit % 8));    // Majority is 1
        }
        else
        {
             CLEAR_VAR_BIT(value[(bit / 8)], (bit % 8));  // Majority is 0
        }
    }//Inner for (bit)


    return DEFS_STATUS_OK;

 }





