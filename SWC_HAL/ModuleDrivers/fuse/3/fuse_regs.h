/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   fuse_regs.h                                                              */
/*            This file contains definitions of FUSE module registers         */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef FUSE_REGS_H
#define FUSE_REGS_H

#include __CHIP_H_FROM_DRV()



/**************************************************************************************************************************/
/*   Fuse Array Control Register (FCTL1,2)                                                                                */
/**************************************************************************************************************************/
#define  FCTL(n)                         (FUSE_BASE_ADDR(n) + 0x14) , FUSE_ACCESS, 32       /* Location: BASE+14h */
#define  FCTL_FCTL                     0 , 32             /* 31-0 FCTL. A sequence of two adjacent writes to this register, first with a value of 0000_0001h and the second with   */

/**************************************************************************************************************************/
/*   Fuse Array Status Register (FST1,2)                                                                                  */
/**************************************************************************************************************************/
#define  FST(n)                          (FUSE_BASE_ADDR(n) + 0x00) , FUSE_ACCESS, 32       /* Location: BASE+00h */
#define  FST_RIEN                      2 , 1               /* 2 RIEN (Ready Interrupt Enable). Enables an interrupt when RDST bit is set. (Bit added in Poleg)                      */
#define  FST_RDST                      1 , 1               /* 1 RDST (Ready Status). This bit is set by hardware when a read or program operation is competed and                   */
#define  FST_RDY                       0 , 1               /* 0 RDY (Ready). If cleared to 0, indicates that the fuse array interface is busy processing a read or                  */

/**************************************************************************************************************************/
/*   Fuse Array Address Register (FADDR1,2)                                                                               */
/**************************************************************************************************************************/
#define  FADDR(n)                        (FUSE_BASE_ADDR(n) + 0x04) , FUSE_ACCESS, 32       /* Location: BASE+04h */
#define  FADDR_BITPOS                  10 , 3             /* (n)-10 BITPOS (Bit Position). For write operations, designates the position of the bit (to be programmed) in the byte  */
#define  FADDR_BYTEADDR                0 , 10             /* 9-0 BYTEADDR (Fuse Read Address). Designates the byte address in the fuse array for read and program                  */

/**************************************************************************************************************************/
/*   Fuse Array Data Register (FDATA1,2)                                                                                  */
/**************************************************************************************************************************/
#define  FDATA(n)                        (FUSE_BASE_ADDR(n) + 0x08) , FUSE_ACCESS, 32       /* Location: BASE+08h */
#define  FDATA_FDATA                   0 , 8              /* 7-0 FDATA. On read, returns the data from the read operation. The register contents are valid only if RDY bit in      */

/**************************************************************************************************************************/
/*   Fuse Array Configuration Register (FCFG1,2)                                                                          */
/**************************************************************************************************************************/
#define  FCFG(n)                         (FUSE_BASE_ADDR(n) + 0x0C) , FUSE_ACCESS, 32       /* Location: BASE+0Ch */
#define  FCFG_FDIS                     31 , 1              /* 31 FDIS (Fuse Array Disable). This sticky bit disables access to the first 2048 bits of the fuse array, either        */
#define  FCFG_APBRT                    24 , 6             /* 29-24 APBRT (APB Clock Rate). Informs the fuse array state machine on the APB clock rate in MHz. The                  */
#define  FCFG_FCFGLK                   16 , 8             /* 23-16 FCFGLK (FCFG Lock). Bit FCFGLKn locks the corresponding FPRGLKn and FRDLKn bits. These bits                     */
#define  FCFG_FPRGLK                   8 , 8              /* FPRGLK (Fuse Program Lock). Controls program access to the fuse array. FPRGLKn bit protects the                       */
#define  FCFG_FRDLK                    0 , 8              /* FRDLK (Fuse Read Lock). Controls APB read access to the fuse array. Bit FRDLKn protects the nth                       */

/**************************************************************************************************************************/
/*   Fuse Key Index Register (FKEYIND)                                                                                    */
/**************************************************************************************************************************/
#define  FKEYIND                        (FUSE_BASE_ADDR(0) + 0x10) , FUSE_ACCESS, 32        /* Location: BASE+10h */
#define  FKEYIND_KIND                    18 , 2             /* 19-18 KIND (Key Index). Indicates the address of the key in the fuse array, in 5(n)-bit steps. (Changed in             */
#define  FKEYIND_FUSERR                  8 , 1               /* 8 FUSERR (Fuse Error). Indicates that the ECC decoding mechanism detected an error while reading                      */
#define  FKEYIND_KSIZE                   4 , 3              /* 6-4 KSIZE (Key Size). Indicates the size of the cryptographic key to upload on the sideband key port.                 */
#define  FKEYIND_KVAL                    0 , 1               /* 0 KVAL (Key Valid). Indicates whether the sideband key port contents are valid. This bit is cleared to 0              */

/**************************************************************************************************************************/
/*   Fuse Strap Register (FUSTRAP)                                                                                        */
/**************************************************************************************************************************/
#define  FUSTRAP                        (FUSE_BASE_ADDR(1) + 0x10) , FUSE_ACCESS, 32        /* Location: BASE+10h */
#define  FUSTRAP_oWDEN                   27 , 1              /* 27 oWDEN (Watchdog Enable). If set, tells the ROM Code to enable a 22 seconds watchdog before jumping to              */
#define  FUSTRAP_oHLTOF                  26 , 1              /* 26 oHLTOF (Halt on Failure). If set, tells the ROM Code to halt execution on a signature verification failure.        */
#define  FUSTRAP_oAESKEYACCLK            25 , 1              /* 25 oAESKEYACCLK (AES Key Access Lock). If set, prevents any access to the first 2048 bits of the Key Storage          */
#define  FUSTRAP_oJDIS                   24 , 1              /* 24 oJDIS (JTAG Disable). If set, locks (disables) the BMC CPU JTAG port. It can be reopened via JTAGDIS bit in        */
#define  FUSTRAP_oSECBOOT                23 , 1              /* 23 oSECBOOT (Secure Boot). If set, indicates that the ROM code will perform an integrity check on power-up,           */
#define  FUSTRAP_USEFUSTRAP              22 , 1              /* 22 USEFUSTRAP. When set, indicates the configuration in this register must be used instead of the configuration       */
#define  FUSTRAP_oAltImgLoc              18 , 1              /* 18 oAltImgLoc. Alternate image location definition.                                                                   */
#define  FUSTRAP_FUSTRAP_SFAB            11 , 1              /* 11 FUSTRAP(n). System Flash Attached to BMC (SFAB).                                                                    */
#define  FUSTRAP_FUSTRAP3_1              0 ,  3              /* 2-0 FUSTRAP3-1. CPU core clock and DDR4 memory frequency (CKFRQ). See Power-On Setting Register                       */

/*---------------------------------------------------------------------------------------------------------*/
/* Added on Z2:                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define  FUSTRAP_oHINDDIS               14 , 1               /* oHINDDIS (Host Independence Disable). Disables initialization of a few registers in step 1 of the ROM code */
#define  FUSTRAP_oSecBootDisable        15 , 1               /* {oSecBootDisable} - when set, disables capability enter Secure Mode. Used for Derivatives.*/
#define  FUSTRAP_oCPU1STOP2             16 , 1               /* {oCPU1STOP2} - when set, stops CPU core 1 clock. */
#define  FUSTRAP_oCPU1STOP1             17 , 1               /* {oCPU1STOP1} - when set, CPU core 1 stops and cannot be woken.*/




/*---------------------------------------------------------------------------------------------------------*/
/* FKEYIND field values                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define  FKEYIND_KSIZE_VALUE_128          0x4
#define  FKEYIND_KSIZE_VALUE_192          0x5
#define  FKEYIND_KSIZE_VALUE_256          0x6


#endif /* FUSE_REGS_H */
