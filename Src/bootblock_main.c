/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   bootblock_main.c                                                         */
/*            This file contains the main bootblock flow and helper functions */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/
#define MAIN_C


#include "../SWC_HAL/hal.h"
#include "../SWC_HAL/hal_regs.h"

#include "../SWC_HAL/ModuleDrivers/shm/1/shm_regs.h"
#include "../SWC_HAL/ModuleDrivers/fiu/40/fiu_regs.h"

#include <string.h>
#include "cfg.h"
#include "security.h"
#include "mailbox.h"
#include "boot.h"
#include "fuse_wrapper/fuse_wrapper.h"
#include "Apps/serial_printf/serial_printf.h"


extern void   asm_jump_to_address      (UINT32 address);
extern void   disable_highvecs         (void);
extern void   enable_highvecs          (void);

// dcache is not fully validated. Used only for performance testing.

/*---------------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void          ConfigReset               (void);
void          bootblock_main            (void);
UINT32        Check_ROMCode_Status      (UINT32 ROMcodeLRAddress, BOOLEAN doSignatureCheck_l);
void          PrintClocks               (void);

/*---------------------------------------------------------------------------------------------------------*/
/* OTP values                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT8*     oKap  ;
UINT8*     oFsap ;
UINT8*     oKavfp;
UINT8*     oFsvfp;
UINT8*     oVerificationFaultModuleProtection;
UINT8      oCpFustrap[4];
BOOLEAN    doSignatureCheck;
BOOLEAN    returnToRomCode;

DEVICE_T        dev_t;


extern const  BOOTBLOCK_Version_T             bb_version;


extern        UART_DEV_T                      serial_printf_uart_port;

/*----------------------------------------------------------------------------*/
/* Customer specific code                                                     */
/*----------------------------------------------------------------------------*/

void           (*BOOTBLOCK_Print_Vendor) (void);
void           (*BOOTBLOCK_Init_UART_Vendor) (void);
DEFS_STATUS    (*BOOTBLOCK_Init_CLK_Vendor) (void);
void           (*BOOTBLOCK_Init_Before_Image_Check_Vendor) (void);
void           (*BOOTBLOCK_Init_Before_UBOOT_Vendor) (void);
void           (*BOOTBLOCK_Init_GPIO_Vendor) (void);

#ifdef _UNIT_TEST_
// for tests:
#include "bootblock_unit_test.c"
#endif

static void _INLINE_ print_reg(const char * reg, UINT32 old_val, UINT32 new_val)
{
	if (old_val == new_val)
		serial_printf(">%s  = %#010lx\n", reg, old_val);
	else
		serial_printf(">%s  = %#010lx=>%#010lx\n", reg, old_val   , new_val);
}

/*----------------------------------------------------------------------------*/
/* Function:        bootblock_main                                            */
/*                                                                            */
/* Parameters:      none                                                      */
/* Returns:         none                                                      */
/* Side effects:                                                              */
/* Description:                                                               */
/*                  This is the main bootblock flow                           */
/*----------------------------------------------------------------------------*/
void bootblock_main (void)
{
	BOOLEAN         bootPhase;
	UINT32          iCpFupstrap1;
	UINT32          iCpFupstrap_4_5;
	DEFS_STATUS     status          = DEFS_STATUS_OK;

	DEFS_STATUS     status_clk_init = DEFS_STATUS_FAIL;

	BOARD_T         eBoard;
	BOARD_VENDOR_T  eVendor; // Board vendor.
	HOST_IF_T       eHostIf;
	UINT32          apb4Rate;
	UINT32          SDnIRVn;
	UINT32          corstc,wd0rcr,wd1rcr,wd2rcr,swrstc1,swrstc2,swrstc3,swrstc4,
					clken1, clken2, clken3, clksel, clkdiv1, clkdiv2, clkdiv3,
					pllcon0, pllcon1, pllcon2, vsrcr, intcr2, ressr, mfsel4, mfsel3, mfsel2,
					mfsel1, dscnt, smc_ctl, gpdout7, gpoe7, fiu_drd_, fiu_dwr_, fiu_cfg_0,
					fiu_cfg_3, flockr1, rlockr1, fcfg1, fcfg2, mdlr, spswc, uart_lcr2,
					uart_lcr3, ipsrst2 , etsr, scrpad, pwron, fustrap, intcr3, intcr;
	UINT32			rcr = 0;

#ifdef _UNIT_TEST_
	BOOT_HEADER_T * header;
#endif
	UINT32			bootblockVersion = bb_version.BootblockVersion;
	char reset[6];
	MC_INIT_VALUES mc_init_s;

	Tick();

	/*--------------------------------------------------------------------*/
	/* Prepare the mailbox                                                */
	/*--------------------------------------------------------------------*/
	PCIMBX_ResetMailbox();
	PCIMBX_UpdateMailbox(ST_ROM_INIT);

	/*--------------------------------------------------------------------*/
	/* identify board according to flash header                           */
	/*--------------------------------------------------------------------*/
	eBoard  = BOOTBLOCK_GetBoardType();
	eVendor = BOOTBLOCK_GetVendorType();
	eHostIf = BOOTBLOCK_Get_host_if();


	if (eVendor == VENDOR_NUVOTON)
	{
		BOOTBLOCK_Print_Vendor                   =  *BOOTBLOCK_Print_Nuvoton ;
		BOOTBLOCK_Init_UART_Vendor               =  *BOOTBLOCK_Init_UART_Nuvoton;
		BOOTBLOCK_Init_CLK_Vendor                =  *BOOTBLOCK_Init_CLK_Nuvoton;
		BOOTBLOCK_Init_Before_Image_Check_Vendor =  *BOOTBLOCK_Init_Before_Image_Check_Nuvoton;
		BOOTBLOCK_Init_Before_UBOOT_Vendor       =  *BOOTBLOCK_Init_Before_UBOOT_Nuvoton;
		BOOTBLOCK_Init_GPIO_Vendor               =  *BOOTBLOCK_Init_GPIO_Nuvoton;
	}


	else
	{
		eVendor = VENDOR_UNKNOWN;
		BOOTBLOCK_Print_Vendor                   = *BOOTBLOCK_Print_Vendor_error;

		// If vendor number is erroneous, print an error message, but run as Nuvoton:
		BOOTBLOCK_Init_UART_Vendor               =  *BOOTBLOCK_Init_UART_Nuvoton;
		BOOTBLOCK_Init_CLK_Vendor                =  *BOOTBLOCK_Init_CLK_Nuvoton;
		BOOTBLOCK_Init_Before_Image_Check_Vendor =  *BOOTBLOCK_Init_Before_Image_Check_Nuvoton;
		BOOTBLOCK_Init_Before_UBOOT_Vendor       =  *BOOTBLOCK_Init_Before_UBOOT_Nuvoton;
		BOOTBLOCK_Init_GPIO_Vendor               =  *BOOTBLOCK_Init_GPIO_Nuvoton;
	}

	/*--------------------------------------------------------------*/
	/*                     1. CPU Init                              */
	/*--------------------------------------------------------------*/

	corstc = REG_READ(CORSTC);
	wd0rcr = REG_READ(WD0RCR);
	wd1rcr = REG_READ(WD1RCR);
	wd2rcr = REG_READ(WD2RCR);
	swrstc1 = REG_READ(SWRSTC1);
	swrstc2 = REG_READ(SWRSTC2);
	swrstc3 = REG_READ(SWRSTC3);
	swrstc4 = REG_READ(SWRSTC4);
	clken1  = REG_READ(CLKEN1  );
	clken2  = REG_READ(CLKEN2  );
	clken3  = REG_READ(CLKEN3  );
	clksel  = REG_READ(CLKSEL  );
	clkdiv1 = REG_READ(CLKDIV1 );
	clkdiv2 = REG_READ(CLKDIV2 );
	clkdiv3 = REG_READ(CLKDIV3 );
	pllcon0 = REG_READ(PLLCON0 );
	pllcon1 = REG_READ(PLLCON1 );
	pllcon2 = REG_READ(PLLCON2 );
	vsrcr   = REG_READ(VSRCR   );
	intcr2  = REG_READ(INTCR2  );
	ressr   = REG_READ(RESSR   );
	mfsel4  = REG_READ(MFSEL4  );
	mfsel3  = REG_READ(MFSEL3  );
	mfsel2  = REG_READ(MFSEL2  );
	mfsel1  = REG_READ(MFSEL1  );
	dscnt   = REG_READ(DSCNT   );
	smc_ctl = REG_READ(SMC_CTL );
	gpdout7 = REG_READ(GPnDOUT(7) );
	gpoe7   = REG_READ(GPnOE(7));
	fiu_drd_= REG_READ(FIU_DRD_CFG(0));
	fiu_dwr_= REG_READ(FIU_DWR_CFG(0));
	fiu_cfg_0= REG_READ(FIU_CFG(0));
	fiu_cfg_3= REG_READ(FIU_CFG(3) );
	flockr1 = REG_READ(FLOCKR1 );
	rlockr1 = REG_READ(RLOCKR1 );
	fcfg1   = REG_READ(FCFG(0) );
	fcfg2   = REG_READ(FCFG(1) );
	mdlr    = REG_READ(MDLR    );
	spswc   = REG_READ(SPSWC   );
	uart_lcr2= REG_READ(LCR(2) );
	uart_lcr3= REG_READ(LCR(3) );
	ipsrst2 = REG_READ(IPSRST2 );
	etsr    = REG_READ(ETSR    );
	scrpad  = REG_READ(SCRPAD  );
	pwron   = REG_READ(PWRON   );
	fustrap = REG_READ(FUSTRAP );
	intcr3  = REG_READ(INTCR3  );
	intcr   = REG_READ(INTCR   );


	if (CFG_GetResetNum() == 0)
	{
		ConfigReset();
	}

	enable_highvecs();

	/*----------------------------------------------------------------------*/
	/* Read the straps/fuses (this will set a global value)                 */
	/*----------------------------------------------------------------------*/
	STRP_Select();

	/*----------------------------------------------------------------------*/
	/* 5. User Configuration                                                */
	/*----------------------------------------------------------------------*/
	PCIMBX_UpdateMailbox(ST_ROM_USER_CFG);

	/*----------------------------------------------------------------------*/
	/* Configure clocks if it was not configured regardless of SkipInit mode*/
	/* If SkipInit is set it will configure to mode 7                       */
	/*----------------------------------------------------------------------*/
	if ( CHIP_CfgWasDone() == 0)
	{
		/* set clocks status  */
		PCIMBX_UpdateMailbox(ST_ROM_CFG_CLK_1);

		/* configure the clocks according to straps\fustraps */
		CLK_ConfigureClocks();

			/* notify the upper SW layers (such as the Booter) that chip configuration was already performed.*/
		CHIP_MarkCfgDone();
	}


	/*------------------------------------------------------------------------------------------*/
	/* On DRB need to configure the clock according to straps. On EB\SVB - use the ROM table    */
	/*------------------------------------------------------------------------------------------*/
	if (CFG_GetResetNum() == 0)
	{
		status_clk_init = BOOTBLOCK_Init_CLK_Vendor();
	}

	/*-------------------------------------------------------------------*/
	/* Update fuse APBRT, assuming BB changed it somehow                 */
	/*-------------------------------------------------------------------*/
	apb4Rate = CLK_GetAPBFreq(APB4);
	FUSE_Init((apb4Rate/1000000) + 1); /* Set APBRT to APB4 frequency + 1 (in MHz) */



	/*------------------------------------------------------------------*/
	/* Init UART                                                        */
	/*------------------------------------------------------------------*/
	BOOTBLOCK_Init_UART_Vendor();

#ifdef _UNIT_TEST_
	header = (BOOT_HEADER_T*)SPI0CS0_BASE_ADDR;
	BOOTBLOCK_unit_test_1((BOOT_HEADER_T *)header);
#endif

	/*------------------------------------------------------------------*/
	/* Print general bootblock data                                     */
	/*------------------------------------------------------------------*/
	CLK_Delay_MicroSec(100); // let the dust settle after setting the UART.
	serial_printf(KMAG "\n>================================================\n");
	serial_printf("> BootBlock by Nuvoton Technology Corp. Ver ");
	if (bootblockVersion>>24)
	{
		serial_printf(KRED "\n> WARNING: DEBUG VERSION!\n\t" KNRM);
		serial_printf("%x.", (UINT8)(bootblockVersion>>24));
	}
	serial_printf("%x.", (UINT8)(bootblockVersion>>16));
	serial_printf("%x.", (UINT8)(bootblockVersion>>8));
	serial_printf("%x",  (UINT8)(bootblockVersion));
	serial_printf("\n>================================================\n" KNRM);
	serial_printf("%s \n " KNRM , bb_version.VersionDescription );
	serial_printf("\n\n\n" KNRM);
	print_reg("CORSTC       ", corstc   , REG_READ(CORSTC       ));
	print_reg("WD0RCR       ", wd0rcr   , REG_READ(WD0RCR       ));
	print_reg("WD1RCR       ", wd1rcr   , REG_READ(WD1RCR       ));
	print_reg("WD2RCR       ", wd2rcr   , REG_READ(WD2RCR       ));
	print_reg("SWRSTC1      ", swrstc1  , REG_READ(SWRSTC1      ));
	print_reg("SWRSTC2      ", swrstc2  , REG_READ(SWRSTC2      ));
	print_reg("SWRSTC3      ", swrstc3  , REG_READ(SWRSTC3      ));
	print_reg("SWRSTC4      ", swrstc4  , REG_READ(SWRSTC4      ));
	print_reg("CLKEN1       ", clken1   , REG_READ(CLKEN1       ));
	print_reg("CLKEN2       ", clken2   , REG_READ(CLKEN2       ));
	print_reg("CLKEN3       ", clken3   , REG_READ(CLKEN3       ));
	print_reg("CLKSEL       ", clksel   , REG_READ(CLKSEL       ));
	print_reg("CLKDIV1      ", clkdiv1  , REG_READ(CLKDIV1      ));
	print_reg("CLKDIV2      ", clkdiv2  , REG_READ(CLKDIV2      ));
	print_reg("CLKDIV3      ", clkdiv3  , REG_READ(CLKDIV3      ));
	print_reg("PLLCON0      ", pllcon0  , REG_READ(PLLCON0      ));
	print_reg("PLLCON1      ", pllcon1  , REG_READ(PLLCON1      ));
	print_reg("PLLCON2      ", pllcon2  , REG_READ(PLLCON2      ));
	print_reg("VSRCR        ", vsrcr    , REG_READ(VSRCR        ));
	print_reg("INTCR2       ", intcr2   , REG_READ(INTCR2       ));
	print_reg("RESSR        ", ressr    , REG_READ(RESSR        ));
	print_reg("MFSEL1       ", mfsel1   , REG_READ(MFSEL1       ));
	print_reg("MFSEL2       ", mfsel2   , REG_READ(MFSEL2       ));
	print_reg("MFSEL3       ", mfsel3   , REG_READ(MFSEL3       ));
	print_reg("MFSEL4       ", mfsel4   , REG_READ(MFSEL4       ));
	print_reg("DSCNT        ", dscnt    , REG_READ(DSCNT        ));
	print_reg("SMC_CTL      ", smc_ctl  , REG_READ(SMC_CTL      ));
	print_reg("GPnDOUT(7)   ", gpdout7  , REG_READ(GPnDOUT(7)   ));
	print_reg("GPnOE(7)     ", gpoe7    , REG_READ(GPnOE(7)     ));
	print_reg("FIU_DRD_CFG  ", fiu_drd_ , REG_READ(FIU_DRD_CFG(0)));
	print_reg("FIU_DWR_CFG  ", fiu_dwr_ , REG_READ(FIU_DWR_CFG(0)));
	print_reg("FIU_CFG(0)   ", fiu_cfg_0, REG_READ(FIU_CFG(0)   ));
	print_reg("FIU_CFG(3)   ", fiu_cfg_3, REG_READ(FIU_CFG(3)   ));
	print_reg("FLOCKR1      ", flockr1  , REG_READ(FLOCKR1      ));
	print_reg("RLOCKR1      ", rlockr1  , REG_READ(RLOCKR1      ));
	print_reg("FCFG1        ", fcfg1    , REG_READ(FCFG(0)      ));
	print_reg("FCFG2        ", fcfg2    , REG_READ(FCFG(1)      ));
	print_reg("MDLR         ", mdlr     , REG_READ(MDLR         ));
	print_reg("SPSWC        ", spswc    , REG_READ(SPSWC        ));
	print_reg("LCR(2)       ", uart_lcr2, REG_READ(LCR(2)       ));
	print_reg("LCR(3)       ", uart_lcr3, REG_READ(LCR(3)       ));
	print_reg("IPSRST2      ", ipsrst2  , REG_READ(IPSRST2      ));
	print_reg("PWRON        ", pwron    , REG_READ(PWRON        ));
	print_reg("ETSR         ", etsr     , REG_READ(ETSR         ));
	print_reg("FUSTRAP      ", fustrap  , REG_READ(FUSTRAP      ));
	print_reg("SCRPAD       ", scrpad   , REG_READ(SCRPAD       ));

	do {
		UINT8 fv[8] = {0,0,0,0,0,0,0,0};
			serial_printf("\n>OTP values:\n");

		FUSE_WRPR_get(oFINAL_TEST_SIGNATURE_PROPERTY                , fv);  serial_printf(">FINAL_TEST_SIGNATURE (Product OTP)   =  %#010lx %#010lx\n" , *(UINT32*)fv, *(UINT32*)(fv+4));
		FUSE_WRPR_get(oFUSTRAP_PROPERTY                             , fv);  serial_printf(">FUSTRAP                              =  %#010lx\n" , *(UINT32*)fv);
		FUSE_WRPR_get_CP_Fustrap(oCP_FUSTRAP_PROPERTY               , fv);  serial_printf(">CP_FUSTRAP                           =  %#010lx\n" , *(UINT32*)fv);
		FUSE_WRPR_get(oDAC_CALIB_PROPERTY                           , fv);  serial_printf(">DAC_CALIB                            =  %#010lx\n" , *(UINT32*)fv);
		FUSE_WRPR_get(oADC_CALIB_PROPERTY                           , fv);  serial_printf(">ADC_CALIB                            =  %#010lx\n" , *(UINT32*)fv);
		FUSE_WRPR_get(oDIE_LOCATION_PROPERTY                        , fv);  serial_printf(">DIE_LOCATION                         =  %#010lx\n" , *(UINT32*)fv);

		FUSE_WRPR_get(oDERIVATIVE_PROPERTY                          , fv);  serial_printf(">DERIVATIVE                           =  %#010lx\n" , *(UINT32*)fv);

		// Check derivative value : For NPCM730 should be 00300395
		if (  (*(UINT32*)fv & 0x00300395) == 0x00300395)
		{
			serial_printf(KGRN "\n>Device: Poleg BMC NPCM730\n" KNRM);
			dev_t           = DEVICE_NPCM730;
		}
		else
		{
			serial_printf(KGRN "\n>Device: Poleg BMC NPCM750\n" KNRM);
			dev_t           = DEVICE_NPCM750;
		}

	} while (0);



	if( dev_t == DEVICE_NPCM730)
	{
		UINT32 intcr_tmp = intcr;
		UINT32 intcr2_tmp = intcr2;
		UINT32 intcr3_tmp = intcr3;
		//Boot block performs all of the following anti display act:

		SET_VAR_FIELD(intcr_tmp, INTCR_VGAIOEN, 0);
		SET_VAR_FIELD(intcr_tmp, INTCR_DACOFF, 1);
		SET_VAR_FIELD(intcr_tmp, INTCR_LDDRB, 1);

		// Enable PCI RC:
		SET_VAR_FIELD(intcr2_tmp, INTCR2_PCIPHYPD, 0);

		SET_VAR_FIELD(intcr3_tmp, INTCR3_GFXACCDIS, 1);
		SET_VAR_FIELD(intcr3_tmp, INTCR3_FIU_FIX, 1);

		REG_WRITE(INTCR, intcr_tmp);
		REG_WRITE(INTCR2, intcr2_tmp);
		REG_WRITE(INTCR3, intcr3_tmp);

		REG_WRITE(DACTEST, 0x003FFFFE);
	}
	else
	{
		SET_REG_FIELD(INTCR3, INTCR3_FIU_FIX, 1);
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Fix host stuck issue                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(INTCR3, INTCR3_GFXRDEN, 1);
	SET_REG_FIELD(INTCR3, INTCR3_GFXRSTDLY, 0x7);
	serial_printf("\n>set INTCR3_GFXRDEN and INTCR3_GFXRSTDLY\n");


	print_reg("INTCR        ", intcr    , REG_READ(INTCR       ));
	print_reg("INTCR2       ", intcr2   , REG_READ(INTCR2       ));
	print_reg("INTCR3       ", intcr3   , REG_READ(INTCR3       ));


	CLK_Delay_MicroSec(10);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Print board type and vendor                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	if (eBoard == BOARD_EB)
	{
		serial_printf(KGRN ">EB\n");
	}
	else if (eBoard == BOARD_SVB)
	{
		serial_printf(KGRN ">SVB\n");
	}
	else if (eBoard == BOARD_RUN_BMC)
	{
		serial_printf(KGRN ">RUN_BMC\n");
	}


	else
	{
		serial_printf("\n\n>WARNING: board type is unknown. select board on flash header (programmingScripts/inputs/BootBlockAndHeader.xml (board,vendor)\n\n");
		eBoard = BOARD_SVB;
	}

	BOOTBLOCK_Print_Vendor();

	CLK_ConfigureADCClock(25000000);
	PrintClocks();

	if (status_clk_init == DEFS_STATUS_OK)
	{
		serial_printf(KBLU "\n> PLLs were reconfigured by BB\n" KGRN);
	}
	else
	{
		serial_printf(KBLU "\n> PLLs were not reconfigured by BB\n" KGRN);

		if(eBoard == BOARD_RUN_BMC)
			if (status_clk_init == DEFS_STATUS_INVALID_DATA_FIELD)
				serial_printf(KRED "\n>ERROR: if cpu freq != mc freq can't set TOCK to 125MHz. \nNeed on unused PLL for this\n" KNRM);
	}



	// print last reset type:
	CFG_PrintResetType(reset , TRUE);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable misc. GPIOs settings.                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	BOOTBLOCK_Init_GPIO_Vendor();

	/*-----------------------------------------------------------------------------------------------------*/
	/* SD Fix internal straps reset value                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	SDnIRVn = REG_READ(SD1IRV2);
	SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_sdr50support, 0);   // disable sdr50 support
	SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_baseclkfreq, 0x32); // Base Clock Frequency is 50MHz (0x32)
	REG_WRITE(SD1IRV2, SDnIRVn);

	SDnIRVn = REG_READ(SD1IRV1);
	SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlyena,   1);    // Output Tap Delay Enable
	SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlysel, 0xF);    // Output Tap Delay: 15 = ~3ns
	REG_WRITE(SD1IRV1, SDnIRVn);

	/*-----------------------------------------------------------------------------------------------------*/
	/* eMMC Fix internal straps reset value                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	SDnIRVn = REG_READ(SD2IRV2);
	SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_sdr50support, 0);   // disable sdr50 support
	SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_baseclkfreq, 0x32); // Base Clock Frequency is 50MHz (0x32)
	REG_WRITE(SD2IRV2, SDnIRVn);

	SDnIRVn = REG_READ(SD2IRV1);
	SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlyena,   1);    // Output Tap Delay Enable
	SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlysel, 0xF);    // Output Tap Delay: 15 = ~3ns
	REG_WRITE(SD2IRV1, SDnIRVn);

	/*-----------------------------------------------------------------------------------------------------*/
	/* SD and eMMC - set clock to 50Mhz                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_ConfigureSDClock(SD_SD_MODULE);
	CLK_ConfigureSDClock(SD_MMC_MODULE);


	/*--------------------------------------------------------------------*/
	/* HOST IF configuration                                              */
	/*--------------------------------------------------------------------*/
	if (eHostIf == HOST_IF_ESPI)
	{
		if (CFG_GetResetNum() == 0)
		{
			serial_printf(KYEL "\n>HOST IF: ESPI");
			GPIO_Init(162,  GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE); // SERIRQ_N
			CHIP_MuxESPI(24); // 24mA
		 	ESPI_ConfigAutoHandshake(0x0001111F);
			ESPI_Config(ESPI_IO_MODE_SINGLE_DUAL_QUAD, ESPI_MAX_33_MHz, ESPI_RST_OUT_LOW);
		 	SHM_ReleaseHostWait();
		 	serial_printf(KNRM "\n>Host ESPI Released");
	 	}
	}
	else if (eHostIf == HOST_IF_LPC)
	{
		serial_printf(KYEL "\n>HOST IF: LPC");
		GPIO_Init(168,  GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE); // ALERT_N
		CHIP_MuxLPC(12); // 12mA
		CFG_SHM_ReleaseHostWait();
		serial_printf(KNRM "\n>Host LPC Released");
	}
	else if (eHostIf == HOST_IF_GPIO)
	{
		serial_printf(KYEL "\n>HOST IF: None" KNRM);
		GPIO_Init(163,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(161,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(164,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(165,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(166,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(95,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(168,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(162,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(170,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(190,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);
		GPIO_Init(169,	GPIO_DIR_INPUT, GPIO_PULL_NONE, GPIO_OTYPE_OPEN_DRAIN, FALSE);

		// Inside these functions configure FLOCKR1.20, MFSEL1.22, MFSEL3.0, MFSEL3.16, MFSEL4.8 and MFSEL1.26, MFSEL1.31  to 0.
	}

	// else do nothing and continue with the ROM code settings (Same as before version 10.9.4 - backward compatible
	else
	{
		/*-----------------------------------------------------------------------------------------------------*/
		/* Release Host Wait (LPC only)                                                                        */
		/*-----------------------------------------------------------------------------------------------------*/
		SET_REG_FIELD(MFSEL4, MFSEL4_ESPISEL, 0);
		SET_REG_FIELD(INTCR2, INTCR2_DISABLE_EPSI_AUTO_INIT, 1);
		CFG_SHM_ReleaseHostWait();
		serial_printf(KNRM "\n>Host LPC Released (ROM setting)");
	}


	/*----------------------------------------------------------------------------------------------------*/
	/* in secondary reset: to prevent SHM and Bios post code driver contension through shared             */
	/* interrupt need to disable the interrupt                                                            */
	/*----------------------------------------------------------------------------------------------------*/
	if (CFG_GetResetNum() > 0)
	{
		if (READ_REG_FIELD(RESSR, RESSR_CORST) == 1)
			 rcr = REG_READ(CORSTC);
		else if (READ_REG_FIELD(RESSR, RESSR_WD0RST) == 1)
			 rcr = REG_READ(WD0RCR);
		else if (READ_REG_FIELD(RESSR, RESSR_WD1RST) == 1)
			 rcr = REG_READ(WD1RCR);
		else if (READ_REG_FIELD(RESSR, RESSR_WD2RST) == 1)
			 rcr = REG_READ(WD2RCR);
		else if (READ_REG_FIELD(RESSR, RESSR_SWRST1) == 1)
			 rcr = REG_READ(SWRSTC1);
		else if (READ_REG_FIELD(RESSR, RESSR_SWRST2) == 1)
			 rcr = REG_READ(SWRSTC2);
		else if (READ_REG_FIELD(RESSR, RESSR_SWRST3) == 1)
			 rcr = REG_READ(SWRSTC3);
		else if (READ_REG_FIELD(RESSR, RESSR_SWRST4) == 1)
			 rcr = REG_READ(SWRSTC4);


		if(READ_VAR_FIELD(rcr, WD0RCR_SHMKCS) == 0)
		{
			serial_printf("\n>Disable KCS and BPC ints\n");

			// disable KCS interrupt:
			REG_WRITE(KCS1IE, 0);
			REG_WRITE(KCS2IE, 0);
			REG_WRITE(KCS3IE, 0);

			// disable post code interrupt:
			REG_WRITE(BPCFEN, 0);
			REG_WRITE(BPCFMSTAT, 0xFF); // remove previous status

			// disable KBCI interrupts:
			REG_WRITE(HICTRL, 0x00);

			// Disable IBFIE:
			SET_REG_FIELD(KCS1CTL , KCSnCTL_IBFIE, 0);
			SET_REG_FIELD(KCS2CTL , KCSnCTL_IBFIE, 0);
			SET_REG_FIELD(KCS3CTL , KCSnCTL_IBFIE, 0);



		}



	}


	/*-------------------------------------------------------------------------------------------------*/
	/* Memory Controller Init                                                                          */
	/*-------------------------------------------------------------------------------------------------*/
	BOOTBLOCK_Get_MC_Init_val(&mc_init_s);
	status = MC_ConfigureDDR(BOOTBLOCK_Get_MC_config(), &mc_init_s);

	/*-------------------------------------------------------------------------------------------------*/
	/* If secondary reset MC init skipped. And this is OK.                                             */
	/*-------------------------------------------------------------------------------------------------*/
	if ( status == DEFS_STATUS_SYSTEM_NOT_INITIALIZED)
	{
		serial_printf(KBLU  "\n>Skip DDR init.\n" KNRM );
	}
	else if ( status != DEFS_STATUS_OK)
	{
	// DDR init fail  (BMC can load code to RAM2 or XIP from flash, but the SDRAM is not usable    )
		serial_printf(KRED  "\n\n>ERROR: DDR INIT FAIL.\n\n" KNRM );

		goto return_to_rom;
	}
	else
	{
		serial_printf(KMAG  "\n>DDR init pass\n" KNRM );
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/*  DDR init pass: check SDRAM size and copy vectors to it                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	if((status == DEFS_STATUS_OK) || (status == DEFS_STATUS_SYSTEM_NOT_INITIALIZED))
	{
		SET_REG_FIELD(FLOCKR1, FLOCKR1_RAMV, 1);

	// Copy Interrupt Vector to DDR address 0
		// only for DS-5 compiler
#if (defined __ARMCC_VERSION) && (__ARMCC_VERSION >= 5040049)
#pragma push
#pragma diag_suppress 2748
		memcpy((void*)SDRAM_BASE_ADDR, (void*)ROM_BASE_ADDR, 0x100);
#pragma pop
#else
		memcpy((void*)SDRAM_BASE_ADDR, (void*)ROM_BASE_ADDR, 0x100);
#endif

		disable_highvecs();
		SET_REG_FIELD(FLOCKR1, FLOCKR1_RAMV, 0);

	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* DAC init                                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	{
		UINT32 DACLVLR_var;
		if (DEFS_STATUS_OK == FUSE_WRPR_get(oDAC_CALIB_PROPERTY, (UINT8*)&DACLVLR_var))
		{
			if (0 != DACLVLR_var)
			{
				REG_WRITE(DACLVLR, DACLVLR_var);
			}
		}
		// Turn off automatic DAC_SNS after VSYNC
		SET_REG_FIELD(INTCR, INTCR_DAC_SNS, 1);
	}

//#define _TEST_CP_
#ifdef _TEST_CP_

	serial_printf(">CP: release from reset\n");

	/*-----------------------------------------------------------------------------------------------------*/
	/* CP release from reset:                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	/* boot block for CP testing: release CP from reset and wait                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(CP2BST, 0xFFFFFFFF);

	SET_REG_FIELD(IPSRST2, IPSRST2_CP, 0);

	REG_WRITE(CP2BST, 0xFFFFFFFF);

	SET_REG_FIELD(IPSRST2, IPSRST2_CP, 1);

	REG_WRITE(CP2BST, 0xFFFFFFFF);

#endif

	BOOTBLOCK_Init_Before_Image_Check_Vendor();



	/*-----------------------------------------------------------------------------------------------------*/
	/*                                                                                                     */
	/*                         BASIC BOOT FLOW (None secure boot )                                         */
	/*                                                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	if (doSignatureCheck == FALSE)
	{
		serial_printf(">Basic mode\n");
		// if although we are in security mode the ROM code asked us no to do signature check
		// it is because it moved from security to basic mode (and halt off is disabled)
		if (STRP_SecureBoot_On () == TRUE)
		{
			bootPhase = 1;
		}
		else
		{
			bootPhase = 0;
		}

		PCIMBX_UpdateMailbox((bootPhase == 0 ) ? ST_ROM_SPI_BOOT_BASIC: ST_ROM_HALT_BYPASS);

		/*-------------------------------------------------------------------------------------------------*/
		/* 10 Select first image to check                                                                  */
		/* 11. Check image header                                                                          */
		/* 12. Last boot block                                                                             */
		/* 13. Select next image to check                                                                  */
		/*-------------------------------------------------------------------------------------------------*/
		BOOTBLOCK_FindSpiFlashImage(FALSE);   /* load image without signature check */
		// if we got here it means the basic boot failed.

	}


	PCIMBX_Print();

	serial_printf(KRED "\n\n>Failed to load UBOOT image\n" KNRM );

	/*-----------------------------------------------------------------------------------------------------*/
	/* If it was Fast Boot don't return                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (returnToRomCode == FALSE)
	{

		/*-----------------------------------------------------------------------------------------------------*/
		/* Disable WD                                                                                          */
		/*-----------------------------------------------------------------------------------------------------*/
		TMC_StopWatchDog(TMC_MODULE_0);

		Tock();

		serial_printf(KRED "\n>Halt and catch fire.\n" KNRM);

		while(1);
	}


return_to_rom:

	// restore high vectors and reset the memory controller
	enable_highvecs();

	// reset the UART (because BB configured the UART by itself, it should clean what it did.
	// reset uart only on PORST
	if (CFG_GetResetNum() == 0)
	{
		CLK_ResetUART();
	}

	// restore INTCR2.WDC which is the rStartImage so ROM code will consider that
	// the current image booBlock image didn't pass authentication
	SET_REG_FIELD(INTCR2, INTCR2_WDC, !READ_REG_FIELD(INTCR2, INTCR2_WDC));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Disable WD                                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	TMC_StopWatchDog(TMC_MODULE_0);

	serial_printf(KRED ">Return to ROM\n" KNRM );

	// Here we return to bootblock_start.s and from there back to the ROM code
	return;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Check_ROMCode_Status                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  set values for doSignatureCheck and returnToRomCode according to ROM status\LR address */
/*                  function is called from the start routine                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 Check_ROMCode_Status (UINT32 ROMcodeLRAddress, BOOLEAN doSignatureCheck_l)
{
	const UINT32 ROM_CODE_Z2_STATUS_ON_RAM = 0xFFFD0100;

	/*-----------------------------------------------------------------------------------------------------*/
	/* On A1 use the copy on RAM of the status to decide whether or not BB is on secure mode               */
	/*-----------------------------------------------------------------------------------------------------*/
	ROM_STATUS_MSG *ROM_status_ram = (ROM_STATUS_MSG *)ROM_CODE_Z2_STATUS_ON_RAM;

	// According to ROM_status_ram->status we can decide if we need to do security check
	if ((ROM_status_ram->status >= ST_ROM_USE_KEY0_IMAGE0) && (ROM_status_ram->status <= ST_ROM_USE_KEY2_IMAGE2))
	{
	    doSignatureCheck = 1;
	    returnToRomCode = TRUE;
	}
	// if not security don't return to ROM code
	else if ((ROM_status_ram->status >= ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET0) && (ROM_status_ram->status <= ST_ROM_BASIC_USE_IMAGE_SPI0_CS1_OFFSET0))
	{
	    doSignatureCheck = 0;
	    returnToRomCode = FALSE;
	}
	return ROMcodeLRAddress;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ConfigReset                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Config handling of future reset                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void ConfigReset (void)
{
	UINT32 var = 0;
	/*-----------------------------------------------------------------------------------------------------*/
	/* CORSTC = 0xFFFFFFFD                                                                                 */
	/* For Z2 we will need to change it to other value 9but this is also true for other reset registers.   */
	/*-----------------------------------------------------------------------------------------------------*/
	var =
		BUILD_FIELD_VAL(WD0RCR_LPCESPI , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_PCIE    , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_SHMKCS  , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_PWM     , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_SPER    , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_SPI     , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_SIOX2   , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_SIOX1   , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM7  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM6  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM5  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM4  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM3  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM2  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM1  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_GPIOM0  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_TIMER   , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_PCIMBX  , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_AHB2PCI , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_SD      , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_MMC     , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_DMA     , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_USBHST  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_USBDEV  , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_ETH     , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_CLKS    , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_MC      , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_RV      , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_SEC     , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_CP      , 1  )   |
		BUILD_FIELD_VAL(WD0RCR_A9DBG   , 0  )   |
		BUILD_FIELD_VAL(WD0RCR_CA9C    , 1  );

	// 0x4FFF9F9D

	REG_WRITE(CORSTC, var);

	REG_WRITE(WD0RCR, var);
	REG_WRITE(WD1RCR, var);
	REG_WRITE(WD2RCR, var);

	REG_WRITE(SWRSTC1, var);
	REG_WRITE(SWRSTC2, var);
	REG_WRITE(SWRSTC3, var);
	REG_WRITE(SWRSTC4, var);

	return;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PrintClocks                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                 Print clocks                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void PrintClocks (void)
{

	serial_printf( KGRN "\n> CPU CLK is %d Hz   (header val is %d Hz )\n",
	CLK_GetCPUFreq(),BOOTBLOCK_Get_CPU_freq() );
	serial_printf( "> MC  CLK is %d Hz   (header val is %d Hz )\n\n" ,
	CLK_GetMCFreq(), BOOTBLOCK_Get_MC_freq());

	serial_printf(">PLL0 = %dMHz \n",CLK_GetPll0Freq()/1000000);
	serial_printf(">PLL1 = %dMHz \n",CLK_GetPll1Freq()/1000000);
	serial_printf(">PLL2 = %dMHz \n",CLK_GetPll2Freq()/1000000);
	serial_printf(">APB1 = %dMHz  \n",CLK_GetAPBFreq(APB1)/1000000);
	serial_printf(">APB2 = %dMHz \n",CLK_GetAPBFreq(APB2)/1000000);
	serial_printf(">APB3 = %dMHz \n",CLK_GetAPBFreq(APB3)/1000000);
	serial_printf(">APB4 = %dMHz \n",CLK_GetAPBFreq(APB4)/1000000);
	serial_printf(">APB5 = %dMHz \n",CLK_GetAPBFreq(APB5)/1000000);
	serial_printf(">SPI0 = %dMHz \n",CLK_GetAPBFreq(SPI0)/1000000);
	serial_printf(">SPI3 = %dMHz \n",CLK_GetAPBFreq(SPI3)/1000000);
	serial_printf(">ADC  = %dMHz \n",CLK_GetADCClock()/1000000);
	serial_printf(">CP   = %dMHz \n",CLK_GetCPFreq()/1000000);
	serial_printf(">TMC  = %dMHz \n",CLK_GetTimerFreq()/1000000);
	serial_printf(">GFX  = %dMHz \n",CLK_GetGFXClock()/1000000);
	serial_printf(">PCI  = %dMHz \n\n" KNRM,CLK_GetPCIClock()/1000000);

	return;
}


#undef MAIN_C
