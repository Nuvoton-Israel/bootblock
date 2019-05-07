/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   boot.h                                                                   */
/*            This file contains API of routines to boot an image             */
/*  Project:                                                                  */
/*            ROM Code                                                        */
/*----------------------------------------------------------------------------*/

#ifndef _BOOT_H
#define _BOOT_H

#include "hal.h"
#include "shared_defs.h"



/*---------------------------------------------------------------------------------------------------------*/
/* Boot module definitions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define FLASH_EXAM_ADDR_1       (SPI0CS0_BASE_ADDR)
#define FLASH_EXAM_ADDR_2       (SPI0CS0_BASE_ADDR + 0x80000)
#define FLASH_EXAM_ADDR_3       (SPI0CS1_BASE_ADDR)


/*---------------------------------------------------------------------------------------------------------*/
/* Boot code image structure                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#pragma pack(1)
typedef union BOOT_HEADER_tag
{
	struct
	{
									        // Offset    Size(bytes) Description                            Signed  Used in ROM Code for Basic Boot
		UINT8   startTag[START_TAG_SIZE];      // 0         8           AA55_0750h, 424F_4F54h (‘BOOT’)                                 No      Yes
		UINT32  signature[SIGNATURE_SIZE >> 2];// 8         256         Boot Block SHA256 signature decrypted with Customer’s Private Key   No      No
		UINT32  fiu0_drd_cfg_set;              // 0x108       4           Word contents copied by ROM code do FIU0 FIU_DRD_CFG register1  No      No

		UINT8   fiu0_clk_divider;              // 0x10C       1           Defines the clock divide ratio from AHB to FIU0 clock.1         No      No
		UINT8   fiu0_cs1_en;                   // 0x10D (At uboot header only) 269       1           FIU0 CS1 enable (UBOOT header only!)
		UINT8   fiu0_cs2_en;                   // 0x10E (At uboot header only) 270       1           FIU0 CS2 enable (UBOOT header only!)
		UINT8   fiu0_cs3_en;                   // 0x10F (At uboot header only) 271       1           FIU0 CS3 enable (UBOOT header only!)

		UINT32  fiu3_drd_cfg_set;              // 0x110 (At uboot header only) 272       4           Word contents copied by BB code do FIU3 FIU_DRD_CFG register1  No      No
		UINT32  fiu3_dwr_cfg_set;              // 0x114 (At uboot header only) 276       4           Word contents copied by BB code do FIU3 FIU_DWR_CFG register1  No      No

		UINT8   fiu3_clk_divider;              // 0x118 (At uboot header only) 280       1           Defines the clock divide ratio from AHB to FIU3 clock.1         No      No
		UINT8   fiu3_cs1_en;                   // 0x119 (At uboot header only) 282       1           FIU3 CS1 enable
		UINT8   fiu3_cs2_en;                   // 0x11A (At uboot header only) 283       1           FIU3 CS2 enable
		UINT8   fiu3_cs3_en;                   // 0x11B (At uboot header only) 284       1           FIU3 CS3 enable

		UINT16  mc_freq;                       // 0x11C       2
		UINT8   reserved[0x22];                // 0x11E       34          Reserved for future use

		//  Start signed area:
		UINT32  destAddr;                      // 0x140       4           Code destination address2, 32-bit aligned                       Yes     No
		UINT32  codeSize;                      // 0x144       4           Boot Block Code size                                            Yes     No
		UINT32  version;                       // 0x148       4           Version (Major.Minor)                                           Yes     No

		UINT32  vendor;                        // 0x14C       4
		UINT32  board_type;                    // 0x150       4

		UINT16  cpu_freq;                      // 0x154       2
		UINT8   mc_config;                     // 0x156       1
		UINT8   host_if;                       // 0x157       1

		UINT32  fiu0_dwr_cfg_set;              // 0x158       4           Word contents copied by BB code to FIU0 FIU_DWR_CFG register1   Yes     No
	        UINT8   reservedSigned[0xA4];          // 0x15C    164         Reserved for future use
	} header ;

	UINT8    bytes[512];

	UINT32   words[128];

}  BOOT_HEADER_T;
#pragma pack()


/*---------------------------------------------------------------------------------------------------------*/
/* Device types supported by this bootblock                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum DEVICE_T
{
	DEVICE_NPCM750   = 0,
	DEVICE_NPCM730   = 1,
} DEVICE_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Board type                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum   BOARD_T
{

	BOARD_SVB       = 1,
	BOARD_EB        = 2,
	BOARD_RUN_BMC   = 10,



	BOARD_UNKNOWN   = -1,

} BOARD_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Vendor (of the board)                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum   BOARD_VENDOR_T
{
	VENDOR_CUSTOMER_0  = 0,  // use external customization file : bootblock_$vendor$_cutomoize.c
	VENDOR_CUSTOMER_1  = 1,  // use external customization file : bootblock_$vendor$_cutomoize.c
	VENDOR_CUSTOMER_2  = 2,  // use external customization file : bootblock_$vendor$_cutomoize.c
	VENDOR_CUSTOMER_3  = 3,  // use external customization file : bootblock_$vendor$_cutomoize.c
	VENDOR_CUSTOMER_4  = 4,  // use external customization file : bootblock_$vendor$_cutomoize.c
	VENDOR_CUSTOMER_5  = 5,  // use external customization file : bootblock_$vendor$_cutomoize.c
	VENDOR_CUSTOMER_6  = 6,  // use external customization file : bootblock_$vendor$_cutomoize.c

	VENDOR_NUVOTON     = 100,  // The headen champion. Select options specific for EB and SVB. useing bootblock_nuvoton_cutomoize.c.
	VENDOR_UNKNOWN     = 101
} BOARD_VENDOR_T;


/*---------------------------------------------------------------------------------------------------------*/
/* HOST IF                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum   HOST_IF_T
{
	HOST_IF_UNKNOWN_0  = 0xFF,  // BB does not change ROM code configuration
	HOST_IF_LPC        = 0,     // BB selects HOST IF LPC.
	HOST_IF_ESPI       = 1,     // BB selects HOST IF ESPI
	HOST_IF_GPIO       = 2,     // No IF is configured. Pins are set as GPIOs.
} HOST_IF_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Header Field                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum     HEADER_FIELD_T
{
	HEADER_FIELD_STARTTAG = 0,
	HEADER_FIELD_SIGNATURE = 1,
	HEADER_FIELD_FIU0_DRD_CFG_SET =2,
	HEADER_FIELD_FIU0_CLK_DIVIDER =3,
	HEADER_FIELD_FIU0_CS1_EN =4,
	HEADER_FIELD_FIU0_CS2_EN=5,
	HEADER_FIELD_FIU0_CS3_EN=6,
	HEADER_FIELD_FIU3_DRD_CFG_SET=7,
	HEADER_FIELD_FIU3_DWR_CFG_SET=8,
	HEADER_FIELD_FIU3_CLK_DIVIDER=9,
	HEADER_FIELD_FIU3_CS1_EN=11,
	HEADER_FIELD_FIU3_CS2_EN=12,
	HEADER_FIELD_FIU3_CS3_EN=13,
	HEADER_FIELD_RESERVED0=14,
	HEADER_FIELD_RESERVED1=15,
	HEADER_FIELD_RESERVED2=16,
	HEADER_FIELD_BOARD_TYPE=17,
	HEADER_FIELD_VENDOR=18,
	HEADER_FIELD_MC_FREQ=19,
	HEADER_FIELD_CPU_FREQ=20,
	HEADER_FIELD_HOST_IF=21,
	HEADER_FIELD_SECURITY_LEVEL=22,
	HEADER_FIELD_MC_CONFIG=23,
	HEADER_FIELD_DESTADDR=24,
	HEADER_FIELD_CODESIZE=25,
	HEADER_FIELD_VERSION=26,
	HEADER_FIELD_SECURITY_LOG=27, // do not reorder the next 6 fields.
	HEADER_FIELD_SECURITY_LOG_SIZE=28,
	HEADER_FIELD_HOLE0=29,
	HEADER_FIELD_HOLE0_SIZE=30,
	HEADER_FIELD_HOLE1=31,
	HEADER_FIELD_HOLE1_SIZE=32,
	HEADER_FIELD_HOLE2=33,
	HEADER_FIELD_HOLE2_SIZE=34,
	HEADER_FIELD_HOLE3=35,
	HEADER_FIELD_HOLE3_SIZE=36,
	HEADER_FIELD_REVOKE_KEYS=37,
	HEADER_FIELD_GPIO_HEARTBEAT=38,
	HEADER_FIELD_WD_NUM=39,
	HEADER_FIELD_WD_STAY_ON=40
} HEADER_FIELD_T;

typedef struct {
	UINT8 VersionDescription[0xB0];
	UINT32 BootBlockTag;
	UINT32 BootblockVersion;
} BOOTBLOCK_Version_T;

typedef struct {
	UINT32 ChipID;
	UINT32 RomVersion;
} ROM_Version_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Boot module exported functions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS    BOOTBLOCK_GetLoadedBootBlockHeader (BOOT_HEADER_T** uBootHeader, UINT32* image);
DEFS_STATUS    BOOTBLOCK_Init_FIU (BOOT_HEADER_T *uBootHeader, UINT8 image);
DEFS_STATUS    BOOTBLOCK_FindSpiFlashImage  (BOOLEAN        sigCheck);
DEFS_STATUS    BOOTBLOCK_CheckImageCopyAndJump (BOOT_HEADER_T *uBootHeader, BOOLEAN sigCheck, UINT8 image, BOOLEAN bJump);
void           BOOTBLOCK_JumpToUBOOT (BOOT_HEADER_T *uBootHeader, UINT8 image);

DEFS_STATUS    BOOTBLOCK_ReadHeaderParameter (HEADER_FIELD_T field, UINT8 * retVal, BOOT_HEADER_T *bootBlockHeader);
BOARD_T        BOOTBLOCK_GetBoardType (void);
BOARD_VENDOR_T BOOTBLOCK_GetVendorType (void);

UINT32         BOOTBLOCK_Get_MC_freq (void);
UINT32         BOOTBLOCK_Get_CPU_freq (void);
UINT8          BOOTBLOCK_Get_MC_config (void);
HOST_IF_T      BOOTBLOCK_Get_host_if (void);


/*---------------------------------------------------------------------------------------------------------*/
/* External board vendor 0 specific functions                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void           BOOTBLOCK_Print_Vendor_error (void);

void           BOOTBLOCK_Print_Nuvoton (void);
void           BOOTBLOCK_Init_UART_Nuvoton (void);
DEFS_STATUS    BOOTBLOCK_Init_CLK_Nuvoton (void);
void           BOOTBLOCK_Init_Before_Image_Check_Nuvoton (void);
void           BOOTBLOCK_Init_Before_UBOOT_Nuvoton (void);
void           BOOTBLOCK_Init_GPIO_Nuvoton (void);


/*---------------------------------------------------------------------------------------------------------*/
/* External board vendor 0 specific functions                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void           BOOTBLOCK_Print_Vendor_0 (void);
void           BOOTBLOCK_Init_UART_Vendor_0 (void);
DEFS_STATUS    BOOTBLOCK_Init_CLK_Vendor_0 (void);
void           BOOTBLOCK_Init_Before_Image_Check_Vendor_0 (void);
void           BOOTBLOCK_Init_Before_UBOOT_Vendor_0 (void);
void           BOOTBLOCK_Init_GPIO_Vendor_0 (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton specific functions                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void           BOOTBLOCK_Print_Vendor_1 (void);
void           BOOTBLOCK_Init_UART_Vendor_1 (void);
DEFS_STATUS    BOOTBLOCK_Init_CLK_Vendor_1 (void);
void           BOOTBLOCK_Init_Before_Image_Check_Vendor_1 (void);
void           BOOTBLOCK_Init_Before_UBOOT_Vendor_1 (void);
void           BOOTBLOCK_Init_GPIO_Vendor_1 (void);


/*---------------------------------------------------------------------------------------------------------*/
/* External board vendor 2 specific functions                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void           BOOTBLOCK_Print_Vendor_2 (void);
void           BOOTBLOCK_Init_UART_Vendor_2 (void);
DEFS_STATUS    BOOTBLOCK_Init_CLK_Vendor_2 (void);
void           BOOTBLOCK_Init_Before_Image_Check_Vendor_2 (void);
void           BOOTBLOCK_Init_Before_UBOOT_Vendor_2 (void);
void           BOOTBLOCK_Init_GPIO_Vendor_2 (void);

/*---------------------------------------------------------------------------------------------------------*/
/* External board vendor 3 specific functions                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void           BOOTBLOCK_Print_Vendor_3 (void);
void           BOOTBLOCK_Init_UART_Vendor_3 (void);
DEFS_STATUS    BOOTBLOCK_Init_CLK_Vendor_3 (void);
void           BOOTBLOCK_Init_Before_Image_Check_Vendor_3 (void);
void           BOOTBLOCK_Init_Before_UBOOT_Vendor_3 (void);
void           BOOTBLOCK_Init_GPIO_Vendor_3 (void);

#endif /* _BOOT_H */
