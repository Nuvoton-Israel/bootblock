/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   fiu_regs.h                                                               */
/*            This file contains Flash Interface Unit (FIU) module registers  */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _FIU_REGS_H
#define _FIU_REGS_H

#include __CHIP_H_FROM_DRV()


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                  Flash Interface Unit (FIU) Registers                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/**************************************************************************************************************************/
/*   FIU Direct Read Configuration Register (FIU_DRD_CFG)                                                                 */
/**************************************************************************************************************************/
#define  FIU_DRD_CFG(n)                 (FIU_BASE_ADDR(n) + 0x00) , FIU_ACCESS , 32 /* Location: SPIn_BA + 00h */
#define  FIU_DRD_CFG_R_BURST             24 , 2             /* 25-24 R_BURST (Read Burst). Sets the maximum length of a valid read burst. This value must be set                     */
#define  FIU_DRD_CFG_ADDSIZ              16 , 2             /* 17-16 ADDSIZ (Address Size). Selects the SPI address size. This field must be set according to the command            */
#define  FIU_DRD_CFG_DBW                 12 , 2             /* 13-12 DBW (Dummy Bytes Wait). Selects the SPI byte time to wait until the data is returned from the flash.            */
#define  FIU_DRD_CFG_ACCTYPE             8 , 2              /* 9-8 ACCTYPE (Access Type). Selects the SPI access type. This field must be set according to the access                */
#define  FIU_DRD_CFG_RDCMD               0 , 8              /* 7-0 RDCMD (Read command). Controls the value of the SPI command byte. Default is 0BH, fast read with                  */

/**************************************************************************************************************************/
/*   FIU Direct Write Configuration Register (FIU_DWR_CFG)                                                                */
/**************************************************************************************************************************/
#define  FIU_DWR_CFG(n)                    (FIU_BASE_ADDR(n) + 0x04) , FIU_ACCESS , 32             /* Offset: 04h */
#define  FIU_DWR_CFG_LCK                 31 , 1             /* 31 LCK (Lock). When the bit is set, writing to this register is ignored. This bit is not reset by setting the         */
#define  FIU_DWR_CFG_W_BURST             24 , 2             /* 25-24 W_BURST (Write Burst). Sets the maximum length of a valid write burst. This value must be set                   */
#define  FIU_DWR_CFG_ADDSIZ              16 , 2             /* 17-16 ADDSIZ (Address Size). Selects the SPI address size. This field must be set according to the                    */
#define  FIU_DWR_CFG_ABPCK               10 , 2             /* 11-10 ABPCK (Address Bits per Clock). Selects how many address bits are transferred per clock. This field             */
#define  FIU_DWR_CFG_DBPCK               8 , 2              /* 9-8 DBPCK (Data Bits per Clock). Selects how many data bits are transferred per clock. This field must                */
#define  FIU_DWR_CFG_WRCMD               0 , 8              /* 7-0 WRCMD (Write command). Controls the value of the SPI command byte. Default is 02H: write with a                   */

/**************************************************************************************************************************/
/*   FIU UMA Configuration Register (FIU_UMA_CFG)                                                                         */
/**************************************************************************************************************************/
#define  FIU_UMA_CFG(n)                   (FIU_BASE_ADDR(n) + 0x08) , FIU_ACCESS , 32            /* Offset: 08h */
#define  FIU_UMA_CFG_LCK                 31 , 1              /* 31 LCK (Lock). When the bit is set, writing to this register is ignored. This bit is not reset by setting the         */
#define  FIU_UMA_CFG_CMMLCK              30 , 1              /* 30 CMMLCK (Command Mode Lock). When the bit is set, writing to this bit and to the CMBPCK and                         */
#define  FIU_UMA_CFG_RDATSIZ             24 , 5             /* 28-24 RDATSIZ (Read Data Size). Selects how many read data bytes are read during the SPI transaction.                 */
#define  FIU_UMA_CFG_DBSIZ               21 , 3             /* 23-21 DBSIZ (Dummy Byte Size). Selects how many dummy bytes are present during the SPI transaction.                   */
#define  FIU_UMA_CFG_WDATSIZ             16 , 5             /* 20-16 WDATSIZ (Write Data Size). Selects how many write data bytes are sent during the SPI transaction.               */
#define  FIU_UMA_CFG_ADDSIZ              11 , 3             /* 13-11 ADDSIZ (Address Size). Selects how many address bytes are sent during the SPI transaction. This                 */
#define  FIU_UMA_CFG_CMDSIZ              10 , 1              /* 10 CMDSIZ (Command Size). Selects how many command bytes are sent during the SPI transaction.                         */
#define  FIU_UMA_CFG_RDBPCK              8 , 2              /* 9-8 RDBPCK (Read Data Bits per Clock). Selects how many read data bits are transferred per clock. This                */
#define  FIU_UMA_CFG_DBPCK               6 , 2              /* 7-6 DBPCK (Dummy Bytes Bits per Clock). Selects how many dummy bytes bits are transferred per                         */
#define  FIU_UMA_CFG_WDBPCK              4 , 2              /* 5-4 WDBPCK (Write Data Bits per Clock). Selects how many write data bits are transferred per clock.                   */
#define  FIU_UMA_CFG_ADBPCK              2 , 2              /* 3-2 ADBPCK (Address Bits per Clock). Selects how many address bits are transferred per clock. This                    */
#define  FIU_UMA_CFG_CMBPCK              0 , 2              /* 1-0 CMBPCK (Command Bits per Clock). Selects how many command bits are transferred per clock. This                    */

/**************************************************************************************************************************/
/*   FIU UMA Control and Status Register (FIU_UMA_CTS)                                                                    */
/**************************************************************************************************************************/
#define  FIU_UMA_CTS(n)                    (FIU_BASE_ADDR(n) + 0x0C) , FIU_ACCESS , 32            /* Offset: 0Ch */
#define  FIU_UMA_CTS_RDYIE               25 , 1              /* 25 RDYIE (UMA Ready Interrupt Enable). Enables an interrupt if the RDYST bit is set.                                  */
#define  FIU_UMA_CTS_RDYST               24 , 1              /* 24 RDYST (UMA Ready Status). Indicates the UMA state machine has finished a transaction. This bit is                  */
#define  FIU_UMA_CTS_SW_CS               16 , 1              /* 16 SW_CS (Software-Controlled Chip Select). When set to 0, activates the flash chip-select (F_CSx)                    */
#define  FIU_UMA_CTS_DEV_NUM             8 , 2              /* 9-8 DEV_NUM (Device Number). Selects the chip select to be used in the following UMA transaction.                     */
#define  FIU_UMA_CTS_EXEC_DONE           0 , 1               /* 0 EXEC_DONE (Operation Execute/Done). Writing 1 triggers a UMA flash transaction.                                     */

/**************************************************************************************************************************/
/*   FIU UMA Command Register (FIU_UMA_CMD)                                                                               */
/**************************************************************************************************************************/
#define  FIU_UMA_CMD(n)                    (FIU_BASE_ADDR(n) + 0x10) , FIU_ACCESS , 32               /* Offset: 10h */
#define  FIU_UMA_CMD_DUM3                24 , 8             /* 31-24 DUM3 (Third Dummy Byte). Source of the data output during third and subsequent dummy bytes (if                  */
#define  FIU_UMA_CMD_DUM2                16 , 8             /* 23-16 DUM2 (Second Dummy Byte). Source of the data output during second dummy byte (if DBDSIZ > 1).                   */
#define  FIU_UMA_CMD_DUM1                8 , 8              /* 15-8 DUM1 (First Dummy Byte). Source of the data output during first dummy byte (if DBDSIZ > 0). This                 */
#define  FIU_UMA_CMD_CMD                 0 , 8              /* 7-0 CMD (Command Byte). Source of the command byte sent in the UMA SPI transaction (if CMDSIZ is                      */

/**************************************************************************************************************************/
/*   FIU UMA Address Register (FIU_UMA_ADDR)                                                                              */
/**************************************************************************************************************************/
#define  FIU_UMA_ADDR(n)                   (FIU_BASE_ADDR(n) + 0x14) , FIU_ACCESS , 32      /* Offset: 14h */
#define  FIU_UMA_ADDR_UMA_ADDR           0 , 32             /* 31-0 UMA_ADDR (UMA address). Source of the address bytes sent in the UMA SPI transaction. Bits 31-24                  */

/**************************************************************************************************************************/
/*   FIU UMA Write Data Bytes 0-3 Register (FIU_UMA_DW0)                                                                  */
/**************************************************************************************************************************/
#define  FIU_UMA_DW(module, reg)           (FIU_BASE_ADDR(module) + 0x20 + 4*(reg)) , FIU_ACCESS , 32   /* Offset: 20h-2Ch */
#define  FIU_UMA_DW0(n)                    (FIU_BASE_ADDR(n) + 0x20) , FIU_ACCESS , 32      /* Offset: 20h */
#define  FIU_UMA_DW0_WB3                 24 , 8             /* 31-24 WB3 (Write Byte 3). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >3).             */
#define  FIU_UMA_DW0_WB2                 16 , 8             /* 23-16 WB2 (Write Byte 2). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >2).             */
#define  FIU_UMA_DW0_WB1                 8 , 8              /* 15-8 WB1 (Write Byte 1). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >1).              */
#define  FIU_UMA_DW0_WB0                 0 , 8              /* 7-0 WB0 (Write Byte 0). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >0).               */

/**************************************************************************************************************************/
/*   FIU UMA Write Data Bytes 4-7 Register (FIU_UMA_DW1)                                                                  */
/**************************************************************************************************************************/
#define  FIU_UMA_DW1(n)                    (FIU_BASE_ADDR(n) + 0x24) , FIU_ACCESS , 32      /* Offset: 24h */
#define  FIU_UMA_DW1_WB7                 24 , 8             /* 31-24 WB7 (Write Byte 7). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >7).             */
#define  FIU_UMA_DW1_WB6                 16 , 8             /* 23-16 WB6 (Write Byte 6). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >6).             */
#define  FIU_UMA_DW1_WB5                 8 , 8              /* 15-8 WB5 (Write Byte 5). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >5).              */
#define  FIU_UMA_DW1_WB4                 0 , 8              /* 7-0 WB4 (Write Byte 4). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >4).               */

/**************************************************************************************************************************/
/*   FIU UMA Write Data Bytes 8-11 Register (FIU_UMA_DW2)                                                                 */
/**************************************************************************************************************************/
#define  FIU_UMA_DW2(n)                    (FIU_BASE_ADDR(n) + 0x28) , FIU_ACCESS , 32      /* Offset: 28h */
#define  FIU_UMA_DW2_WB11                24 , 8             /* 31-24 WB11 (Write Byte 11). Source of the write data bytes sent in the UMA SPI transaction                            */
#define  FIU_UMA_DW2_WB10                16 , 8             /* 23-16 WB10 (Write Byte 10). Source of the write data bytes sent in the UMA SPI transaction                            */
#define  FIU_UMA_DW2_WB9                 8 , 8              /* 15-8 WB9 (Write Byte 9). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >9).              */
#define  FIU_UMA_DW2_WB8                 0 , 8              /* 7-0 WB8 (Write Byte 8). Source of the write data bytes sent in the UMA SPI transaction (if WDATSIZ >8).               */

/**************************************************************************************************************************/
/*   FIU UMA Write Data Bytes 12-15 Register (FIU_UMA_DW3)                                                                */
/**************************************************************************************************************************/
#define  FIU_UMA_DW3(n)                    (FIU_BASE_ADDR(n) + 0x2C) , FIU_ACCESS , 32      /* Offset: 2Ch */
#define  FIU_UMA_DW3_WB15                24 , 8             /* 31-24 WB15 (Write Byte 15). Source of the write data bytes sent in the UMA SPI transaction                            */
#define  FIU_UMA_DW3_WB14                16 , 8             /* 23-16 WB14 (Write Byte 14). Source of the write data bytes sent in the UMA SPI transaction                            */
#define  FIU_UMA_DW3_WB13                8 , 8              /* 15-8 WB13 (Write Byte 13). Source of the write data bytes sent in the UMA SPI transaction                             */
#define  FIU_UMA_DW3_WB12                0 , 8              /* 7-0 WB12 (Write Byte 12). Source of the write data bytes sent in the UMA SPI transaction                              */

/**************************************************************************************************************************/
/*   FIU UMA Read Data Bytes 0-3 Register (FIU_UMA_DR0)                                                                   */
/**************************************************************************************************************************/
#define  FIU_UMA_DR(module, reg)           (FIU_BASE_ADDR(module) + 0x30 + 4*(reg)) , FIU_ACCESS , 32   /* Offset: 30h-3Ch */
#define  FIU_UMA_DR0(n)                    (FIU_BASE_ADDR(n) + 0x30) , FIU_ACCESS , 32      /* Offset: 30h */
#define  FIU_UMA_DR0_RB3                 24 , 8             /* 31-24 RB3 (Read Byte 3). Destination of the read data bytes received in the UMA SPI transaction                       */
#define  FIU_UMA_DR0_RB2                 16 , 8             /* 23-16 RB2 (Read Byte 2). Destination of the read data bytes received in the UMA SPI transaction                       */
#define  FIU_UMA_DR0_RB1                 8 , 8              /* 15-8 RB1 (Read Byte 1). Destination of the read data bytes received in the UMA SPI transaction                        */
#define  FIU_UMA_DR0_RB0                 0 , 8              /* 7-0 RB0 (Read Byte 0). Destination of the read data bytes received in the UMA SPI transaction                         */

/**************************************************************************************************************************/
/*   FIU UMA Read Data Bytes 4-7 Register (FIU_UMA_DR1)                                                                   */
/**************************************************************************************************************************/
#define  FIU_UMA_DR1(n)                    (FIU_BASE_ADDR(n) + 0x34) , FIU_ACCESS , 32      /* Offset: 34h */
#define  FIU_UMA_DR1_RB15                24 , 8             /* 31-24 RB15 (Read Byte 7). Destination of the read data bytes received in the UMA SPI transaction                      */
#define  FIU_UMA_DR1_RB14                16 , 8             /* 23-16 RB14 (Read Byte 6). Destination of the read data bytes received in the UMA SPI transaction                      */
#define  FIU_UMA_DR1_RB13                8 , 8              /* 15-8 RB13 (Read Byte 5). Destination of the read data bytes received in the UMA SPI transaction                       */
#define  FIU_UMA_DR1_RB12                0 , 8              /* 7-0 RB12 (Read Byte 4). Destination of the read data bytes received in the UMA SPI transaction                        */

/**************************************************************************************************************************/
/*   FIU UMA Read Data Bytes 8-11 Register (FIU_UMA_DR2)                                                                  */
/**************************************************************************************************************************/
#define  FIU_UMA_DR2(n)                    (FIU_BASE_ADDR(n) + 0x38) , FIU_ACCESS , 32      /* Offset: 38h */
#define  FIU_UMA_DR2_RB15                24 , 8             /* 31-24 RB15 (Read Byte 11). Destination of the read data bytes received in the UMA SPI transaction                     */
#define  FIU_UMA_DR2_RB14                16 , 8             /* 23-16 RB14 (Read Byte 10). Destination of the read data bytes received in the UMA SPI transaction                     */
#define  FIU_UMA_DR2_RB13                8 , 8              /* 15-8 RB13 (Read Byte 9). Destination of the read data bytes received in the UMA SPI transaction                       */
#define  FIU_UMA_DR2_RB12                0 , 8              /* 7-0 RB12 (Read Byte 8). Destination of the read data bytes received in the UMA SPI transaction                        */

/**************************************************************************************************************************/
/*   FIU UMA Read Data Bytes 12-15 Register (FIU_UMA_DR3)                                                                 */
/**************************************************************************************************************************/
#define  FIU_UMA_DR3(n)                    (FIU_BASE_ADDR(n) + 0x3C) , FIU_ACCESS , 32      /* Offset: 3Ch */
#define  FIU_UMA_DR3_RB15                24 , 8             /* 31-24 RB15 (Read Byte 15). Destination of the read data bytes received in the UMA SPI transaction                     */
#define  FIU_UMA_DR3_RB14                16 , 8             /* 23-16 RB14 (Read Byte 14). Destination of the read data bytes received in the UMA SPI transaction                     */
#define  FIU_UMA_DR3_RB13                8 , 8              /* 15-8 RB13 (Read Byte 13). Destination of the read data bytes received in the UMA SPI transaction                      */
#define  FIU_UMA_DR3_RB12                0 , 8              /* 7-0 RB12 (Read Byte 12). Destination of the read data bytes received in the UMA SPI transaction                       */

/**************************************************************************************************************************/
/*   FIU Protection Configuration Register (FIU_PRT_CFG)                                                                  */
/**************************************************************************************************************************/
#define  FIU_PRT_CFG(n)                    (FIU_BASE_ADDR(n) + 0x18) , FIU_ACCESS , 32      /* Offset: 18h */
#define  FIU_PRT_CFG_LCK                 31 , 1              /* 31 LCK (Lock). When the bit is set, writing to this register is ignored. This bit is not reset by setting the         */
#define  FIU_PRT_CFG_PEN                 30 , 1              /* 30 PEN (Protection Enable). Enables the protection mechanism.                                                         */
#define  FIU_PRT_CFG_DEVSIZ              8 , 3              /* 10-8 DEVSIZ (Device Size). Defines the size of the devices on CS0 and CS1. When protection is enabled,                */
#define  FIU_PRT_CFG_OCALWD              4 , 1               /* 4 OCALWD (Other Commands Allowed). Defines if commands not defined in the FIU_PRT_CMD9-0                              */
#define  FIU_PRT_CFG_PRTASIZ             0 , 2              /* 1-0 PRTASIZ (Protected Area Size). Selects the size of the area protected by the protection mechanism.                */

/**************************************************************************************************************************/
/*   FIU Protection Command Register (FIU_PRT_CMD0-9)                                                                     */
/**************************************************************************************************************************/
#define  FIU_PRT_CMD0(n)                   (FIU_BASE_ADDR(n) + 0x40) , FIU_ACCESS , 32      /* Offset: FIU_PRT_CMD0: 40h */
#define  FIU_PRT_CMD1(n)                   (FIU_BASE_ADDR(n) + 0x44) , FIU_ACCESS , 32      /* FIU_PRT_CMD1: 44h  */
#define  FIU_PRT_CMD2(n)                   (FIU_BASE_ADDR(n) + 0x48) , FIU_ACCESS , 32      /* FIU_PRT_CMD2: 48h  */
#define  FIU_PRT_CMD3(n)                   (FIU_BASE_ADDR(n) + 0x4C) , FIU_ACCESS , 32      /* FIU_PRT_CMD3: 4Ch  */
#define  FIU_PRT_CMD4(n)                   (FIU_BASE_ADDR(n) + 0x50) , FIU_ACCESS , 32      /* FIU_PRT_CMD4: 50h  */
#define  FIU_PRT_CMD5(n)                   (FIU_BASE_ADDR(n) + 0x54) , FIU_ACCESS , 32      /* FIU_PRT_CMD5: 54h  */
#define  FIU_PRT_CMD6(n)                   (FIU_BASE_ADDR(n) + 0x58) , FIU_ACCESS , 32      /* FIU_PRT_CMD6: 58h  */
#define  FIU_PRT_CMD7(n)                   (FIU_BASE_ADDR(n) + 0x5C) , FIU_ACCESS , 32      /* FIU_PRT_CMD7: 5Ch  */
#define  FIU_PRT_CMD8(n)                   (FIU_BASE_ADDR(n) + 0x60) , FIU_ACCESS , 32      /* FIU_PRT_CMD8: 60h  */
#define  FIU_PRT_CMD9(n)                   (FIU_BASE_ADDR(n) + 0x64) , FIU_ACCESS , 32      /* FIU_PRT_CMD9: 64h  */
#define  FIU_PRT_CMD9_ADBPCKB            29 , 2             /* 30-29 ADBPCKB (Address B Bits per Clock). Selects how many address B bits are checked per clock.                      */
#define  FIU_PRT_CMD9_CMBPCKB            27 , 2             /* 28-27 CMBPCKB (Command B Bits per Clock). Selects how many command B bits are checked per clock.                      */
#define  FIU_PRT_CMD9_ADDSZB             26 , 1              /* 26 ADDSZB (Address Size for limiting command B). Defines the address size of command B for limit                      */
#define  FIU_PRT_CMD9_FRBDCB             24 , 2             /* 25-24 FRBDCB (Forbid Command B). Defines if Command B is forbidden.                                                   */
#define  FIU_PRT_CMD9_CMDB               16 , 8             /* 23-16 CMDB (Command B). Sets the value of command B.                                                                  */
#define  FIU_PRT_CMD9_ADBPCKA            13 , 2             /* 14-13 ADBPCKA (Address A Bits per Clock). Selects how many address A bits are checked per clock.                      */
#define  FIU_PRT_CMD9_CMBPCKA            11 , 2             /* 12-11 CMBPCKA (Command Bits per Clock A). Selects how many command A bits are checked per clock.                      */
#define  FIU_PRT_CMD9_ADDSZA             10 , 1              /* 10 ADDSZA (Address Size for limiting command A). Defines the address size of command A for limit                      */
#define  FIU_PRT_CMD9_FRBDCA             8 , 2              /* 9-8 FRBDCA (Forbid Command A). Defines if Command A is forbidden.                                                     */
#define  FIU_PRT_CMD9_CMDA               0 , 8              /* 7-0 CMDA (Command A). Sets the value of a command A.                                                                  */

/**************************************************************************************************************************/
/*   FIU Status Polling Configuration Register (FIU_STPL_CFG)                                                             */
/**************************************************************************************************************************/
#define  FIU_STPL_CFG(n)                   (FIU_BASE_ADDR(n) + 0x1C) , FIU_ACCESS , 32      /* Offset: 1Ch */
#define  FIU_STPL_CFG_LCK                31 , 1              /* 31 LCK (Lock). When the bit is set, writing to this register is ignored, except for ENPOL, RDY and BUST               */
#define  FIU_STPL_CFG_BUST               30 , 1              /* 30 BUST (Busy Status). Represents the status of the flash after a direct write. This bit is set by hardware           */
#define  FIU_STPL_CFG_RDYIE              29 , 1              /* 29 RDYIE (Ready Interrupt Enable). Enables an interrupt request when RDY bit is set.                                  */
#define  FIU_STPL_CFG_RDY                28 , 1              /* 28 RDY (Ready Status). A status bit for interrupt generation. It is set by detection of ready status by the           */
#define  FIU_STPL_CFG_SPDWR              27 , 1              /* 27 SPDWR (Start Polling on Direct Write). Enables setting the ENPOL bit after a direct write to the flash.            */
#define  FIU_STPL_CFG_POLLPER            16 , 11            /* 26-16 POLLPER (Polling Period). Sets the period of the Read Status command sent to the SPI flash, in SPI              */
#define  FIU_STPL_CFG_ENPOL              12 , 1              /* 12 ENPOL (Enable Polling). Enables sending periodic Read Status commands. This bit is cleared by                      */
#define  FIU_STPL_CFG_BUSYPOL            11 , 1              /* 11 BUSYPOL (Busy bit Polarity). Defines the polarity of the selected bit.                                             */
#define  FIU_STPL_CFG_BUSYBS             8 , 3              /* 10-8 BUSYBS (Busy Bit Select). Selects which of the bits of the read value from the status register is the            */
#define  FIU_STPL_CFG_CMD                0 , 8              /* 7-0 CMD (Command). Sets the value of the Status Read command. The default is 05h.                                     */

/**************************************************************************************************************************/
/*   FIU Configuration Register (FIU_CFG)                                                                                 */
/**************************************************************************************************************************/
#define  FIU_CFG(n)                        (FIU_BASE_ADDR(n) + 0x78) , FIU_ACCESS , 32      /* Offset: 78h */
#define  FIU_CFG_SPI_CS_INACT            1 , 3              /* 3-1 SPI_CS_INACT (SPI Chip Select, Inactive Time). Selects the minimum number of FCLK cycles during                   */
#define  FIU_CFG_INCRSING                0 , 1               /* 0 INCRSING (INCR as Singles). Defines treating INCR burst as single beats on the bus. Each beat on                    */

/**************************************************************************************************************************/
/*   FIU Version Register (FIU_VER) updated                                                                               */
/**************************************************************************************************************************/
#define  FIU_VER(n)                        (FIU_BASE_ADDR(n) + 0x7C) , FIU_ACCESS , 32      /* Offset: 7Ch */
#define  FIU_VER_FIU_VER                 0 , 8              /* 7-0 FIU_VER (FIU Version). Indicates the current version of the FIU module. In the , its value is 40h.                */



/*---------------------------------------------------------------------------------------------------------*/
/* FIU Field value enumeration                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FIU_DRD_CFG_ADDSIZ_VAL_tag
{
    FIU_DRD_CFG_ADDSIZE_24BIT = 0,   /* 0 0: 24 bits (3 bytes) (default).  */
    FIU_DRD_CFG_ADDSIZE_32BIT = 1    /* 0 1: 32 bits (4 bytes)             */
}  FIU_DRD_CFG_ADDSIZ_VAL;

#endif /* _FIU_REGS_H */

