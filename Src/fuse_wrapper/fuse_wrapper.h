/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*                         fuse_wrapper.h                                     */
/* This file contains fuse wrapper implementation. it wraps all access to the otp. */
/* For the user: call FUSE_WRPR_set or FUSE_WRPR_get with the property fields. */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/

#ifndef _FUSE_WRAPPER_
#define _FUSE_WRAPPER_

#include "hal.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Address of fuse elemnt                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FUSE_ADDRESS_START_tag
{
	oFUSTRAP_START                              = 0                     ,    /* Start address of FUSTRAP   */
	oCP_FUSTRAP_START                           = 12                    ,    /* Start address of Coprocessor CP_FUSTRAP   */
	oDAC_CALIB_START                            = 16                    ,    /* Start address of DAC Calibration Word   */
	oADC_CALIB_START                            = 24                    ,    /* Start address of ADC Calibration Word   */
	RESERVED_START                              = 64                    ,    /* Start address of Reserved for Nuvoton use   */
	oDERIVATIVE_START                           = 64                    ,    /* Start address of Derivative Word   */
	oFINAL_TEST_SIGNATURE_START                 = 72                    ,    /* Start address of Final test signature   */
	oDIE_LOCATION_START                         = 74                    ,    /* Start address of Die location: Horizontal   , Vertical, Wafer#   */
	RESERVED1_START                             = 77                    ,    /* Start address of Reserved   */
	oGENERAL_PURPOSE1_START                     = 80                    ,    /* Start address of General Purpose   */
	oGENERAL_PURPOSE2_START                     = 128                   ,    /* Start address of General Purpose   */
	oAESKEY0_START                              = 0                     ,    /* Start address of AES Key 0 (oAESKEY0)   */
	oAESKEY1_START                              = 64                    ,    /* Start address of AES Key 1 (oAESKEY1)   */
	oAESKEY2_START                              = 128                   ,    /* Start address of AES Key 2 (oAESKEY2)   */
	oAESKEY3_START                              = 192                   ,    /* Start address of AES Key 3 (oAESKEY3)   */
} FUSE_ADDRESS_START_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Length of fuse element. This is the full length, not the length after decode which is smaller           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum  FUSE_LENGTH_tag
{
	oFUSTRAP_LENGTH                             = 12                    ,    /* length of OTP field FUSTRAP   */
	oCP_FUSTRAP_LENGTH                          = 4                     ,    /* length of OTP field Coprocessor CP_FUSTRAP   */
	oDAC_CALIB_LENGTH                           = 8                     ,    /* length of OTP field DAC Calibration Word   */
	oADC_CALIB_LENGTH                           = 8                     ,    /* length of OTP field ADC Calibration Word   */
	RESERVED_LENGTH                             = 16                    ,    /* length of OTP field Reserved for Nuvoton use   */
	oDERIVATIVE_LENGTH                          = 8                     ,    /* length of OTP field Derivative Word   */
	oFINAL_TEST_SIGNATURE_LENGTH                = 2                     ,    /* length of OTP field Final test signature   */
	oDIE_LOCATION_LENGTH                        = 3                     ,    /* length of OTP field Die location: Horizontal, Vertical, Wafer#   */
	RESERVED1_LENGTH                            = 3                     ,    /* length of OTP field Reserved   */
	oGENERAL_PURPOSE1_LENGTH                    = 48                    ,    /* length of OTP field General Purpose   */
	oGENERAL_PURPOSE2_LENGTH                    = 128                   ,    /* length of OTP field General Purpose   */
	oAESKEY0_LENGTH                             = 64                    ,    /* length of OTP field AES Key 0 (oAESKEY0)   */
	oAESKEY1_LENGTH                             = 64                    ,    /* length of OTP field AES Key 1 (oAESKEY1)   */
	oAESKEY2_LENGTH                             = 64                    ,    /* length of OTP field AES Key 2 (oAESKEY2)   */
	oAESKEY3_LENGTH                             = 64                    ,    /* length of OTP field AES Key 3 (oAESKEY3)   */
} FUSE_LENGTH_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Type of ECC of each element in otp                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum  FUSE_ECC_TYPES_tag  /* FUSE_ECC_TYPE_T */
{
	oFUSTRAP_ECC                                = FUSE_ECC_MAJORITY      ,    /*  FUSTRAP   */
	oCP_FUSTRAP_ECC                             = FUSE_ECC_MAJORITY      ,    /*  Coprocessor CP_FUSTRAP   */
	oDAC_CALIB_ECC                              = FUSE_ECC_NIBBLE_PARITY ,    /*  DAC Calibration Word   */
	oADC_CALIB_ECC                              = FUSE_ECC_NIBBLE_PARITY ,    /*  ADC Calibration Word   */
	RESERVED_ECC                                = FUSE_ECC_NONE          ,    /*  Reserved for Nuvoton use   */
	oDERIVATIVE_ECC                             = FUSE_ECC_NIBBLE_PARITY ,    /*  Derivative Word   */
	oFINAL_TEST_SIGNATURE_ECC                   = FUSE_ECC_NONE          ,    /*  Final test signature   */
	oDIE_LOCATION_ECC                           = FUSE_ECC_NONE          ,    /*  Die location: Horizontal, Vertical, Wafer#   */
	RESERVED1_ECC                               = FUSE_ECC_NONE          ,    /*  Reserved   */
	oGENERAL_PURPOSE1_NONE                      = FUSE_ECC_NONE          ,    /*  General Purpose   */
	oGENERAL_PURPOSE2_NONE                      = FUSE_ECC_NONE          ,    /*  General Purpose   */
	oAESKEY0_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 0 (oAESKEY0)   */
	oAESKEY1_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 1 (oAESKEY1)   */
	oAESKEY2_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 2 (oAESKEY2)   */
	oAESKEY3_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 3 (oAESKEY3)   */
} FUSE_ECC_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Lacation of each element in otp                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum  FUSE_ARRAY_tag /* FUSE_STORAGE_ARRAY_T */
{
	oFUSTRAP_ARRAY                              =  FUSE_SA              ,  /* FUSTRAP */
	oCP_FUSTRAP_ARRAY                           =  FUSE_SA              ,  /* Coprocessor CP_FUSTRAP */
	oDAC_CALIB_ARRAY                            =  FUSE_SA              ,  /* DAC Calibration Word */
	oADC_CALIB_ARRAY                            =  FUSE_SA              ,  /* ADC Calibration Word */
	RESERVED_ARRAY                              =  FUSE_SA              ,  /* Reserved for Nuvoton use */
	oDERIVATIVE_ARRAY                           =  FUSE_SA              ,  /* Derivative Word */
	oFINAL_TEST_SIGNATURE_ARRAY                 =  FUSE_SA              ,  /* Final test signature */
	oDIE_LOCATION_ARRAY                         =  FUSE_SA              ,  /* Die location: Horizontal                      , Vertical, Wafer# */
	RESERVED1_ARRAY                             =  FUSE_SA              ,  /* Reserved */
	oGENERAL_PURPOSE1_ARRAY                     =  FUSE_SA              ,  /* General Purpose */
	oGENERAL_PURPOSE2_ARRAY                     =  FUSE_SA              ,  /* General Purpose */
	oAESKEY0_ARRAY                              =  KEY_SA               ,  /* AES Key 0 (oAESKEY0) */
	oAESKEY1_ARRAY                              =  KEY_SA               ,  /* AES Key 1 (oAESKEY1) */
	oAESKEY2_ARRAY                              =  KEY_SA               ,  /* AES Key 2 (oAESKEY2) */
	oAESKEY3_ARRAY                              =  KEY_SA               ,  /* AES Key 3 (oAESKEY3) */
} FUSE_ARRAY_T;


/*---------------------------------------------------------------------------------------------------------*/
/* OTP elements. use them in setter in getter below                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define      oFUSTRAP_PROPERTY                                (UINT16)oFUSTRAP_START, (UINT16)oFUSTRAP_LENGTH, oFUSTRAP_ECC, oFUSTRAP_ARRAY
#define      oCP_FUSTRAP_PROPERTY                             (UINT16)oCP_FUSTRAP_START, (UINT16)oCP_FUSTRAP_LENGTH, oCP_FUSTRAP_ECC, oCP_FUSTRAP_ARRAY
#define      oDAC_CALIB_PROPERTY                              (UINT16)oDAC_CALIB_START, (UINT16)oDAC_CALIB_LENGTH, oDAC_CALIB_ECC, oDAC_CALIB_ARRAY
#define      oADC_CALIB_PROPERTY                              (UINT16)oADC_CALIB_START, (UINT16)oADC_CALIB_LENGTH, oADC_CALIB_ECC, oADC_CALIB_ARRAY

#define      RESERVED_PROPERTY                                (UINT16)RESERVED_START, (UINT16)RESERVED_LENGTH, RESERVED_ECC, RESERVED_ARRAY
#define      oDERIVATIVE_PROPERTY                             (UINT16)oDERIVATIVE_START, (UINT16)oDERIVATIVE_LENGTH, oDERIVATIVE_ECC, oDERIVATIVE_ARRAY
#define      oFINAL_TEST_SIGNATURE_PROPERTY                   (UINT16)oFINAL_TEST_SIGNATURE_START, (UINT16)oFINAL_TEST_SIGNATURE_LENGTH, oFINAL_TEST_SIGNATURE_ECC, oFINAL_TEST_SIGNATURE_ARRAY
#define      oDIE_LOCATION_PROPERTY                           (UINT16)oDIE_LOCATION_START, (UINT16)oDIE_LOCATION_LENGTH, oDIE_LOCATION_ECC, oDIE_LOCATION_ARRAY
#define      RESERVED1_PROPERTY                               (UINT16)RESERVED1_START, (UINT16)RESERVED1_LENGTH, RESERVED1_ECC, RESERVED1_ARRAY

#define      oGENERAL_PURPOSE1_PROPERTY                       (UINT16)oGENERAL_PURPOSE1_START, (UINT16)oGENERAL_PURPOSE1_LENGTH, oGENERAL_PURPOSE1_NONE, oGENERAL_PURPOSE1_ARRAY
#define      oGENERAL_PURPOSE2_PROPERTY                       (UINT16)oGENERAL_PURPOSE2_START, (UINT16)oGENERAL_PURPOSE2_LENGTH, oGENERAL_PURPOSE2_NONE, oGENERAL_PURPOSE2_ARRAY


#define      oAESKEY0_PROPERTY                                (UINT16)oAESKEY0_START, (UINT16)oAESKEY0_LENGTH, oAESKEY0_ECC, oAESKEY0_ARRAY
#define      oAESKEY1_PROPERTY                                (UINT16)oAESKEY1_START, (UINT16)oAESKEY1_LENGTH, oAESKEY1_ECC, oAESKEY1_ARRAY
#define      oAESKEY2_PROPERTY                                (UINT16)oAESKEY2_START, (UINT16)oAESKEY2_LENGTH, oAESKEY2_ECC, oAESKEY2_ARRAY
#define      oAESKEY3_PROPERTY                                (UINT16)oAESKEY3_START, (UINT16)oAESKEY3_LENGTH, oAESKEY3_ECC, oAESKEY3_ARRAY



/*---------------------------------------------------------------------------------------------------------*/
/* Getter for otp elements                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length, FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value);

/*---------------------------------------------------------------------------------------------------------*/
/* Getter for cp fustrp. It has a design bug. 10 bits instead of 16 :)                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get_CP_Fustrap (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length,   FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value);

#endif // _FUSE_WRAPPER_
