/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   bootblock_nuvoton_cusomize.c                                             */
/* This file contains the vendor specific settings. All changes on vendor side should be handled */
/*            at this file only.                                              */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/

#include "../SWC_HAL/hal.h"
#include "../SWC_HAL/hal_regs.h"

#include <string.h>
#include "cfg.h"
#include "security.h"
#include "mailbox.h"
#include "boot.h"
#include "fuse_wrapper/fuse_wrapper.h"
#include "Apps/serial_printf/serial_printf.h"



extern DEVICE_T        dev_t;

/*---------------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/* OTP values                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
extern UINT8*     oKap  ;                             // using only 2 bytes.
extern UINT8*     oFsap ;                             // using only 2 bytes.
extern UINT8*     oKavfp;                             // using only 2 bytes.
extern UINT8*     oFsvfp;                             // using only 2 bytes.
extern UINT8*     oVerificationFaultModuleProtection; // using only 4 bytes.
extern UINT8      oCpFustrap[4];                      // using only 2 bytes
extern BOOLEAN    doSignatureCheck;
extern BOOLEAN    returnToRomCode;



extern        UART_DEV_T                      serial_printf_uart_port;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Print_Nuvoton                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Print vendor specific string.                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void         BOOTBLOCK_Print_Nuvoton (void)
{
	serial_printf(">Board manufacturer: Nuvoton \n" KNRM);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Print_Vendor_error                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Print vendor specific string.                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void         BOOTBLOCK_Print_Vendor_error (void)
{
	UINT8 val1[4];
	UINT8 val2[4];
	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_VENDOR, val1, NULL);
	BOOTBLOCK_ReadHeaderParameter(HEADER_FIELD_BOARD_TYPE, val2, NULL);
	serial_printf(KRED "ERROR! VENDOR UNKNOWN. PLEASE UPDATE HEADER: programmingScripts"
						"\\inputs\\BootBlockAndHeader.xml (board,vendor) (val: %d, %d)!\n" ,
						*(UINT32*)val1,  *(UINT32*)val2 );
	return;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Init_UART_Nuvoton                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  init the uart according to straps (per vendor configuration                            */
/*---------------------------------------------------------------------------------------------------------*/
void BOOTBLOCK_Init_UART_Nuvoton (void)
{
	UINT reset_is_PORST = !CFG_GetResetNum();

	/*-----------------------------------------------------------------------------------------------------*/
	/* Init UART											   */
	/*-----------------------------------------------------------------------------------------------------*/
	serial_printf_uart_port = UART0_DEV;

	// reset uart only on PORST
	if (reset_is_PORST)
	{
	    CLK_ResetUART();
	    CLK_Delay_MicroSec(100);
	}


	if (reset_is_PORST)
	{
	    // UART0 on UART debug port + mux set
	    UART_Init(serial_printf_uart_port, UART_MUX_MODE1_HSP1_SI2____HSP2_UART2__UART1_s_HSP1__UART3_s_SI2, UART_BAUDRATE_115200);
	}
	else
	{
	    // UART0 on UART debug port + mux set
	    UART_Init(serial_printf_uart_port, UART_MUX_SKIP_CONFIG, UART_BAUDRATE_115200);
	}

	CLK_Delay_MicroSec(100);

	return;


}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Init_CLK_Nuvoton                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  init the clks according to straps (per vendor configuration)                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS BOOTBLOCK_Init_CLK_Nuvoton (void)
{
	DEFS_STATUS  status ;
	BOARD_T      eBoard;
	UINT32       cpuClk;
	UINT32       mcClk;
	UINT32       polegVersion = CHIP_Get_Version();

	eBoard = BOOTBLOCK_GetBoardType();

	//UINT32   mcClk		= 666666667; // version D10.6.8
	//UINT32   mcClk		= 775000000; // version D20.6.8
	//UINT32   mcClk		= 787500000; // version D30.6.8

	cpuClk = BOOTBLOCK_Get_CPU_freq();
	mcClk = BOOTBLOCK_Get_MC_freq();

	// For version 10.7.0 limit the options. For full range remove next 7 lines:

	/*-----------------------------------------------------------------------------------------------------*/
	/* Special handling for Run_BMC: set PLL0 to provide 125 MHz on on TOCK                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (eBoard == BOARD_RUN_BMC)
	{
		if (cpuClk != mcClk)
		{
			//ERROR: if cpu freq != mc freq can't set TOCK to 125MHz. Need on unused PLL for this
			return DEFS_STATUS_INVALID_DATA_FIELD;
		}

		status = CLK_Configure_CPU_MC_Clock(mcClk, cpuClk, 125000000) ;

		return status;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Reconfigure the MC clock                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	status = CLK_Configure_CPU_MC_Clock(mcClk, cpuClk, 0) ;
	if ( status == DEFS_STATUS_SYSTEM_NOT_INITIALIZED)
	{
		serial_printf(">DDR and CPU clk already configured.\n");// secondary reset
	}

	if ( status == DEFS_STATUS_CLK_ERROR)
	{
		serial_printf(">DDR and CPU unsupported values.\n");
		CFG_HeartBeatToggle(7);
	}




	return status;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Init_Before_Image_Check_Nuvoton                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Customer specific code: placeholder for future enhancements :  before search image     */
/*---------------------------------------------------------------------------------------------------------*/
void BOOTBLOCK_Init_Before_Image_Check_Nuvoton (void)
{

	// Note: This is the place to put miscellaneous vendor settings.
	//	     This function is invoked before the bootblock starts to search for a valid uboot image.
	return;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Init_Before_UBOOT_Nuvoton                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Customer specific code: placeholder for future enhancements : before uboot             */
/*---------------------------------------------------------------------------------------------------------*/
void BOOTBLOCK_Init_Before_UBOOT_Nuvoton (void)
{

	// Note: This is the place to put miscellaneous vendor setting.
	//	     This function is invoked just before booblock jumps to uboot.
	return;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BOOTBLOCK_Init_GPIO_Nuvoton                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  init the gpio misc settings                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void         BOOTBLOCK_Init_GPIO_Nuvoton (void)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* NPCM730: Do not configure graphics. GPIOs remain GPIOs                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( dev_t == DEVICE_NPCM730)
	{
		CHIP_Mux_GSPI(FALSE);
		serial_printf(">vgaioen=0 and don't mux gspi.\n");
	}

	/*----------------------------------------------------------------------------------------------------*/
	/* NPCM750: Configure VGA according to strap 6                                                        */
	/*----------------------------------------------------------------------------------------------------*/
	else //  ( dev_t == DEVICE_NPCM750)
	{
		if ( STRP_get(STRP_6_ECC) == 0)
		{
			// Turn off INTCR.6 (VGAIOEN)
			SET_REG_FIELD(INTCR, INTCR_VGAIOEN, 0 );

			// GSPI pins (GSPISEL=0)
			CHIP_Mux_GSPI(FALSE);
			serial_printf(">vgaioen=0 and don't mux gspi.\n");
		}
		else
		{
			// Turn on INTCR.6 (VGAIOEN)
			SET_REG_FIELD(INTCR, INTCR_VGAIOEN, 1 );

			// Select GSPISEL=1
			CHIP_Mux_GSPI(TRUE);

			serial_printf(">vgaioen=1 and mux gspi.\n");
		}
	}

	return;
}
