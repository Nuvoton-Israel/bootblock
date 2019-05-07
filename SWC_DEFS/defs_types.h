/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*    defs_types.h                                                            */
/* This file contains NTIL generic types, including compiler and core dependent */
/* Project:                                                                   */
/*            SWC DEFS                                                        */
/*----------------------------------------------------------------------------*/

#ifndef __TYPES_H__
#define __TYPES_H__

/*---------------------------------------------------------------------------------------------------------*/
/*                                                CONSTANTS                                                */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  FALSE
#define  FALSE      (BOOLEAN)0
#endif

#ifndef  TRUE
#define  TRUE       (BOOLEAN)1
#endif

#ifndef  NULL
#define  NULL       0
#endif

#define ENABLE      1
#define DISABLE     0

/*---------------------------------------------------------------------------------------------------------*/
/*                                        GENERIC TYPES DEFINITIONS                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef unsigned char  UINT8;                       /* Unsigned  8 bit quantity                            */
typedef signed   char  INT8;                        /* Signed    8 bit quantity                            */
typedef unsigned short UINT16;                      /* Unsigned 16 bit quantity                            */
typedef short          INT16;                       /* Signed   16 bit quantity                            */
typedef float          FP32;                        /* Single precision floating point

*/
typedef double         FP64;                        /* Double precision floating point                     */

#if defined (WDFAPI) || defined (_WIN32)
    /*-----------------------------------------------------------------------------------------------------*/
    /* UINT32, INT32, UINT64, INT64 are defined for Windows OS                                             */
    /*-----------------------------------------------------------------------------------------------------*/
#else

#ifdef __LP64__
    /*-----------------------------------------------------------------------------------------------------*/
    /* unsigned long is 64bit (mostly on 64bit Linux systems)                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    typedef unsigned int    UINT32;                 /* Unsigned 32 bit quantity                            */
    typedef signed   int    INT32;                  /* Signed   32 bit quantity                            */
    typedef unsigned long   UINT64;                 /* Unsigned 64 bit quantity                            */
    typedef signed   long   INT64;                  /* Signed 64 bit quantity                              */

 #else
 
    #if ((~0ULL>>1) == 0x7FFFFFFFLL) || defined (__CR16C__)
        /*-----------------------------------------------------------------------------------------------------*/
        /* unsigned long is 32bit (32bit Linux, 32bit Windows and 64bit Windows                                */
        /*-----------------------------------------------------------------------------------------------------*/
        typedef unsigned long   UINT32;                 /* Unsigned 32 bit quantity                            */
        typedef signed   long   INT32;                  /* Signed   32 bit quantity                            */
    #else
        /*-----------------------------------------------------------------------------------------------------*/
        /* unsigned int is 32bit for linux kernel and uboot                                                    */
        /*-----------------------------------------------------------------------------------------------------*/
        typedef unsigned int   UINT32;                 /* Unsigned 32 bit quantity                            */
        typedef signed   int   INT32;                  /* Signed   32 bit quantity                            */
    #endif
 
    #if ((~0ULL>>1) == 0x7FFFFFFFLL)
        /*-------------------------------------------------------------------------------------------------*/
        /* long long type is 32bit                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        typedef UINT32              UINT64[2];      /* Unsigned 64 bit quantity                            */
        typedef INT32               INT64[2];       /* Signed 64 bit quantity                              */
    #else
        /*-------------------------------------------------------------------------------------------------*/
        /* long long type is 64bit                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        typedef unsigned long long  UINT64;         /* Unsigned 64 bit quantity                            */
        typedef long long           INT64;          /* Signed 64 bit quantity                              */
    #endif

#endif
#endif
/*---------------------------------------------------------------------------------------------------------*/
/*                               OS OR CORE HW ACCESS DEFINITIONS AND TYPES                                */
/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Description:                                                                                            */
/*   HW specific code can reside in 2 different environments:                                              */
/*      1. OS/framework/test environment - The code doesn't access the HW registers directly but rather    */
/*                                         uses OS/framework services that wraps the needed HW accesses    */
/*      2. Standalone code -               The code can access directly the HW registers                   */
/*                                                                                                         */
/*   Therefore the HW access definitions are divided into 2 types: OS and CORE.                            */
/*   The OS definitions must always precede the CORE definitions                                           */
/*   For example, code that compiles for Linux kernel runnig or ARM must include the Linux kernel          */
/*   definitions rather than ARM ones, as it must use the Linux Kernel HW access wrapping functions        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/*                          OS / framework/test environment dependent definitions                          */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Windows Driver Foundation                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (WDFAPI)
#include "defs_os_wdf.h"

/*---------------------------------------------------------------------------------------------------------*/
/* TestEC Core and PC                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (__TESTEC__)
#include "defs_os_testec.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Windows OS Applications                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (_WIN32)
#include "defs_os_windows.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Linux Kernel                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (KBUILD_STR) && defined(__KERNEL__) && !defined(__UBOOT__)
#define __LINUX_KERNEL_ONLY__
#include "defs_os_linux.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Linux Application                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (__LINUX_APP__)
#include "defs_os_linux_app.h"



/*---------------------------------------------------------------------------------------------------------*/
/*                                       Core dependent definitions                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Core: x86                                                                                               */
/* Supported Tools: Watcom tools for DOS                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (__WATCOMC__) && defined (__X86__)
#include "defs_core_x86.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Core: CR16                                                                                              */
/* Supported Tools: NSC tools                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (__CR16C__)
#include "defs_core_cr16.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Core: 8051                                                                                              */
/* Supported Tools: Keil tools                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#elif (defined (__C51__) && defined (__KEIL__))
#include "defs_core_8051.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Core: ARM                                                                                               */
/* Supported Tools: GCC 3.1 (based) and above, RVDS 3.0 for windows and above                              */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (_ARM_) || defined (__arm__) || defined (__arm) || defined (__thumb__)
#include "defs_core_arm.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Core: COP8                                                                                              */
/* Supported Tools: IAR tools                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined(__TID__) && (((__TID__ >> 8) & 0x7F) == 0x58) && defined(__IAR_SYSTEMS_ICC__)
#include "defs_core_cop8.h"

#else
/*---------------------------------------------------------------------------------------------------------*/
/* Unsupported OS or CORE                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#error "Unsupported OS or Core architecture"
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Default definitions                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#include "defs_core_default.h"


#endif /* __TYPES_H__ */
