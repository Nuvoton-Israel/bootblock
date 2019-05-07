/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   mailbox.h                                                                */
/*            This file contains API of routines for handling the PCI MailBox */
/*  Project:                                                                  */
/*            Poleg Bootblock and ROM Code (shared header)                    */
/*----------------------------------------------------------------------------*/

#ifndef _MAILBOX_H
#define _MAILBOX_H

#include "hal.h"
#include "shared_defs.h"
#include "cfg.h"

/*---------------------------------------------------------------------------------------------------------*/
/* MailBox module definitions                                                                              */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Rom status                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum ROM_CODE_MAILBOX_STATUS_E_tag
{
 // Status Name                                   Status Code        Description
    ST_ROM_NO_STATUS =                            0x00,            // NO STATUS (0x00)
	ST_ROM_INIT =                                 0x01,            // ROM code started running
	ST_ROM_CFG_CLK_1 =                            0x02,            // CLK configuration
	ST_ROM_SKIPINIT =                             0x03,            // CLK configuration not performed
	ST_ROM_UFPP_NO_SEC =                          0x05,            // UART Command Mode without security
    ST_ROM_SPI_BOOT_SECURE =                      0x06,            // Search bootable image on SPI flash in secure mode
    ST_ROM_SPI_BOOT_BASIC =                       0x07,            // Search bootable image on SPI flash in none secure mode
	ST_ROM_UFPP_NO_IMAGE =                        0x09,            // UART Command Mode after no bootable image found
	ST_ROM_CFG_CLK_2 =                            0x0A,            // CLK configuration after no bootable image found
	ST_ROM_SEC_FAIL =                             0x0B,            // Security check failed
	ST_ROM_HALT =                                 0x0C,            // ROM code halted after security check failure
	ST_ROM_HALT_BYPASS =                          0x0D,            // Continue boot after security check failure
	ST_ROM_UART =                                 0x0E,            // UART Command Mode after security check failure
	ST_ROM_HALT2 =                                0x0F,            // Security failed and no bootable image found
	ST_ROM_INDICATE_RESET_TYPE =                  0x12,            // ROM code is checking the reset type.
	ST_ROM_USER_CFG =                             0x13,            // user configuration
	ST_ROM_DERIVATIVES =                          0x14,            // config derivatives and fuse locks.

	ST_ROM_FUP_UART2_NORMAL_BOOT =                0x15,            // UART programming via UART2, in normal boot and strap FUP (9,10) settings is 0x1
	ST_ROM_FUP_UART3_NORMAL_BOOT =                0x16,            // UART programming via UART3. in normal boot and strap FUP (9,10) settings is 0x2
    ST_ROM_FUP_UART2_NORMAL_BOOT_FAIL =           0x17,            // UART programming via UART2, in normal boot after no bootable image found
    ST_ROM_FUP_UART2_SEC_AND_NORMAL_FAIL =        0x18,            // Both secure boot and normal boot failed (no halt, strap FUP (9,10) settings are not 0x1 or 0x2)
    ST_ROM_FUP_UART2_SECURE_BOOT =                0x19,            // UART programming via UART2 after secure boot failure, and strap FUP (9,10) settings is 0x1
	ST_ROM_FUP_UART3_SECURE_BOOT =                0x20,            // UART programming via UART3 after secure boot failure, and strap FUP (9,10) settings is 0x2 or 0x03

    ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET0 =           0x21,            // Select image at SPI0 CS0 offset 0      ( address 0x80000000)
    ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET80000 =       0x22,            // Select image at SPI0 CS0 offset 8000   ( address 0x80080000)
    ST_ROM_BASIC_USE_IMAGE_SPI0_CS1_OFFSET0 =           0x23,            // Select image at SPI0 CS1 offset 0      ( address 0x88000000)

    ST_ROM_CHECK_IMAGE_SPI0_CS0_OFFSET0 =         0x24,            // Checking image at SPI0 CS0 offset 0    ( address 0x80000000)
    ST_ROM_CHECK_IMAGE_SPI0_CS0_OFFSET80000 =     0x25,            // Checking image at SPI0 CS0 offset 8000 ( address 0x80080000)
    ST_ROM_CHECK_IMAGE_SPI0_CS1_OFFSET0 =         0x26,            // Checking image at SPI0 CS1 offset 0    ( address 0x88000000)

    ST_ROM_USE_KEY0_IMAGE0 =                      0x27,            // checking image 0, select Pk0 according to fuses for signature varification.
    ST_ROM_USE_KEY1_IMAGE0 =                      0x28,            // checking image 0, select Pk1 according to fuses for signature varification.
    ST_ROM_USE_KEY2_IMAGE0 =                      0x29,            // checking image 0, select Pk2 according to fuses for signature varification.

    ST_ROM_USE_KEY0_IMAGE1 =                      0x2A,            // checking image 1, select Pk0 according to fuses for signature varification.
    ST_ROM_USE_KEY1_IMAGE1 =                      0x2B,            // checking image 1, select Pk1 according to fuses for signature varification.
    ST_ROM_USE_KEY2_IMAGE1 =                      0x2C,            // checking image 1, select Pk2 according to fuses for signature varification.

    ST_ROM_USE_KEY0_IMAGE2 =                      0x2D,            // checking image 2, select Pk0 according to fuses for signature varification.
    ST_ROM_USE_KEY1_IMAGE2 =                      0x2E,            // checking image 2, select Pk1 according to fuses for signature varification.
    ST_ROM_USE_KEY2_IMAGE2 =                      0x2F,            // checking image 2, select Pk2 according to fuses for signature varification.

    ST_ROM_HALT_SECURITY_CPU_STOP_CONFLICT =      0x30,            // If ( (oSecBootDisable == 1)  &&  ( (SECBOOT_ON)  )  then Conflict: jump to step 15 (halt)


} ROM_CODE_MAILBOX_STATUS_E;


/*---------------------------------------------------------------------------------------------------------*/
/* Image states                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define  IMAGE_NOT_TESTED                0x00
#define  IMAGE_WRONG_START_TAG           0x01
#define  IMAGE_DEST_ADDRESS_UNALIGNED    0x02
#define  IMAGE_BAD_SIGNATURE             0x04
#define  IMAGE_MEMORY_OVERLAP            0x08
#define  IMAGE_HEADER_OK_COPY_IMAGE      0x10
#define  IMAGE_OK_RUN_FROM_FLASH         0x40
#define  IMAGE_OK                        0x80
#define  IMAGE_REJECTED_BY_BB            0x11
#define  IMAGE_NEW_COPY                  0x12
#define  IMAGE_NOT_IN_USE                0xFF    /* image is not selected - depends on FUSE_FUSTRAP_oAltImgLoc */


/*---------------------------------------------------------------------------------------------------------*/
/* Key state ( per image )                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define  PK_NOT_TESTED                   0x00
#define  PK_INVALID                      0x01
#define  PK_WRONG_SIGNATURE              0x02
#define  PK_OK                           0x03
#define  PK_INVALID_CRC                  0x04
#define  PK_IMAGE_SKIPPED                0xFF   /* image is not selected - depends on FUSE_FUSTRAP_oAltImgLoc */



/*---------------------------------------------------------------------------------------------------------*/
/* MailBox module internal structure definitions                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct _ROM_STATUS_MSG
{
    union
    {
        struct
        {
            UINT8  image0State;
            UINT8  image1State;
            UINT8  image2state;

            union
            {
                struct
                {
                    UINT8  image0_pk0;
                    UINT8  image0_pk1;
                    UINT8  image0_pk2;
                    UINT8  image1_pk0;
                    UINT8  image1_pk1;
                    UINT8  image1_pk2;
                    UINT8  image2_pk0;
                    UINT8  image2_pk1;
                    UINT8  image2_pk2;
                } pk_states;
                UINT8 pk_bytes[9];
            } pk;
        } state ;
        UINT8 bytes[12];
    } imageState ;

    UINT8   startTag[START_TAG_SIZE];
    UINT32  status;

} ROM_STATUS_MSG;
#pragma pack()



/*---------------------------------------------------------------------------------------------------------*/
/* MailBox module internal definitions                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define PCIMAILBOX_START_ADDR   PCIMBX_BASE_ADDR                                 /* 0xF0848000 */
#define PCIMAILBOX_END_ADDR     PCIMBX_BASE_ADDR + 0x4000                        /* 0xF084C000 */
#define ROM_STATUS_MSG_ADDR     (PCIMAILBOX_END_ADDR - sizeof(ROM_STATUS_MSG))   /* 0xF084BFE8 */
#define BOOTBLK_STATUS_MSG_ADDR (ROM_STATUS_MSG_ADDR - sizeof(ROM_STATUS_MSG))   /* 0xF084BFD0 */

#define ROM_MAILBOX_DEBUG0(val)
#define ROM_MAILBOX_DEBUG1(val)
#define ROM_MAILBOX_DEBUG2(val)
#define ROM_MAILBOX_DEBUG3(val)
#define ROM_MAILBOX_DEBUG4(val)
#define ROM_MAILBOX_DEBUG5(val)
#define ROM_MAILBOX_DEBUG6(val)
#define ROM_MAILBOX_DEBUG7(val)
#define ROM_MAILBOX_DEBUG8(val)
#define ROM_MAILBOX_DEBUG9(val)
#define ROM_MAILBOX_DEBUG10(val)
#define ROM_MAILBOX_DEBUG11(val)
#define ROM_MAILBOX_DEBUG12(val)
#define ROM_MAILBOX_DEBUG13(val)
#define ROM_MAILBOX_DEBUG14(val)
#define ROM_MAILBOX_DEBUG15(val)
#define ROM_MAILBOX_DEBUG16(val)
#define ROM_MAILBOX_DEBUG17(val)
#define ROM_MAILBOX_DEBUG18(val)
#define ROM_MAILBOX_DEBUG19(val)


/*---------------------------------------------------------------------------------------------------------*/
/* Mailbox module exported functions                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void    PCIMBX_ResetMailbox            (void);
void    PCIMBX_UpdateMailbox           (UINT32 status);
void    PCIMBX_SetImageState           (UINT8 image, UINT8 state);
void    PCIMBX_SetImageKeyState        (UINT8 image, UINT8 keyInd , UINT8 pk_state);
UINT8   PCIMBX_GetImageKeyState        (UINT8 image, UINT8 keyInd);
void    PCIMBX_NotifyHost              (void);
void    PCIMBX_GetStatus               (ROM_STATUS_MSG *status, UINT8 image, BOOLEAN bROM);
void    PCIMBX_SetStatus               (ROM_STATUS_MSG *status, UINT8 image, BOOLEAN bROM);
INT     PCIMBX_GetLoadedImageNumber    (BOOLEAN bROM);
void    PCIMBX_Print                   (void);



#endif /* _MAILBOX_H */

