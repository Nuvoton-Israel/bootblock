/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   gpio_regs.h                                                              */
/*            This file contains registers definition of GPIO module          */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _GPIO_REGS_H
#define _GPIO_REGS_H

#include __CHIP_H_FROM_DRV()

/*---------------------------------------------------------------------------------------------------------*/
/* Utility for defining global variables for GPIO in H file                                                */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef GPIO_DRV_C
#define EXTERN extern
#else
#define EXTERN
#endif


/**************************************************************************************************************************/
/*   GPIOn Temporary Lock Register 1 (GPnTLOCK1)                                                                          */
/**************************************************************************************************************************/
#define  GPnTLOCK1(n)                      (GPIO_BASE_ADDR(n) + 0x0000) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0000h */
#define  GPnTLOCK1_LOCK1                 0 , 32             /* 31-0 LOCK1. Writing any value to this register (when unlocked) locks all GPIO module registers to write (except for   */

/**************************************************************************************************************************/
/*   GPIOn Data In Register (GPnDIN)                                                                                      */
/**************************************************************************************************************************/
#define  GPnDIN(n)                         (GPIO_BASE_ADDR(n) + 0x0004) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0004h */
#define  GPnDIN_DIN                      0 , 32             /* 31-0 DIN (Data In). Bits 31-0 correspond to GPIO 31-0 of this port. The corresponding read data bit reflects the      */

/**************************************************************************************************************************/
/*   GPIOn Polarity Register (GPnPOL)                                                                                     */
/**************************************************************************************************************************/
#define  GPnPOL(n)                         (GPIO_BASE_ADDR(n) + 0x0008) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0008h */
#define  GPnPOL_POL                      0 , 32             /* 31-0 POL (Polarity). Bits 31-0 control GPIO 31-0 of this port. The corresponding POL bit controls the polarity of the */

/**************************************************************************************************************************/
/*   GPIOn Data Out Register (GPnDOUT)                                                                                    */
/**************************************************************************************************************************/
#define  GPnDOUT(n)                        (GPIO_BASE_ADDR(n) + 0x000C) , GPIO_ACCESS, 32       /* Location: GPIO_BA+000Ch */
#define  GPnDOUT_DOUT                    0 , 32             /* 31-0 DOUT (Data Out). Bits 31-0 control GPIO 31-0 of this port. The corresponding DOUT bit controls the output        */

/**************************************************************************************************************************/
/*   GPIOn Data Out Register Set (GPnDOS)                                                                                 */
/**************************************************************************************************************************/
#define  GPnDOS(n)                         (GPIO_BASE_ADDR(n) + 0x0068) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0068h */
#define  GPnDOS_DOS                      0 , 32             /* 31-0 DOS (Data Out Set). Bits 31-0 control GPIO 31-0 of this port. The corresponding bit in GPnDOUT register is       */

/**************************************************************************************************************************/
/*   GPIOn Data Out Register Clear (GPnDOC)                                                                               */
/**************************************************************************************************************************/
#define  GPnDOC(n)                        (GPIO_BASE_ADDR(n) + 0x006C) , GPIO_ACCESS, 32        /* Location: GPIO_BA+006Ch */
#define  GPnDOC_DOC                      0 , 32             /* 31-0 DOC (Data Out Clear). Bits 31-0 control GPIO 31-0 of this port. The corresponding bit in GPnDOUT register is     */

/**************************************************************************************************************************/
/*   GPIOn Output Enable Register (GPnOE)                                                                                 */
/**************************************************************************************************************************/
#define  GPnOE(n)                          (GPIO_BASE_ADDR(n) + 0x0010) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0010h */
#define  GPnOE_OE                        0 , 32             /* 31-0 OE (Output Enable). Bits 31-0 control GPIO 31-0 of this port. The corresponding OE bit controls the output       */

/**************************************************************************************************************************/
/*   GPIOn Output Enable Register Set (GPnOES)                                                                            */
/**************************************************************************************************************************/
#define  GPnOES(n)                         (GPIO_BASE_ADDR(n) + 0x0070) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0070h */
#define  GPnOES_OES                      0 , 32             /* 31-0 OES (Output Enable Set). Bits 31-0 control GPIO 31-0 of this port. The corresponding bit in GPnOE register       */

/**************************************************************************************************************************/
/*   GPIOn Output Enable Register Clear (GPnOEC)                                                                          */
/**************************************************************************************************************************/
#define  GPnOEC(n)                         (GPIO_BASE_ADDR(n) + 0x0074) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0074h */
#define  GPnOEC_OEC                      0 , 32             /* 31-0 OEC (Output Enable Clear). Bits 31-0 control GPIO 31-0 of this port. The corresponding bit in GPnOE register     */

/**************************************************************************************************************************/
/*   GPIOn Output Type Register (GPnOTYP)                                                                                 */
/**************************************************************************************************************************/
#define  GPnOTYP(n)                        (GPIO_BASE_ADDR(n) + 0x0014) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0014h */
#define  GPnOTYP_OTYP                    0 , 32             /* 31-0 OTYP (Output Type). Bits 31-0 control GPIO 31-0 of this port. The corresponding OTYP bit controls the output     */

/**************************************************************************************************************************/
/*   GPIOn on Main Power Register (GPnMP)                                                                                 */
/**************************************************************************************************************************/
#define  GPnMP(n)                          (GPIO_BASE_ADDR(n) + 0x0018) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0018h */
#define  GPnMP_MP                        0 , 32             /* 31-0 MP (Main Power). Bits 31-0 control GPIO 31-0 of this port. The corresponding MP bit controls the output power    */

/**************************************************************************************************************************/
/*   GPIOn Pull-Up Control Register (GPnPU)                                                                               */
/**************************************************************************************************************************/
#define  GPnPU(n)                          (GPIO_BASE_ADDR(n) + 0x001C) , GPIO_ACCESS, 32       /* Location: GPIO_BA+001Ch */
#define  GPnPU_PUC                       0 , 32             /* 31-0 PUC (Pull-Up Control). Bits 31-0 control GPIO 31-0 of this port. The corresponding PUC bit controls the static   */

/**************************************************************************************************************************/
/*   GPIOn Pull-Down Control Register (GPnPD)                                                                             */
/**************************************************************************************************************************/
#define  GPnPD(n)                          (GPIO_BASE_ADDR(n) + 0x0020) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0020h */
#define  GPnPD_PDC                       0 , 32             /* 31-0 PDC (Pull-Down Control). Bits 31-0 control GPIO 31-0 of this port. The corresponding PDC bit controls the        */

/**************************************************************************************************************************/
/*   GPIOn Debounce Enable Register (GPnDBNC)                                                                             */
/**************************************************************************************************************************/
#define  GPnDBNC(n)                        (GPIO_BASE_ADDR(n) + 0x0024) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0024h */
#define  GPnDBNC_DBNCE                   0 , 32             /* 31-0 DBNCE (Debounce Enable). Bits 31-0 control GPIO 31-0 of this port. The corresponding DBNCE bit controls          */

/**************************************************************************************************************************/
/*   GPIOn Event Type Register (GPnEVTYP)                                                                                 */
/**************************************************************************************************************************/
#define  GPnEVTYP(n)                       (GPIO_BASE_ADDR(n) + 0x0028) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0028h */
#define  GPnEVTYP_EVTYP                  0 , 32             /* 31-0 EVTYP (Event Type). Bits 31-0 control GPIO 31-0 of this port. The corresponding EVTYP bit selects the Event      */

/**************************************************************************************************************************/
/*   GPIOn Event for Both Edges Register (GPnEVBE)                                                                        */
/**************************************************************************************************************************/
#define  GPnEVBE(n)                        (GPIO_BASE_ADDR(n) + 0x002C) , GPIO_ACCESS, 32       /* Location: GPIO_BA+002Ch */
#define  GPnEVBE_EVBE                    0 , 32             /* 31-0 EVBE (Event for Both Edges). Bits 31-0 control GPIO 31-0 of this port. The corresponding EVBE bit enables        */

/**************************************************************************************************************************/
/*   GPIOn Output Blink Control Register 0 (GPnOBL0)                                                                      */
/**************************************************************************************************************************/
#define  GPnOBL0(n)                        (GPIO_BASE_ADDR(n) + 0x0030) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0030h */
#define  GPnOBL0_GP07BLK                 28 , 3             /* 30-28 GP07BLK (GPIO07 Blink Selection).                                                                               */

/**************************************************************************************************************************/
/*   GPIOn Output Blink Control Register 1 (GPnOBL1)                                                                      */
/**************************************************************************************************************************/
#define  GPnOBL1(n)                        (GPIO_BASE_ADDR(n) + 0x0034) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0034h */
#define  GPnOBL1_GP15BLK                 28 , 3             /* 30-28 GP15BLK (GPIO15 Blink Selection).                                                                               */

/**************************************************************************************************************************/
/*   GPIOn Output Blink Control Register 2 (GPnOBL2)                                                                      */
/**************************************************************************************************************************/
#define  GPnOBL2(n)                        (GPIO_BASE_ADDR(n) + 0x0038) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0038h */
#define  GPnOBL2_GP23BLK                 28 , 3             /* 30-28 GP23BLK (GPIO23 Blink Selection).                                                                               */

/**************************************************************************************************************************/
/*   GPIOn Output Blink Control Register 3 (GPnOBL3)                                                                      */
/**************************************************************************************************************************/
#define  GPnOBL3(n)                        (GPIO_BASE_ADDR(n) + 0x003C) , GPIO_ACCESS, 32       /* Location: GPIO_BA+003Ch */
#define  GPnOBL3_GP31BLK                 28 , 3             /* 30-28 GP31BLK (GPIO31 Blink Selection).                                                                               */

/**************************************************************************************************************************/
/*   GPIOn Event Enable Register (GPnEVEN)                                                                                */
/**************************************************************************************************************************/
#define  GPnEVEN(n)                        (GPIO_BASE_ADDR(n) + 0x0040) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0040h */
#define  GPnEVEN_EVNTEN                  0 , 32             /* 31-0 EVNTEN (Event Enable). Bits 31-0 control GPIO 31-0 of this port. The corresponding EVNTEN bit enables the        */

/**************************************************************************************************************************/
/*   GPIOn Event Enable Set Register (GPnEVENS)                                                                           */
/**************************************************************************************************************************/
#define  GPnEVENS(n)                       (GPIO_BASE_ADDR(n) + 0x0044) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0044h */
#define  GPnEVENS_EVENSET                0 , 32             /* 31-0 EVENSET (Event Enable Set). Bits 31-0 control GPIO 31-0 of this port. Writing 1 to the corresponding             */

/**************************************************************************************************************************/
/*   GPIOn Event Enable Clear Register (GPnEVENC)                                                                         */
/**************************************************************************************************************************/
#define  GPnEVENC(n)                       (GPIO_BASE_ADDR(n) + 0x0048) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0048h */
#define  GPnEVENC_EVENCLR                0 , 32             /* 31-0 EVENCLR (Event Enable Clear). Bits 31-0 control GPIO 31-0 of this port. Writing 1 to the corresponding           */

/**************************************************************************************************************************/
/*   GPIOn Event Status Register (GPnEVST)                                                                                */
/**************************************************************************************************************************/
#define  GPnEVST(n)                        (GPIO_BASE_ADDR(n) + 0x004C) , GPIO_ACCESS, 32       /* Location: GPIO_BA+004Ch */
#define  GPnEVST_EVSTAT                  0 , 32             /* 31-0 EVSTAT (Event Status). Bits 31-0 control GPIO 31-0 of this port. The corresponding EVSTAT bit reflects the       */

/**************************************************************************************************************************/
/*   GPIOn Standby Power Lock Register (GPnSPLCK)                                                                         */
/**************************************************************************************************************************/
#define  GPnSPLCK(n)                       (GPIO_BASE_ADDR(n) + 0x0050) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0050h */
#define  GPnSPLCK_SPLOCK                 0 , 32             /* 31-0 SPLOCK (Standby Power Lock). Bits 31-0 control GPIO 31-0 of this port. Writing 1 to the corresponding            */

/**************************************************************************************************************************/
/*   GPIOn Main Power Lock Register (GPnMPLCK)                                                                            */
/**************************************************************************************************************************/
#define  GPnMPLCK(n)                       (GPIO_BASE_ADDR(n) + 0x0054) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0054h */
#define  GPnMPLCK_MPLOCK                 0 , 32             /* 31-0 MPLOCK (Main Power Lock). Bits 31-0 control GPIO 31-0 of this port. Writing 1 to the corresponding               */

/**************************************************************************************************************************/
/*   GPIOn Input Enable Mask Register (GPnIEM)                                                                            */
/**************************************************************************************************************************/
#define  GPnIEM(n)                         (GPIO_BASE_ADDR(n) + 0x0058) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0058h */
#define  GPnIEM_IEMASK                   0 , 32             /* 31-0 IEMASK (Input Enable Mask). Bits 31-0 control GPIO 31-0 of this port. The corresponding INMASK bit               */

/**************************************************************************************************************************/
/*   GPIOn Output Slew-Rate Control Register (GPnOSRC)                                                                    */
/**************************************************************************************************************************/
#define  GPnOSRC(n)                        (GPIO_BASE_ADDR(n) + 0x005C) , GPIO_ACCESS, 32       /* Location: GPIO_BA+005Ch */
#define  GPnOSRC_OSRC                    0 , 32             /* 31-0 OSRC (Output Slew-Rate control). Bits 31-0 control GPIO 31-0 of this port. The corresponding OSRC bit            */

/**************************************************************************************************************************/
/*   GPIOn Output Drive Strength Control Register (GPnODSC)                                                               */
/**************************************************************************************************************************/
#define  GPnODSC(n)                        (GPIO_BASE_ADDR(n) + 0x0060) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0060h */
#define  GPnODSC_ODSC                    0 , 32             /* 31-0 ODSC (Output Drive Strength control). Bits 31-0 control GPIO 31-0 of this port. The corresponding ODSC bit       */

/**************************************************************************************************************************/
/*   GPIOn Version Register (GPnVER)                                                                                      */
/**************************************************************************************************************************/
#define  GPnVER(n)                         (GPIO_BASE_ADDR(n) + 0x0064) , GPIO_ACCESS, 32       /* Location: GPIO_BA+0064h */
#define  GPnVER_VERSION                  0 , 8              /* 7-0 VERSION. Is 01h in this first revision on the spec.                                                               */

/**************************************************************************************************************************/
/*   GPIOn Temporary Lock Register 2 (GPnTLOCK2)                                                                          */
/**************************************************************************************************************************/
#define  GPnTLOCK2(n)                      (GPIO_BASE_ADDR(n) + 0x007C) , GPIO_ACCESS, 32       /* Location: GPIO_BA+007Ch */
#define  GPnTLOCK2_LOCK2                 0 , 32             /* 31-0 LOCK2. Writing any value to this register (when unlocked) locks all GPIO module registers to write (except for   */
#define  GPnTLOCK2_refers                0 , 1               /* 0 refers to XI07, pin number 1 refers to XI06, pin number 7 refers to XI00, pin number 8 refers to XIN17 and so forth.*/






/*---------------------------------------------------------------------------------------------------------*/
/*                                   Local definitions for port handling                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_PORTS_PER_MODULE           32
#define GPIO_EVENT_GPIOS_PER_MODULE     32

/*---------------------------------------------------------------------------------------------------------*/
/* We define 'undefined registers' in reserved space after module registers but within module's reserved   */
/* address space.                                                                                          */
/* This solution is a bit tricky and should be checked if applicable against every new chip                */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_UNDEF_REG      0xF0


#if _YARKON_GPIO_DRIVER_
/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Registers offset table for O(1) access                                                             */
/*---------------------------------------------------------------------------------------------------------*/
static const UINT8 GPIO_RegOffset[5][GPIO_PORTS_PER_MODULE] =
{
    {   0x14,         0x24,  0x3C,  0x50,  0x64,  0x78,  GPIO_UNDEF_REG,  0x90 },
    {   0x18,         0x28,  0x40,  0x54,  0x68,  0x7C,  GPIO_UNDEF_REG,  0x94 },
    {GPIO_UNDEF_REG,  0x2C,  0x44,  0x58,  0x6C,  0x80,  GPIO_UNDEF_REG,  0x98 },
    {   0x1C,         0x34,  0x48,  0x5C,  0x70,  0x84,  GPIO_UNDEF_REG,  0x9C },
    {   0x20,         0x38,  0x4C,  0x60,  0x74,  0x88,  0x8c,            0xA0 },
};
#endif  // _YARKON_GPIO_DRIVER_


#endif // _GPIO_REGS_H

