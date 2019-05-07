/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   fuse_drv.c                                                               */
/*            This file contains FUSE module routines                         */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#include __CHIP_H_FROM_DRV()

#include "hal.h"

#if defined AES_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_DRV(aes)
#endif

#if defined RSA_MODULE_TYPE
#include __LOGICAL_IF_HEADER_FROM_DRV(rsa)
#endif


#include "fuse_drv.h"
#include "fuse_regs.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Fuse module constant definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/


// Read cycle initiation value:
#define READ_INIT                   0x02

// Program cycle initiation values (a sequence of two adjacent writes is required):
#define PROGRAM_ARM                 0x1
#define PROGRAM_INIT                0xBF79E5D0

// Value to clean FDATA contents:
#define FDATA_CLEAN_VALUE           0x01

// Default APB Clock Rate (in MHz):
#define DEFAULT_APB_RATE            0x30

#define MIN_PROGRAM_PULSES          4
#define MAX_PROGRAM_PULSES          20


/*---------------------------------------------------------------------------------------------------------*/
/* Fuse module local macro definitions                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
// #define STORAGE_ARRAY_READY(sa)   READ_REG_FIELD(FST(sa),     FST_RDY)
#define KEY_IS_VALID()            READ_REG_FIELD(FKEYIND,     FKEYIND_KVAL)
#define DISABLE_KEY_ACCESS()      SET_REG_FIELD(FCFG(KEY_SA), FCFG_FDIS, 1)     /* Lock OTP module access  */


/*---------------------------------------------------------------------------------------------------------*/
/* Internal functions for this module                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_WaitForOTPReadyWithTimeout                                                        */
/*                                                                                                         */
/* Parameters:      array - fuse array to wait for                                                         */
/* Returns:         DEFS_STATUS_OK on successful read completion, DEFS_ERROR_* otherwise.                   */
/* Side effects:                                                                                           */
/* Description:     Initialize the Fuse HW module.                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WaitForOTPReadyWithTimeout(FUSE_STORAGE_ARRAY_T array, UINT32 timeout)
{
    volatile UINT32 time = timeout;

    /*-----------------------------------------------------------------------------------------------------*/
    /* check parameters validity                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK (array <= FUSE_SA , DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

    while (--time > 1)
    {
        if (READ_REG_FIELD(FST(array), FST_RDY))
        {
            /* fuse is ready, clear the status. */
            SET_REG_FIELD(FST(array), FST_RDST, 1);

            return DEFS_STATUS_OK;
        }
    }
    /* try to clear the status in case it was set */
    SET_REG_FIELD(FST(array), FST_RDST, 1);

    return DEFS_STATUS_HARDWARE_ERROR;
}

/*---------------------------------------------------------------------------------------------------------*/
/* HW level functions                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_Init                                                                              */
/*                                                                                                         */
/* Parameters:      APBclock - APB clock rate in MHz                                                       */
/* Returns:         DEFS_STATUS_OK on successful read completion, DEFS_ERROR_* otherwise.                   */
/* Side effects:                                                                                           */
/* Description:     Initialize the Fuse HW module.                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_Init (UINT8 APBclock)
{

    // APBRT (APB Clock Rate). Informs the fuse array state machine on the APB clock rate in MHz. The
    // software must update this field before writing the OTP, and before APB4 actual clock rate change. The
    // state machine contains an internal copy of this field, sampled at the beginning of every read or program
    // operation. Software should not write this field with 0. The reset value of this field is 1Fh (31 MHz). The
    // accuracy of the setting should be 10%.
    // Note: The minimum APB allowed frequency for accessing the fuse arrays is 10 MHz.

    DEFS_STATUS_COND_CHECK(APBclock > 0, DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

    /* Configure the Key Storage Array APB Clock Rate */
    SET_REG_FIELD(FCFG(KEY_SA), FCFG_APBRT, APBclock & 0x3F);

    /* Configure the Fuse Storage Array APB Clock Rate */
    SET_REG_FIELD(FCFG(FUSE_SA), FCFG_APBRT, APBclock & 0x3F);

    return DEFS_STATUS_OK;


}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_Read                                                                              */
/*                                                                                                         */
/* Parameters:      arr  - Storage Array type [input].                                                     */
/*                  addr - Byte-address to read from [input].                                              */
/*                  data - Pointer to result [output].                                                     */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     Read 8-bit data from an OTP storage array.                                             */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_Read (FUSE_STORAGE_ARRAY_T arr,
                UINT16               addr,
                UINT8               *data
)
{
    /* Wait for the Fuse Box Idle */
    FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); // TODO: decide proper timeout

    /* Configure the byte address in the fuse array for read operation */
    SET_REG_FIELD(FADDR(arr), FADDR_BYTEADDR, addr);

    /* Initiate a read cycle from the byte in the fuse array, pointed by FADDR */
    REG_WRITE(FCTL(arr),  READ_INIT);

    /* Wait for read operation completion */
    FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); // TODO: decide proper timeout

    /* Read the result */
    *data = READ_REG_FIELD(FDATA(arr), FDATA_FDATA);

    /* Clean FDATA contents to prevent unauthorized software from reading sensitive information */
    SET_REG_FIELD(FDATA(arr), FDATA_FDATA, FDATA_CLEAN_VALUE);
}


#if (defined RSA_MODULE_TYPE) || (defined AES_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_SelectKey                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  keyIndex - AES key index in the key array (in 128-bit steps) [input].                  */
/* Returns:         DEFS_STATUS_OK on successful read completion, DEFS_STATUS_ERROR* otherwise.            */
/* Side effects:                                                                                           */
/* Description:     Select a key from the key storage array.                                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_SelectKey (
    UINT8            keyIndex  )
{

    UINT32  fKeyInd = 0;
    volatile UINT32 time = 0xDAEDBEEF;

    /* 4 AES keys */
    DEFS_STATUS_COND_CHECK (keyIndex < 4 , DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

    /* Do not destroy ECCDIS bit */
    fKeyInd = REG_READ(FKEYIND);

    /* Configure the key size */
    SET_VAR_FIELD(fKeyInd, FKEYIND_KSIZE, FKEYIND_KSIZE_VALUE_256);

    /* Configure the key index (0 to 3) */
    SET_VAR_FIELD(fKeyInd, FKEYIND_KIND, keyIndex);

    REG_WRITE(FKEYIND, fKeyInd);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Wait for selection completetion                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    while (--time > 1)
    {
        if (READ_REG_FIELD(FKEYIND, FKEYIND_KVAL))
            return DEFS_STATUS_OK;
    }

    return DEFS_STATUS_HARDWARE_ERROR;



}

#if defined (AES_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_UploadKey                                                                         */
/*                                                                                                         */
/* Parameters:      keySize  - AES key size [input].                                                       */
/*                  keyIndex - AES key index in the key array (in 128-bit steps) [input].                  */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     Upload key from key array OTP to AES engine through side-band port.                    */
/*                  The function assumes that the AES engine is IDLE.                                      */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_UploadKey (
    AES_KEY_SIZE_T keySize,
    UINT8          keyIndex
)
{
    UINT32  fKeyInd = 0;

    /* Do not destroy ECCDIS bit */
    fKeyInd = REG_READ(FKEYIND);

    /* Configure the key size */
    SET_VAR_FIELD(fKeyInd, FKEYIND_KSIZE, keySize);

    /* Activate the key (if not, key is considered as a 0-bit key, sideband port is reset) */
    // TODO: SET_VAR_FIELD(fKeyInd, FKEYIND_KACTIVE, 1);

    /* Configure the index of the key in the fuse array in 128-bit steps */
    SET_VAR_FIELD(fKeyInd, FKEYIND_KIND, keyIndex);

    /* FKEYIND must be configured in one operation */
    REG_WRITE(FKEYIND, fKeyInd);

    /* Wait for upload operation completion */
    while (! KEY_IS_VALID());

}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_ReadKey                                                                           */
/*                                                                                                         */
/* Parameters:      keySize  - AES key size [input].                                                       */
/*                  keyIndex - AES key index in the key array (in 128-bit steps) [input].                  */
/*                  output   - Byte-array to hold the key [output].                                        */
/* Returns:         DEFS_STATUS_OK on successful read completion, HAL_ERROR_* otherwise.                   */
/* Side effects:                                                                                           */
/* Description:     Read a key from the key storage array.                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_ReadKey (
    FUSE_KEY_TYPE_T  keyType,
    UINT8            keyIndex,
    UINT8            *output)
{
    UINT  i;
    UINT16 addr = 0 ;
    UINT16 keySizeByte = 0;
    UINT8 fuseReadLock, keyBlockMask = 0;

    /* Read the Fuse Read Lock field */
    fuseReadLock = READ_REG_FIELD(FCFG(KEY_SA), FCFG_FRDLK);

    //AES_KEY_SIZE_256

    /*-----------------------------------------------------------------------------------------------------*/
    /* keyType can be either AES (4 keys, 32 bytes - AES_KEY_SIZE_256) or RSA (2 keys , 256 bytes)         */
    /*-----------------------------------------------------------------------------------------------------*/
#if defined ( AES_MODULE_TYPE )
    if ( keyType == FUSE_KEY_AES)
    {
        keyBlockMask = 3;
        keySizeByte = AES_KEY_SIZE_256 >> 3;
    }
#endif

#if defined ( RSA_MODULE_TYPE )
    if (keyType == FUSE_KEY_RSA)
    {
        keyBlockMask = 3;
        keySizeByte = RSA_KEY_BYTE_SIZE;
    }
#endif

    /* Build the key block mask based on key index */
    keyBlockMask <<= keyIndex;

    /* Verify that APB Read Access to the Key Storage Array is allowed */
    if (READ_REG_FIELD(FUSTRAP, FUSTRAP_oAESKEYACCLK) ||
        READ_VAR_MASK(fuseReadLock, keyBlockMask))
    {
        return DEFS_STATUS_FAIL;
    }

    /* Calculate key size in bytes */
    //TODO:   keySizeByte = AES_KEY_BYTE_SIZE(keySize);

    /* Calculate key start address in Key Storage Array -
       Note that a key always starts on a 128-bit (i.e., a complete block) boundary */
    //TODO:   addr = keyIndex * AES_BLOCK_SIZE;

    /* Read Key */
    for (i = 0; i < keySizeByte; i++, addr++)
    {
        FUSE_Read(KEY_SA, addr, &output[i]);
    }
    return DEFS_STATUS_OK;
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_DisableKeyAccess                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disables read and write accees to key array                               */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_DisableKeyAccess ()
{
    DISABLE_KEY_ACCESS();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_LockAccess                                                                        */
/*                                                                                                         */
/* Parameters:      lockForRead: bitwise, which block to lock for reading                                  */
/* Parameters:      lockForWrite: bitwise, which block to lock for program                                 */
/* Returns:         DEFS_STATUS_OK on successful read completion, HAL_ERROR_* otherwise.                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine lock the otp blocks                                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_LockAccess (FUSE_STORAGE_ARRAY_T array, UINT8 lockForRead, UINT8 lockForWrite, BOOLEAN lockRegister)
{

    UINT32 FCFG_VAR = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* check parameters validity                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK (array <= FUSE_SA , DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read reg for modify all fields apart APBRT                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    FCFG_VAR = REG_READ(FCFG(array));


    SET_VAR_FIELD(FCFG_VAR, FCFG_FRDLK,  lockForRead & 0x00FF);

    SET_VAR_FIELD(FCFG_VAR, FCFG_FPRGLK, lockForWrite & 0x00FF);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Lock any access to this register (until next POR)                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    if ( lockRegister == TRUE)
    {
        SET_VAR_FIELD(FCFG_VAR, FCFG_FCFGLK, (lockForWrite | lockForRead) & 0x00FF);
    }
    else
    {
		SET_VAR_FIELD(FCFG_VAR, FCFG_FCFGLK, 0);
    }

     /*----------------------------------------------------------------------------------------------------*/
     /* Lock the side band in case it's a key array, and read is locked                                    */
     /*----------------------------------------------------------------------------------------------------*/
    if ( array == KEY_SA)
    {
        /* Set FDIS bit if oKAP bit 7 is set, to disable the side-band key loading. */
        if ( (lockForRead & 0x80) > 0 )
        {
            SET_VAR_FIELD(FCFG_VAR, FCFG_FDIS, 1);  // 1: Access to the first 2048 bits of the fuse array is disabled.
        }
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Return the moified value                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(FCFG(array), FCFG_VAR);

    return DEFS_STATUS_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Logical level functions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_NibParEccDecode                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  datain -       pointer to encoded data buffer (buffer size should be 2 x dataout)      */
/*                  dataout -      pointer to decoded data buffer                                          */
/*                  encoded_size - size of encoded data (decoded data x 2)                                 */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on successful read completion, HAL_ERROR_* otherwise.                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Decodes the data according to nibble parity ECC scheme.                                */
/*                  Size specifies the encoded data size.                                                  */
/*                  Decodes whole bytes only                                                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_NibParEccDecode (
    UINT8  *datain,
    UINT8  *dataout,
    UINT32  encoded_size
)
{
    UINT32 i;
    UINT8 BER;
    UINT8 src_re_calc;
    UINT8 dst;
    UINT8 E0;
    UINT8 E1;
    UINT8 E2;
    UINT8 E3;
    UINT8 E4;
    UINT8 E5;
    UINT8 E6;
    UINT8 E7;
    DEFS_STATUS status = DEFS_STATUS_OK;

//Define the Bit Field macros in order to use the SET_VAR_FIELD macro:
#define BITF0   0, 1
#define BITF1   1, 1
#define BITF2   2, 1
#define BITF3   3, 1
#define BITF4   4, 1
#define BITF5   5, 1
#define BITF6   6, 1
#define BITF7   7, 1

#define LSNF    0, 4
#define MSNF    4, 4

    for (i = 0; i < encoded_size; i++)
    {
        E0 = READ_VAR_FIELD(datain[i], BITF0);
        E1 = READ_VAR_FIELD(datain[i], BITF1);
        E2 = READ_VAR_FIELD(datain[i], BITF2);
        E3 = READ_VAR_FIELD(datain[i], BITF3);
        E4 = READ_VAR_FIELD(datain[i], BITF4);
        E5 = READ_VAR_FIELD(datain[i], BITF5);
        E6 = READ_VAR_FIELD(datain[i], BITF6);
        E7 = READ_VAR_FIELD(datain[i], BITF7);

        if (i % 2)
        {//Decode higher nibble
            SET_VAR_FIELD(dataout[i/2], BITF4, ((E0 & (E1 ^ E4)) | (E0 & (E2 ^ E6)) | ((E1 ^ E4) & (E2 ^ E6))));
            SET_VAR_FIELD(dataout[i/2], BITF5, ((E1 & (E0 ^ E4)) | (E1 & (E3 ^ E7)) | ((E0 ^ E4) & (E3 ^ E7))));
            SET_VAR_FIELD(dataout[i/2], BITF6, ((E2 & (E0 ^ E6)) | (E2 & (E3 ^ E5)) | ((E0 ^ E6) & (E3 ^ E5))));
            SET_VAR_FIELD(dataout[i/2], BITF7, ((E3 & (E2 ^ E5)) | (E3 & (E1 ^ E7)) | ((E2 ^ E5) & (E1 ^ E7))));

            dst = MSN( dataout[i/2] );
        }
        else
        {//Decode lower nibble
            SET_VAR_FIELD(dataout[i/2], BITF0, ((E0 & (E1 ^ E4)) | (E0 & (E2 ^ E6)) | ((E1 ^ E4) & (E2 ^ E6))));
            SET_VAR_FIELD(dataout[i/2], BITF1, ((E1 & (E0 ^ E4)) | (E1 & (E3 ^ E7)) | ((E0 ^ E4) & (E3 ^ E7))));
            SET_VAR_FIELD(dataout[i/2], BITF2, ((E2 & (E0 ^ E6)) | (E2 & (E3 ^ E5)) | ((E0 ^ E6) & (E3 ^ E5))));
            SET_VAR_FIELD(dataout[i/2], BITF3, ((E3 & (E2 ^ E5)) | (E3 & (E1 ^ E7)) | ((E2 ^ E5) & (E1 ^ E7))));

            dst = LSN( dataout[i/2] );
        }


        /*-------------------------------------------------------------------------------------------------*/
        /* calculate the encoded value back from the decoded value and compare the original value for      */
        /* comparison                                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        /* Take decode byte*/
        src_re_calc = dst;

        /* calc its' parity */
        E0 = READ_VAR_FIELD(dst, BITF0);
        E1 = READ_VAR_FIELD(dst, BITF1);
        E2 = READ_VAR_FIELD(dst, BITF2);
        E3 = READ_VAR_FIELD(dst, BITF3);

        SET_VAR_FIELD(src_re_calc, BITF4, E0 ^ E1);
        SET_VAR_FIELD(src_re_calc, BITF5, E2 ^ E3);
        SET_VAR_FIELD(src_re_calc, BITF6, E0 ^ E2);
        SET_VAR_FIELD(src_re_calc, BITF7, E1 ^ E3);

        /*-------------------------------------------------------------------------------------------------*/
        /* Check that only one bit is corrected per byte                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        BER = src_re_calc ^ datain[i];

        BER =    READ_VAR_FIELD(BER, BITF0)
               + READ_VAR_FIELD(BER, BITF1)
               + READ_VAR_FIELD(BER, BITF2)
               + READ_VAR_FIELD(BER, BITF3)
               + READ_VAR_FIELD(BER, BITF4)
               + READ_VAR_FIELD(BER, BITF5)
               + READ_VAR_FIELD(BER, BITF6)
               + READ_VAR_FIELD(BER, BITF7);

        /*-------------------------------------------------------------------------------------------------*/
        /* Bit Error Rate can be 0x00 (no change) or 0x01 0x02 0x04 0x08 -> one bit change only            */
        /*-------------------------------------------------------------------------------------------------*/
        if ( BER > 1 )
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Use original nible :                                                                        */
            /*---------------------------------------------------------------------------------------------*/
            if (i % 2)
            { // copy lower nibble to higher nibble
                SET_VAR_FIELD(dataout[i/2], MSNF, LSN( datain[i] ));

            }
            else
            { // copy lower nibble to lower nibble
                SET_VAR_FIELD(dataout[i/2], LSNF, LSN( datain[i] ) );
            }

            status = DEFS_STATUS_BAD_CHECKSUM;
        }


    }

    return status;



}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_MajRulEccDecode                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  datain -       pointer to encoded data buffer (buffer size should be 3 x dataout)      */
/*                  dataout -      pointer to decoded data buffer                                          */
/*                  encoded_size - size of encoded data (decoded data x 3)                                 */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on successful read completion, DEFS_ERROR_* otherwise.                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Decodes the data according to Major Rule ECC scheme.                                   */
/*                  Size specifies the encoded data size.                                                  */
/*                  Decodes whole bytes only                                                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_MajRulEccDecode (
    UINT8 *datain,
    UINT8 *dataout,
    UINT32 encoded_size
)
{
    UINT  byte;
    UINT  bit;
    UINT8 E1, E2, E3;
	UINT32 decoded_size;

    if (encoded_size % 3)
        // return DEFS_STATUS_INVALID_PARAMETER;
        encoded_size -= encoded_size % 3;

    decoded_size = encoded_size/3;

    for (byte = 0; byte < decoded_size; byte++)
    {
        for (bit = 0; bit < 8; bit++)
        {
            E1 = READ_VAR_BIT(datain[decoded_size*0+byte], bit);
            E2 = READ_VAR_BIT(datain[decoded_size*1+byte], bit);
            E3 = READ_VAR_BIT(datain[decoded_size*2+byte], bit);
            if ((E1+E2+E3) >= 2)
            {
                SET_VAR_BIT(dataout[byte], bit);    //Majority is 1
            }
            else
            {
                 CLEAR_VAR_BIT(dataout[byte], bit); //Majority is 0
            }
        }//Inner for (bit)
    }//Outer for (byte)

    return DEFS_STATUS_OK;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_ReadFustrap                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  oFuse - fuse value to read                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This is a getter for fustrap                                                           */
/*---------------------------------------------------------------------------------------------------------*/
UINT FUSE_Fustrap_Get (FUSE_FUSTRAP_FIELDS_T oFuse)
{
    UINT retVal = 0;
    switch (oFuse)
    {
        case FUSE_FUSTRAP_oWDEN:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oWDEN);
            break;

        case FUSE_FUSTRAP_oHLTOF:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oHLTOF);
            break;

        case FUSE_FUSTRAP_oAESKEYACCLK:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oAESKEYACCLK);
            break;

        case FUSE_FUSTRAP_oJDIS:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oJDIS);
            break;

        case FUSE_FUSTRAP_oSECBOOT:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oSECBOOT);
            break;

        case FUSE_FUSTRAP_USEFUSTRAP:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_USEFUSTRAP);
            break;

        case FUSE_FUSTRAP_oAltImgLoc:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oAltImgLoc);
            break;
        case FUSE_FUSTRAP_oCPU1STOP2:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oCPU1STOP2);
            break;

        case FUSE_FUSTRAP_oCPU1STOP1:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oCPU1STOP1);
            break;

        case FUSE_FUSTRAP_oHINDDIS:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oHINDDIS);
            break;


        default:
            ASSERT(FALSE);
            break;
    }

    return retVal;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_PrintRegs (void)
{
    UINT i;

    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*     FUSE     */\n");
    HAL_PRINT("/*--------------*/\n\n");

    for (i = 0; i <= FUSE_SA; i++)
    {
        FUSE_PrintModuleRegs((FUSE_STORAGE_ARRAY_T)i);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_PrintModuleRegs                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  array - The Storage Array type module to be printed.                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module instance registers                                      */
/*lint -e{715}      Supress 'array' not referenced                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_PrintModuleRegs (FUSE_STORAGE_ARRAY_T array)
{
    ASSERT(array <= FUSE_SA);

    HAL_PRINT("FUSE%1X:\n", (array+1));
    HAL_PRINT("------\n");
    HAL_PRINT("FST%d                = 0x%08X\n", (array+1), REG_READ(FST(array)));
    HAL_PRINT("FADDR%d              = 0x%08X\n", (array+1), REG_READ(FADDR(array)));
    HAL_PRINT("FDATA%d              = 0x%08X\n", (array+1), REG_READ(FDATA(array)));
    HAL_PRINT("FCFG%d               = 0x%08X\n", (array+1), REG_READ(FCFG(array)));

    if (array == KEY_SA)
    {
        HAL_PRINT("FKEYIND%d             = 0x%08X\n", (array+1), REG_READ(FKEYIND));
    }
    else
    {
        HAL_PRINT("FUSTRAP%d             = 0x%08X\n", (array+1), REG_READ(FUSTRAP));
    }

    HAL_PRINT("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_PrintVersion                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_PrintVersion (void)
{
    HAL_PRINT("FUSE        = %d\n", FUSE_MODULE_TYPE);
}


