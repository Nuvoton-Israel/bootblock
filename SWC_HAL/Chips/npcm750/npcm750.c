/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   npcm750.c                                                                */
/*            This file contains chip dependent functionality implementation  */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "npcm750_if.h"
#include "npcm750_regs.h"
// #include "../../ModuleDrivers/clk/3/clk_regs.h"



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/




/*---------------------------------------------------------------------------------------------------------*/
/* Register access macroes                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define POR_STATUS()         !(READ_REG_FIELD(INTCR2, INTCR2_PORST))             // Reset source was POR (powerOn) reset
#define COR_STATUS()         !(READ_REG_FIELD(INTCR2, INTCR2_CORST))             // Reset source was COR reset
#define MARK_CFG_DONE()      SET_REG_FIELD(INTCR2, INTCR2_CFGDone, 1)           // Chip configuration (MC, CLK, Flash, etc') is done
#define CFG_IS_DONE()        (READ_REG_FIELD(INTCR2, INTCR2_CFGDone))           // Chip configuration (MC, CLK, Flash, etc') is done
#define ENABLE_JTAG()        SET_REG_FIELD(FLOCKR1, FLOCKR1_JTAGDIS, 0)         // Rnables J-Tag
#define DISABLE_JTAG()       SET_REG_FIELD(FLOCKR1, FLOCKR1_JTAGDIS, 1)         // Disables J-Tag


#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_Uart                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  CoreSP -                                                                               */
/*                  redirection_mode -                                                                     */
/*                  sp1 -                                                                                  */
/*                  sp2 -                                                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects UART muxing                                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CHIP_Mux_Uart (UART_MUX_T redirection_mode, BOOLEAN CoreSP, BOOLEAN sp1, BOOLEAN sp2)
{
	/* 111 combination is reserved: */
	if (redirection_mode < 7)
	{
		SET_REG_FIELD(SPSWC, SPSWC_SPMOD, redirection_mode & 0x7); /* redirection mode number in enum == value at register */
	}
	else if (CoreSP == FALSE)
	{
		// if both redirection_mode >= 7 and CoreSP (which does not need a mode)
		// was not selected it is error
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
	}

	if (sp1)
	{
	SET_REG_FIELD(MFSEL1, MFSEL1_HSI1SEL, 1);
	SET_REG_FIELD(MFSEL4, MFSEL4_BSPASEL, 0); /* Select TXD1+RXD1 */
	}
	if (sp2)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_HSI2SEL, 1);
		SET_REG_FIELD(MFSEL4, MFSEL4_BSPASEL, 0); /* Select TXD2+RXD2 */
	}
	if (CoreSP)
	{
		if (STRP_get(STRP_11_BSPA) == 0)
		{
			SET_REG_FIELD(MFSEL4, MFSEL4_BSPASEL, 1);

			// Note: If this bit is set, MFSEL1 bit 9 and 11 must be set to 0.
			SET_REG_FIELD(MFSEL1, MFSEL1_BSPSEL, 0);
			SET_REG_FIELD(MFSEL1, MFSEL1_HSI2SEL, 0);
		}
		else
		{
			SET_REG_FIELD(MFSEL4, MFSEL4_BSPASEL, 0);
			SET_REG_FIELD(MFSEL1, MFSEL1_BSPSEL, 1);
		}
	}

	return DEFS_STATUS_OK;

}
#endif // UART_MODULE_TYPE


#if defined (FIU_MODULE_TYPE)

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Quad_Enable                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum  - SPI Flash device number                                                      */
/*                  FIU_cs - CS SPI to configure.                                                          */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine qonfigures the SPI to be QUAD mode enabled                                */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Quad_Enable(FIU_MODULE_T devNum, FIU_CS_T FIU_cs)
{
	UINT8 status2;
	FIU_UMA_Read(devNum,  FIU_cs, 0x35, 0, FALSE, &status2, FIU_UMA_DATA_SIZE_1, 0);
	if ((status2 & 0x2) == 0)
	{
		status2 |= 0x2;
		// write as non volatile register
		FIU_UMA_Write(devNum, FIU_cs, 0x50,  0, FALSE, NULL, FIU_UMA_DATA_SIZE_0, 0);
		FIU_UMA_Write(devNum, FIU_cs, 0x31, 0, FALSE, &status2, FIU_UMA_DATA_SIZE_1, 0);
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxFIU                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum  - SPI Flash device number                                                      */
/*                  csXEnable - enable CS.                                                                 */
/*                  quadMode -  is Quad mode                                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects FIU muxing . Notice it does not disable a CS!                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CHIP_MuxFIU (   FIU_MODULE_T      devNum,
				BOOLEAN           cs0Enable,
				BOOLEAN           cs1Enable,
				BOOLEAN           cs2Enable,
				BOOLEAN           cs3Enable,
				BOOLEAN           quadMode)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* CS2 and CS3 are multiplexed with spid2 and spid3. So when in quad mode, can't use CS2 and CS3       */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_COND_CHECK ((cs2Enable == FALSE) || (quadMode == FALSE), DEFS_STATUS_INVALID_PARAMETER);

	DEFS_STATUS_COND_CHECK ((cs3Enable == FALSE) || (quadMode == FALSE), DEFS_STATUS_INVALID_PARAMETER);

	switch (devNum)
	{
	case FIU_MODULE_0:
	{
		/* config CS */
		if ( cs0Enable == TRUE)
		{
		// no muxing to do.
		}

		/* config CS1 */
		if ( cs1Enable == TRUE)
		{
			SET_REG_FIELD(MFSEL1, MFSEL1_S0CS1SEL, 1);
		}

		/* Config CS2 */
		if ( cs2Enable == TRUE)
		{
			SET_REG_FIELD(MFSEL1, MFSEL1_S0CS2SEL, 1);
		}

		/* Config CS3 */
		if ( cs3Enable == TRUE)
		{
			SET_REG_FIELD(MFSEL1, MFSEL1_S0CS3SEL, 1);
		}


		/* select io bus width (1/2/4  <=> single/dual/quad ) */
		if (quadMode == TRUE)
		{
			SET_REG_FIELD (MFSEL1, MFSEL1_S0CS2SEL, 0);  /*  0: GPIO33/SPI0D2 ,1: nSPI0CS2  */
			SET_REG_FIELD (MFSEL1, MFSEL1_S0CS3SEL, 0);  /*  0: GPIO34/SPI0D3 ,1: nSPI0CS3  */
			SET_REG_FIELD (MFSEL4, MFSEL4_SP0QSEL , 1);  /*  enable quad                    */
			if (cs0Enable == TRUE)
			{
				SPI_Quad_Enable(FIU_MODULE_0,  FIU_CS_0);
			}
			if (cs1Enable == TRUE)
			{
				SPI_Quad_Enable(FIU_MODULE_0,  FIU_CS_1);
			}
		}
		break;
	}
	case FIU_MODULE_3:
	{
		/* Select SPI3 */
		SET_REG_FIELD(MFSEL4, MFSEL4_SP3SEL, 1);

		/* config CS */
		if ( cs0Enable == TRUE)
		{
		// nothing to do.
		}

		/* config CS1 */
		if ( cs1Enable == TRUE)
		{
			SET_REG_FIELD(MFSEL4, MFSEL4_S3CS1SEL, 1);
		}

		/* Config CS2 */
		if ( cs2Enable == TRUE)
		{
			SET_REG_FIELD(MFSEL4, MFSEL4_S3CS2SEL, 1);
		}

		/* Config CS3 */
		if ( cs3Enable == TRUE)
		{
			SET_REG_FIELD(MFSEL4, MFSEL4_S3CS3SEL, 1);
		}


		/* select io bus width (1/2/4  <=> single/dual/quad ) */
		if (quadMode == TRUE)
		{
			SET_REG_FIELD (MFSEL4, MFSEL4_S3CS2SEL, 0);  /*  0: GPIO33/SPI0D2 ,1: nSPI0CS2     */
			SET_REG_FIELD (MFSEL4, MFSEL4_S3CS3SEL, 0);  /*  0: GPIO34/SPI0D3 ,1: nSPI0CS3     */
			SET_REG_FIELD (MFSEL4, MFSEL4_SP3QSEL , 1);  /* enable quad */
		if (cs0Enable == TRUE)
		{
			SPI_Quad_Enable(FIU_MODULE_3,  FIU_CS_0);
		}
		if (cs1Enable == TRUE)
		{
			SPI_Quad_Enable(FIU_MODULE_3,  FIU_CS_1);
		}
		}
		break;
	}
	case FIU_MODULE_X:
	{
		/* config CS */
		if ( cs0Enable == TRUE)
		{
			// nothing to do.
		}

		/* config CS1 */
		if ( cs1Enable == TRUE)
		{
			SET_REG_FIELD(MFSEL4, MFSEL4_SXCS1SEL, 1);
		}

			SET_REG_FIELD(MFSEL4, MFSEL4_SPXSEL, 1);
		break;
	}

	default:
		ASSERT(0);
		break;
	}
	return DEFS_STATUS_OK;
}
#endif




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_RMII                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects RMII mux                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_RMII(UINT devNum)
{
	if(devNum == 0)
	{
		//SET_REG_FIELD(CLKEN3, CLKEN3_GDMA1, 1);
		SET_REG_FIELD(MFSEL3, MFSEL3_RMII1SEL, 1);
		SET_REG_FIELD(MFSEL1, MFSEL1_R1MDSEL, 1);
		SET_REG_FIELD(MFSEL1, MFSEL1_R1ERRSEL, 1);
		SET_REG_FIELD(INTCR,  INTCR_R1EN, 1);

		SET_REG_FIELD(MFSEL4, MFSEL4_RG1MSEL, 1);
		SET_REG_FIELD(MFSEL4, MFSEL4_RG1SEL, 1);
	}
	else
	{
		//SET_REG_FIELD(CLKEN3, CLKEN3_GDMA2, 1);
		SET_REG_FIELD(MFSEL1, MFSEL1_RMII2SEL, 1);
		SET_REG_FIELD(MFSEL1, MFSEL1_R2MDSEL, 1);
		SET_REG_FIELD(MFSEL1, MFSEL1_R2ERRSEL, 1);

		SET_REG_FIELD(MFSEL4, MFSEL4_RG2MSEL, 1);
		SET_REG_FIELD(MFSEL4, MFSEL4_RG2SEL, 1);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SetDacSense                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Makes the trigger for sense circuits to be GFX core output signal (miscOUT2)           */
/*                  instead of VSYNC falling edge                                                          */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_SetDacSense(void)
{
	SET_REG_FIELD(INTCR, INTCR_DAC_SNS, 1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_GMII                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects GMII mux                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_GMII(UINT devNum)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting the GMII Muxing                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(MFSEL3, MFSEL3_RMII1SEL, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting MII TX clock mux                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	//SET_REG_FIELD(MFSEL3, MFSEL3_MTCKSEL, 1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_SIOXH                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  enable -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SIOXH mux                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_SIOXH(BOOLEAN enable)
{
	if (enable)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_IOXHSEL, 1);
	}
	else
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_IOXHSEL, 0);
	}
}


#if defined (SPI_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxSPI                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - PSPI device number                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs muxing for Peripheral SPI device                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_MuxSPI(SPI_MODULE_T devNum)
{
	switch (devNum)
	{
		case SPI_MODULE_0:     SET_REG_FIELD(MFSEL3, MFSEL3_PSPI1SEL, 2);          break;
		case SPI_MODULE_1:     SET_REG_FIELD(MFSEL3, MFSEL3_PSPI2SEL, 1);          break;
		default :              ASSERT(0);                                          break;
	}
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_GSPI                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs muxing for Graphics Core SPI Signals                             */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_GSPI(BOOLEAN bOn)
{
	if ( bOn == TRUE)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_GSPISEL, 1);  // Config to Graphics Core SPI
	}
	else
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_GSPISEL, 0);  // Config to GPIO
	}
}



#ifdef MC_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_PowerOn_GetMemorySize                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns configured DDR memory size                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CHIP_PowerOn_GetMemorySize(void)
{
	UINT32 pwronRAMsize = STRP_MEMSIZ_512MB_MODE;
#if 0
// TODO: this will come from flash !!!!

	STRP_MemSize_Get();
#endif

	switch(pwronRAMsize)
	{
	case STRP_MEMSIZ_512MB_MODE:      // 512 MiB
		return 0x20000000;
	case STRP_MEMSIZ_1GB_MODE:        // 1 GiB.
		return 0x40000000;
	case STRP_MEMSIZ_2GB_MODE:        // 2 GiB.
		return 0x80000000;
	case STRP_MEMSIZ_RESERVED:        // 512 MiB. // use minimum
		return 0x20000000;
	}

	return 0;
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_GPIO                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs mux for given GPIO                                               */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_GPIO(UINT gpio_num)
{
	//TODO:GPIO_MUX_REGID_T    mux_reg1, mux_reg2;
	//TODO:GPIO_MUX_FIELD_T    mux1, mux2;


	/*-----------------------------------------------------------------------------------------------------*/
	/* Getting Mux information from GPIO module                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	//TODO: GPIO_GetMuxInfo(gpio_num, &mux_reg1, &mux1, &mux_reg2, &mux2);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Bypass for using DEFS.H macros                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	//TODO:#define GCR_GPIO_MUX_FIELD1  mux1.position, mux1.size
	//TODO:#define GCR_GPIO_MUX_FIELD2  mux2.position, mux2.size


	/*-----------------------------------------------------------------------------------------------------*/
	/* Muxing                                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	//TODO:if (mux1.size != 0)
	//TODO:{
	//TODO:    SET_REG_FIELD(MFSEL(mux_reg1), GCR_GPIO_MUX_FIELD1, mux1.value);
	//TODO:}

	//TODO:if (mux2.size != 0)
	//TODO:{
	//TODO:    SET_REG_FIELD(MFSEL(mux_reg2), GCR_GPIO_MUX_FIELD2, mux2.value);
	//TODO:}
	if ( gpio_num == 169)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SCISEL, 0);
	}
	if ( gpio_num == 162)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_SIRQSEL, 1);
	}
	if ( gpio_num == 168)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_CLKRUNSEL, 1);	// Selects GPIO168 not LPC nCLKRUN. When this bit is 1, MFSEL4.8 = 0
		SET_REG_FIELD(MFSEL4, MFSEL4_ESPISEL, 0);
	}

	if ( gpio_num == 163)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_CLKRUNSEL, 1);
		SET_REG_FIELD(MFSEL4, MFSEL4_ESPISEL, 0);
	}

	if ( (gpio_num == 161)  ||
	    ((gpio_num >= 164) && (gpio_num <= 167)) ||
	     (gpio_num == 166) ||
		(gpio_num == 95))
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_LPCSEL, 1);
		SET_REG_FIELD(MFSEL4, MFSEL4_ESPISEL, 0);
	}

	if (gpio_num == 190)
	{
		SET_REG_FIELD(FLOCKR1, FLOCKR1_PSMISEL, 1);
	}



	// TODO: need to finish this table :(

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_PinPullUpDown                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num    -                                                                          */
/*                  enable      -                                                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets internal Pull Up/Down resistor for the given pin                     */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_PinPullUpDown(UINT32 gpio_num, BOOLEAN enable)
{
	//UINT    port_num = 0;
	//UINT    port_bit = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Retriving GPIO info                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	// TODO:GPIO_GetGPIOInfo(gpio_num, &port_num, &port_bit);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting Pull Up/Down configuration                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	//if (enable)
	//{
	//SET_REG_BIT(GPIOPxPE(port_num), port_bit);
	//}
	//else
	//{
	//    //CLEAR_REG_BIT(GPIOPxPE(port_num), port_bit);
	//}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_SD                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                 sdNum    - [in], SD device number                                                       */
/*                 devType  - [in], SD1 device type (SD or MMC)                                            */
/*                                                                                                         */
/* Returns:         SWC HAL Error code                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SD mux                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ( SD_MODULE_TYPE)
DEFS_STATUS CHIP_Mux_SD(SD_DEV_NUM_T sdNum, SD_DEV_TYPE_T sdType)
{
	if (sdNum >= SD_NUM_OF_MODULES)
	{
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
	}

	if (sdNum == SD1_DEV)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SD1SEL, 1);
	}
	else if (sdNum == SD2_DEV)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_MMCSEL, 1);
	}

	return DEFS_STATUS_OK;
}
#endif //  ( SD_MODULE_TYPE)

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootTimeConfig                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  clk - Card clock frequency (in Hz units)                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates and configures the number of SDCLK clock cycles:               */
/*                   1) In a 50 millisecond time.                                                          */
/*                   1) In a 1 second time.                                                                */
/*                  These values are being used to determine whether Boot Time-Out has occured.            */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ( SD_MODULE_TYPE)
void CHIP_EmmcBootTimeConfig (UINT32 clk)
{
	// TODO: UINT32 cycles = 0;

	/* Configure the number of SDCLK clock cycles in 50ms */
	// TODO: cycles = (clk/_1SEC_IN_MSEC_) * 50;

	// TODO: SET_REG_FIELD(EMMCBTCNT, EMMCBTCNT_TIME50MSEC, cycles);

	/* Configure the number of SDCLK clock cycles in 1s -
	   Note that card clock frequency is actually the number of SDCLK clock cycles in 1 second */
	// TODO: cycles = clk;
	// TODO: REG_WRITE(EMMCBTT, cycles);
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootStart                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ack - TRUE means expect the boot acknowledge data, FALSE otherwise.                    */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initiates a EMMC boot sequence.                                           */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ( SD_MODULE_TYPE)
void CHIP_EmmcBootStart (BOOLEAN ack)
{
	/* Start boot */
	// TODO: SET_REG_FIELD(EMMCBTCNT, EMMCBTCNT_BTACK, ack);
	// TODO: SET_REG_FIELD(EMMCBTCNT, EMMCBTCNT_BOOTREQ, 1);    //Also enables the clock to the card
	// TODO: REG_WRITE(EMMCBTST, MASK_FIELD(EMMCBTST_BOOTERRORS));  //W1C for bits 1-3, bit 0 is RO.

	/* Wait untill boot is acknowledged */
	// TODO: while (!READ_REG_FIELD(EMMCBTST, EMMCBTST_BOOTRUN));
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootInvalid                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         TRUE if boot is invalid, FALSE otherwise                                               */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks whether the current boot process is invalid,                       */
/*                  i.e., one of the following has occured:                                                */
/*                   1) The device did not respond in time with the boot acknowledge sequence or data.     */
/*                   2) An error in data structure occurred.                                               */
/*                   3) A CRC error occurred when reading from the eMMC device.                            */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ( SD_MODULE_TYPE)
BOOLEAN CHIP_EmmcBootInvalid (void)
{

	// TODO: return (READ_REG_FIELD(EMMCBTST, EMMCBTST_BOOTTO) ||
	// TODO:         READ_REG_FIELD(EMMCBTST, EMMCBTST_BOOTERR) ||
	// TODO:         READ_REG_FIELD(EMMCBTST, EMMCBTST_BOOTCRC));
	return FALSE;
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootEnd                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ends a EMMC boot sequence, either when the boot sequence is done,         */
/*                  or an error is discovered.                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ( SD_MODULE_TYPE)
void CHIP_EmmcBootEnd (void)
{
	// TODO: SET_REG_FIELD(EMMCBTCNT, EMMCBTCNT_BOOTREQ, 0);
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SetVCDInput   :  TODO                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  headNum - Number of the head (1 or 2)                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the intput for the VCD                                            */
/*---------------------------------------------------------------------------------------------------------*/
#if  defined (VCD_MODULE_TYPE) // TODO: need to implement
void CHIP_SetVCDInput(/* TODO : CHIP_VCD_HEAD_T headNum */ void)
{
#if 0
	if (headNum == GCR_VCD_HEAD1)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_DVH1SEL, 1);
	}
	else if (headNum == GCR_VCD_HEAD2)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_DVH1SEL, 0);
	}
#endif
	ASSERT(0);
}
#endif //  VCD_MODULE_TYPE


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SetGFXInput                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  config - GFX configuration                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the intput for the VCD                                            */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (VCD_MODULE_TYPE)
void CHIP_SetGFXConfig(/* TODO: VCD_CONFIG_T config */ void)
{
#if 0
	if      (config == GCR_GFX_CONFIG_OUTPUT_FROM_HEAD1)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_DVH1SEL, MFSEL1_DVOSEL_OUTPUT_HEAD1);
	}
	else if (config == GCR_GFX_CONFIG_OUTPUT_FROM_HEAD2)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_DVH1SEL, MFSEL1_DVOSEL_OUTPUT_HEAD2);
	}
	else if (config == GCR_GFX_CONFIG_INPUT_TO_KVM)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_DVH1SEL, MFSEL1_DVOSEL_INPUT_KVM);
	}
#endif
	ASSERT(0);
}
#endif // VCD_MODULE_TYPE


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_ConfigUSBBurstSize                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the USB burst size in USB test register                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_ConfigUSBBurstSize(void)
{
	// SET_REG_FIELD(USBTEST, USBTEST_BURSTSIZE, 3);   // Burst configured to 8 + 4
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_ResetTypeIsPor                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns true if reset source was POR (powerOn) reset                                   */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN CHIP_ResetTypeIsPor()
{
	return POR_STATUS();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_ResetTypeIsCor                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns TRUE if and only if a Core reset is performed                                  */
/*                  and FALSE on any other reset type.                                                     */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN CHIP_ResetTypeIsCor()
{
	return (COR_STATUS() & !POR_STATUS());
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MarkCfgDone                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Sets a flag in scratch register specifying that CLK/DDR configuration were performed   */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_MarkCfgDone()
{
	MARK_CFG_DONE();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_CfgWasDone                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns true if a flag in scratch register indicates                                   */
/*                  that CLK/DDR configuration was already done                                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN CHIP_CfgWasDone()
{
	return CFG_IS_DONE();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EnableJtag                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Enable Jtag access                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_EnableJtag()
{
	ENABLE_JTAG() ;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_DisableJtag                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Disable Jtag access                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_DisableJtag()
{
	DISABLE_JTAG();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_DisableModules                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  mdlr -  Value to write to MDLR register                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Writes to the MDLR register which determines which modules to disable                  */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_DisableModules(UINT32 mdlr)
{
	REG_WRITE(MDLR, mdlr);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SMBPullUp                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  module_num - SMB module number.                                                        */
/*                  enable     - TRUE to configre as Pull-Up; FALSE to configre as Pull-Down.              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures pull-up/down for SMB interface pins.                           */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (SMB_MODULE_TYPE)
void CHIP_SMBPullUp (
	SMB_MODULE_T module_num,
	BOOLEAN      enable
)
{
	ASSERT(module_num < SMB_NUM_OF_MODULES);



	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting Pull Up/Down configuration                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	//if (enable)
	//{
	//    SET_REG_BIT(DEVPU0, module_num);
	//}

	//else
	//{
	//    CLEAR_REG_BIT(DEVPU0, module_num);
	//}
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxPECI                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         TRUE if PECI is enabled; FALSE otherwise.                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables the PECI signal for the PECI module (only in PECI bonding option, */
/*                  which is indicated by PECIST internal strap being set to 1).                           */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (PECI_MODULE_TYPE)
BOOLEAN CHIP_MuxPECI (void)
{
	// TBD
	return TRUE;
}
#endif




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxADC                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  adc_module - The ADC module to be selected.                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the ADCx function to the corresponding pin.                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (ADC_MODULE_TYPE)
void CHIP_MuxADC (ADC_INPUT_T adc_module)
{
	ASSERT(adc_module < ADC_NUM_OF_INPUTS);

	/// SET_REG_BIT(DEVALT6, adc_module);
}
#endif




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxPWM                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  pwm_module - The PWM module to be selected.                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the PWMx function to the corresponding pin.                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (PWM_MODULE_TYPE)
void CHIP_MuxPWM (PWM_MODULE_T pwm_module)
{
	switch(pwm_module)
	{
	case PWM_MODULE_0:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM0SEL, 1);  break;
	case PWM_MODULE_1:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM1SEL, 1);  break;
	case PWM_MODULE_2:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM2SEL, 1);  break;
	case PWM_MODULE_3:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM3SEL, 1);  break;
	case PWM_MODULE_4:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM4SEL, 1);  break;
	case PWM_MODULE_5:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM5SEL, 1);  break;
	case PWM_MODULE_6:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM6SEL, 1);  break;
	case PWM_MODULE_7:      SET_REG_FIELD(MFSEL2, MFSEL2_PWM7SEL, 1);  break;
	default:                ASSERT(0); break;
   }

}
#endif






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxMFT                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  mft_module - The MFT module to be selected.                                            */
/*                  mft_timer -  The MFT timer to be selected.                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the TXn function to the corresponding pin, where:                 */
/*                  · 'X' is the MFT timer number  (A/B).                                                  */
/*                  · 'n' is the MFT module number (1/2/etc').                                             */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (MFT_MODULE_TYPE)
void CHIP_MuxMFT (
	MFT_MODULE_T mft_module,
	MFT_TIMER_T  mft_timer
)
{
	//SET_REG_BIT(MFSEL, (2*mft_module + mft_timer));
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxSMB                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  smb_module - SMB Module number                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the SMBx function to the corresponding pin.                       */
/*                  NOTE: This function does not set the segment! Use CHIP_Mux_Segment in addition      */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (SMB_MODULE_TYPE)

#ifdef NPCM750_CP
const UINT SMB_INT[SMB_NUM_OF_MODULES] = {SMB0_INT  ,
					SMB1_INT  ,
					SMB2_INT  ,
					SMB3_INT  ,
					SMB4_INT  ,
					SMB5_INT  ,
					SMB6_INT  ,
					SMB7_INT  ,
					SMB8_INT  ,
					SMB9_INTs ,
					SMB10_INT ,
					SMB11_INT ,
					SMB12_PCIMAILBOX_INT  ,
					SMB13_SIOX1_2_INTs    ,
					SMB14_UART1_3_INT     ,
					SMB15_MFT6_7_INT}   ;
#endif
void CHIP_MuxSMB (SMB_MODULE_T smb_module, BOOLEAN bEnable)
{
	switch (smb_module)
	{
	case 0:
	if (READ_REG_FIELD(MFSEL1, MFSEL1_SMB0SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_SMB0SEL, bEnable);
	}
	SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0DECFG , 0x0);  // Smbus 0 Drive enabled: set it to float.
	SET_REG_FIELD(I2CSEGCTL , I2CSEGCTL_S0D_WE_EN, 0x3) ; // Smbus 0 Drive enabled: set it to float.
	break;

	case 1:
	if (READ_REG_FIELD(MFSEL1, MFSEL1_SMB1SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_SMB1SEL, bEnable);
	}
	break;

	case 2:
	if (READ_REG_FIELD(MFSEL1, MFSEL1_SMB2SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_SMB2SEL, bEnable);
	}
	break;

	case 3:
	if (READ_REG_FIELD(MFSEL1, MFSEL1_SMB3SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_SMB3SEL, bEnable);
	}
	break;

	case 4:
	if (READ_REG_FIELD(MFSEL1, MFSEL1_SMB4SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_SMB4SEL, bEnable);
	}
	SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4DECFG , 0x0);  // Smbus 4 Drive enabled: set it to float.
	SET_REG_FIELD(I2CSEGCTL , I2CSEGCTL_S4D_WE_EN, 0x3) ; // Smbus 4 Drive enabled: set it to float.
	break;

	case 5:
	if (READ_REG_FIELD(MFSEL1, MFSEL1_SMB5SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL1, MFSEL1_SMB5SEL, bEnable);
	}
	break;

	case 6:
	if (READ_REG_FIELD(MFSEL3, MFSEL3_SMB6SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB6SEL, bEnable);
	}
	break;

	case 7:
	if (READ_REG_FIELD(MFSEL3, MFSEL3_SMB7SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB7SEL, bEnable);
	}
	break;

	case 8:
	if (READ_REG_FIELD(MFSEL4, MFSEL4_SMB8SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB8SEL, bEnable);
	}
	break;

	case 9:
	if (READ_REG_FIELD(MFSEL4, MFSEL4_SMB9SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB9SEL, bEnable);
	}
	break;

	case 10:
	if (READ_REG_FIELD(MFSEL4, MFSEL4_SMB10SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB10SEL, bEnable);
	}
	break;

	case 11:
	if (READ_REG_FIELD(MFSEL4, MFSEL4_SMB11SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB11SEL, bEnable);
	}

	case 12:
	if (READ_REG_FIELD(MFSEL3, MFSEL3_SMB12SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB12SEL, bEnable);
	}
	break;

	case 13:
	if (READ_REG_FIELD(MFSEL3, MFSEL3_SMB13SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB13SEL, bEnable);
	}
	break;

	case 14:
	if (READ_REG_FIELD(MFSEL3, MFSEL3_SMB14SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB14SEL, bEnable);
	}
	break;

	case 15:
	if (READ_REG_FIELD(MFSEL3, MFSEL3_SMB15SEL) == !bEnable)
	{
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB15SEL, bEnable);
	}
	break;

	default:
	ASSERT(FALSE);
	break;
	}

	CHIP_SMBPullUp(smb_module, bEnable);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_Segment                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  smb_module - SMB Module number                                                         */
/*                  segment - 0 => A, 1 => B , 2 => C, 3 => D                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the SMBx function to the corresponding pin.                       */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_Segment (SMB_MODULE_T smb_module, UINT32 segment, BOOLEAN bEnable)
{
	if ((smb_module > (SMB_MODULE_T)5) || (segment > 3))
	{
		return;
	}


	switch (smb_module)
	{
	case 0:
		{
			REG_WRITE(I2CSEGCTL, (REG_READ(I2CSEGCTL) & 0xFFFC0000) | BUILD_FIELD_VAL(I2CSEGCTL_SMB0SS,segment) | BUILD_FIELD_VAL(I2CSEGCTL_WEN0SS,1));
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SBSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SCSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SDSEL, 0);
			if (bEnable == TRUE)
			{
			    switch(segment)
			    {
			        case 1:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SBSEL, bEnable);
			            break;

			        case 2:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SCSEL, bEnable);
			            break;

			        case 3:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SDSEL, bEnable);
			            break;

			        case 0:
			        default:

			            break;
			    }
		    }
		    break;
		}
		case 1:
		{
			REG_WRITE(I2CSEGCTL, (REG_READ(I2CSEGCTL) & 0xFFFC0000) | BUILD_FIELD_VAL(I2CSEGCTL_SMB1SS,segment) | BUILD_FIELD_VAL(I2CSEGCTL_WEN1SS,1));
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SBSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SCSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SDSEL, 0);
			if (bEnable == TRUE)
			{
			    switch(segment)
			    {
			        case 1:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SBSEL, bEnable);
			            break;

			        case 2:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SCSEL, bEnable);
			            break;

			        case 3:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SDSEL, bEnable);
			            break;

			        case 0:
			        default:

			            break;
			    }
		    }
		    break;

		}


		 case 2:
		 {
		 	REG_WRITE(I2CSEGCTL, (REG_READ(I2CSEGCTL) & 0xFFFC0000) | BUILD_FIELD_VAL(I2CSEGCTL_SMB2SS,segment) | BUILD_FIELD_VAL(I2CSEGCTL_WEN2SS,1));
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SBSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SCSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SDSEL, 0);
			if (bEnable == TRUE)
			{
			    switch(segment)
			    {
			        case 1:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SBSEL, bEnable);
			            break;

			        case 2:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SCSEL, bEnable);
			            break;

			        case 3:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SDSEL, bEnable);
			            break;

			        case 0:
			        default:

			            break;
			    }
		    }
		    break;

		}


		case 3:
		{
			REG_WRITE(I2CSEGCTL, (REG_READ(I2CSEGCTL) & 0xFFFC0000) | BUILD_FIELD_VAL(I2CSEGCTL_SMB3SS,segment) | BUILD_FIELD_VAL(I2CSEGCTL_WEN3SS,1));
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SBSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SCSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SDSEL, 0);
			if (bEnable == TRUE)
			{
			    switch(segment)
			    {
			        case 1:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SBSEL, bEnable);
			            break;

			        case 2:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SCSEL, bEnable);
			            break;

			        case 3:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SDSEL, bEnable);
			            break;

			        case 0:
			        default:

			            break;
			    }
		    }
		    break;

		}

		case 4:
		{
			REG_WRITE(I2CSEGCTL, (REG_READ(I2CSEGCTL) & 0xFFFC0000) | BUILD_FIELD_VAL(I2CSEGCTL_SMB4SS,segment) | BUILD_FIELD_VAL(I2CSEGCTL_WEN4SS,1));
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SBSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SCSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SDSEL, 0);
			if (bEnable == TRUE)
			{
			    switch(segment)
			    {
			        case 1:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SBSEL, bEnable);
			            break;

			        case 2:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SCSEL, bEnable);
			            break;

			        case 3:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SDSEL, bEnable);
			            break;

			        case 0:
			        default:

			            break;
			    }
		    }
		    break;

		}
	   case 5:
	   {
	   		REG_WRITE(I2CSEGCTL, (REG_READ(I2CSEGCTL) & 0xFFFC0000) | BUILD_FIELD_VAL(I2CSEGCTL_SMB5SS,segment) | BUILD_FIELD_VAL(I2CSEGCTL_WEN5SS,1));
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SBSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SCSEL, 0);
			SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SDSEL, 0);
			if (bEnable == TRUE)
			{
			    switch(segment)
			    {
			        case 1:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SBSEL, bEnable);
			            break;

			        case 2:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SCSEL, bEnable);
			            break;

			        case 3:
			            SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SDSEL, bEnable);
			            break;

			        case 0:
			        default:

			            break;
			    }
		    }
		    break;

		}
		default:
		    ASSERT(FALSE);
		    break;
	}
	return;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SMB_GetGpioNumbers                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  smb_module - SMB Module number                                                         */
/*                  segment - SMB segment number (0,1,2,3 means A,B,C,D pins)                              */
/*                  ret_scl_gpio - SCL to gpio pin number                                                  */
/*                  ret_sda_gpio - SDA to gpio pin number                                                  */
/*                                                                                                         */
/* Returns:         status                                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine return the gpio number of SCL signal                                      */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS  CHIP_SMB_GetGpioNumbers (SMB_MODULE_T smb_module, UINT segment, INT* ret_scl_gpio, INT* ret_sda_gpio )
{
	const INT SMB_SCL_GPIO[SMB_NUM_OF_MODULES][SMB_NUM_OF_SEGMENT] = {
		               {114, 194,  196,      199    },
		               {116, 127,  125,      5      },
		               {118, 123,  121,      7      },
		               {31,  40,   38 ,     60      },
		               {29,  19,   21 ,     23      },
		               {27,  12,   14 ,     93      },
		               {171, -1,   -1 ,     -1      },
		               {173, -1,   -1 ,     -1      },
		               {128, -1,   -1 ,     -1      },
		               {130, -1,   -1 ,     -1      },
		               {132, -1,   -1 ,     -1      },
		               {134, -1,   -1 ,     -1      },
		               {220, -1,   -1 ,     -1      },
		               {222, -1,   -1 ,     -1      },
		               {23,  -1,   -1 ,     -1      },
		               {21,  -1,   -1 ,     -1      }};

	const INT SMB_SDA_GPIO[SMB_NUM_OF_MODULES][SMB_NUM_OF_SEGMENT] = {
		               { 115   , 195   , 202   , 198 },
		               { 117   , 126   , 124   , 4   },
		               { 119   , 122   , 120   , 6   },
		               { 30    , 39    , 37    , 59  },
		               { 28    , 18    , 20    , 22  },
		               { 26    , 13    , 15    , 94  },
		               { 172   , -1    , -1    , -1  },
		               { 174   , -1    , -1    , -1  },
		               { 129   , -1    , -1    , -1  },
		               { 131   , -1    , -1    , -1  },
		               { 133   , -1    , -1    , -1  },
		               { 135   , -1    , -1    , -1  },
		               { 221   , -1    , -1    , -1  },
		               { 223   , -1    , -1    , -1  },
		               { 22    , -1    , -1    , -1  },
		               { 20    , -1    , -1    , -1  }};

	INT ret_val = 0;
	if ((smb_module >= SMB_NUM_OF_MODULES) || (segment >= SMB_NUM_OF_SEGMENT))
		return DEFS_STATUS_INVALID_PARAMETER;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Find SCL GPIO number on the table                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	ret_val = SMB_SCL_GPIO[smb_module][segment];

	if (ret_val == -1)
		return DEFS_STATUS_INVALID_PARAMETER;

	*ret_scl_gpio = ret_val;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Find SDA GPIO number on the table                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	ret_val = SMB_SDA_GPIO[smb_module][segment];

	if (ret_val == -1)
		return DEFS_STATUS_INVALID_PARAMETER;

	*ret_sda_gpio = ret_val;


	return DEFS_STATUS_OK;

}
#endif



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxSMB2Segment                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  smb_module - SMB Module number                                                         */
/*                  segment - 0 => A, 1 => B , 2 => C, 3 => D                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the SMBx function to the to a segment mapping.                    */
/*                  This feature is applicable to SMB0 to SMB5 only                                        */
/* NOTE! This function should be performed by the MP side to mux channels and segemtns. The code is here for*/
/*       Reference only                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if 0
void CHIP_MuxSMB2Segment(SMB_MODULE_T smb_module, UINT segment)
{
	switch (smb_module)
	{
	case 0:
	SET_REG_FIELD(MFSEL1, MFSEL1_SMB0SEL, TRUE);
	SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );        // DVO is muxed with the SMB
	SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0DECFG, 0x2);  // Smbus 0 Drive enabled: output signal that is driven by some targets.
	break;

	case 1:
		SET_REG_FIELD(MFSEL1, MFSEL1_SMB1SEL, TRUE);
		switch(segment)
		{
		case 1:
			SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
			break;

		case 2:
			SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
			break;

		case 3:
			SET_REG_FIELD(MFSEL3, MFSEL3_IOX2SEL, 0 );
			break;

		case 0:
		default:
			// SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
			break;
		}

		break;

	 case 2:
	SET_REG_FIELD(MFSEL1, MFSEL1_SMB2SEL, TRUE);
	SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
	switch(segment)
	{
		case 1:
		SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
		break;

		case 2:
		SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
		break;

		case 3:
		SET_REG_FIELD(MFSEL3, MFSEL3_IOX2SEL, 0 );
		break;

		case 0:
		default:
		break;
	}

	break;

	case 3:
	SET_REG_FIELD(MFSEL1, MFSEL1_SMB3SEL, TRUE);
	SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
	switch(segment)
	{
		case 1:
		SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
		break;

		case 2:
		SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, 0 );
		break;

		case 3:
		SET_REG_FIELD(MFSEL2, MFSEL2_HG6SEL, 0 );
		SET_REG_FIELD(MFSEL2, MFSEL2_HG7SEL, 0 );
		break;

		case 0:
		default:
		break;
	}
	break;

	case 4:
	SET_REG_FIELD(MFSEL1, MFSEL1_SMB4SEL, TRUE);
	switch(segment)
	{
		case 1:
		SET_REG_FIELD(MFSEL3, MFSEL3_PSPI2SEL, 0 );
		break;

		case 2:
		SET_REG_FIELD(MFSEL2, MFSEL2_HG0SEL, 0 );
		SET_REG_FIELD(MFSEL2, MFSEL2_HG1SEL, 0 );
		break;

		case 3:
		SET_REG_FIELD(MFSEL2, MFSEL2_HG2SEL, 0 );
		SET_REG_FIELD(MFSEL2, MFSEL2_HG3SEL, 0 );
		break;

		case 0:
		default:
		break;
	}

	SET_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4DECFG, 0);  // Smbus 0 Drive enabled
	break;

   case 5:
	SET_REG_FIELD(MFSEL1, MFSEL1_SMB5SEL, TRUE);
	switch(segment)
	{
		case 1:
		SET_REG_FIELD(MFSEL1, MFSEL1_GSPISEL, 0 );
		break;


		case 2:
		SET_REG_FIELD(MFSEL1, MFSEL1_GSPISEL, 0 );
		break;

		case 3:

		SET_REG_FIELD(MFSEL1, MFSEL1_KBCICSEL, 0 );
		break;

		case 0:
		default:
		break;
	}
	break;
	case 6:
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB6SEL, 1);
		break;

	case 7:
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB7SEL, 1);
		break;

	case 8:
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB8SEL, 1);
		break;

	case 9:
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB9SEL, 1);
		break;

	case 10:
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB10SEL, 1);
		break;

	case 11:
		SET_REG_FIELD(MFSEL4, MFSEL4_SMB11SEL, 1);


	case 12:
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB12SEL, 1);
		break;

	case 13:
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB13SEL, 1);
		break;

	case 14:
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB14SEL, 1);
		break;

	case 15:
		SET_REG_FIELD(MFSEL3, MFSEL3_SMB15SEL, 1);
		break;

	default:
	ASSERT(FALSE);
	break;
	}

	CHIP_Mux_Segment(smb_module, segment, TRUE);

	CHIP_SMBPullUp(smb_module, TRUE);
}

#endif



#if defined (SMB_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SMB_GetSegment                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  smb_module - SMB Module number                                                         */
/*                  segment - 0 => A, 1 => B , 2 => C, 3 => D                                              */
/*                                                                                                         */
/* Returns:         status                                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the segment number which is currently muxed to a given module     */
/*                  This feature is applicable to SMB0 to SMB5 only                                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CHIP_SMB_GetSegment (SMB_MODULE_T smb_module, UINT* segment)
{
	*segment = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* parameter validity check                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	if (smb_module > SMB_NUM_OF_MODULES )
	{
		return DEFS_STATUS_INVALID_PARAMETER;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* SMB0 to SMB5 have segments, the rest has a single segment only                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	if (smb_module > 5 )
	{
		return DEFS_STATUS_OK;
	}

	switch (smb_module)
	{
	case 0:
	if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SBSEL) != 0 )
		*segment = 1;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SCSEL) != 0 )
		*segment = 2;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S0SDSEL) != 0 )
		*segment = 3;
	else
		*segment = 0;
	break;

	case 1:
	if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SBSEL) != 0 )
		*segment = 1;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SCSEL) != 0 )
		*segment = 2;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S1SDSEL) != 0 )
		*segment = 3;
	else
		*segment = 0;
	break;

	case 2:
	if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SBSEL) != 0 )
		*segment = 1;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SCSEL) != 0 )
		*segment = 2;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S2SDSEL) != 0 )
		*segment = 3;
	else
		*segment = 0;
	break;

	case 3:
	if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SBSEL) != 0 )
		*segment = 1;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SCSEL) != 0 )
		*segment = 2;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S3SDSEL) != 0 )
		*segment = 3;
	else
		*segment = 0;
	break;

	case 4:
	if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SBSEL) != 0 )
		*segment = 1;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SCSEL) != 0 )
		*segment = 2;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S4SDSEL) != 0 )
		*segment = 3;
	else
		*segment = 0;
	break;


	case 5:
	if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SBSEL) != 0 )
		*segment = 1;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SCSEL) != 0 )
		*segment = 2;
	else if ( READ_REG_FIELD(I2CSEGSEL, I2CSEGSEL_S5SDSEL) != 0 )
		*segment = 3;
	else
		*segment = 0;
	break;

	default:
	ASSERT(0);


	}

	return DEFS_STATUS_OK;

}

#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxGPIOPort                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port      - GPIO port number.                                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine mux chip pins to function as port GPIOs                                   */
/*lint -efunc(715,CHIP_MuxGPIOPort)                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (GPIO_MODULE_TYPE)
void CHIP_MuxGPIOPort (UINT8 port)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* NOT IMPLEMENTED                                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	ASSERT(FALSE);
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxDAC                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dac_input - DAC Module number                                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the DACx outputs interface on multiplexed pin.                    */
/*                  The function should be called prior to any other DAC operation.                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (DAC_MODULE_TYPE)
void CHIP_MuxDAC (DAC_INPUT_T dac_input)
{
	ASSERT(dac_input < DAC_NUM_OF_INPUTS);

	/*-----------------------------------------------------------------------------------------------------*/
	/* SET_REG_BIT(DEVALT4, dac_input);                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Get_Version                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the chip version (POLEG_VERSION_Z1/ POLEG_VERSION_Z2              */
/*                                                         POLEG_VERSION_A1)                               */
/*---------------------------------------------------------------------------------------------------------*/
UINT32   CHIP_Get_Version (void)
{
	return READ_REG_FIELD(PDID, PDID_VERSION);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Disable_CPU1                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable second core for singel core solution (used in Z2 and above)       */
/*---------------------------------------------------------------------------------------------------------*/
void   CHIP_Disable_CPU1 (void)
{
	UINT32        flockr1 = 0;

	flockr1 = REG_READ(FLOCKR1);

	SET_VAR_FIELD(flockr1, FLOCKR1_CPU1CKDIS, 1);

	SET_VAR_FIELD(flockr1, FLOCKR1_CPU1CKDLK, 1);

	REG_WRITE(FLOCKR1, flockr1);

	return;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxESPI                                                                           */
/*                                                                                                         */
/* Parameters:      driveStrength_mA ESPI drive strength in mA                                             */
/* Returns:         TRUE if ESPI is enabled; FALSE otherwise.                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables the ESPI                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (ESPI_MODULE_TYPE)
BOOLEAN CHIP_MuxESPI  (UINT driveStrength_mA)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable ESPI signals                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(MFSEL4, MFSEL4_ESPISEL, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Select ESPI drive strength                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if (driveStrength_mA <= 8)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 2); // 0 1 0: 8 mA
	}
	else if(driveStrength_mA <= 12)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 3); // 0 1 1: 12 mA (LPC default)
	}
	else if(driveStrength_mA <= 16)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 6); // 1 1 0: 16 mA (may be used for eSPI)
	}
	else if(driveStrength_mA <= 24)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 7); // 1 1 1: 24 mA (Should be used for eSPI – requires an external 13.7 ohm series resistor)
	}
	else
	{
		return FALSE;
	}

	return TRUE;

}
#endif



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxLPC                                                                            */
/*                                                                                                         */
/* Parameters:      driveStrength_mA LPC drive strength in mA                                              */
/* Returns:         TRUE if LPC is enabled; FALSE otherwise.                                               */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables the LPC                                                           */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN CHIP_MuxLPC  (UINT driveStrength_mA)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable ESPI signals                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(MFSEL4, MFSEL4_ESPISEL, 0);
	SET_REG_FIELD(MFSEL3, MFSEL3_CLKRUNSEL, 1);
	SET_REG_FIELD(INTCR2, INTCR2_DISABLE_EPSI_AUTO_INIT, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Select ESPI drive strength                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if (driveStrength_mA <= 8)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 2); // 0 1 0: 8 mA
	}
	else if(driveStrength_mA <= 12)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 3); // 0 1 1: 12 mA (LPC default)
	}
	else if(driveStrength_mA <= 16)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 6); // 1 1 0: 16 mA (may be used for eSPI)
	}
	else if(driveStrength_mA <= 24)
	{
		SET_REG_FIELD(DSCNT, DSCNT_ESPI, 7); // 1 1 1: 24 mA (Should be used for eSPI – requires an external 13.7 ohm series resistor)
	}
	else
	{
		return FALSE;
	}

	return TRUE;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the chip registers                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_PrintRegs (void)
{
	HAL_PRINT("/*--------------*/\n");
	HAL_PRINT("/*     CHIP     */\n");
	HAL_PRINT("/*--------------*/\n\n");

	HAL_PRINT("\n");
}


