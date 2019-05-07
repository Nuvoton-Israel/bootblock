/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   cfg.c                                                                    */
/* This file contains configuration routines for ROM code and Booter          */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/
#define CFG_C


#include <string.h>

#include "hal.h"
#include "../SWC_HAL/hal_regs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Including chip definitions                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include "../SWC_HAL/Chips/npcm750/npcm750_regs.h"

#include "cfg.h"

#include "../SWC_HAL/ModuleDrivers/shm/1/shm_regs.h"


#include "shared_defs.h"
#include "Apps/serial_printf/serial_printf.h"





/*---------------------------------------------------------------------------------------------------------*/
/* Function Implementation                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

UINT32          gTimeTick[2];

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Tick                                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  time tick  (start counting time)                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void Tick (void)
{
	CLK_GetTimeStamp(gTimeTick);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Tock                                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  time tock  (stop and print time since the Tick)                                        */
/*---------------------------------------------------------------------------------------------------------*/
void Tock (void)
{
	UINT32          time_tock = 0;

	time_tock = CLK_Delay_Since(0, gTimeTick);
	serial_printf("\n>Boot block run for %ld us.\n", time_tock);
}






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CFG_HandleResetIndication                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine updates INTCR2 reg with latest reset type indications                     */
/*---------------------------------------------------------------------------------------------------------*/
void CFG_HandleResetIndication (void)
{
	// Copy the opposite value of RESSR.31-23 into INTCR2.23-31 :
	if ( READ_REG_FIELD(INTCR2, INTCR2_WDC) == 0)
	{
		SET_REG_FIELD(INTCR2, INTCR2_PORST , !READ_REG_FIELD(RESSR, RESSR_PORST ));
		SET_REG_FIELD(INTCR2, INTCR2_CORST , !READ_REG_FIELD(RESSR, RESSR_CORST ));
		SET_REG_FIELD(INTCR2, INTCR2_WD0RST, !READ_REG_FIELD(RESSR, RESSR_WD0RST));
		SET_REG_FIELD(INTCR2, INTCR2_SWR1ST, !READ_REG_FIELD(RESSR, RESSR_SWRST1));
		SET_REG_FIELD(INTCR2, INTCR2_SWR2ST, !READ_REG_FIELD(RESSR, RESSR_SWRST2));
		SET_REG_FIELD(INTCR2, INTCR2_SWR3ST, !READ_REG_FIELD(RESSR, RESSR_SWRST3));
		SET_REG_FIELD(INTCR2, INTCR2_SWR4ST, !READ_REG_FIELD(RESSR, RESSR_SWRST4));
		SET_REG_FIELD(INTCR2, INTCR2_WD1RST, !READ_REG_FIELD(RESSR, RESSR_WD1RST));
		SET_REG_FIELD(INTCR2, INTCR2_WD2RST, !READ_REG_FIELD(RESSR, RESSR_WD2RST));


		// Set bits RESSR.23-31 (W1C) :
		SET_REG_FIELD(RESSR, RESSR_PORST , 1);
		SET_REG_FIELD(RESSR, RESSR_CORST , 1);
		SET_REG_FIELD(RESSR, RESSR_WD0RST, 1);
		SET_REG_FIELD(RESSR, RESSR_SWRST1, 1);
		SET_REG_FIELD(RESSR, RESSR_SWRST2, 1);
		SET_REG_FIELD(RESSR, RESSR_SWRST3, 1);
		SET_REG_FIELD(RESSR, RESSR_SWRST4, 1);
		SET_REG_FIELD(RESSR, RESSR_WD1RST, 1);
		SET_REG_FIELD(RESSR, RESSR_WD2RST, 1);
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CFG_MiscConfigurations                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config graphic SPI and release host wait                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CFG_MiscConfigurations (void)
{
	// VGA BIOS SPI EEPROM enable:
	CHIP_Mux_GSPI(TRUE);

	// Release the LPC bus:
	CFG_SHM_ReleaseHostWait();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CFG_SHM_ReleaseHostWait                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine releases the LPC bus stall, allowing host to communicate with the device. */
/*---------------------------------------------------------------------------------------------------------*/
void CFG_SHM_ReleaseHostWait (void)
{
	SET_REG_FIELD(SMC_CTL, SMC_CTL_HOSTWAIT, TRUE);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CFG_PrintResetType                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the type of last reset                                             */
/*---------------------------------------------------------------------------------------------------------*/
void CFG_PrintResetType (char *str, BOOLEAN bPrint)
{
	UINT32 val = REG_READ(RESSR);
	/*-----------------------------------------------------------------------------------------------------*/
	/* on first reset RESSR is cleared by the ROM code. Use INTCR2 inverted value instead                  */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( val == 0)
	{
		val = REG_READ(INTCR2);
		val = ~val;
	}

	if ( READ_VAR_FIELD(val, RESSR_CORST) == 1 )
		memcpy(str, "CORST ", 6);

	if ( READ_VAR_FIELD(val, RESSR_PORST) == 1 )
		memcpy(str, "PORST ", 6);

	if ( READ_VAR_FIELD(val, RESSR_WD0RST) == 1 )
		memcpy(str, "WD0   ", 6);

	if ( READ_VAR_FIELD(val, RESSR_WD1RST) == 1 )
		memcpy(str, "WD1   ", 6);

	if ( READ_VAR_FIELD(val, RESSR_WD2RST) == 1 )
		memcpy(str, "WD2   ", 6);

	if ( READ_VAR_FIELD(val, RESSR_SWRST1) == 1 )
		memcpy(str, "SW1   ", 6);

	if ( READ_VAR_FIELD(val, RESSR_SWRST2) == 1 )
		memcpy(str, "SW2   ", 6);

	if ( READ_VAR_FIELD(val, RESSR_SWRST3) == 1 )
		memcpy(str, "SW3   ", 6);

	if ( READ_VAR_FIELD(val, RESSR_SWRST4) == 1 )
		memcpy(str, "SW4   ", 6);


	if (bPrint == TRUE)
		serial_printf(">Last reset was %s\n", str);

	return;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CFG_GetResetNum                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         reset number ( zero for first reset, 1 for secondary reset)                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the type of last reset                                            */
/*---------------------------------------------------------------------------------------------------------*/
UINT CFG_GetResetNum (void)
{
	UINT32 val = REG_READ(RESSR);
	UINT   reset_num = 1;

	/*-----------------------------------------------------------------------------------------------------*/
	/* on first reset RESSR is cleared by the ROM code. Use INTCR2 inverted value instead                  */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( val == 0)
	{
		val = REG_READ(INTCR2);
		val = ~val;
	}

	if ( READ_VAR_FIELD(val, RESSR_PORST) == 1 )
	{
		reset_num = 0;
	}

	return reset_num;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CFG_HeartBeatToggle                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  blink the heartbeat LED iCnt times in infinte loop. Function will not return.          */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* GPIO 0 is used for MP heartbeat. It toggles every 1sec. Can blink different in case of error            */
/*---------------------------------------------------------------------------------------------------------*/
#define HEARTBEAT_COSMOS (0)

void CFG_HeartBeatToggle (int iCnt)
{

	UINT32 iHeartBeat = 0;
	UINT32 volatile time = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Disable WD                                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	TMC_StopWatchDog(TMC_MODULE_0);
	TMC_StopWatchDog(TMC_MODULE_1);
	TMC_StopWatchDog(TMC_MODULE_2);


	/*-----------------------------------------------------------------------------------------------------*/
	/* For testing: toggle GPIO218. / (192 + 26) . Should be every 1ms									   */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(MFSEL1, MFSEL1_IOX1SEL, 0);
	SET_REG_BIT(GPnOE(  GPIO_GET_PORT_NUM(HEARTBEAT_COSMOS)), GPIO_GET_BIT_NUM(HEARTBEAT_COSMOS));
	SET_REG_BIT(GPnDOUT(GPIO_GET_PORT_NUM(HEARTBEAT_COSMOS)), GPIO_GET_BIT_NUM(HEARTBEAT_COSMOS));



	/*-----------------------------------------------------------------------------------------------------*/
	/*	Togle heart beat GPIO																			   */
	/*-----------------------------------------------------------------------------------------------------*/
	while(1)
	{
		time = 0;
		for(iHeartBeat = 0; iHeartBeat < iCnt; iHeartBeat++)
		{
			GPIO_Write( HEARTBEAT_COSMOS,  1 );
			while(time < 1500000) time++;
			time = 0;

			GPIO_Write( HEARTBEAT_COSMOS,  0 );
			while(time < 1500000) time++;
			time = 0;
		}

		while(time < 3000000) time++;
		time = 0;
	}

}



#undef CFG_C
