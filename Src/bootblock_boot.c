/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   boot.c                                                                   */
/* This file contains routines responsible to find and boot an image from flash */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/
#define BOOT_C

#ifndef NO_LIBC
#include <string.h>
#endif


#include "hal.h"
#include "hal_regs.h"
#include "boot.h"
#include "shared_defs.h"
#include "security.h"
#include "cfg.h"
#include "mailbox.h"
#include "Apps/serial_printf/serial_printf.h"
#include "../SWC_HAL/ModuleDrivers/fiu/40/fiu_regs.h"




#define WTCR_655_MICRO_COUNTER  (BUILD_FIELD_VAL(WTCR_WTR, 1)|BUILD_FIELD_VAL(WTCR_WTRE, 1)|BUILD_FIELD_VAL(WTCR_WTE, 1)) // 0x83


extern void asm_jump_to_address(UINT32 address);


/*---------------------------------------------------------------------------------------------------------*/
/* Boot module local functions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
extern void           (*BOOTBLOCK_Init_Before_Image_Check_Vendor) (void);
extern void           (*BOOTBLOCK_Init_Before_UBOOT_Vendor) (void);
extern void           (*BOOTBLOCK_Init_GPIO_Vendor) (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Init_FIU                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  uBootHeader -    Pointer to the image header to be checked                             */
/*                  image      -    image number (0/1/2)                                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Init FIU according to the header                                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS      BOOTBLOCK_Init_FIU (BOOT_HEADER_T *uBootHeader, UINT8 image)
{
	DEFS_STATUS         result = DEFS_STATUS_FAIL;
	BOOLEAN             isQuad;
	UINT32              fiu_dwr_cfg_set;
	UINT32              fiu_drd_cfg_set;
	UINT8               fiu_clk_divider;

	// Take FIU0 fields
	fiu_drd_cfg_set = uBootHeader->header.fiu0_drd_cfg_set;
	fiu_clk_divider = uBootHeader->header.fiu0_clk_divider;
	fiu_dwr_cfg_set = uBootHeader->header.fiu0_dwr_cfg_set;


	//    Ignored if FIU_Clk_Divider is either 0 or 0FFh.
	if ( (fiu_clk_divider != 0) && (fiu_clk_divider != 0xFF))
	{
		// Config the FIU settings and clocks according to the header
		CLK_ConfigureFIUClock(FIU_MODULE_0, fiu_clk_divider);
		serial_printf(">set FIU0 divider to  %#lx \n", fiu_clk_divider );
	}

	//    Ignored if fiu_drd_cfg_set is either 0 or 0xFFFFFFFF.
	if ( (fiu_drd_cfg_set != 0) && (fiu_drd_cfg_set != 0xFFFFFFFF))
	{
		// Word contents copied to FIUx FIU_DRD_CFG register1
		REG_WRITE(FIU_DRD_CFG(FIU_MODULE_0), fiu_drd_cfg_set);
		serial_printf(">set FIU0 FIU_DRD_CFG to  %#lx \n", fiu_drd_cfg_set );
	}


	if ( (fiu_dwr_cfg_set != 0) && (fiu_dwr_cfg_set != 0xFFFFFFFF))
	{
		// Word contents copied to FIUx FIU_DRD_CFG register1
		REG_WRITE(FIU_DWR_CFG(FIU_MODULE_0), fiu_dwr_cfg_set);
		serial_printf(">set FIU0 FIU_DWR_CFG to  %#lx \n", fiu_dwr_cfg_set );
	}

	/*-------------------------------------------------------------------------------------------------*/
	/* Parse fiu0_drd_cfg_set. If it's quad configure FIU mux differently                              */
	/*-------------------------------------------------------------------------------------------------*/
	isQuad = (FIU_GetReadMode(FIU_MODULE_0) == FIU_FAST_READ_QUAD_IO) ? TRUE: FALSE;


	result = CHIP_MuxFIU ( FIU_MODULE_0,
	        TRUE,
	        uBootHeader->header.fiu0_cs1_en == 1,
	        uBootHeader->header.fiu0_cs2_en == 1,
	        uBootHeader->header.fiu0_cs3_en == 1,
	        isQuad);


	if (result != DEFS_STATUS_OK)
	{
		serial_printf(KRED ">Erroneous fiu0 params" KNRM);
		return result;
	}


	// Take FIU3 fields
	fiu_dwr_cfg_set = uBootHeader->header.fiu3_dwr_cfg_set;
	fiu_drd_cfg_set = uBootHeader->header.fiu3_drd_cfg_set;
	fiu_clk_divider = uBootHeader->header.fiu3_clk_divider;

	//    Ignored if FIU_Clk_Divider is either 0 or 0FFh.
	if ( (fiu_clk_divider != 0) && (fiu_clk_divider != 0xFF))
	{
		// Config the FIU settings and clocks according to the header
		CLK_ConfigureFIUClock(FIU_MODULE_3, fiu_clk_divider);
		serial_printf(">set FIU3 divider to  %#lx \n", fiu_clk_divider );
	}

	//    Ignored if fiu_drd_cfg_set is either 0 or 0xFFFFFFFF.
	if ( (fiu_drd_cfg_set != 0) && (fiu_drd_cfg_set != 0xFFFFFFFF))
	{
		// Word contents copied to FIUx FIU_DRD_CFG register1
		REG_WRITE(FIU_DRD_CFG(FIU_MODULE_3), fiu_drd_cfg_set);
		serial_printf(">set FIU3 FIU_DRD_CFG to  %#lx \n", fiu_drd_cfg_set );
	}

	//    Ignored if fiu_dwr_cfg_set is either 0 or 0xFFFFFFFF.
	if ( (fiu_dwr_cfg_set != 0) && (fiu_dwr_cfg_set != 0xFFFFFFFF))
	{
		// Word contents copied to FIUx FIU_DWR_CFG register1
		REG_WRITE(FIU_DWR_CFG(FIU_MODULE_3), fiu_dwr_cfg_set);
		serial_printf(">set FIU3 FIU_DWR_CFG to  %#lx \n", fiu_dwr_cfg_set );
	}

	/*-------------------------------------------------------------------------------------------------*/
	/* Parse fiu0_drd_cfg_set. If it's quad configure FIU mux differently                              */
	/*-------------------------------------------------------------------------------------------------*/
	isQuad = (FIU_GetReadMode(FIU_MODULE_3) == FIU_FAST_READ_QUAD_IO) ? TRUE: FALSE;
	result = CHIP_MuxFIU ( FIU_MODULE_3,
		TRUE,
		uBootHeader->header.fiu3_cs1_en == 1,
		uBootHeader->header.fiu3_cs2_en == 1,
		uBootHeader->header.fiu3_cs3_en == 1,
		isQuad);

	if (result != DEFS_STATUS_OK)
	{
		serial_printf(KRED ">Erroneous fiu3 params" KNRM);
	}

 	return result;
 }



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_CheckImageCopyAndJump_l                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  uBootHeader -    Pointer to the image header to be checked                             */
/*                  image      -    image number (0/1/2)                                                   */
/*                  sigCheck   -    Determines whether to verify the image's signature                     */
/*                  bJump      -    Continue to uboot if it's valid.                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Checks the image in specified location and jumps to it if it's valid                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS      BOOTBLOCK_CheckImageCopyAndJump (BOOT_HEADER_T *uBootHeader, BOOLEAN sigCheck, UINT8 image, BOOLEAN bJump)
{
	DEFS_STATUS         result = DEFS_STATUS_FAIL;
	int                 ber = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* 11. Check Header:                                                                                   */
	/* In both Secure boot and Basic boot: read from the SPI flash and check the image header validity:    */
	/*-----------------------------------------------------------------------------------------------------*/
	result = BOOTBLOCK_CheckUbootHeader_l(uBootHeader, image);
	if (result != DEFS_STATUS_OK)
	{
		return result;
	}
	PCIMBX_SetImageState(image, IMAGE_HEADER_OK_COPY_IMAGE);


	/*---------------------------------------------------------*/
	/* 	FIU                                                */
	/*---------------------------------------------------------*/
	result = BOOTBLOCK_Init_FIU(uBootHeader, image);


	/*---------------------------------------------------------*/
	/* Copy the image and the header to destination address:   */
	/*---------------------------------------------------------*/
	if (uBootHeader->header.destAddr != 0)
	{
		UINT32 i;
		UINT32 print_errors;
		UINT32 repeat = 0;

		for (repeat = 0 ; repeat < 2 ; repeat++)
		{
		        UINT32* dest   = (UINT32 *)(uBootHeader->header.destAddr);
		        UINT32* source = (UINT32 *)uBootHeader;
		        UINT32  size  = (uBootHeader->header.codeSize + sizeof(BOOT_HEADER_T));

		        memcpy(dest, source, size);
		        serial_printf(KNRM "\n>copied uboot image to %#lx, size %#lx, from %#lx \n",
		             (UINT32)uBootHeader->header.destAddr, (UINT32)size, (UINT32)source);

		        if ((ber = memcmp(dest, source, size)) != 0)
		        {
		        	serial_printf(KRED "\n\n\n>ERROR! BB failed to copy uboot image to DDR.\nYour DRAM or FLASH device might be faulty.\nBER = %d.\n", ber);

				serial_printf("\n>DRAM                    | FLASH \n\n");

				print_errors = 0;
				for ( i = 0; i < size/4 ; i++)
				{
					if (dest[i] != source[i])
					{
						if(print_errors < 256)
						        serial_printf(">%#010lx : %#010lx | %#010lx : %#010lx \n",
				    				(UINT32)(dest+i), dest[i],
				    				(UINT32)(source+i), source[i]);
			    		print_errors++;
			    	}
			    }
			    serial_printf("\n>Number of erroneous dwords %d\nPerform mem test:\n", print_errors);


				ber = MC_MemStressTest(FALSE, FALSE);
				if (ber != 0)
					serial_printf(KNRM "\n>DRAM  memory test failed! \n\n");
				else
					serial_printf(KNRM "\n>DRAM  memory test passed! \n\n");

				if(repeat == 1){
					serial_printf("\n>   Halt\n\n");

					// retry the whole bootblock.
					SET_REG_FIELD(INTCR2, INTCR2_MC_INIT, 0);

					asm_jump_to_address(RAM2_BASE_ADDR + sizeof(BOOT_HEADER_T));

		        		while(1);
		        	}
		        }
		        else
		        {
				repeat = 2;
				break;
		        }
		}


	}

		result = DEFS_STATUS_OK;

	/*-----------------------------------------------------------------------------------------------------*/
	/* If the message is authenticated, the code is trusted and the following steps are performed:         */
	/*-----------------------------------------------------------------------------------------------------*/
	if(result == DEFS_STATUS_OK)
	{
		PCIMBX_SetImageState(image, IMAGE_OK);

		/*-------------------------------------------------------------------------------------------------*/
		/* i.(c) Jump to the destination address.                                                          */
		/*-------------------------------------------------------------------------------------------------*/
		if ( sigCheck == FALSE)
			PCIMBX_UpdateMailbox(ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET0 + image); // Assuming image states are sequential

		BOOTBLOCK_Init_Before_Image_Check_Vendor();

		if (bJump == TRUE)
		{
			BOOTBLOCK_JumpToUBOOT(uBootHeader, image);
		}
	}

	return result;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:	    BOOTBLOCK_JumpToUBOOT        							   */
/*													   */
/* Parameters:												   */
/*		    uBootHeader -    Pointer to the image header to be checked				   */
/*		    image      -    image number (0/1/2)						   */
/*													   */
/* Returns:	    none										   */
/* Side effects:											   */
/* Description: 											   */
/*		    Jump to uboot                                                       		   */
/*---------------------------------------------------------------------------------------------------------*/
void	 BOOTBLOCK_JumpToUBOOT (BOOT_HEADER_T *uBootHeader, UINT8 image)
{

	if (uBootHeader->header.destAddr == 0)
	{
		PCIMBX_SetImageState(image, IMAGE_OK_RUN_FROM_FLASH);

		PCIMBX_Print();

		serial_printf("\n>Jump to uboot at 0x%x (XIP)\n", (UINT32)uBootHeader + sizeof(BOOT_HEADER_T));

		/* b. Else, jump to the SPI flash at offset 200h. (code was not copied. Run it from flash)*/
		((volatile void(*)(void))(++uBootHeader))();
		while(1);
	}
	else
	{
		PCIMBX_SetImageState(image, IMAGE_OK);

		PCIMBX_Print();

		serial_printf("\n>Jump to uboot at 0x%x\n", uBootHeader->header.destAddr);
		/* clear the pipeline before jumping to image */
		// __asm{ ISB; };

		/* a. If destAddr != NULL,  jump to the code at offset 200h.*/
		((volatile void(*)(void))(uBootHeader->header.destAddr + sizeof(BOOT_HEADER_T)))();
		while(1);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_CheckBBHeader_l                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  bbHeader - Pointer to a BOOT_HEADER_T structure, contains image header                 */
/*                  image      - image number (0/1/2)                                                      */
/* Returns:         TRUE if the header PASSed the validity check, FALSE otherwise                          */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Performs image header parameters validity check on the specified image                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS       BOOTBLOCK_CheckBBHeader_l (BOOT_HEADER_T *bbHeader, UINT8 image)
{
	const UINT8         startTag[START_TAG_SIZE] = START_TAG_ARR_BOOTBLOCK;


	if ( ((UINT32)bbHeader < SPI0CS0_BASE_ADDR) || ((UINT32)bbHeader > (SPI0CS1_BASE_ADDR + FLASH_MEMORY_SIZE(1))))
	{
		PCIMBX_SetImageState(image, IMAGE_WRONG_START_TAG);
		serial_printf("\n>BB : illegal BB address %#lx\n\n", (UINT32)bbHeader );
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
	}

	/* — startTag (64-bit) = AA55_0750h, ‘BOOT’. */
	if (memcmp(bbHeader->header.startTag, startTag, START_TAG_SIZE) != 0)
	{
		PCIMBX_SetImageState(image, IMAGE_WRONG_START_TAG);
		serial_printf(KRED "\n>bb%d : found no start tag at %#lx  !\n\n", image, (UINT32)bbHeader );
		serial_printf("\n>%#lx  : %#lx %#lx %#lx %#lx %#lx %#lx %#lx %#lx \n" KNRM, (UINT32)bbHeader,
					bbHeader->bytes[0], bbHeader->bytes[1],
					bbHeader->bytes[2], bbHeader->bytes[3],
					bbHeader->bytes[4], bbHeader->bytes[5],
					bbHeader->bytes[6], bbHeader->bytes[7]);

		return DEFS_STATUS_INVALID_DATA_FIELD;
	}

	/* When destAddr != NULL, the following restrictions apply: */
	if (bbHeader->header.destAddr != 0)
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* — destAddr must be 32-bit aligned.                                                              */
		/*-------------------------------------------------------------------------------------------------*/
		if ( (bbHeader->header.destAddr & 0x00000003) != 0)
		{
			PCIMBX_SetImageState(image, IMAGE_DEST_ADDRESS_UNALIGNED);
			serial_printf("\n>uboot : image is unaligned! dest address %#lx\n", bbHeader->header.destAddr);
			return DEFS_STATUS_INVALID_DATA_FIELD;
		}
	}

	if (bbHeader->header.codeSize > MAX_CODE_SIZE)
	{
		serial_printf("\n>uboot : image too big. size %#lx\n", bbHeader->header.codeSize);
		PCIMBX_SetImageState(image, IMAGE_MEMORY_OVERLAP);
		return DEFS_STATUS_INVALID_DATA_SIZE;
	}

	return DEFS_STATUS_OK;
}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_CheckUbootHeader_l                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  uBootHeader - Pointer to a BOOT_HEADER_T structure, contains image header              */
/*                  image      - image number (0/1/2)                                                      */
/* Returns:         TRUE if the header PASSed the validity check, FALSE otherwise                          */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Performs image header parameters validity check on the specified image                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS       BOOTBLOCK_CheckUbootHeader_l (BOOT_HEADER_T *uBootHeader, UINT8 image)
{
	const UINT8         startTag[START_TAG_SIZE] = START_TAG_ARR_UBOOT;

	if ( ((UINT32)uBootHeader < SPI0CS0_BASE_ADDR) || ((UINT32)uBootHeader > (SPI0CS1_BASE_ADDR + FLASH_MEMORY_SIZE(1))))
	{
		PCIMBX_SetImageState(image, IMAGE_WRONG_START_TAG);
		serial_printf("\n>uboot : illegal uboot address %#lx\n\n", (UINT32)uBootHeader );
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
	}



	/* — startTag (64-bit) = AA55_0750h, ‘BOOT’. */
	if (memcmp(uBootHeader->header.startTag, startTag, START_TAG_SIZE) != 0)
	{
		PCIMBX_SetImageState(image, IMAGE_WRONG_START_TAG);
		serial_printf(KRED "\n>uboot : found no start tag at %#lx  !\n\n", (UINT32)uBootHeader );
		serial_printf("\n>%#lx  : %#lx %#lx %#lx %#lx %#lx %#lx %#lx %#lx \n" KNRM, (UINT32)uBootHeader,
					uBootHeader->bytes[0], uBootHeader->bytes[1],
					uBootHeader->bytes[2], uBootHeader->bytes[3],
					uBootHeader->bytes[4], uBootHeader->bytes[5],
					uBootHeader->bytes[6], uBootHeader->bytes[7]);

		return DEFS_STATUS_INVALID_DATA_FIELD;
	}

	/* When destAddr != NULL, the following restrictions apply: */
	if (uBootHeader->header.destAddr != 0)
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* — destAddr must be 32-bit aligned.                                                              */
		/*-------------------------------------------------------------------------------------------------*/
		if ( (uBootHeader->header.destAddr & 0x00000003) != 0)
		{
			PCIMBX_SetImageState(image, IMAGE_DEST_ADDRESS_UNALIGNED);
			serial_printf("\n>uboot : image is unaligned! dest address %#lx\n", uBootHeader->header.destAddr);
			return DEFS_STATUS_INVALID_DATA_FIELD;
		}
	}


	return DEFS_STATUS_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_FindSpiFlashImage                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sigCheck -  Determines whether to verify the image's signature                         */
/* Returns:         status                                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Goes over all possible boot image locations in SPI flash,                              */
/*                  checked whether they are valid/secure and jumps to the first valid image               */
/*  The images are checked in the following order untill                                                   */
/*  the first success:                                                                                     */
/*                                                                                                         */
/*      SPI:                                                                                               */
/*      1.	SPI chip select 0 (nSPICS0) SPI flash, offset 0.                                               */
/*      2.	SPI chip select 0 (nSPICS0) SPI flash, offset 256 KB.                                          */
/*      3.	SPI chip select 1 (nSPICS1) SPI flash, offset 0.                                               */
/*                                                                                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS        BOOTBLOCK_FindSpiFlashImage (BOOLEAN sigCheck)
{
	BOOT_HEADER_T     *BootBlockHeaderPtr;
	BOOT_HEADER_T     *uBootHeaderPtr;
	UINT32            image = 0;

	DEFS_STATUS        result = DEFS_STATUS_OK;


	/*--------------------------------------------------------------------------------------------------*/
	/* Get current loaded image number                                                                  */
	/*--------------------------------------------------------------------------------------------------*/
	BOOTBLOCK_GetLoadedBootBlockHeader(&BootBlockHeaderPtr, &image);

	PCIMBX_UpdateMailbox(ST_ROM_CHECK_IMAGE_SPI0_CS0_OFFSET0 + image);  /* image numbers are sequential */

	/*-------------------------------------------------------------------------------------------------*/
	/* The u-boot image starts after the BootBlock image aligned to 0x1000                             */
	/* The ROM code already validated the BootBlock header itself is valid                             */
	/*-------------------------------------------------------------------------------------------------*/
	uBootHeaderPtr = (BOOT_HEADER_T *)(0xFFFFF000 &
	  ((UINT32)BootBlockHeaderPtr + sizeof(BOOT_HEADER_T) + BootBlockHeaderPtr->header.codeSize + 0xFFF));

	serial_printf("\n\nbootblock at %#lx, uboot at %#lx, image num %d\n",
					  BootBlockHeaderPtr,  uBootHeaderPtr, image);

	/*-------------------------------------------------------------------------------------------------*/
	/* Check current image.  if ths image is valid, do not come back                                   */
	/*-------------------------------------------------------------------------------------------------*/
	result = BOOTBLOCK_CheckImageCopyAndJump(uBootHeaderPtr, sigCheck, image, TRUE);

	/*-----------------------------------------------------------------------------------------------------*/
	/* If we have found a good image, ROM would not have reached this point                                */
	/*-----------------------------------------------------------------------------------------------------*/
	return result;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_ReadHeaderParameter                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        UINT32 status, retVal: requested field value                                            */
/* Description:    Read a parameter from bootblock header ( of the currently running bootblock )           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS       BOOTBLOCK_ReadHeaderParameter (HEADER_FIELD_T field, UINT8 * retVal, BOOT_HEADER_T *bootBlockHeader)
{
	UINT32            image = 0;


	/*--------------------------------------------------------------------------------------------------*/
	/* Get current loaded image number (if not provided by the caller function)                         */
	/*--------------------------------------------------------------------------------------------------*/
	if (bootBlockHeader == NULL)
	{
		BOOTBLOCK_GetLoadedBootBlockHeader(&bootBlockHeader, &image);
	}

	switch (field)
	{
		case HEADER_FIELD_STARTTAG:
		{
		    memcpy(retVal, &bootBlockHeader->header.startTag, sizeof (bootBlockHeader->header.startTag));
		    break;
		}

		case HEADER_FIELD_SIGNATURE:
		{
		    memcpy(retVal, &bootBlockHeader->header.signature, sizeof (bootBlockHeader->header.signature));
		    break;
		}

		case HEADER_FIELD_FIU0_DRD_CFG_SET:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu0_drd_cfg_set, sizeof (bootBlockHeader->header.fiu0_drd_cfg_set));
		    break;
		}

		case HEADER_FIELD_FIU0_CLK_DIVIDER:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu0_clk_divider   , sizeof (bootBlockHeader->header.fiu0_clk_divider    ));
		    break;
		}

		case HEADER_FIELD_FIU0_CS1_EN:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu0_cs1_en   , sizeof (bootBlockHeader->header.fiu0_cs1_en    ));
		    break;
		}

		case HEADER_FIELD_FIU0_CS2_EN:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu0_cs2_en   , sizeof (bootBlockHeader->header.fiu0_cs2_en    ));
		    break;
		}

		case HEADER_FIELD_FIU0_CS3_EN:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu0_cs3_en   , sizeof (bootBlockHeader->header.fiu0_cs3_en    ));
		    break;
		}

		case HEADER_FIELD_FIU3_DRD_CFG_SET:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu3_drd_cfg_set   , sizeof (bootBlockHeader->header.fiu3_drd_cfg_set    ));
		    break;
		}

		case HEADER_FIELD_FIU3_DWR_CFG_SET:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu3_dwr_cfg_set   , sizeof (bootBlockHeader->header.fiu3_dwr_cfg_set    ));
		    break;
		}

		case HEADER_FIELD_FIU3_CLK_DIVIDER:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu3_clk_divider   , sizeof (bootBlockHeader->header.fiu3_clk_divider    ));
		    break;
		}

		case HEADER_FIELD_FIU3_CS1_EN:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu3_cs1_en   , sizeof (bootBlockHeader->header.fiu3_cs1_en    ));
		    break;
		}

		case HEADER_FIELD_FIU3_CS2_EN:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu3_cs2_en   , sizeof (bootBlockHeader->header.fiu3_cs2_en    ));
		    break;
		}

		case HEADER_FIELD_FIU3_CS3_EN:
		{
		    memcpy(retVal, &bootBlockHeader->header.fiu3_cs3_en   , sizeof (bootBlockHeader->header.fiu3_cs3_en    ));
		    break;
		}


		case HEADER_FIELD_BOARD_TYPE:
		{
		    memcpy(retVal, &bootBlockHeader->header.board_type   , sizeof (bootBlockHeader->header.board_type    ));
		    break;
		}

		case HEADER_FIELD_VENDOR:
		{
		    memcpy(retVal, &bootBlockHeader->header.vendor   , sizeof (bootBlockHeader->header.vendor    ));
		    break;
		}

		case HEADER_FIELD_HOST_IF:
		{
		    memcpy(retVal, &bootBlockHeader->header.host_if   , sizeof (bootBlockHeader->header.host_if    ));
		    break;
		}

		case HEADER_FIELD_MC_FREQ:
		{
			retVal[0] = LSB(bootBlockHeader->header.mc_freq);
			retVal[1] = MSB(bootBlockHeader->header.mc_freq);
			break;
		}

		case HEADER_FIELD_CPU_FREQ:
		{
			retVal[0] = LSB(bootBlockHeader->header.cpu_freq);
			retVal[1] = MSB(bootBlockHeader->header.cpu_freq);
			break;
		}

		case HEADER_FIELD_MC_CONFIG:
		{
			retVal[0] = bootBlockHeader->header.mc_config;
			break;
		}

		case HEADER_FIELD_MC_INIT:
		{
			memcpy(retVal, &bootBlockHeader->header.DQS_input   , sizeof (MC_INIT_VALUES));
			break;
		}

		case HEADER_FIELD_DESTADDR:
		{
		    memcpy(retVal, &bootBlockHeader->header.destAddr   , sizeof (bootBlockHeader->header.destAddr    ));
		    break;
		}

		case HEADER_FIELD_CODESIZE:
		{
		    memcpy(retVal, &bootBlockHeader->header.codeSize   , sizeof (bootBlockHeader->header.codeSize    ));
		    break;
		}

		case HEADER_FIELD_VERSION:
		{
		    memcpy(retVal, &bootBlockHeader->header.version   , sizeof (bootBlockHeader->header.version    ));
		    break;
		}
		default:
		    return DEFS_STATUS_FAIL;
	}


	return DEFS_STATUS_OK;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_GetBoardType                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        get board type: SVB, DRB, EB, .. make sure board vendor name matches board              */
/*---------------------------------------------------------------------------------------------------------*/
BOARD_T       BOOTBLOCK_GetBoardType (void)
{
	UINT8 val[4];

	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_BOARD_TYPE, val, NULL);

	return (BOARD_T)*val;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Get_MC_config                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        get MC config sesttings: bit 0 :                                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT8       BOOTBLOCK_Get_MC_config (void)
{
	UINT8 val;

	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_MC_CONFIG, &val, NULL);

	return val;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Get_MC_Init_val                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  mc_init_s -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Read MC init parameters from header                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void           BOOTBLOCK_Get_MC_Init_val (MC_INIT_VALUES *mc_init_s)
{
	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_MC_INIT, (UINT8*)mc_init_s, NULL);
	return;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Get_host_if                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        get host if: espi\LPC\none from header                                                  */
/*---------------------------------------------------------------------------------------------------------*/
HOST_IF_T       BOOTBLOCK_Get_host_if (void)
{
	UINT8 val;

	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_HOST_IF, &val, NULL);

	return (HOST_IF_T)val;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_GetVendorType                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        get board vendor type: customer, Nuvoton                                                */
/*---------------------------------------------------------------------------------------------------------*/
BOARD_VENDOR_T       BOOTBLOCK_GetVendorType (void)
{
	UINT8 val[4];

	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_VENDOR, val, NULL);

	switch (val[0])
	{
		case VENDOR_NUVOTON:
			return VENDOR_NUVOTON;





		default:
			return VENDOR_UNKNOWN;
	 }

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Get_MC_freq                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        get MC freq (in MHZ) from header. If value is 0 or 0xFFFF return 800000000 (800MHz)     */
/*                 example: if uesr wants 800MHz, write 0x320 on the header at offset 0x128                */
/*---------------------------------------------------------------------------------------------------------*/
UINT32   BOOTBLOCK_Get_MC_freq (void)
{
	UINT8 val[4] = {0,0,0,0};
	UINT32 ret_val;
	UINT16 val_header = 0;
	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_MC_FREQ, val, NULL);

	val[2] = val[3] = 0;
	val_header = MAKE16(val[0],val[1]);

	// backward compatible, default is 800MHz.
	if ((val_header == 0) || (val_header == 0xFFFF))
		ret_val = 800 * _1MHz_;   // In Hz
	else
		ret_val =  val_header * _1MHz_;

	return ret_val;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Get_CPU_freq                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        get CPU freq (in MHZ) from header. If value is 0 or 0xFFFF return 800000000 (800MHz)    */
/*                 example: if uesr wants 800MHz, write 0x320 on the header at offset 0x12A                */
/*---------------------------------------------------------------------------------------------------------*/
UINT32   BOOTBLOCK_Get_CPU_freq (void)
{
	UINT8 val[4] = {0,0,0,0};
	UINT32 ret_val;
	UINT16 val_header = 0;

	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_CPU_FREQ, val, NULL);
	val[2] = val[3] = 0;

	val_header = MAKE16(val[0],val[1]);

	// backward compatible, default is 800MHz.
	if ((val_header == 0) || (val_header == 0xFFFF))
		ret_val = 800 * _1MHz_;   // In Hz
	else
		ret_val =  val_header * _1MHz_;

	return ret_val;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_GetLoadedBootBlockHeader                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:        get currently loadded bootblock header address                                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS BOOTBLOCK_GetLoadedBootBlockHeader (BOOT_HEADER_T** bootBlockHeader, UINT32* image)
{
	BOOLEAN            bChosenImage;
	BOOLEAN            bUseSecondLocationForSecondImage;
	UINT32             swapAddress = 0;
	UINT32             flashExamAddrArr[FLASH_BOOT_ALTERNATIVES];

	/* init the location array */
	flashExamAddrArr[0] = FLASH_EXAM_ADDR_1;
	flashExamAddrArr[1] = FLASH_EXAM_ADDR_2;

	/* check if location array needs update according to WDC and oAltImgLoc */
	bUseSecondLocationForSecondImage = FUSE_Fustrap_Get(FUSE_FUSTRAP_oAltImgLoc);
	bChosenImage                     = READ_REG_FIELD(INTCR2, INTCR2_rChosenImage);


	/* take second image from CS1 */
	if (bUseSecondLocationForSecondImage)
	{
		flashExamAddrArr[1] = FLASH_EXAM_ADDR_3;
	}

	/* WDC (whatchdog counter) > 1 ? start from second image */
	if (bChosenImage)
	{
		swapAddress         = flashExamAddrArr[0] ;
		flashExamAddrArr[0] = flashExamAddrArr[1];
		flashExamAddrArr[1] = swapAddress ;
	}

	/*-------------------------------------------------------------------------------------------------*/
	/* Update the cureently tested image location. This is done for debug and visibility of ROM code   */
	/*-------------------------------------------------------------------------------------------------*/
	if ( flashExamAddrArr[0] == FLASH_EXAM_ADDR_1)
		*image = 0;

	if ( flashExamAddrArr[0] == FLASH_EXAM_ADDR_2)
		*image = 1;

	if ( flashExamAddrArr[0] == FLASH_EXAM_ADDR_3)
		*image = 2;

	*bootBlockHeader = (BOOT_HEADER_T*)flashExamAddrArr[0] ;

	return DEFS_STATUS_OK;
}



#undef BOOT_C

