/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   strp_if.h                                                                */
/* This file contains API of straps and fuse straps access routines           */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef STRP_IF_H
#define STRP_IF_H


#include "hal.h"

#if defined (AES_MODULE_TYPE)
#include "../aes/aes_if.h"
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* STRP types definition                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum STRP_TYPE_T_tag
{
    STRP_1_3_CKFRQ = 1,         // CPU core and DDR4 memory clock frequency (by firmware) Also one combination is SkipInit and one  for clocks bypass
    STRP_4_J2EN = 4 ,           // Select JTAG2 for Coprocessor debug and enable Ring Oscillator output (set by ROM firmware)
    STRP_5 = 5,                 // Reserved
    STRP_6_ECC= 6,              // ECC for DRAM enable
    STRP_7_HIZ = 7,             // HI-Z Control
    STRP_8_SECEN = 8,           // SECEN - for debug.
    STRP_9_10_FUP = 9,          // UART Command routine enable (by ROM firmware) and UART source selection
    STRP_11_BSPA = 11,          // Select BMC debug Serial Port (BSP) on Serial Interface 2. (set by ROM code when STRAP11 is pulled low)
    STRP_12_SFAB = 12,          // BIOS ROM included in SPI3 flash or POLEG does not respond to host access to BIOS (default no response) (set by ROM firmware).
    STRP_13_SPI0F18 = 13,       // SPI0 bus powered by 1.8V (set by ROM code when STRAP13 is pulled low)
    STRP_14_15_DDR_SPEED,
    STRP_16_17_DDR_SIZE,
} STRP_TYPE_T;


/*---------------------------------------------------------------------------------------------------------*/
/* DDR Memory Size - used by STRP_MemSize_Get()                                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*   3 memory capacities  DDR_SIZE0 -                                                                      */
/*    GPIO190 Existing.       DDR_SIZE1 - GPIO17 NEW                                                       */
/*    Board options for       DDR_SIZE1 is currently                                                       */
/*    Pulldown (default)      named I2C_BMC_SEG4_EN                                                        */
/*    and pullup.             with a 10K pulldown                                                          */
/*    Not a dipswitch         (no provision to pullup).                                                    */
/*    which is OK.            I2C SEG4 enable function                                                     */
/*                            to be internaly configured                                                   */
/*                            to use PWRGD_PS input instead.                                               */
/*                            Thus when needing 2GB,                                                       */
/*                            add a rework pullup on                                                       */
/*                            this net (e.g., J_PSU1_2                                                     */
/*                            pin 8 2K pullup to 3.3V)                                                     */
/*                                                                                                         */
/* a.  512MB      0           0                       Default for iDRAC                                    */
/* b.  1GB        1           0                       Default for Enclosure Contorller and                 */
/*                                                    iDRAC special builds for mem test tools              */
/* c.  2GB        1           1                       EC special build - one off.                          */
/* d.  Reserved   0           1                       Nobody uses. Do not need a 256MB variant.            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum STRP_MEM_SIZE_T_tag
{
    STRP_MEMSIZ_512MB_MODE   = 0x0,   // 512 MB. Default for iDRAC  
    STRP_MEMSIZ_1GB_MODE     = 0x2,   // 1024 MB. Default for Enclosure Contorller and iDRAC special builds for mem test tools  
    STRP_MEMSIZ_2GB_MODE     = 0x3,   // 2048 MB. EC special build - one off.   
    STRP_MEMSIZ_RESERVED     = 0x1    // Reserved, Nobody uses. Do not need a 256MB variant.    
} STRP_MEM_SIZE_T;




 /*--------------------------------------------------------------------------------------------------------*/
 /* DDR Memory Speed - used by STRP_MemSpeed_Get()                                                         */
 /*--------------------------------------------------------------------------------------------------------*/

 /*--------------------------------------------------------------------------------------------------------*/
 /* DDR speeds                                                                                             */
 /*                      DDR_SPEED0    DDR_SPEED1 =                                                        */
 /*                      STRAP6        GPIO169.                                                            */
 /*                                                                                                        */
 /* a.      667 MHz      0            1       Default                                                      */
 /* b.      800 MHz      1            0       Only a couple dare devils try this.                          */
 /* c.      333 MHz      0            0       General populace tries this setting if instability           */
 /*                                           is observed  and CPU slow down did not help.                 */
 /* d.      Res.         1            1       Nobody runs this                                             */
 /*                                                                                                        */
 /* GPIO169 is currently called JMP_NO_RST_IERR_N which will be relocated on real platforms and not needed */
 /* on DRB.                                                                                                */
 /*  It is a FW function on whether the BMC should reset the host of detecteing a crash or not.            */
 /*--------------------------------------------------------------------------------------------------------*/
typedef enum STRP_MEM_SPEED_T_tag
{                                         //            highBit(s6) lowBit(SW1.4)
    STRP_MEM_SPEED_667MHz_MODE   = 0x3,   // 667 MHz    1           1       Default 
    STRP_MEM_SPEED_800MHz_MODE   = 0x1,   // 800 MHz    0           1       Only a couple dare devils try this. 
    STRP_MEM_SPEED_333MHz_MODE   = 0x2,   // 333 MHz    1           0       General populace tries this setting if instability is observed and CPU slow down did not help.  
    STRP_MEM_SPEED_RESERVED      = 0x0    // Reserved   0           0       Nobody runs this
} STRP_MEM_SPEED_T; 






/*---------------------------------------------------------------------------------------------------------*/
/* CPU core clock and DDR memory frequency - used by STRP_Ckfrq_Get()                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum STRP_CLKFRQ_T_tag
{
    STRP_CLKFRQ_OPTION0   =  0  , /* DDR=MCBPCK  CORE=FT     AXI=FT/2    AHB=FT/4    AHB3=FT/2   SPI0=FT/2   APB5=FT/4   APB1_4=FT/4     PCI=FT/5    GFX=FT/4 */
    STRP_CLKFRQ_OPTION1   =  1  , /* DDR=25      CORE=25     AXI=12.5    AHB=6.25    AHB3=3.1    SPI0=6.25   APB5=3.1    APB1_4=1.5      PCI=4.17    GFX=5    */
    STRP_CLKFRQ_OPTION2   =  2  , /* DDR=1066    CORE=1000   AXI=500     AHB=250     AHB3=19.23  SPI0=19.23  APB5=62.5   APB1_4=31.25    PCI=142.8   GFX=167  */
    STRP_CLKFRQ_OPTION3   =  3  , /* DDR=667     CORE=500    AXI=250     AHB=125     AHB3=17.85  SPI0=17.85  APB5=62.5   APB1_4=31.25    PCI=166.7   GFX=167  */
    STRP_CLKFRQ_OPTION4   =  4  , /* DDR=850     CORE=800    AXI=400     AHB=200     AHB3=20     SPI0=20     APB5=50     APB1_4=25       PCI=133     GFX=160  */
    STRP_CLKFRQ_OPTION5   =  5  , /* DDR=900     CORE=888    AXI=444     AHB=222     AHB3=18.5   SPI0=18.5   APB5=55.5   APB1_4=27.75    PCI=148     GFX=177.6*/
    STRP_CLKFRQ_OPTION6   =  6  , /* DDR=800     CORE=850    AXI=425     AHB=212.5   AHB3=19.3   SPI0=19.3   APB5=53.1   APB1_4=26.6     PCI=170     GFX=170  */
    STRP_CLKFRQ_OPTION7   =  7  , /* DDR=800     CORE=800    AXI=400     AHB=200     AHB3=20     SPI0=20     APB5=50     APB1_4=25       PCI=133     GFX=160  */

} STRP_CLKFRQ_T;


/*---------------------------------------------------------------------------------------------------------*/
/* UART programming modes enum                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum STRP_FUP_T_tag
{
    STRP_FUP_NONE_UART3     = 0,    // Normal boot operation. If boot will fail, go to FUP at UART3
    STRP_FUP_ENABLE_UART2   = 1,    // Programming enabled. After initialization, the core waits for commands from UART2, which is connected to host SP2 (internal connection). In this case, there is no jump to an SPI flash device.
    STRP_FUP_ENABLE_UART3   = 2,    // Programming enabled. After initialization, the core waits for commands from UART3, which is connected to external SI2 interface. In this case, there is no jump to an SPI flash device.
    STRP_FUP_NONE_UART2     = 3,    // Normal boot operation. If boot will fail, go to FUP at UART2
} STRP_FUP_T;


/*---------------------------------------------------------------------------------------------------------*/
/* 2   1   0   DDR         CORE    AXI     AHB     AHB3    SPI0    APB5    APB1_4  PCI     GFX             */
/* Straps                                                                                                  */
/* 3   2   1                                                                                               */
/* 0   0   0   MCBPCK       FT     FT/2    FT/4    FT/2    FT/2    FT/4    FT/4    FT/5    FT/4            */
/* 0   0   1   25           25     12.5    6.25    3.1     6.25    3.1     1.5     4.17    5               */
/* 0   1   0   1066         1000    500    250     19.23   19.23   62.5    31.25   142.8   167             */
/* 0   1   1   667          500     250    125     17.85   17.85   62.5    31.25   166.7   167             */
/* 1   0   0   850          800     400    200     20      20      50      25      133     160             */
/* 1   0   1   900          888     444    222     18.5    18.5    55.5    27.75   148     177.6           */
/* 1   1   0   800          850     425    212.5   19.3    19.3    53.1    26.6    170     170             */
/* 1   1   1   800          800     400    200     20      20      50      25      133     160             */
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/* Straps module exported functions (API)                                                                  */
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
void           STRP_Select (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_SecureBoot_On                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine  Read the SB strap and oSecboot and oSecBootDisabled.                     */
/*                  without init, which will be done                                                       */
/*                  If the   oSecBootDisable conflicts with oSecBoot enable return error                   */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN        STRP_SecureBoot_On(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_SkipInit_On                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets the skip init mode. It is derived from the clk configuration register*/
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN        STRP_SkipInit_On (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_Ecc_On                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets strap 6 (ECC)  value . Used by boot block                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN        STRP_Ecc_On (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        STRP_FUP_mode                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets the FUP mode ( Flash update mode)                                    */
/*---------------------------------------------------------------------------------------------------------*/
STRP_FUP_T     STRP_FUP_mode (void);

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
UINT                     STRP_get           (STRP_TYPE_T strp);


UINT32                   STRP_Ckfrq_Get        (void);                   
BOOLEAN                  STRP_ShmBypass_On     (void);
BOOLEAN                  STRP_Propf_On         (void);
BOOLEAN                  STRP_BootFromSpi      (void);
BOOLEAN                  STRP_HaltOnFailure    (void);
BOOLEAN                  STRP_Gp_On            (void);
BOOLEAN                  STRP_Sioxh_On         (void);
BOOLEAN                  STRP_KeyAccessLock_On (void);
BOOLEAN                  STRP_BiosEn_On        (void);
STRP_MEM_SPEED_T         STRP_MemSpeed_Get     (void);


#endif //STRP_IF_H

