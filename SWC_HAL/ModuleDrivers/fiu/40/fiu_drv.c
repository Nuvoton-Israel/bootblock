/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   fiu_drv.c                                                                */
/*            This file contains FIU driver implementation                    */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _FIU_DRV_C
#define _FIU_DRV_C

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_DRV()

/*---------------------------------------------------------------------------------------------------------*/
/* Include FIU driver definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#include "fiu_drv.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Include FIU registers definition                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#include "fiu_regs.h"

#include <string.h>

/*---------------------------------------------------------------------------------------------------------*/
/* Local Defines                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Verbose prints                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#if 0 //  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && (defined(NPCM750) || defined(NPCM850)))
#include "../../../../Src/Apps/serial_printf/serial_printf.h"
	#ifndef __LINUX_KERNEL_ONLY__
	#define FIU_MSG_DEBUG(fmt,args...)   serial_printf(fmt ,##args)
	#else
	#define FIU_MSG_DEBUG(fmt,args...)   printk(fmt ,##args)
	#endif

#else
#define           FIU_MSG_DEBUG(x,...)         (void)0
#define           FIU_MSG_DEBUG(x,...)         (void)0
#endif

/*--------------------------------------------------*/
void *memcpy(void *dest, const void *src, UINT32 n);


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                   LOCAL FUNCTIONS FORWARD DECLERATION                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_BusyWait_l (FIU_MODULE_T fiu_module, UINT32 timeout);

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_Config                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module       - fiu module number. Value is ignored if only one FIU module on chip  */
/*                  flash_size       - The flash size (If there are few flash devices -                    */
/*                                     The flash size of the largest flash device).                        */
/*                  read_mode        - Read mode select (Normal/Fast-Read).                                */
/*                  read_burst_size  - Flash read burst size (1/16 bytes).                                 */
/*                  write_burst_size - Flash write burst size (1/4 bytes).                                 */
/*                  iad_trap         - IAD trap generation enable.                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the FIU module.                                                */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_Config (   FIU_MODULE_T    fiu_module,
					FIU_DEV_SIZE_T  flash_size,
					FIU_READ_MODE_T read_mode,
					FIU_R_BURST_T   read_burst_size,
					FIU_W_BURST_T   write_burst_size,
					FIU_IAD_TRAP_T  iad_trap)
{

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure Flash Size                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_ConfigFlashSize( fiu_module, flash_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure read mode                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_ConfigReadMode( fiu_module, read_mode);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure burst sizes                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_ConfigBurstSize( fiu_module, read_burst_size, write_burst_size);

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigCommand                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module       - fiu module number. Value is ignored if only one FIU module on chip  */
/*                  readCommandByte - byte to write on commands cycle                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the FIU module command byte                                    */
/*                  Notice that 0 command is not on the spec If user wish to avvoid configuration - set it */
/*                  to zero !                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_ConfigCommand (   FIU_MODULE_T    fiu_module,  UINT8  readCommandByte, UINT8  writeCommandByte)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* read command byte. Notice that 0 command is not on the spec If user wish to avvoid configuration - set it to zero ! */
	/*-----------------------------------------------------------------------------------------------------*/
	if( readCommandByte != 0)
	{
		SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_RDCMD, readCommandByte);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* write command byte                                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( writeCommandByte != 0)
	{
		SET_REG_FIELD( FIU_DWR_CFG(fiu_module),  FIU_DWR_CFG_WRCMD, writeCommandByte);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigReadMode                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module - fiu module number. Value is ignored if only one FIU module on chip        */
/*                  read_mode - FIU read mode                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the FIU read mode.                                             */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_ConfigReadMode (   FIU_MODULE_T fiu_module,  FIU_READ_MODE_T read_mode)
{

	/*-----------------------------------------------------------------------------------------------------*/
	/* Change the access type. Warning: Once you go QUAD, you can't go back                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (( read_mode == FIU_FAST_READ) || ( read_mode == FIU_NORMAL_READ))
	{
		SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_ACCTYPE, 0);
	}
	else if ( (read_mode == FIU_FAST_READ_DUAL_IO)  ||  (read_mode == FIU_FAST_READ_DUAL_OUTPUT) )
	{
		SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_ACCTYPE, 1);
	}
#ifdef FIU_CAPABILITY_QUAD_READ
	else if (read_mode == FIU_FAST_READ_QUAD_IO)
	{
		SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_ACCTYPE, 2);
	}
#endif
#ifdef FIU_CAPABILITY_SPI_X
	else if (read_mode == FIU_FAST_READ_SPI_X)
	{
		SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_ACCTYPE, 3);
	}
#endif
	else
	{
		ASSERT(FALSE);
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigReadMode                                                                     */
/*                                                                                                         */
/* Parameters:      fiu_module - fiu module number. Value is ignored if only one FIU module on chip        */
/* Returns:         read_mode - FIU read mode                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the FIU read mode.                                                */
/*---------------------------------------------------------------------------------------------------------*/
FIU_READ_MODE_T FIU_GetReadMode (FIU_MODULE_T fiu_module)
{

	/*-----------------------------------------------------------------------------------------------------*/
	/* read FIU_DRD_CFG_ACCTYPE + FIU_DRD_CFG_DBW to get the mode                                          */
	/*-----------------------------------------------------------------------------------------------------*/

	switch (READ_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_ACCTYPE))
	{
		case 0:
			if ( (READ_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_DBW)) == 1)
				return FIU_FAST_READ;
			else
				return FIU_NORMAL_READ;
		case 1:
			if ( (READ_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_DBW)) == 1)
				return FIU_FAST_READ_DUAL_IO;
			else
				return FIU_FAST_READ_DUAL_OUTPUT;
		case 2:
			return FIU_FAST_READ_QUAD_IO;
		case 3:
			return FIU_FAST_READ_SPI_X;
		default:
			ASSERT(FALSE);
			break;
	 }

	 return FIU_NORMAL_READ;


}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigFlashSize                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module- fiu module number. Value is ignored if only one FIU module on chip         */
/*                  flash_size - Flash size to configure                                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures FIU flash size                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_ConfigFlashSize   (FIU_MODULE_T      fiu_module,
							FIU_DEV_SIZE_T flash_size)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure flash address                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( flash_size <= FIU_DEV_SIZE_16MB )
	{
		SET_REG_FIELD( FIU_DRD_CFG(fiu_module) ,  FIU_DRD_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_24BIT) ;
		SET_REG_FIELD( FIU_DWR_CFG(fiu_module) ,  FIU_DWR_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_24BIT) ;
	}
	else
	{
		SET_REG_FIELD( FIU_DRD_CFG(fiu_module) ,  FIU_DRD_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_32BIT) ;
		SET_REG_FIELD( FIU_DWR_CFG(fiu_module) ,  FIU_DWR_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_32BIT) ;
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigBurstSize                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module         - fiu module number. Value is ignored if only one FIU module on chip*/
/*                  read_burst_size     - FIU read burst size                                              */
/*                  write_burst_size    - FIU write burst size                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures read/write burst size                                          */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_ConfigBurstSize (   FIU_MODULE_T  fiu_module,
							 FIU_R_BURST_T read_burst_size,
							 FIU_W_BURST_T write_burst_size)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure write burst size                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD( FIU_DWR_CFG(fiu_module),  FIU_DWR_CFG_W_BURST, write_burst_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure read burst size                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_R_BURST, read_burst_size);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigDummyCyclesCount                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module       - fiu module number.                                                  */
/*                  writeDummyCycles - Number of Dummy cycles for write                                    */
/*                  readDummyCycles  - Number of Dummy cycles for read                                     */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config the number of dummy cycles                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_ConfigDummyCyclesCount (FIU_MODULE_T fiu_module, FIU_DUMMY_COUNT_T readDummyCycles)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Config read dummy cycles                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	if( readDummyCycles != FIU_DUMMY_SKIP_CONFIG)
	{
		if ( (readDummyCycles == FIU_DUMMY_COUNT_0) || (readDummyCycles == FIU_DUMMY_COUNT_0p5))
		{
			SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_DBW, 0);
		}
		else if (readDummyCycles == FIU_DUMMY_COUNT_1)
		{
			SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_DBW, 1);
		}
		else if (readDummyCycles == FIU_DUMMY_COUNT_2)
		{
			SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_DBW, 2);
		}
		else if (readDummyCycles == FIU_DUMMY_COUNT_3)
		{
			SET_REG_FIELD( FIU_DRD_CFG(fiu_module),  FIU_DRD_CFG_DBW, 3);
		}
		else
		{
			return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
		}
	}

	return DEFS_STATUS_OK;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_UMA_Disable                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disables the UMA operation                                                */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_UMA_Disable (FIU_MODULE_T fiu_module)
{
	SET_REG_FIELD(FIU_DWR_CFG(fiu_module), FIU_DWR_CFG_LCK, 0x01);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ManualWrite                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              fiu_module       - fiu module number. Value is ignored if only one FIU module on chip      */
/*              device           - Select the flash device to be accessed                                  */
/*              transaction_code - Specify the SPI UMA transaction code                                    */
/*              address          - Location on the flash, in the flash address space                       */
/*              data             - a pointer to a data buffer (buffer of bytes)                            */
/*              data_size        - data buffer size in bytes. Legal sizes are 0-256                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine "manual" page programming without using UMA.                              */
/*                  The data can be programmed upto the size of the whole page in a single SPI transaction */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_ManualWrite(FIU_MODULE_T            fiu_module,
							FIU_CS_T                device,
							UINT8                   transaction_code,
							UINT32                  address,
							UINT8 *                 data,
							UINT32                  data_size)

{
	UINT8   uma_cfg  = 0x0;
	UINT32  num_data_chunks;
	UINT32  remain_data;
	UINT32  idx = 0;


	FIU_MSG_DEBUG("FIU_ManualWrite fiu=%d cs=%d ta=0x%x addr=%#08lx, %d bytes \n", fiu_module, device, transaction_code, address, data_size);

	SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_SW_CS, 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Constructing var for FIU_UMA_CFG register status                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDATSIZ, 16);        /* Setting Write Data size */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculating relevant data                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	num_data_chunks  = data_size / 16;
	remain_data  = data_size % 16;

	/*-----------------------------------------------------------------------------------------------------*/
	/* First we activate Chip Select (CS) for the given flash device                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_DEV_NUM, (UINT32)device);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Writing the transaction code and the address to the bus                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_UMA_Write(fiu_module, device, transaction_code, address, TRUE, NULL, FIU_UMA_DATA_SIZE_0, 0);


	/*-----------------------------------------------------------------------------------------------------*/
	/* Starting the data writing loop in multiples of 16                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	for(idx = 0; idx < num_data_chunks; ++idx)
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* first byte command and follow 3 bytes address used as DATA                                      */
		/*-------------------------------------------------------------------------------------------------*/
		FIU_UMA_Write(fiu_module, device, data[0], (UINT32)NULL, FALSE, &data[1], FIU_UMA_DATA_SIZE_15, 0);

		data += 16;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Handling chunk remains                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (remain_data > 0)
	{
		FIU_UMA_Write(fiu_module, device, data[0], (UINT32)NULL, FALSE, &data[1], (FIU_UMA_DATA_SIZE_T)(remain_data - 1), 0);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Finally we de-activating the Chip select and returning to "automatic" CS control                    */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_SW_CS, 1);


	return DEFS_STATUS_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_UMA_read                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                fiu_module       - fiu module number. Value is ignored if only one FIU module on chip    */
/*                device           - Select the flash device to be accessed                                */
/*                transaction_code - Specify the SPI UMA transaction code                                  */
/*                address          - Location on the flash , in the flash address space                    */
/*                address_size     - if TRUE, 3 bytes address, to be placed in FIU_UMA_AB0-2               */
/*                                   else (FALSE), no address for this SPI UMA transaction                 */
/*                data             - a pointer to a data buffer to hold the read data.                     */
/*                data_size        - data buffer size                                                      */
/*                timeout          - command timeout                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Read up to 4 bytes from the flash. using the FIU User Mode Access (UMA)   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_UMA_Read (  FIU_MODULE_T            fiu_module,
							FIU_CS_T                device,
							UINT8                   transaction_code,
							UINT32                  address,
							BOOLEAN                 is_address_size,
							UINT8*                  data,
							FIU_UMA_DATA_SIZE_T     data_size,
							UINT32                  timeout)
{

	/* TODO:   address size should be a number, not a boolean. Here it's either 0, 3, 4  */
	/* TODO:   add mode parameter (single\dual\quad mode). */

	UINT32 uma_cfg = 0;

	DEFS_STATUS ret = DEFS_STATUS_OK;

	UINT32 data_reg[4];
	UINT32 address_size = 0;

	FIU_MSG_DEBUG("> UMA_read fiu%d cs%d tr=0x%02x addr=0x%x (%x),  %d bytes ",
	                 fiu_module, device, transaction_code, address, address_size, (int)data_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* set device number - DEV_NUM in FIU_UMA_CTS                                                          */
	/* legal device numbers are 0,1,2,3                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	switch(device)
	{
		case 0 :
		case 1 :
			SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_DEV_NUM, (UINT32)device);
			break;
		default:
			return DEFS_STATUS_FAIL;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* set transaction code in FIU_UMA_CODE                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CMD(fiu_module), FIU_UMA_CMD_CMD, transaction_code);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMDSIZ, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set address size bit                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (is_address_size)
	    address_size = 3;

	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADDSIZ, address_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA address registers                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_ADDR(fiu_module), address);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set data size and direction                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_RDATSIZ, data_size);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDATSIZ, 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set UMA CFG                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CFG(fiu_module), uma_cfg);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initiate the read                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_EXEC_DONE, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait for indication that transaction has terminated                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	ret = FIU_BusyWait_l(fiu_module, timeout);

	/*-----------------------------------------------------------------------------------------------------*/
	/* copy read data from FIU_UMA_DB0-3 regs to data buffer                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA data registers - FIU_UMA_DB0-3                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if (data_size >= FIU_UMA_DATA_SIZE_1)
	{
		data_reg[0] = REG_READ(FIU_UMA_DR0(fiu_module));
	}
	if (data_size >= FIU_UMA_DATA_SIZE_5)
	{
		data_reg[1] = REG_READ(FIU_UMA_DR1(fiu_module));
	}
	if (data_size >= FIU_UMA_DATA_SIZE_9)
	{
		data_reg[2] = REG_READ(FIU_UMA_DR2(fiu_module));
	}
	if (data_size >= FIU_UMA_DATA_SIZE_13)
	{
		data_reg[3] = REG_READ(FIU_UMA_DR3(fiu_module));
	}

	memcpy(data, data_reg, data_size);
	FIU_MSG_DEBUG(" %#08lx \n", data_reg[0] );


	return ret;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_UMA_Write                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              fiu_module       - fiu module number. Value is ignored if only one FIU module on chip      */
/*              device           - Select the flash device to be accessed                                  */
/*              transaction_code - Specify the SPI UMA transaction code                                    */
/*              address          - Location on the flash, in the flash address space                       */
/*              address_size     - if TRUE, 3 bytes address, to be placed in FIU_UMA_AB0-2                 */
/*                                 else (FALSE), no address for this SPI UMA transaction                   */
/*              data             - a pointer to a data buffer (buffer of bytes)                            */
/*              data_size        - data buffer size                                                        */
/*              timeout          - command timeout                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*           This routine write up to 4 bytes to the flash using the FIU User Mode Access (UMA)            */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_UMA_Write ( FIU_MODULE_T            fiu_module,
							FIU_CS_T                device,
							UINT8                   transaction_code,
							UINT32                  address,
							BOOLEAN                 is_address_size,
							const UINT8*            data,
							FIU_UMA_DATA_SIZE_T     data_size,
							UINT32                  timeout)
{

	/* TODO:   address size should be a number, not a boolean. Here it's either 0, 3, 4  */
	/* TODO:   add mode parameter (single\dual\quad mode). */

	UINT32 uma_cts;
	UINT32 uma_cfg;
	// UINT32 fiu_dwr_cfg;
	UINT32 chunk_data[FIU_MAX_UMA_DATA_SIZE/sizeof(UINT32)];
	DEFS_STATUS ret = DEFS_STATUS_OK;

	FIU_MSG_DEBUG("> UMA write fiu%d cs%d ta 0x%02x ", fiu_module, device, transaction_code);

	/*-----------------------------------------------------------------------------------------------------*/
	/* set device number - DEV_NUM in FIU_UMA_CTS                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_COND_CHECK(device < FIU_DEVICES_PER_MODULE, DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

	if(is_address_size)
		FIU_MSG_DEBUG(" addr %#08lx ", address);



	// fiu_dwr_cfg = REG_READ(FIU_DWR_CFG(fiu_module));
	uma_cfg = 0x0;
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMBPCK,  0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADBPCK,  0); // Support only single bit in programming. else do: READ_VAR_FIELD(fiu_dwr_cfg, FIU_DWR_CFG_ABPCK));
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDBPCK,  0); // Support only single bit in programming. else do:READ_VAR_FIELD(fiu_dwr_cfg, FIU_DWR_CFG_DBPCK));
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_DBPCK,   0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_RDBPCK,  0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMDSIZ,  1);


	// TALIP
	if (is_address_size == FALSE)
	{
		SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADDSIZ,0);
	}
	else
	{
		// Set address size bytes to 3 if FIU_DWR.CFG_ADDSIZ == 0 and to 4 otherwize
		SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADDSIZ,  3);
	}
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDATSIZ, data_size);     // Set data size and write direction
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_DBSIZ,   0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_RDATSIZ, 0);
	/*-----------------------------------------------------------------------------------------------------*/
	/* Set UMA configuration                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CFG(fiu_module), uma_cfg);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set transaction code (command byte source)                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CMD(fiu_module), transaction_code);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA address registers                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_ADDR(fiu_module), is_address_size? address: 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* copy write data to FIU_UMA_DW0-3 regs                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA data registers - FIU_UMA_DW0-3                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	memcpy(chunk_data, data, data_size);
	REG_WRITE(FIU_UMA_DW(fiu_module, 0), chunk_data[0]);
	REG_WRITE(FIU_UMA_DW(fiu_module, 1), chunk_data[1]);
	REG_WRITE(FIU_UMA_DW(fiu_module, 2), chunk_data[2]);
	REG_WRITE(FIU_UMA_DW(fiu_module, 3), chunk_data[3]);
#if 0
	for (i=0; i<data_size; i+=4)
	{
		UINT j;
		UINT8  write_data[4];

		*(UINT32*)(&write_data) = 0;

		for (j=0; j<4; j++)
		{
			if ((j+i)<data_size)
			{
				write_data[j] = data[j+i];
			}
		}
		REG_WRITE(FIU_UMA_DW(fiu_module, i/4), *(UINT32*)(&write_data));
	}
#endif

	uma_cts = REG_READ(FIU_UMA_CTS(fiu_module));
	SET_VAR_FIELD(uma_cts, FIU_UMA_CTS_RDYST, 1); // set 1 to clear
	SET_VAR_FIELD(uma_cts, FIU_UMA_CTS_EXEC_DONE, 1);
	SET_VAR_FIELD(uma_cts, FIU_UMA_CTS_DEV_NUM, device);

	FIU_MSG_DEBUG(" CTS %#08lx , cmd %#08lx , cfg %#08lx, addr %#08lx,  data %#08lx \n",
		uma_cts,
		REG_READ(FIU_UMA_CMD(fiu_module)),
		REG_READ(FIU_UMA_CFG(fiu_module)),
		REG_READ(FIU_UMA_ADDR(fiu_module)),
		REG_READ(FIU_UMA_DW(fiu_module, 0)));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initiate the write                                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CTS(fiu_module), uma_cts);


	if ( data_size > 0)
		FIU_MSG_DEBUG(" %#08lx %#08lx %#08lx %#08lx  \n", chunk_data[0], chunk_data[1], chunk_data[2], chunk_data[3]);
	else
		FIU_MSG_DEBUG("\n");

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait for indication that transaction has terminated                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	ret = FIU_BusyWait_l(fiu_module, timeout);

	return ret;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_PageWrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              fiu_module       - fiu module number. Value is ignored if only one FIU module on chip      */
/*              device           - Select the flash device to be accessed                                  */
/*              transaction_code - Specify the SPI UMA transaction code                                    */
/*              address          - Location on the flash, in the flash address space                       */
/*              address_size     - if TRUE, 3 bytes address, to be placed in FIU_UMA_AB0-2                 */
/*                                 else (FALSE), no address for this SPI UMA transaction                   */
/*              data             - a pointer to a data buffer (buffer of bytes)                            */
/*              data_size        - data buffer size in bytes                                               */
/*              timeout          - command timeout                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine executes page programming without using UMA.                              */
/*                  The data can be programmed upto the size of the whole page in a single SPI transaction */
/*                  The data address/size must fit page boundaries for page programming to succeed         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_PageWrite ( FIU_MODULE_T    fiu_module,
							FIU_CS_T        device,
							UINT8           transaction_code,
							UINT32          address,
							const UINT8*    data,
							UINT16          data_size,
							UINT32          timeout)
{
	/* TODO:   address size should be a number, not a boolean. Here it's either 0, 3, 4  */
	/* TODO:   add mode parameter (single\dual\quad mode). */


	//UINT   i;
	UINT32 uma_cts;
	UINT32 uma_cfg;
	UINT32 fiu_dwr_cfg;
	UINT16 chunk_size;
	DEFS_STATUS ret = DEFS_STATUS_OK;


	/*-----------------------------------------------------------------------------------------------------*/
	/* set device number - DEV_NUM in FIU_UMA_CTS                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_COND_CHECK (device < FLASH_NUM_OF_DEVICES, DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

	fiu_dwr_cfg = REG_READ(FIU_DWR_CFG(fiu_module));
	uma_cfg = 0x0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Send the Address                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/

	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMBPCK,  0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADBPCK,  READ_VAR_FIELD(fiu_dwr_cfg, FIU_DWR_CFG_ABPCK));
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDBPCK,  READ_VAR_FIELD(fiu_dwr_cfg, FIU_DWR_CFG_DBPCK));
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_DBPCK,   0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_RDBPCK,  0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMDSIZ,  1);
	// Set address size bytes to 3 if FIU_DWR.CFG_ADDSIZ == 0 and to 4 otherwize
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADDSIZ,  (READ_VAR_FIELD(fiu_dwr_cfg, FIU_DWR_CFG_ADDSIZ)==0)?3:4);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_DBSIZ,   0);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_RDATSIZ, 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set transaction code (command byte source)                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CMD(fiu_module), transaction_code);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA address registers                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_ADDR(fiu_module), address);


	uma_cts = REG_READ(FIU_UMA_CTS(fiu_module));
	SET_VAR_FIELD(uma_cts, FIU_UMA_CTS_RDYST, 1); // set 1 to clear
	SET_VAR_FIELD(uma_cts, FIU_UMA_CTS_EXEC_DONE, 1);
	SET_VAR_FIELD(uma_cts, FIU_UMA_CTS_DEV_NUM, device % 4);

#if 0
	/*-----------------------------------------------------------------------------------------------------*/
	/* Initiate the read                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CTS(fiu_module), uma_cts);

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait for indication that transaction has terminated                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	ret = FIU_BusyWait_l(fiu_module, timeout);
	if (ret != DEFS_STATUS_OK)
	{
		SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_SW_CS, 1); // cs returns to default
		return ret;
	}
#endif

	/*-----------------------------------------------------------------------------------------------------*/
	/* Send the Data                                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/

	while (data_size)
	{
		UINT32 chunk_data[FIU_MAX_UMA_DATA_SIZE/sizeof(UINT32)];

		/*-----------------------------------------------------------------------------------------------------*/
		/* In order to save the busy wait time we first do all calculation than we wait for previous           */
		/* transaction to end and only then update the registers                                               */
		/*-----------------------------------------------------------------------------------------------------*/
		chunk_size = MIN(FIU_MAX_UMA_DATA_SIZE, data_size);

		SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDATSIZ, chunk_size);     // Set data size and write direction

		memcpy(chunk_data, data, chunk_size);

		/*-----------------------------------------------------------------------------------------------------*/
		/* wait for indication that transaction has terminated                                                 */
		/*-----------------------------------------------------------------------------------------------------*/
		ret = FIU_BusyWait_l(fiu_module, timeout);
		if (ret != DEFS_STATUS_OK)
		{
			SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_SW_CS, 1); // cs returns to default
			return ret;
		}

		/*-----------------------------------------------------------------------------------------------------*/
		/* copy write data to FIU_UMA_DW0-3 regs                                                               */
		/*-----------------------------------------------------------------------------------------------------*/
		/*-----------------------------------------------------------------------------------------------------*/
		/* Set the UMA data registers - FIU_UMA_DW0-3                                                          */
		/*-----------------------------------------------------------------------------------------------------*/
		REG_WRITE(FIU_UMA_DW(fiu_module, 0), chunk_data[0]);
		REG_WRITE(FIU_UMA_DW(fiu_module, 1), chunk_data[1]);
		REG_WRITE(FIU_UMA_DW(fiu_module, 2), chunk_data[2]);
		REG_WRITE(FIU_UMA_DW(fiu_module, 3), chunk_data[3]);
#if 0
		for (i=0; i<chunk_size; i+=4)
		{
			UINT j;
			UINT8  write_data[4];

			*(UINT32*)(&write_data) = 0;

			for (j=0; j<4; j++)
			{
				if ((j+i)<chunk_size)
				{
					write_data[j] = data[j+i];
				}
			}
			REG_WRITE(FIU_UMA_DW(fiu_module, i/4), *(UINT32*)(&write_data));
		}
#endif
		/*-----------------------------------------------------------------------------------------------------*/
		/* Set UMA configuration                                                                               */
		/*-----------------------------------------------------------------------------------------------------*/
		REG_WRITE(FIU_UMA_CFG(fiu_module), uma_cfg);

		/*-----------------------------------------------------------------------------------------------------*/
		/* Initiate the write                                                                                  */
		/*-----------------------------------------------------------------------------------------------------*/
		REG_WRITE(FIU_UMA_CTS(fiu_module), uma_cts);

		// next transactions do not include command and address
		if (READ_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMDSIZ))
		{
			SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMDSIZ,  0);  // no command needed
			SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADDSIZ,  0);  // no address needed
		}

		data_size -= chunk_size;
		data += chunk_size;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait for indication that last transaction has terminated                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	ret = FIU_BusyWait_l(fiu_module, timeout);
	if (ret != DEFS_STATUS_OK)
	{
		SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_SW_CS, 1); // cs returns to default
		return ret;
	}

	SET_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_SW_CS, 1); // cs returns to default
	return ret;
}


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                     LOCAL FUNCTIONS IMPLEMENTATION                                      */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_BusyWait_l                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module  - fiu module number.                                                       */
/*                  timeout     - FIU operation timeout                                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is polling on FIU transaction completeness bit.                           */
/*                  If timeout equals zero - The routine can poll forever                                  */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_BusyWait_l (FIU_MODULE_T  fiu_module,
							UINT32        timeout)
{
	if (timeout == 0)
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* wait for indication that transaction has put on the bus                                         */
		/*-------------------------------------------------------------------------------------------------*/
		while (READ_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_EXEC_DONE) == FIU_TRANS_STATUS_IN_PROG){}
	}
	else
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* wait for indication that transaction has put on the bus                                         */
		/*-------------------------------------------------------------------------------------------------*/
		BUSY_WAIT_TIMEOUT(READ_REG_FIELD(FIU_UMA_CTS(fiu_module), FIU_UMA_CTS_EXEC_DONE) == FIU_TRANS_STATUS_IN_PROG,
						  FIU_USEC_TO_LOOP_COUNT(timeout));
	}


	return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_SetDirection                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fiu_module       - fiu module number. Value is ignored if only one FIU module on chip  */
/* Returns:         DEFS_STATUS                                                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config the read direction of FIU                                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FIU_SetDirection (FIU_MODULE_T fiu_module , FIU_DIRECTION_MODE_T mode )
{
	SET_REG_FIELD(FIU_CFG(fiu_module), FIU_CFG_INCRSING, mode);

	return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_PrintRegs (void)
{
	HAL_PRINT("/*--------------*/\n");
	HAL_PRINT("/*     FIU      */\n");
	HAL_PRINT("/*--------------*/\n\n");

	HAL_PRINT("FIU_CFG             = 0x%02X\n", REG_READ(FIU_CFG));

#if TODO
	HAL_PRINT("BURST_CFG           = 0x%02X\n", REG_READ(BURST_CFG));
	HAL_PRINT("RESP_CFG            = 0x%02X\n", REG_READ(RESP_CFG));
	HAL_PRINT("FIU_TEST            = 0x%02X\n", REG_READ(FIU_TEST));
	HAL_PRINT("SPI_FL_CFG          = 0x%02X\n", REG_READ(SPI_FL_CFG));
	HAL_PRINT("UMA_CODE            = 0x%02X\n", REG_READ(FIU_UMA_CODE));
	HAL_PRINT("UMA_AB0             = 0x%02X\n", REG_READ(FIU_UMA_AB0));
	HAL_PRINT("UMA_AB1             = 0x%02X\n", REG_READ(FIU_UMA_AB1));
	HAL_PRINT("UMA_AB2             = 0x%02X\n", REG_READ(FIU_UMA_AB2));
	HAL_PRINT("UMA_DB0             = 0x%02X\n", REG_READ(FIU_UMA_DB0));
	HAL_PRINT("UMA_DB1             = 0x%02X\n", REG_READ(FIU_UMA_DB1));
	HAL_PRINT("UMA_DB2             = 0x%02X\n", REG_READ(UMA_DB2));
	HAL_PRINT("UMA_DB3             = 0x%02X\n", REG_READ(UMA_DB3));
	HAL_PRINT("UMA_CTS             = 0x%02X\n", REG_READ(UMA_CTS));
	HAL_PRINT("UMA_ECTS            = 0x%02X\n", REG_READ(UMA_ECTS));
	HAL_PRINT("UMA_DB0_3           = 0x%08X\n", REG_READ(UMA_DB0_3));
#ifdef FIU_CAPABILITY_CRC
	HAL_PRINT("CRCCON              = 0x%02X\n", REG_READ(CRCCON));
	HAL_PRINT("CRCENT              = 0x%02X\n", REG_READ(CRCENT));
	HAL_PRINT("CRCRSLT             = 0x%08X\n", REG_READ(CRCRSLT));
#endif
#ifdef FIU_CAPABILITY_CONFIG_DUMMY_CYCLES
	HAL_PRINT("FIU_DMM_CYC         = 0x%02X\n", REG_READ(FIU_DMM_CYC));
#endif
#if defined FIU_CAPABILITY_SUPPORT_64MB_FLASH  || defined FIU_CAPABILITY_4B_ADDRESS || \
	defined FIU_CAPABILITY_CONFIG_DUMMY_CYCLES || defined FIU_CAPABILITY_SEC_CHIP_SELECT
	HAL_PRINT("FIU_EXT_CFG         = 0x%02X\n", REG_READ(FIU_EXT_CFG));
#endif
#ifdef FIU_CAPABILITY_4B_ADDRESS
	HAL_PRINT("UMA_AB0_3           = 0x%08X\n", REG_READ(UMA_AB0_3));
#endif
#if defined FIU_CAPABILITY_4B_ADDRESS || defined FIU_CAPABILITY_SEC_CHIP_SELECT
	HAL_PRINT("SPI0_DEV            = 0x%02X\n", REG_READ(SPI_DEV(0)));
	HAL_PRINT("SPI1_DEV            = 0x%02X\n", REG_READ(SPI_DEV(1)));
#endif

#endif

	HAL_PRINT("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_PrintVersion (void)
{
	HAL_PRINT("FIU         = %X\n", MODULE_VERSION(FIU_MODULE_TYPE));
}


#endif // _FIU_DRV_C

