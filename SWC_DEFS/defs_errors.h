/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   defs_errors.h                                                            */
/*            This file contains NTIL standard error codes set                */
/* Project:                                                                   */
/*            SWC DEFS                                                        */
/*----------------------------------------------------------------------------*/

#ifndef __ERRORS_H__
#define __ERRORS_H__


/*---------------------------------------------------------------------------------------------------------*/
/*                                               ERROR CODES                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
#if defined LARGE_HAMMING_DISTANCE
   DEFS_STATUS_OK                                = 0xA5A5,
   DEFS_STATUS_FAIL                              = 0x5A5A,
#else
   DEFS_STATUS_OK                                = 0x00,
   DEFS_STATUS_FAIL                              = 0x01,
#endif
   DEFS_STATUS_ABORTED                           = 0x02,

   /*------------------------------------------------------------------------------------------------------*/
   /* Parameters validity                                                                                  */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_INVALID_PARAMETER                 = 0x10,

   DEFS_STATUS_INVALID_DATA_SIZE                 = 0x11,
   DEFS_STATUS_PARAMETER_OUT_OF_RANGE            = 0x12,
   DEFS_STATUS_INVALID_DATA_FIELD                = 0x13,

   /*------------------------------------------------------------------------------------------------------*/
   /* Response                                                                                             */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_RESPONSE_CANT_BE_PROVIDED         = 0x20,

   DEFS_STATUS_SYSTEM_BUSY                       = 0x21,
   DEFS_STATUS_SYSTEM_NOT_INITIALIZED            = 0x22,
   DEFS_STATUS_SYSTEM_IN_INCORRECT_STATE         = 0x23,
   DEFS_STATUS_RESPONSE_TIMEOUT                  = 0x24,
   DEFS_STATUS_RESPONSE_ABORT                    = 0x25,
   DEFS_STATUS_BAD_CHECKSUM                      = 0x26,

   /*------------------------------------------------------------------------------------------------------*/
   /* Security                                                                                             */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_SECURITY_ERROR                    = 0x30,

   DEFS_STATUS_UNSUFFIENT_PRIVILEDGE_LEVEL       = 0x31,
   DEFS_STATUS_AUTHENTICATION_FAIL               = 0x32,
   DEFS_STATUS_BAD_SIGNATURE                     = 0x33,

   /*------------------------------------------------------------------------------------------------------*/
   /* Communication                                                                                        */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_COMMUNICATION_ERROR               = 0x40,

   DEFS_STATUS_NO_CONNECTION                     = 0x41,
   DEFS_STATUS_CANT_OPEN_CONNECTION              = 0x42,
   DEFS_STATUS_CONNECTION_ALREADY_OPEN           = 0x43,

   /*------------------------------------------------------------------------------------------------------*/
   /* Hardware                                                                                             */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_HARDWARE_ERROR                    = 0x50,

   DEFS_STATUS_IO_ERROR                          = 0x51,
   DEFS_STATUS_CLK_ERROR                         = 0x52,

   /*------------------------------------------------------------------------------------------------------*/
   /* Resource Allocation                                                                                  */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_RESOURCE_ERROR                    = 0x60,

   DEFS_STATUS_ALLOCATION_FAILED                 = 0x61,

   /*------------------------------------------------------------------------------------------------------*/
   /* Custom error codes                                                                                   */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_CUSTOM_ERROR_00                   = 0xF0,
   DEFS_STATUS_CUSTOM_ERROR_01                   = 0xF1,
   DEFS_STATUS_CUSTOM_ERROR_02                   = 0xF2,
   DEFS_STATUS_CUSTOM_ERROR_03                   = 0xF3,
   DEFS_STATUS_CUSTOM_ERROR_04                   = 0xF4,
   DEFS_STATUS_CUSTOM_ERROR_05                   = 0xF5,
   DEFS_STATUS_CUSTOM_ERROR_06                   = 0xF6,
   DEFS_STATUS_CUSTOM_ERROR_07                   = 0xF7,
   DEFS_STATUS_CUSTOM_ERROR_08                   = 0xF8,
   DEFS_STATUS_CUSTOM_ERROR_09                   = 0xF9,
   DEFS_STATUS_CUSTOM_ERROR_10                   = 0xFA,
   DEFS_STATUS_CUSTOM_ERROR_11                   = 0xFB,
   DEFS_STATUS_CUSTOM_ERROR_12                   = 0xFC,
   DEFS_STATUS_CUSTOM_ERROR_13                   = 0xFD,
   DEFS_STATUS_CUSTOM_ERROR_14                   = 0xFE,
   DEFS_STATUS_CUSTOM_ERROR_15                   = 0xFF
} DEFS_STATUS;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              ERROR MACROS                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_CHECK                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - Function to check                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks if given function returns DEFS_STATUS error, and returns the error   */
/*                  immediately                                                                            */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    DEFS_STATUS myFunc(INT p1, INT p2);                                                                  */
/*                                                                                                         */
/*    DEFS_STATUS otherFunc                                                                                */
/*    {                                                                                                    */
/*        ...                                         // Some code                                         */
/*        DEFS_STATUS_RET_CHECK(myFunc(p1,p2));       // Executing myFunc                                  */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK(func)                                 \
{                                                                   \
    DEFS_STATUS ret;                                                \
                                                                    \
    if ((ret = func) != DEFS_STATUS_OK)                             \
    {                                                               \
       return ret;                                                  \
    }                                                               \
}


/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_CHECK_ACTION                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - Function to check                                                            */
/*                  action  - Action done if function doesn't return DEFS_STATUS_OK                        */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks if given function returns DEFS_STATUS error, if not in performs      */
/*                  a pre-defined action                                                                   */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    DEFS_STATUS myFunc(INT p1, INT p2);                                                                  */
/*                                                                                                         */
/*    status myFirstFunc(void* ptr)                                                                        */
/*    {                                                                                                    */
/*        ...                                                // Some code                                  */
/*        DEFS_STATUS_RET_CHECK(myFunc(p1,p2), break);       // Executing myFunc, doing action on failure  */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK_ACTION(func, action)                  \
{                                                                   \
    if (func != DEFS_STATUS_OK)                                     \
    {                                                               \
        action;                                                     \
    }                                                               \
}


/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_COND_CHECK                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cond    - Condition to check                                                           */
/*                  err     - Error to through if condition is not met                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks given condition and returns the given error if condition was not   */
/*                  met                                                                                    */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    DEFS_STATUS myFunc(void* ptr)                                                                        */
/*    {                                                                                                    */
/*        DEFS_STATUS_COND_CHECK(ptr, DEFS_STATUS_INVALID_PARAMETER);                                      */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_COND_CHECK(cond, err)                           \
{                                                                   \
    if (!(cond))                                                    \
    {                                                               \
        return err;                                                 \
    }                                                               \
}



/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_COND_CHECK_ACTION                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cond    - Condition to check                                                           */
/*                  action  - Action done if condition is not met                                          */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks given condition and performs required actionif condition was not   */
/*                  met                                                                                    */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    status myFunc(void* ptr)                                                                             */
/*    {                                                                                                    */
/*        STATUS_COND_CHECK(ptr, break);                                                                   */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_COND_CHECK_ACTION(cond, action)                 \
{                                                                   \
    if (!(cond))                                                    \
    {                                                               \
        action;                                                     \
    }                                                               \
}


/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_ERR_CHECK                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - Function to check                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro returns TRUE if given function returns DEFS_STATUS error;                   */
/*                  Otherwise it returns FALSE.                                                            */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    DEFS_STATUS myFunc(INT p1, INT p2);                                                                  */
/*                                                                                                         */
/*    status otherFunc                                                                                     */
/*    {                                                                                                    */
/*        ...                                                       // Some code                           */
/*        BOOLEAN error = DEFS_STATUS_ERR_CHECK(myFunc(p1,p2));     // Executing myFunc                    */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_ERR_CHECK(func)             (BOOLEAN)((func) ^ DEFS_STATUS_OK)


/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_ASSERT                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - Function to check                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine puts ASSERT on function return status                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef DEBUG
    #define DEFS_STATUS_RET_ASSERT(func)        ASSERT(func == DEFS_STATUS_OK)
#else
    #define DEFS_STATUS_RET_ASSERT(func)        (void)func
#endif


#endif /* __ERRORS_H__ */
