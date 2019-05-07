/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   mc_drv.c                                                                 */
/*        This file contains configuration routines for Memory Controller     */
/* Project:                                                                   */
/*        SWC HAL                                                             */
/*----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/*                         INCLUDES                        */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_DRV()

#include "mc_drv.h"
#include "mc_regs.h"
#ifndef NO_LIBC
#include <string.h>
#endif

#define ECC_GLITCH_BYPASS

#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && (defined(NPCM750) || defined(NPCM850)))
#include "cfg.h"
#include "boot.h"
#if (defined __ARMCC_VERSION)
	#define BOOTBLOCK_IMAGE_START	Image$$BOOTBLOCK$$Base
	#define BOOTBLOCK_IMAGE_END		Image$$BOOTBLOCK$$Limit
#else // defined (__GNUC__)
	#define BOOTBLOCK_IMAGE_START	__bootblock_start
	#define BOOTBLOCK_IMAGE_END		__bootblock_end
#endif
#endif


#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && (defined(NPCM750) || defined(NPCM850)))
#include "../../../../Src/Apps/serial_printf/serial_printf.h"
#else
#define           serial_printf(x,...)         (void)0
#define           serial_printf(x,...)         (void)0
#endif

#define           LANE_MASK_MAX     7
#define           NUM_OF_LANES_MAX      3

static int        LANE_MASK         = 3;  // 3 if lane_num == 2;  7 if lane_num == 3
static int        NUM_OF_LANES      = 2;
static int        dlls_trim_adrctl_clk  = 26;

UINT8             g_mc_config = 0;





// Note:
// PHY is set to VREF_STARTPOINT value. This is a tyical value for DDR_DRV_DQ_48R with PHY_ODT_DQ_120R. VREF training will later-on look for the optimal value.
// DDR device is set to VREF_STARTPOINT value. This is tyical value for PHY_DRV_DQ_48R with DDR_ODT_DQ_120R. VREF training will look for the optimal value.
// For a different DRV and ODT values, VREF_STARTPOINT must be upadte accordantly.
// In case VREF training is skiped, this value will be use for the PHY VREF and user should issue MR6 write to set DDR device VREF value.
// In Z11, PHY VREF bit 0 is tie to VREF select therefore must be fixed to 1
#define           VREF_STARTPOINT       (0x7|(1)) // @ range 1 it value is 65% of DRAM Voltage.



static void             MC_Sleep_l                (UINT32 Cycles);
static DEFS_STATUS          WaitForSclDone_l          (void);
static DEFS_STATUS                  MC_PHY_Init_l             (void);
static DEFS_STATUS          phy_init_l                (void);
static DEFS_STATUS              MC_Init_test_l            (void)  ;
static DEFS_STATUS          phy_read_leveling_l       (BOOLEAN bPrint);                    // Step 7 to 8 in Uniquify doc
static DEFS_STATUS          phy_write_leveling_l      (void);
static DEFS_STATUS          phy_bit_leveling_l    (BOOLEAN EnableLog) ;
static DEFS_STATUS          vref_training_l       (void);
static DEFS_STATUS          Sweep_DQs_ECC_Trim_l      (BOOLEAN bInput);
static DEFS_STATUS          Sweep_DQs_Trim_l      (BOOLEAN bInput);
static DEFS_STATUS          Sweep_Trim_Input_DQS_l    (int *pEyeCenter, int *pEyeSize, BOOLEAN bECC, BOOLEAN bSilent);
static DEFS_STATUS          Sweep_Input_DSQ_Enhanced_Training_l(BOOLEAN bECC);
static void                 FindMidBiggestEyeBitwise_l(const UINT16 *SweepDataBuff, const int BuffSize,  int *pEyeCenter, int *pEyeSize, int bits);
static INT32                MC_CalcBestTrim_l         (INT32 dlls_trim_l);
static void                         phy_update_dlls_trim_adrctl_clk_l(void);
static DEFS_STATUS          phy_leveling_l            (UINT16 mode);
static DEFS_STATUS                  MC_ConfigureDDR_l         (void);
static void             MC_PrintLeveling          (BOOLEAN bIn, BOOLEAN bOut);
static DEFS_STATUS          WriteModeReg              (UINT8 Index, UINT32 Data);
static DEFS_STATUS          MPR_Readout_RawData       (UINT8 Page, /*OUT*/ UINT16 *MPR_RawData);
static DEFS_STATUS          MPR_Page_Readout          (UINT8 Page, /*OUT*/ UINT8 *MPR);
static void                 SDRAM_PrintRegs           (void);
static UINT16                       GetECCSyndrom             (UINT32 readVal);




#define REG_SET_FIELD(address,width,position,value) IOW32(address, (IOR32(address) & (~((((UINT32)1<<(width))-1)<<(position))))  | ((value)<<(position)))
#define REG_READ_FIELD(address,width,position)     ((IOR32(address)>>(position)) & ((((UINT32)1)<<(width))-1))


#define BINARY_PRINT16(x)   READ_VAR_BIT(x,15), READ_VAR_BIT(x,14), READ_VAR_BIT(x,13), READ_VAR_BIT(x,12), \
	READ_VAR_BIT(x,11), READ_VAR_BIT(x,10), READ_VAR_BIT(x,9 ), READ_VAR_BIT(x,8 ), \
	READ_VAR_BIT(x, 7), READ_VAR_BIT(x, 6), READ_VAR_BIT(x, 5), READ_VAR_BIT(x, 4), \
	READ_VAR_BIT(x, 3), READ_VAR_BIT(x, 2), READ_VAR_BIT(x, 1), READ_VAR_BIT(x, 0)
#define BINARY_PRINT4(x)    READ_VAR_BIT(x, 3), READ_VAR_BIT(x, 2), READ_VAR_BIT(x, 1), READ_VAR_BIT(x, 0)
#define BINARY_PRINT7(x)    READ_VAR_BIT(x, 6), READ_VAR_BIT(x, 5), READ_VAR_BIT(x, 4), \
				READ_VAR_BIT(x, 3), READ_VAR_BIT(x, 2), READ_VAR_BIT(x, 1), READ_VAR_BIT(x, 0)



/*---------------------------------------------------------------------------------------------------------*/
/* Trim values are 2's complement, on 7 bits instead of 8. These are the conversion macros:        */
/*---------------------------------------------------------------------------------------------------------*/
#define  IP_DQ_REG_TO_VALUE(x)   ((int)((((x)&0x40)==0)?(-((x)&0x3F)):((x)&0x3F)))
#define  IP_DQ_VALUE_TO_REG(x)   (((UINT32)(((x)>=0)?(0x40|(x)):(-(x)))&0x7F))


const UINT32 Denali_Ctl_Initialization[] = {
	DENALI_CTL_00_DATA,
	DENALI_CTL_01_DATA,
	DENALI_CTL_02_DATA,
	DENALI_CTL_03_DATA,
	DENALI_CTL_04_DATA,
	DENALI_CTL_05_DATA,
	DENALI_CTL_06_DATA,
	DENALI_CTL_07_DATA,
	DENALI_CTL_08_DATA,
	DENALI_CTL_09_DATA,
	DENALI_CTL_10_DATA,
	DENALI_CTL_11_DATA,
	DENALI_CTL_12_DATA,
	DENALI_CTL_13_DATA,
	DENALI_CTL_14_DATA,
	DENALI_CTL_15_DATA,
	DENALI_CTL_16_DATA,
	DENALI_CTL_17_DATA,
	DENALI_CTL_18_DATA,
	DENALI_CTL_19_DATA,
	DENALI_CTL_20_DATA,
	DENALI_CTL_21_DATA,
	DENALI_CTL_22_DATA,
	DENALI_CTL_23_DATA,
	DENALI_CTL_24_DATA,
	DENALI_CTL_25_DATA,
	DENALI_CTL_26_DATA,
	DENALI_CTL_27_DATA,
	DENALI_CTL_28_DATA,
	DENALI_CTL_29_DATA,
	DENALI_CTL_30_DATA,
	DENALI_CTL_31_DATA,
	DENALI_CTL_32_DATA,
	DENALI_CTL_33_DATA,
	DENALI_CTL_34_DATA,
	DENALI_CTL_35_DATA,
	DENALI_CTL_36_DATA,
	DENALI_CTL_37_DATA,
	DENALI_CTL_38_DATA,
	DENALI_CTL_39_DATA,
	DENALI_CTL_40_DATA,
	DENALI_CTL_41_DATA,
	DENALI_CTL_42_DATA,
	DENALI_CTL_43_DATA,
	DENALI_CTL_44_DATA,
	DENALI_CTL_45_DATA,
	DENALI_CTL_46_DATA,
	DENALI_CTL_47_DATA,
	DENALI_CTL_48_DATA,
	DENALI_CTL_49_DATA,
	DENALI_CTL_50_DATA_DEFAULT,
	DENALI_CTL_51_DATA,
	DENALI_CTL_52_DATA,
	DENALI_CTL_53_DATA,
	DENALI_CTL_54_DATA,
	DENALI_CTL_55_DATA,
	DENALI_CTL_56_DATA,
	DENALI_CTL_57_DATA,
	DENALI_CTL_58_DATA,
	DENALI_CTL_59_DATA,
	DENALI_CTL_60_DATA,
	DENALI_CTL_61_DATA,
	DENALI_CTL_62_DATA,
	DENALI_CTL_63_DATA,
	DENALI_CTL_64_DATA,
	DENALI_CTL_65_DATA,
	DENALI_CTL_66_DATA,
	DENALI_CTL_67_DATA,
	DENALI_CTL_68_DATA,
	DENALI_CTL_69_DATA,
	DENALI_CTL_70_DATA,
	DENALI_CTL_71_DATA,
	DENALI_CTL_72_DATA,
	DENALI_CTL_73_DATA,
	DENALI_CTL_74_DATA,
	DENALI_CTL_75_DATA,
	DENALI_CTL_76_DATA,
	DENALI_CTL_77_DATA,
	DENALI_CTL_78_DATA,
	DENALI_CTL_79_DATA,
	DENALI_CTL_80_DATA,
	DENALI_CTL_81_DATA,
	DENALI_CTL_82_DATA,
	DENALI_CTL_83_DATA,
	DENALI_CTL_84_DATA,
	DENALI_CTL_85_DATA,
	DENALI_CTL_86_DATA,
	DENALI_CTL_87_DATA,
	DENALI_CTL_88_DATA,
	DENALI_CTL_89_DATA,
	DENALI_CTL_90_DATA,
	DENALI_CTL_91_DATA,
	DENALI_CTL_92_DATA,
	DENALI_CTL_93_DATA,
	DENALI_CTL_94_DATA,
	DENALI_CTL_95_DATA,
	DENALI_CTL_96_DATA,
	DENALI_CTL_97_DATA,
	DENALI_CTL_98_DATA,
	DENALI_CTL_99_DATA,
	DENALI_CTL_100_DATA,
	DENALI_CTL_101_DATA,
	DENALI_CTL_102_DATA,
	DENALI_CTL_103_DATA,
	DENALI_CTL_104_DATA,
	DENALI_CTL_105_DATA,
	DENALI_CTL_106_DATA,
	DENALI_CTL_107_DATA_1GIB,
	DENALI_CTL_108_DATA,
	DENALI_CTL_109_DATA,
	DENALI_CTL_110_DATA,
	DENALI_CTL_111_DATA,
	DENALI_CTL_112_DATA,
	DENALI_CTL_113_DATA,
	DENALI_CTL_114_DATA,
	DENALI_CTL_115_DATA,
	DENALI_CTL_116_DATA,
	DENALI_CTL_117_DATA,
	DENALI_CTL_118_DATA,
	DENALI_CTL_119_DATA,
	DENALI_CTL_120_DATA,
	DENALI_CTL_121_DATA,
	DENALI_CTL_122_DATA,
	DENALI_CTL_123_DATA,
	DENALI_CTL_124_DATA,
	DENALI_CTL_125_DATA,
	DENALI_CTL_126_DATA,
	DENALI_CTL_127_DATA,
	DENALI_CTL_128_DATA,
	DENALI_CTL_129_DATA,
	DENALI_CTL_130_DATA,
	DENALI_CTL_131_DATA,
	DENALI_CTL_132_DATA,
	DENALI_CTL_133_DATA,
	DENALI_CTL_134_DATA,
	DENALI_CTL_135_DATA,
	DENALI_CTL_136_DATA,
	DENALI_CTL_137_DATA,
	DENALI_CTL_138_DATA,
	DENALI_CTL_139_DATA,
	DENALI_CTL_140_DATA,
	DENALI_CTL_141_DATA,
	DENALI_CTL_142_DATA,
	DENALI_CTL_143_DATA,
	DENALI_CTL_144_DATA,
	DENALI_CTL_145_DATA,
	DENALI_CTL_146_DATA,
	DENALI_CTL_147_DATA,
	DENALI_CTL_148_DATA,
	DENALI_CTL_149_DATA,
	DENALI_CTL_150_DATA,
	DENALI_CTL_151_DATA,
	DENALI_CTL_152_DATA,
	DENALI_CTL_153_DATA,
	DENALI_CTL_154_DATA,
	DENALI_CTL_155_DATA,
	DENALI_CTL_156_DATA,
	DENALI_CTL_157_DATA,
	DENALI_CTL_158_DATA,
	DENALI_CTL_159_DATA,
	DENALI_CTL_160_DATA,
	DENALI_CTL_161_DATA,
	DENALI_CTL_162_DATA,
	DENALI_CTL_163_DATA,
	DENALI_CTL_164_DATA,
	DENALI_CTL_165_DATA,
	DENALI_CTL_166_DATA,
	DENALI_CTL_167_DATA,
	DENALI_CTL_168_DATA,
	DENALI_CTL_169_DATA,
	DENALI_CTL_170_DATA,
	DENALI_CTL_171_DATA,
	DENALI_CTL_172_DATA,
	DENALI_CTL_173_DATA,
	DENALI_CTL_174_DATA,
	DENALI_CTL_175_DATA,
	DENALI_CTL_176_DATA,
	DENALI_CTL_177_DATA,
	DENALI_CTL_178_DATA,
	DENALI_CTL_179_DATA,
	DENALI_CTL_180_DATA,
	DENALI_CTL_181_DATA,
	DENALI_CTL_182_DATA,
	DENALI_CTL_183_DATA,
	DENALI_CTL_184_DATA,
	DENALI_CTL_185_DATA,
	DENALI_CTL_186_DATA,
	DENALI_CTL_187_DATA,
	DENALI_CTL_188_DATA,
	DENALI_CTL_189_DATA,
	DENALI_CTL_190_DATA,
	DENALI_CTL_191_DATA,
	DENALI_CTL_192_DATA,
	DENALI_CTL_193_DATA,
	DENALI_CTL_194_DATA,
	DENALI_CTL_195_DATA,
	DENALI_CTL_196_DATA,
	DENALI_CTL_197_DATA,
	DENALI_CTL_198_DATA,
	DENALI_CTL_199_DATA,
	DENALI_CTL_200_DATA,
	DENALI_CTL_201_DATA,
	DENALI_CTL_202_DATA,
	DENALI_CTL_203_DATA,
	DENALI_CTL_204_DATA,
	DENALI_CTL_205_DATA,
	DENALI_CTL_206_DATA,
	DENALI_CTL_207_DATA,
	DENALI_CTL_208_DATA,
	DENALI_CTL_209_DATA,
	DENALI_CTL_210_DATA,
	DENALI_CTL_211_DATA,
	DENALI_CTL_212_DATA,
	DENALI_CTL_213_DATA,
	DENALI_CTL_214_DATA,
	DENALI_CTL_215_DATA,
	DENALI_CTL_216_DATA,
	DENALI_CTL_217_DATA,
	DENALI_CTL_218_DATA,
	DENALI_CTL_219_DATA,
	DENALI_CTL_220_DATA,
	DENALI_CTL_221_DATA,
	DENALI_CTL_222_DATA,
	DENALI_CTL_223_DATA,
	DENALI_CTL_224_DATA,
	DENALI_CTL_225_DATA,
	DENALI_CTL_226_DATA,
	DENALI_CTL_227_DATA,
	DENALI_CTL_228_DATA,
	DENALI_CTL_229_DATA,
	DENALI_CTL_230_DATA,
	DENALI_CTL_231_DATA,
	DENALI_CTL_232_DATA,
	DENALI_CTL_233_DATA,
	DENALI_CTL_234_DATA,
	DENALI_CTL_235_DATA,
	DENALI_CTL_236_DATA,
	DENALI_CTL_237_DATA
};


#if defined(_PALLADIUM_)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_Init_Denali                                          */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine init the memory controller                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS MC_Init_Denali (void)
{
	int Registerindex = 0;
	for (Registerindex = 0; Registerindex < (sizeof(Denali_Ctl_Initialization)/sizeof(Denali_Ctl_Initialization[0])); Registerindex++)
	{
		IOW32(MC_BASE_ADDR + (Registerindex*4), Denali_Ctl_Initialization[Registerindex]);
	}

	return DEFS_STATUS_OK;

}

static void MC_PrintLeveling (BOOLEAN bIn, BOOLEAN bOut)
{
	return;
}

void MC_PrintRegs (void)
{
	return;
}

static void SDRAM_PrintRegs (void)
{
	return;
}

void MC_PrintVersion (void)
{
	return;
}


#else //_PALLADIUM_
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_CheckDramSize                                       */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:     none                                           */
/* Side effects:                                               */
/* Description:                                                */
/*          measure the DRAM size                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 MC_CheckDramSize (void)
{

	const UINT32 MEM_CHECK_DATA = 0x3596ACE7;
	const UINT32 MEM_CHECK_ADDR = 0x8000;

	UINT32 shift;
	UINT32 address =  0;
	UINT32 orig_data = MEMR32(MEM_CHECK_ADDR);
	BOOLEAN bFoundWrap = FALSE;



	// memory size is between 128MB to 2GB
	for (shift = 0; shift < 4; shift++)
	{
		address = (_128MB_ << shift) + MEM_CHECK_ADDR;

		MEMW32(MEM_CHECK_ADDR, MEM_CHECK_DATA); // write data

		//check if the data is mirrored at address
		if (MEMR32(address) == MEM_CHECK_DATA)
		{
			MEMW32(MEM_CHECK_ADDR, ~MEM_CHECK_DATA); // write the reversed data

			// check if the reversed data is mirrored at address
			if (MEMR32(address) == ~MEM_CHECK_DATA)
			{
				bFoundWrap = TRUE;
				break;
			}
		}
	}

	// restore original data
	MEMW32(MEM_CHECK_ADDR, orig_data);

	if ( bFoundWrap == FALSE)
	{
		serial_printf(KRED ">DRAM size is 0\n");
		return 0;
	}


	// shift holds the correct value of the boundary update INTCR3.GMMAP
	SET_REG_FIELD(INTCR3, INTCR3_GMMAP, (UINT32)shift);

	serial_printf(">DRAM size is: %#010lx ( %dMB ), shift is %d\n", _128MB_ << shift, (_128MB_ << shift) / _1MB_, shift);

	// return the memory size:
	return (_128MB_ << shift);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_Sleep_l                                         */
/*                                                     */
/* Parameters:                                                 */
/*          Cycles  -                                          */
/*                                                     */
/* Returns:     none                                           */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine is the delay function for MC                          */
/*---------------------------------------------------------------------------------------------------------*/
static void MC_Sleep_l (UINT32 Cycles)
{
	CLK_Delay_Cycles(10*Cycles);
	return;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        WaitForSclDone_l                                       */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine waits for SCL training completion                     */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS WaitForSclDone_l (void)
{
	volatile UINT32 TimeOut = 200000;
	MC_Sleep_l(1); // ~ 10usec
	while (1)
	{
		if (READ_REG_FIELD(SCL_START, SCL_START_SET_DDR_SCL_GO_DONE)== 0)
		return DEFS_STATUS_OK;

		if (TimeOut==0)
		return DEFS_STATUS_FAIL;

		TimeOut--;
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        phy_update_dlls_trim_adrctl_clk_l                              */
/*                                                     */
/* Parameters:                                                 */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine init dlls_trim_clk, dlls_trim_adrctl and dlls_trim_adrctrl_ma.        */
/*          Prior to write-leveling these registers must be update.               */
/*---------------------------------------------------------------------------------------------------------*/
static void phy_update_dlls_trim_adrctl_clk_l(void)
{
	REG_WRITE(PHY_LANE_SEL,0);
	REG_WRITE(PHY_DLL_ADRCTRL, BUILD_FIELD_VAL(PHY_DLL_ADRCTRL_incr_dly_adrctrl, 1) | (0x3F & dlls_trim_adrctl_clk));
	REG_WRITE(PHY_DLL_TRIM_CLK, 0x00000040 | (0x3F & dlls_trim_adrctl_clk));
	REG_WRITE(PHY_DLL_RECALIB, BUILD_FIELD_VAL(PHY_DLL_RECALIB_ctr_start_val,      0xA) |
	BUILD_FIELD_VAL(PHY_DLL_RECALIB_incr_dly_adrctrl_ma, 1)|
	BUILD_FIELD_VAL(PHY_DLL_RECALIB_recalib_cnt,        0X640)|  // 22/AUG/2016: changed from 0x10 to 0x640; DLLs recalibration period every 512us
	(0x3F & dlls_trim_adrctl_clk)); // PHY_DLL_RECALIB

	serial_printf("\t> Update dlls_trim_adrctl_clk to %d \n", (0x3F & dlls_trim_adrctl_clk));
	return;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        phy_init_l                                         */
/*                                                     */
/* Parameters:                                                 */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine init the phy                                  */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS phy_init_l (void)
{
	// Note: In Z1, software reset does not reset PHY registers

	UINT32 TmpReg32 = 0;
	volatile UINT32 TimeOut  = 0;
	UINT32 ddr_freq = 0;
	UINT32 nfet_cal = 0;
	UINT32 pfet_cal = 0;
	UINT32 ilane    = 0;
	UINT32 ibit     = 0;


	ddr_freq = CLK_GetPll1Freq();

	REG_WRITE(SCL_START, 0x00000000);
	REG_WRITE(PHY_LANE_SEL,0);
	REG_WRITE(DSCL_CNT, 0);
	REG_WRITE(PHY_DLL_INCR_TRIM_3, 0);
	REG_WRITE(PHY_DLL_INCR_TRIM_1, 0);

	REG_WRITE(SCL_LATENCY, 0x00005076);
	REG_WRITE(SCL_WINDOW_TRIM, 0x00000001);

	phy_update_dlls_trim_adrctl_clk_l();

	/*-----------------------------------------------------------------------------------------------------*/
	/* In PHY_PAD_CTRL register, change DQ/DM/DQS,   Address/Control and clock to 48R; ODT to 120R. - check*/
	/* PHY spec for the correct bits.                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(PHY_PAD_CTRL, PHY_DRV_DQ_48R | PHY_DRV_CTL_48R | PHY_DRV_CLK_48R | PHY_ODT_DQ_120R | 0x50EEC0E8);

	// WRLVL_ON_OFF: (MR1 field values to be used during Write Leveling Entry and Exit): DDR4 Device
	// Should be the same value as in DENALI_CTL_80_DATA
	REG_WRITE(WRLVL_ON_OFF, ((MC_DDR_WL_ENABLE|MC_MR1) << 16) | MC_MR1);

	// WRLVL_DYN_ODT: (MRS2 field values to be used during Write Leveling Entry and Exit): DDR4 Device
	// Should be the same value as in DENALI_CTL_81_DATA
	REG_WRITE(WRLVL_DYN_ODT, 0x00D000D0);  //  Dynamic ODT Off, CAS Write Latency = 11, Auto Self Refresh, Write CRC disable

	// Update SCL_CONFIG_1, SCL_CONFIG_2 and SCL_CONFIG_3 according to DENALI MC init !
	REG_WRITE(SCL_CONFIG_1, 0x01004069);
	REG_WRITE(SCL_CONFIG_2, 0x83000601 | MASK_FIELD(SCL_CONFIG_2_analog_dll_for_scl) | MASK_FIELD(SCL_CONFIG_2_double_ref_dl));
	// lior: enable analog_dll_for_scl bit 28 (origin 0x83000601)
	// Version 10.8.6 and above: double refresh time tRFC . Needed for 8Gb devices.


	REG_WRITE(SCL_CONFIG_3, 7 - LANE_MASK); // LANE_MASK 4 for 2 lanes (skip lane num 2); LANE_MASK 0 is for 3 lanes with ECC (no skip)

	REG_WRITE(DDR4_CONFIG_1, 0x81); //  scl_bit_lvl_in_ddr4  | ddr4
	REG_WRITE(DYNAMIC_WRITE_BIT_LVL, 0x000231C1);   // bit0 is write bit-leveling enable in DSCL (in case DSCL is enabled)

	REG_WRITE(SCL_CONFIG_4,0x00000000);

	serial_printf("\t>>Init DLL and wait for lock\n");  // This state will update: mas_dly, dll_mas_dly, dll_slv_dly_wire, dlls_trim_clk, rise_cycle_cnt,
	REG_WRITE(UNQ_ANALOG_DLL_1, 0x00000007); // reset DLL for all lanes
	MC_Sleep_l(1); // ~ 10usec
	REG_WRITE(UNQ_ANALOG_DLL_1, 0x00000000); // release DLL reset
	TimeOut = 200000;
	MC_Sleep_l(1); // ~ 10usec
	while (1)
	{
		TmpReg32 = REG_READ(UNQ_ANALOG_DLL_2);
		if (( TmpReg32 & LANE_MASK ) == LANE_MASK )  // 1-pass, 0-fail
		break;

		if (TimeOut==0)
		{
			serial_printf(KRED "\t>MC error init dll TO\n" KNRM);
			return DEFS_STATUS_FAIL;
		}

		TimeOut--;
	}
	MC_Sleep_l(1); // ~ 10usec
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	serial_printf("\t>ZQ calib\n");

	REG_WRITE(UNIQUIFY_IO_1, 0x00000000);
	MC_Sleep_l(1); // ~ 10usec
	REG_WRITE(UNIQUIFY_IO_1, 0x00000040); // ZQL calibration long start, clear cal_end_seen and upd_phy_reg.
	TimeOut = 60000;
	MC_Sleep_l(1); // ~ 10usec
	while(1)
	{
		TmpReg32 = REG_READ(UNIQUIFY_IO_1);
		if (( TmpReg32 & 0x2 ) != 0 )
		break;

		if (TimeOut==0)
		{
			serial_printf("\t>MC phy init ZQ calib to1\n");
			return DEFS_STATUS_FAIL;
		}
		TimeOut--;
	}

	//-----------------------------------------------------------------------
	// duo to issue in Z1, reorder bits. Read cal values, reoprder and write back.
	//-----------------------------------------------------------------------
	// Read cal values from UNIQUIFY_IO_2
	nfet_cal = READ_REG_FIELD(UNIQUIFY_IO_2, UNIQUIFY_IO_2_nfet_cal_code);
	pfet_cal = READ_REG_FIELD(UNIQUIFY_IO_2, UNIQUIFY_IO_2_pfet_cal_code);

	serial_printf("\t\t*nfet %#lx ; pfet %#lx \n", nfet_cal, pfet_cal);

	// Reorder, if needed.

	TmpReg32 = (UINT32)1<<2/*upd_phy_reg, 0->1*/; // force update I/O
	REG_WRITE(UNIQUIFY_IO_1, TmpReg32);  // affects in 4 cycles
	MC_Sleep_l(1); // ~ 10usec

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	serial_printf("\t>Init lane related regs\n");
	for (ilane = 0; ilane < NUM_OF_LANES_MAX; ilane++)
	{
		// reset all DQs trim delay (will be update on write/read bit leveling)
		for (ibit = 0 ; ibit < 8 ; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
			REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0); // output DQn
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM,   0); // input DQn
		}

		// Init DQS and DM delay
		ibit = 8;
		REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));

		if ( ddr_freq <= 500000000)
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x28); // input DQS

		else
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x1F); // input DQS



		REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0); // output DM
		ibit = 9;
		REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
		REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x0000001F); // output DQS

		// Init BMC and DDR VREF (Note: DDR VREF is update only on VREF training)
		REG_WRITE(PHY_LANE_SEL,(ilane*7));
		REG_WRITE(VREF_TRAINING, (VREF_STARTPOINT<<12) | (VREF_STARTPOINT<<4) | 0x04 /*| 0x02 | 0x01*/);  // vref_output_enable /*| update_ddr_vref | vref_training */

		// Init PHY_DLL_TRIM_1 and PHY_DLL_TRIM_3
		REG_WRITE(PHY_LANE_SEL,(ilane*6));
		REG_WRITE(PHY_DLL_TRIM_1 ,0x00000001);  // value -1,  beacause of PHY_DLL_INCR_TRIM_1
		REG_WRITE(PHY_DLL_TRIM_3, 0x00000010);  // value -16, beacause of PHY_DLL_INCR_TRIM_3

		// Init phase1 and phase2
		REG_WRITE(PHY_LANE_SEL,(ilane*5));
		REG_WRITE(UNQ_ANALOG_DLL_3,0x00001010); // set phase1 and phase2 to delay by 1/4 cycle

		// Init data_capture_clk to default value after reset
		REG_WRITE(PHY_LANE_SEL,(ilane*8));
		REG_WRITE(SCL_DCAPCLK_DLY, 0x10);

		// Init main_clk_delta_dly
		REG_WRITE(PHY_LANE_SEL,(ilane*3));
		REG_WRITE(SCL_MAIN_CLK_DELTA, 0);
	}

	// Init dlls_trim_2
	REG_WRITE(PHY_DLL_TRIM_2, (UINT32)1<<16);  // Clear all 'dlls_trim_2' registers
	//-----------------------------------------------------------------------

	REG_WRITE(PHY_LANE_SEL,0);

	return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        phy_write_leveling_l                                  */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine perfroms write leveling: SCL will set “trim_2” register.          */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS phy_write_leveling_l (void)
{
	UINT32 TmpReg32;
	UINT32 l_clock;
	UINT32 l_half_clock;
	UINT32 trim2, ilane, status = 0;
	int DQS_CLK_Skew;

	serial_printf(">Write-leveling:\n");

	//------------------------------------------------------------------
	// Write-Leveling
	// -----------------
	// It tune output dq/dqs/dm delay (dlls_trim_2) so DQS output signal arrive at same phase as MCLK signal at each DRAM devices side.
	// It send command to DDR4 device to enter write leveling mode. In this mode the DDR sample MCLK using DQSn; output sample results are output on DQnx.
	// For this stage to work, PHY VREF level should be at a good area. Timing input DQnx delay are not relevant duo to static level of DQnx at this test.
	// The SCL update dlls_trim_2 that adjust output dq/dqs/dm timing with respect to DRAM clock;
	//------------------------------------------------------------------
	l_clock = READ_REG_FIELD(PHY_DLL_ADRCTRL, PHY_DLL_ADRCTRL_dll_mas_dly); // one memory clock cycle delay
	l_half_clock = (l_clock+1) / 2;


	REG_WRITE(WRLVL_AUTOINC_TRIM, LANE_MASK);  // Set 'start_wr_lvl' for all active lanes
	REG_WRITE(PHY_DLL_TRIM_2,     BUILD_FIELD_VAL(PHY_DLL_TRIM_2_clear_all_trims, 1) );  // Clear all 'dlls_trim_2' registers
	REG_WRITE(SCL_START,      BUILD_FIELD_VAL(SCL_START_wrlvl, 1) | BUILD_FIELD_VAL(SCL_START_SET_DDR_SCL_GO_DONE, 1));

	if (WaitForSclDone_l() != DEFS_STATUS_OK)
	{
		serial_printf("\t>>Error: Write-leveling TO\n");
		return DEFS_STATUS_FAIL;
	}

	// Write-Leveling completed, check if it passed
	TmpReg32 = READ_REG_FIELD(WRLVL_CTRL, WRLVL_CTRL_wrlvl_rslt);
	if ( (TmpReg32 & LANE_MASK) != LANE_MASK ) // 1-pass, 0-fail
	{
		serial_printf("\t>>Error: Write-leveling failed.status=0x%x\n", TmpReg32);
		return DEFS_STATUS_FAIL;
	}

	//---------------------------------------------------------------------------------------------
	// To detect invalid lock on an incorrect clock phase,
	// lane is consider pass if DQS to CLK skew is less than +/- 1/2 clock cycle (that is ~ +/- 2.5 inch trace lenght skew @ 800MHz).
	// Also found that even DDR device is not exist (e.g., ECC device) status report pass but dlls_trim_2 goes to max value of 0x3F.
	// With the new approach, ECC enabled can be detected from this early state.
	//---------------------------------------------------------------------------------------------

	// DQS to CLK skew calculation:
	// CLK_Delay =>1/4 clock + dlls_trim_adrctl_clk
	// DQS_Delay =>phase1 + trim1 + trim2
	// Where: phase1 is 1/4 clock and trim1 is -1 then
	// DQS_CLK_Skew = dlls_trim_adrctl_clk - (trim2 - 1) = dlls_trim_adrctl_clk - trim2 + 1;


	// print info and check status
	for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
	{
		REG_WRITE(PHY_LANE_SEL,(ilane*6));
		trim2 = READ_REG_FIELD(PHY_DLL_TRIM_2, PHY_DLL_TRIM_2_dlls_trim_2);
		DQS_CLK_Skew = (int)dlls_trim_adrctl_clk - (int)trim2 + (int)1;

		serial_printf("\t*Lane%u: dlls_trim_2:%u; CLK to DQS skew:%d ;",(UINT8)ilane, (UINT8)trim2, DQS_CLK_Skew);

		if (trim2==63) // 63 is the MAX value, meaning there is no SDRAM device
		{
			// error
			serial_printf(" Lane%u SDRAM device is not present\n",(UINT8)ilane);
		}
		else if (ABS(DQS_CLK_Skew)>(int)l_half_clock) // maybe there was a lock on the next clock phase
		{
			// error
			serial_printf(" failed, CLK to DQS skew is out of range (+/- %u)\n", l_half_clock);
		}
		else
		{
			// pass
			serial_printf("\n");
			status |= (1 << ilane);
		}
	}

	//----------------------------------------------------
	// ECC Auto-Detect
	//----------------------------------------------------
	if (status == LANE_MASK_MAX)
	{
		serial_printf(KCYN "\t*ECC is present\n" KNRM);
	}
	else if ((status == 0x03) && (LANE_MASK == LANE_MASK_MAX))
	{
		// found lane-0 and lane-1 passed while lane-2 (ECC) failed.
		// We can assume ECC is not present and disregard lane-2 from this point.
		serial_printf(KCYN "\t*ECC is NOT present\n" KNRM);
		LANE_MASK = 3;
		NUM_OF_LANES = 2;
		REG_WRITE(SCL_CONFIG_3, 7 - LANE_MASK);    // LANE_MASK 4 for 2 lanes (skip lane num 2); LANE_MASK 0 is for 3 lanes with ECC (no skip)
	}
	//----------------------------------------------------
	if ( (status & LANE_MASK) != LANE_MASK ) // 1-pass, 0-fail
	{
		serial_printf("\t>>Error: Write-leveling failed.status=0x%x\n",(UINT8)(status&0x07));
		return DEFS_STATUS_FAIL;
	}
	//------------------------------------------------------------------

	return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        phy_bit_leveling_l                                     */
/*                                                     */
/* Parameters:      BOOLEAN EnableLog; TRUE to display some log or FALSE for quite mode            */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine perfroms bit leveling (called from phy_leveling_l)           */
/*          (Step 3 to 5 of Uniquify doc)                              */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS phy_bit_leveling_l (BOOLEAN EnableLog) //
{
	UINT32 TmpReg32;
	UINT32 ilane, ibit;
	int    sum;

	if (EnableLog == TRUE)
	serial_printf(">Read/Write Bit-Leveling:\n");



	// clear op_dq_trim_override and ip_dq_trim_override
	REG_WRITE(PHY_LANE_SEL,0);
	REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM,0);
	REG_WRITE(IP_DQ_DQS_BITWISE_TRIM,   0);

	/* -----------------------------------------------------------------                       */
	/* Read/Write Bit-Leveling (normal mode using DDR array)                              */
	/* ----------------------------------------------------------------                        */
	/* SCL Read Bit-Leveling expect for 0xFFFF0000 data pattern to previously written to DDR address          */
	/* 0x20-0x3F (0x20 offset from SCL_START when SCL starts)                              */
	/* SCL Write Bit-Leveling write 0xFFFF0000 data pattern to DDR                        */
	/* When SCL Bit-Leveling completed, Input and Output DQn delay registers are update.              */
	/* There is a bug in the PHY that Output DM delay registers are not update and as a bypass, we average   */
	/* Output DQn and use it as Output DM assuming DM traces are same as all other DQn traces in the lane.   */
	/*---------------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/* Step 3. Write in bit leveling data pattern into DDR at address 0x0020 - 0x003F             */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(SCL_DATA_0, 0xff00ff00);
	REG_WRITE(SCL_DATA_1, 0xff00ff00);
	REG_WRITE(PHY_SCL_START_ADDR, BUILD_FIELD_VAL(PHY_SCL_START_ADDR_scl_start_col_addr, 0x08));  // scl_start_col_addr = 0x0008;  scl_start_row_addr = 0x0000; DDR address 0x0020 for 16 x16bit.
	REG_WRITE(SCL_START,  BUILD_FIELD_VAL(SCL_START_wr_only, 1) |
	BUILD_FIELD_VAL(SCL_START_SET_DDR_SCL_GO_DONE, 1)); //  write SCL data to DRAM
	if (WaitForSclDone_l() != DEFS_STATUS_OK)
	{
		if (EnableLog == TRUE)
		serial_printf(KRED ">MC error bit leveling TO step3\n" KNRM);
		return DEFS_STATUS_FAIL;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Step 4                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(PHY_SCL_START_ADDR,0); // scl_start_col_addr = 0x0000;  scl_start_row_addr = 0x0000; DDR address 0x0000 for 16 x16bit.

	/*-----------------------------------------------------------------------------------------------------*/
	/* Step 5 - Run Bit-Leveling : write side                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(SCL_START,  BUILD_FIELD_VAL(SCL_START_write_side_bit_lvl, 1) |
	BUILD_FIELD_VAL(SCL_START_CONTINUOUS_RDS, 1) |
	BUILD_FIELD_VAL(SCL_START_bit_lvl_norm, 1) |
	BUILD_FIELD_VAL(SCL_START_SET_DDR_SCL_GO_DONE, 1)); //  write SCL data to DRAM


	if (WaitForSclDone_l () != DEFS_STATUS_OK)
	{
		if (EnableLog == TRUE)
		serial_printf(KRED ">MC error bit leveling TO step5\n" KNRM);
		return DEFS_STATUS_FAIL;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Bit-Leveling completed, check if it read side passed                        */
	/*-----------------------------------------------------------------------------------------------------*/
	TmpReg32 = READ_REG_FIELD(DYNAMIC_BIT_LVL, DYNAMIC_BIT_LVL_bit_lvl_failure_status);
	if ( (TmpReg32 & LANE_MASK) != 0)  // 1- fail, 0-pass
	{
		if (EnableLog == TRUE)
		serial_printf(KRED "\t>MC error bit leveling read. bit level is %#lx \n" KNRM, TmpReg32);
		return DEFS_STATUS_FAIL;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Bit-Leveling completed, check if it write side passed                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	TmpReg32 = READ_REG_FIELD(DYNAMIC_WRITE_BIT_LVL, DYNAMIC_WRITE_BIT_LVL_bit_lvl_wr_failure_status);
	if ( (TmpReg32 & LANE_MASK) != 0)  // 1- fail, 0-pass
	{
		if (EnableLog == TRUE)
		serial_printf(KRED "\t>MC error bit leveling write. bit level is %#lx \n" KNRM, TmpReg32);
		return DEFS_STATUS_FAIL;
	}

	serial_printf_debug ("\tCompleted ok.\n");

	if (EnableLog == TRUE)
	{
		// print info ...
		for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
		{
			serial_printf("\t*Lane%u Output: ",ilane);
			for (ibit=0; ibit<8; ibit++)
			{
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
				TmpReg32 = READ_REG_FIELD(OP_DQ_DM_DQS_BITWISE_TRIM, OP_DQ_DM_DQS_BITWISE_TRIM_op_dq_dm_dqs_bitwise_trim_reg);
				serial_printf("DQ%lu:0x%02X, ",ibit,TmpReg32);
			}
			serial_printf("\n");
		}
		for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
		{
			serial_printf("\t*Lane%u Input:  ",ilane);
			for (ibit=0; ibit<8; ibit++)
			{
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
				TmpReg32 = READ_REG_FIELD(IP_DQ_DQS_BITWISE_TRIM, IP_DQ_DQS_BITWISE_TRIM_ip_dq_dqs_bitwise_trim_reg) ;
				serial_printf("DQ%lu:0x%02X, ",ibit, TmpReg32);
			}
			serial_printf("\n");
		}
	}
	/*---------------------------------------------------------------------------------------------------------*/
	/* duo to a PHY bug, write DM signal has no training or PHY registers are not update; calculate the average*/
	/* DQn output delay and use the average value as DM delay                                                  */
	/*  this assuming in PCB DM trace is match DQn traces.                                                     */
	/* serial_printf(" >MC calc average DQn output delay to use as DM delay for each lanes");                  */
	/*                                                                                                         */
	/*---------------------------------------------------------------------------------------------------------*/
	for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
	{
		sum = 0;
		for (ibit = 0 ; ibit < 8 ; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL, ilane*7 + ibit*0x100);
			TmpReg32 = READ_REG_FIELD(OP_DQ_DM_DQS_BITWISE_TRIM, OP_DQ_DM_DQS_BITWISE_TRIM_op_dq_dm_dqs_bitwise_trim_reg);
			sum +=   IP_DQ_REG_TO_VALUE(TmpReg32);
		}
		sum = DIV_ROUND(sum, 8);       // sum is int, not UINT.. fixed around func; add +4  ( find the average of the bits)

		REG_WRITE(PHY_LANE_SEL, ilane*7 + 0x800);
		REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, IP_DQ_VALUE_TO_REG(sum));

	}

	REG_WRITE(PHY_LANE_SEL,0);

	if (EnableLog == TRUE)
	{
		// print info ...
		for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)8<<8));
			TmpReg32 = READ_REG_FIELD(OP_DQ_DM_DQS_BITWISE_TRIM, OP_DQ_DM_DQS_BITWISE_TRIM_op_dq_dm_dqs_bitwise_trim_reg);
			serial_printf("\t*Lane%d: DM: 0x%02X\n", ilane, TmpReg32);

		}
	}
	//--------------------------------------------------------

	/*---------------------------------------------------------------------------------------------------------*/
	/* Re-write SCL values with op_dq_trim_override and ip_dq_trim_override bit set (from now we will use the override values)  */
	/* (original code has an issue with restoring ECC DM value when issue sweep !!)                            */
	/* This assume we run sweep and do not enable DSCL for bit-leveling.                                       */
	/*---------------------------------------------------------------------------------------------------------*/
	for (ilane = 0; ilane < 3; ilane++)
	{
		for (ibit=0; ibit<8; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));

			TmpReg32 = REG_READ(IP_DQ_DQS_BITWISE_TRIM);
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM,   0x80/*ip_dq_trim_override*/ | TmpReg32 );

			TmpReg32 = REG_READ(OP_DQ_DM_DQS_BITWISE_TRIM);
			REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80/*op_dq_trim_override*/ | TmpReg32 );
		}

		// rewrite DM too.
		ibit = 9;
		REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
		TmpReg32 = REG_READ(OP_DQ_DM_DQS_BITWISE_TRIM);
		REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM,  0x80/*op_dq_trim_override*/ | TmpReg32 );
	}
	//-------------------------------------------------------------

	return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        phy_read_leveling_l                                                                    */
/*                                                                                                         */
/* Parameters:      BOOLEAN bPrint  : print results                                                        */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*          This routine performs read leveling (called from phy_leveling_l)                               */
/*          (Step 7 to 8 in Uniquify doc).sets SCL will set data_capture_clk and main_clk_delta_dly        */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS phy_read_leveling_l (BOOLEAN bPrint)
{
	UINT32 TmpReg32 = 0;
	UINT  ilane = 0;

	DEFS_STATUS status = DEFS_STATUS_OK;

	// print info ...
	if(bPrint == TRUE)
	{
		serial_printf(">Read-Leveling:\n");
	}

	//------------------------------------------------------------------
	// Round-Trip-Delay and Read-Leveling
	// ************************************
	// Round-Trip-Delay:
	// It tune input gate timing so PHY will know when to expect data arrive from each lane to BMC. This timing relate to the distance between BMC and DDR (Round-Trip-Delay).
	// Note: For unknown reason (bug??) this input gate timing parameter is not exposed to user and can be set by SCL only.
	// Read-Leveling:
	// It tune dcap_byte_dly (data_capture_clk) that sync between DQS clock and PHY internal clock.
	// In addition, it tune main_clk_delta_dly that add extra clock delay to a line between all return data from DDR such a way that a common dfi_rddata_valid can be generated for all byte lanes aligned with the controller’s clock (MC)
	// Since we use a single DDR device when lane0 and lane1 traces are most likely with the same length (in 800MHz boundary), this value is always 0.
	//--------------------------------------------------------------------------------------------------------

	// Step 7 - Write in SCL data pattern into DDR at address 0
	REG_WRITE(PHY_SCL_START_ADDR, 0); // scl_start_col_addr = 0x0000;  scl_start_row_addr = 0x0000; DDR address 0x0000 for 16 x16bit.
	REG_WRITE(SCL_DATA_0, 0x789b3de0);
	REG_WRITE(SCL_DATA_1, 0xf10e4a56);
	REG_WRITE(SCL_START,  BUILD_FIELD_VAL(SCL_START_wr_only, 1) |
	BUILD_FIELD_VAL(SCL_START_SET_DDR_SCL_GO_DONE, 1)); //  write SCL data to DRAM
	if (WaitForSclDone_l() != DEFS_STATUS_OK)
	{
		serial_printf(KRED ">MC error read leveling TO step7\n", KNRM);
		return DEFS_STATUS_FAIL;
	}

	// Step 8 - Run Round-Trip-Delay
	REG_WRITE(SCL_START,  BUILD_FIELD_VAL(SCL_START_INCR_SCL, 1) |
	BUILD_FIELD_VAL(SCL_START_CONTINUOUS_RDS, 1) |
	BUILD_FIELD_VAL(SCL_START_SET_DDR_SCL_GO_DONE, 1)); //  write SCL data to DRAM

	if (WaitForSclDone_l() != DEFS_STATUS_OK)
	{
		serial_printf(KRED ">MC error read leveling TO step8\n" KNRM);
		return DEFS_STATUS_FAIL;
	}

	TmpReg32 = READ_REG_FIELD(SCL_START, SCL_START_cuml_scl_rslt);
	if ((TmpReg32 & LANE_MASK) != LANE_MASK)  // 0-fail, 1-pass
	{

		serial_printf(KRED "\t>MC error read leveling error. SCL_START %#010lx \n" KNRM, REG_READ(SCL_START));
		status = DEFS_STATUS_FAIL;
	}

	// print info ...
	if(bPrint == TRUE)
	{
		for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*8));
			serial_printf("\t*Lane%d: data_capture_clk:0x%02X,", ilane, (UINT8)READ_REG_FIELD(SCL_DCAPCLK_DLY, SCL_DCAPCLK_DLY_dcap_byte_dly));

			REG_WRITE(PHY_LANE_SEL,(ilane*3));
			serial_printf("\tmain_clk_delta_dly:0x%02X,", (UINT8)READ_REG_FIELD(SCL_MAIN_CLK_DELTA, SCL_MAIN_CLK_DELTA_main_clk_delta_dly));

			REG_WRITE(PHY_LANE_SEL,(ilane*8));
			serial_printf("\tcycle_cnt:0x%02X\n",(UINT8)(READ_REG_FIELD(SCL_START, SCL_START_cycle_cnt))); // number of delay elements required to delay the clock signal to align with the read DQS falling edge

		}
		TmpReg32 = READ_REG_FIELD(SCL_LATENCY, SCL_LATENCY_main_clk_dly);
		serial_printf("\t*main_clk_dly: %lu\n", TmpReg32);
	}


	return status;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        vref_training_l                                    */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine perfroms VREF training. It sets the VREF for all lanes + DDR VREF     */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS vref_training_l (void)
{

	const int VREF_NUM_OF_AVRG =  5;
	const int NUM_OF_RUNS   = 10;
	// in Z11 in PHY VREF, bit 0 is fixed to VREF select therefore must be fixed to 1
	// Those same values will be use for both PHY and DDR although DDR has no issue.
	const int VREF_MAX      = 0x31;
	const int VREF_MIN      = 0x01;
	const int VREF_STEP     = 0x02;

	int phy_window_diff[NUM_OF_LANES_MAX  + 1]; // it's sum of diff from several cycles
	int phy_vref[NUM_OF_LANES_MAX  + 1] = { VREF_STARTPOINT, VREF_STARTPOINT, VREF_STARTPOINT, VREF_STARTPOINT }; // 6 bits while bit 0 fixed to 1 //  use as starting point

	int ddr_window_diff; // it's sum of diff from several cycles
	int ddr_vref = VREF_STARTPOINT; // 6 bits // 0x17 is common good value, use as starting point
	int ddr_vref_range = 0; // 1 bit (in this code it fixed to 0 for range 1 on DDR device side only)

	int cycle_iteration;
	int ilane;
	int iave;
	int val;
	DEFS_STATUS bit_lvl_failure_status; // last_bit_lvl_failure_status;
	DEFS_STATUS status = DEFS_STATUS_OK;

	BOOLEAN WasError = FALSE;

	int phy_window_0_l = 0;
	int phy_window_1_l = 0;
	int ddr_window_0_l = 0;
	int ddr_window_1_l = 0;
	int phy_window_diff_l = 0;
	int ddr_window_diff_l = 0;


	serial_printf(">VREF training:\n");

	// disable refresh before doing Vref training to workaround MC Errata issue:
	REG_SET_FIELD(MC_BASE_ADDR + ((UINT32)(TREF_ENABLE_ADDR) * 4), TREF_ENABLE_WIDTH, TREF_ENABLE_OFFSET, 0);
	CLK_Delay_MicroSec(3);
	/*-----------------------------------------------------------------------------------------------------*/
	/* Running VREF training routine                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	cycle_iteration = NUM_OF_RUNS;
	while (cycle_iteration != 0)
	{
		cycle_iteration--;

		//serial_printf(">MC cycle_iteration: %d\n", cycle_iteration);

		//------------------------------------------
		// Set the VREF level at BMC and DDR device
		//-----------------------------------------
		for (ilane = 0 ; ilane < NUM_OF_LANES  ; ilane++)
		{
			REG_WRITE(PHY_LANE_SEL, ilane*7);
			val = (ddr_vref<<12) | (ddr_vref_range<<11) | (phy_vref [ilane]<<4);
			REG_WRITE(VREF_TRAINING, val | 0x04/*vref_output_enable*/ | 0x02/*update_ddr_vref*/ | 0x01/*vref_training*/);
		}
		// Generate DDR VREF update transaction to DDR device
		REG_WRITE(SCL_START, BUILD_FIELD_VAL(SCL_START_SET_DDR_SCL_GO_DONE,1));
		if (WaitForSclDone_l() != DEFS_STATUS_OK)
		{
			serial_printf(KRED "MC error VREF training TO step2\n" KNRM);
			// re-enable refresh:
			REG_SET_FIELD(MC_BASE_ADDR + ((UINT32)(TREF_ENABLE_ADDR) * 4), TREF_ENABLE_WIDTH, TREF_ENABLE_OFFSET, 1);
			return DEFS_STATUS_FAIL;
		}
		// lior: 20-OCT-2015, clear update_ddr_vref bit to avoid from SCL write DDR4 VREF value on the next SCL run.
		REG_WRITE(VREF_TRAINING, val | 0x04/*vref_output_enable*/ | 0x01/*vref_training*/);

		//------------------------------------------
		// Check windows size - do averaging of several runs
		//-----------------------------------------

		// initialize values before averaging; those will be use to sum windows size of several runs
		memset((void *)phy_window_diff, 0, sizeof(phy_window_diff));

		ddr_window_diff = 0;
		WasError = FALSE;

		// do averaging of several runs
		for (iave=0; iave < VREF_NUM_OF_AVRG; iave++)
		{
			bit_lvl_failure_status = phy_bit_leveling_l(FALSE);
			if ( bit_lvl_failure_status != DEFS_STATUS_OK)
			{
				serial_printf("\tBit-Leveling at VREF training state failed (bit_lvl_failure_status=0x%4X).\n", bit_lvl_failure_status);
				WasError = TRUE;

			}
			else
			{
				//------------------------------
				// calculate windows size and add them to average
				for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
				{
					REG_WRITE(PHY_LANE_SEL, ilane*6);
					val = REG_READ(VREF_TRAINING_WINDOWS);

					phy_window_0_l = (val >>0) & 0x3F; // reg_read_val[5:0];
					phy_window_1_l = (val >>8) & 0x3F; // reg_read_val[13:8];
					ddr_window_0_l = (val >>16) & 0x3F;    // reg_read_val[21:16];
					ddr_window_1_l = (val >>24) & 0x3F;    // reg_read_val[29:24];

					phy_window_diff_l = ((int)phy_window_0_l - (int)phy_window_1_l);
					ddr_window_diff_l = ((int)ddr_window_0_l - (int)ddr_window_1_l);

					//serial_printf(" >lane:%d ; val=%#08lx (DDR:%ld, BMC:%ld)\n", ilane, val, ddr_window_diff_l, phy_window_diff_l);

					// Note: PHY window is "averaging" average_num times
					//       "averaging" is sum only, no need to divided.
					phy_window_diff[ilane] += phy_window_diff_l;

					// Note: DDR window is "averaging" average_num*NUM_OF_LANES times
					//       "averaging" is sum only, no need to divided.
					ddr_window_diff += ddr_window_diff_l;
				}
			}

		}

		/*-------------------------------------------------------------------------------------------------*/
		/* check average results                                       */
		/*-------------------------------------------------------------------------------------------------*/
		for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
		{
			serial_printf_debug("\tlane: %d ; sum phy_window_diff: %ld ; use phy_vref: %#lx ",ilane, phy_window_diff[lane], phy_vref[lane]);

			if ( (phy_window_diff[ilane]<(-(VREF_NUM_OF_AVRG))) &&  (phy_vref[ilane]<VREF_MAX) )
			{
				phy_vref[ilane] += VREF_STEP;
				serial_printf_debug(", Step Up.\n");
			}
			else if ( (phy_window_diff[ilane]>(VREF_NUM_OF_AVRG)) &&  (phy_vref[ilane]>VREF_MIN) )
			{
				phy_vref[ilane] -= VREF_STEP;
				serial_printf_debug(", Step Down.\n");
			}
			else
			{
				serial_printf_debug(", No change.\n");
			}
		}
		//--------------------------------
		serial_printf_debug("\t>MC sum ddr_window_diff: %ld ; use ddr_vref: %#lx ",ddr_window_diff, ddr_vref);

		if ( (ddr_window_diff<(-(VREF_NUM_OF_AVRG*NUM_OF_LANES ))) && (ddr_vref<VREF_MAX) )
		{
			ddr_vref += VREF_STEP;
			serial_printf_debug(", Step Up.\n");
		}
		else if ( (ddr_window_diff>(+(VREF_NUM_OF_AVRG*NUM_OF_LANES ))) &&  (ddr_vref>VREF_MIN) )
		{

			ddr_vref -= VREF_STEP;
			serial_printf_debug(", Step Down.\n");
		}
		else
		{
			serial_printf_debug(", No change.\n");
		}
		//---------------------------------------------

	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* clear vref_training and update_ddr_vref bits.                               */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(PHY_LANE_SEL, 0*7);
	val = (ddr_vref<<12) | (ddr_vref_range<<11) | (phy_vref[0]<<4);
	REG_WRITE(VREF_TRAINING, val | 0x04); // vref_output_enable

	if (WasError==TRUE)
	{
		// fail, there was at least one error on the last iteration
		serial_printf(KRED "\t>MC error VREF training\n" KNRM);
		// re-enable refresh:
		REG_SET_FIELD(MC_BASE_ADDR + ((UINT32)(TREF_ENABLE_ADDR) * 4), TREF_ENABLE_WIDTH, TREF_ENABLE_OFFSET, 1);
		return DEFS_STATUS_FAIL;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Print VREF training                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	for (ilane = 0; ilane < NUM_OF_LANES ; ilane++)
	{
		REG_WRITE(PHY_LANE_SEL, ilane*7);
		serial_printf("\t*Lane %d PHY VREF: %#lx \n", ilane , READ_REG_FIELD(VREF_TRAINING, VREF_TRAINING_vref_value));
	}
	REG_WRITE(PHY_LANE_SEL, 0);
	serial_printf("\t*DDR VREF: %#lx \n", READ_REG_FIELD(VREF_TRAINING, VREF_TRAINING_ddr_vref_value) >>1); // Note: When this field is read, value is shifted left by 1.Why? because Uniquify are evil.

	// re-enable refresh:
	REG_SET_FIELD(MC_BASE_ADDR + ((UINT32)(TREF_ENABLE_ADDR) * 4), TREF_ENABLE_WIDTH, TREF_ENABLE_OFFSET, 1);

	return status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* PHY options  (used as mode in  phy_leveling_l                              */
/*---------------------------------------------------------------------------------------------------------*/
#define INIT_PARAM          ((UINT16)1<<0) // Init Freq, DS, ODT parameters
#define INIT_PARAM_DEBUG    ((UINT16)1<<1) // Init VREF and delay parameters (for debug)
#define INIT_PLL            ((UINT16)1<<2)
#define INIT_MC_PHY         ((UINT16)1<<3)
#define INIT_PLL_MC_PHY     (INIT_PLL|INIT_MC_PHY)
#define PHY_WR_LVL          ((UINT16)1<<4)
#define PHY_VREF_TRNG       ((UINT16)1<<5)
#define PHY_RD_BIT_LVL      ((UINT16)1<<6)
#define PHY_RW_BIT_LVL      ((UINT16)1<<7)
#define PHY_RD_LVL          ((UINT16)1<<8)
#define PHY_RD_LVL_FAST     ((UINT16)1<<9)
#define PHY_LEVELLING       (PHY_WR_LVL|PHY_VREF_TRNG|PHY_RW_BIT_LVL|PHY_RD_LVL)


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        phy_leveling_l                                    */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine perfoms Phy leveling (VREF, bit leveling and PHY read and write leveling) */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS phy_leveling_l(UINT16 mode)
{
	DEFS_STATUS status = DEFS_STATUS_OK ;

	if((mode & PHY_WR_LVL) !=0)
	{
		if ((status = phy_write_leveling_l()) != DEFS_STATUS_OK)
		{
			return DEFS_STATUS_FAIL;
		}
	}

	// Step 1. Enable gating for bit-leveling
	REG_WRITE(DISABLE_GATING_FOR_SCL, 0x0);

	// Step 2. Enable DDR4 logic for scl/bit-lvl
	REG_WRITE(DDR4_CONFIG_1,  BUILD_FIELD_VAL(DDR4_CONFIG_1_scl_bit_lvl_in_ddr4, 1) |
	BUILD_FIELD_VAL(DDR4_CONFIG_1_ddr4, 1));      // ((UINT32)1<<7) | ((UINT32)1<<0)); // scl_bit_lvl_in_ddr4  | ddr4

	// Issue BMC and DDR device VREF trainning
	if((mode & PHY_VREF_TRNG) !=0)
	{
		if ((status = vref_training_l()) != DEFS_STATUS_OK) // including phy_bit_leveling()
		goto phy_leveling_l_end;
	}

	// Issue Read/Write Bit-Levleing
	if((mode & PHY_RW_BIT_LVL) !=0)
	{
		if ((status = phy_bit_leveling_l(TRUE)) != DEFS_STATUS_OK) // Step 3 to 5
		goto phy_leveling_l_end;
	}


	// Step 6 - Disable gating to allow proper SCL gating
	REG_WRITE(DISABLE_GATING_FOR_SCL, BUILD_FIELD_VAL(DISABLE_GATING_FOR_SCL_disable_scl_gating, 1));

	// Issue Read Levleing
	if((mode & PHY_RD_LVL) !=0)
	{
		if ((status = phy_read_leveling_l(TRUE)) != DEFS_STATUS_OK) // Step 7 to 8
		goto phy_leveling_l_end;
	}

	// Issue Read Leveling fast.
	if((mode & PHY_RD_LVL_FAST) !=0)
	{
		if ((status = phy_read_leveling_l(FALSE)) != DEFS_STATUS_OK) // Step 7 to 8
		goto phy_leveling_l_end;
	}

	phy_leveling_l_end:

	// Lior - keep DDR4_CONFIG_1 fix to 0x81 and not set to 0x01
	// Step 9
	REG_WRITE(DDR4_CONFIG_1, BUILD_FIELD_VAL(DDR4_CONFIG_1_scl_bit_lvl_in_ddr4, 1) |
	BUILD_FIELD_VAL(DDR4_CONFIG_1_ddr4, 1));     // scl_bit_lvl_in_ddr4  | ddr4

	// Step 10 - Turn off x-prop fix in simulation
	REG_WRITE(DISABLE_GATING_FOR_SCL, BUILD_FIELD_VAL(DISABLE_GATING_FOR_SCL_disable_scl_gating, 1)  |
	BUILD_FIELD_VAL(DISABLE_GATING_FOR_SCL_disable_x_prop_fix, 1));  // register h6E    - only in simulation



	REG_WRITE(PHY_LANE_SEL,0); // 10/03/2016: add to reset lane select

	return status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Sweep_Trim_Input_DQS_l                                 */
/*                                                     */
/* Parameters:      BOOLEAN bECC    : sweep ECC lane                           */
/* Parameters:      BOOLEAN bSilent : print results                            */
/* Returns:     pEyeCenter,     pEyeSize                               */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine sweeps DQS delay and set to an optimized value in the middle of eye center */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS Sweep_Trim_Input_DQS_l (int *pEyeCenter, int *pEyeSize, BOOLEAN bECC, BOOLEAN bSilent )
{
	UINT32       ilane;
	UINT32       NewTrimReg;
	UINT16       TmpReg16;
	UINT16       l_LaneStatus [0x40]; // 16bit for 16 bit of data. each bit is 0 for pass or 1 for fail.
	UINT32       l_PresetTrim[NUM_OF_LANES_MAX];
	int      laneStart = 0;
	int      laneStop = 2;
	DEFS_STATUS  status = DEFS_STATUS_OK;

	int eye_center[16];
	int eye_size[16];

#define DISPLAY_DQS_SWEEP 0
#if DISPLAY_DQS_SWEEP
	int          Separation;
#endif


	if (bECC == TRUE)
	{
		laneStart = NUM_OF_LANES_MAX - 1;
		laneStop =  NUM_OF_LANES_MAX;
	}

	if (bSilent == FALSE)
	{
		serial_printf("\n>Sweep Input DQS ");
		if (bECC == TRUE)
			serial_printf("ECC\n");
		else
			serial_printf("\n ");
	}

	memset(l_LaneStatus, 0xFFFF, sizeof(l_LaneStatus));


	/*---------------------------------------------------------------------------------------------------------*/
	/*  Get default values (of all lanes)                                      */
	/*---------------------------------------------------------------------------------------------------------*/
	for (ilane = laneStart ; ilane < laneStop; ilane++)
	{
		REG_WRITE(PHY_LANE_SEL,(ilane*7)+(8<<8));

		// keep register value
		l_PresetTrim[ilane] = READ_REG_FIELD(IP_DQ_DQS_BITWISE_TRIM, IP_DQ_DQS_BITWISE_TRIM_ip_dq_dqs_bitwise_trim_reg);
	}




	/*---------------------------------------------------------------------------------------------------------*/
	/*  Sweep                                                      */
	/*---------------------------------------------------------------------------------------------------------*/
	for (NewTrimReg = 0x00 ; NewTrimReg < 0x40; NewTrimReg++)
	{
		REG_WRITE(PHY_LANE_SEL, 0);

		// Update trim value into DQS
		for (ilane = laneStart; ilane < laneStop ; ilane++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(8<<8));
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 |  (0x7F & NewTrimReg));
		}

		REG_WRITE(PHY_LANE_SEL, 0);

		// dummy accsess to DDR
		IOW32 (0x1000,IOR32(0x1000));

		// memory stress test
		l_LaneStatus[NewTrimReg] =  MC_MemStressTest(bECC, bSilent); //  for enhanced sweep silent is TRUE, and also can reduce the mem test
		// for final sweep silent is false.
	}



	/*---------------------------------------------------------------------------------------------------------*/
	/*  Display                                                                                             */
	/*---------------------------------------------------------------------------------------------------------*/
#if DISPLAY_DQS_SWEEP
	if (bECC == TRUE)
	{
		serial_printf("\t\t\t\tlane1        lane0\n");
		serial_printf("\t\t\t\t7... ...0    7... ...0\n");
		for (NewTrimReg = 0x00; NewTrimReg < 0x40; NewTrimReg++)
		{
			if (( l_LaneStatus[NewTrimReg] != 0xFFFF) && ( l_LaneStatus[NewTrimReg] != 0xFF80))
			{
				serial_printf("\treg= 0x%02X  bitwise ber= %d%d%d%d %d%d%d%d    %d%d%d%d %d%d%d%d\n",
					NewTrimReg, BINARY_PRINT16(l_LaneStatus[NewTrimReg]));
			}
		}
	}
#endif // DISPLAY_DQS_SWEEP



	// align bit results to lane results
	if (bECC == FALSE)
	{
		for (NewTrimReg = 0x00; NewTrimReg < 0x40; NewTrimReg++)
		{
			TmpReg16 = 0;
			for (ilane = laneStart; ilane < laneStop ; ilane++)
			{
				// DQS delay is lane related, if one of the bits failed, report as all lane failed.
				if (((l_LaneStatus[NewTrimReg]>>(ilane*8)) & 0xFF) != 0x00)
					TmpReg16 |= 0xFF<<(ilane*8);
			}
			l_LaneStatus[NewTrimReg] = TmpReg16;
		}
	}
	else
	{
		for (NewTrimReg = 0x00; NewTrimReg < 0x40; NewTrimReg++)
		{
			// DQS delay is lane related, if one of the bits failed, report as all lane failed.
			if ((l_LaneStatus[NewTrimReg] & 0x007F ) != 0x0000)
			{
				l_LaneStatus[NewTrimReg] = 0xFFFF ; // one bit fails -> all lane fail.
			}
		}
	}






	FindMidBiggestEyeBitwise_l(l_LaneStatus, ARRAY_SIZE(l_LaneStatus),
						eye_center, eye_size, (bECC == TRUE) ? 1 : 16);


	// squeeze the results to use the first two numbers of the results array.
	// location 0 for lane zero, location 1 on the arrays for lane 1.
	if (bECC == FALSE)
	{
		pEyeCenter[0] = eye_center[0];
		pEyeSize[0]  = eye_size[0];
		pEyeCenter[1] = eye_center[8];
		pEyeSize[1]  = eye_size[8];
	}
	else
	{
		pEyeCenter[2] = eye_center[0];
		pEyeSize[2]  = eye_size[0];
	}

	/*---------------------------------------------------------------------------------------------------------*/
	/* Check results and print info                                        */
	/*---------------------------------------------------------------------------------------------------------*/
	if (bSilent == FALSE)
		serial_printf("\n");
	for (ilane = laneStart; ilane < laneStop ; ilane++)
	{

		/*---------------------------------------------------------------------------------------------*/
		/* Check if there is a valid eye (more then 5).  If not: use the original value:           */
		/*---------------------------------------------------------------------------------------------*/
		if  (pEyeSize[ilane] < 5)
		{
			serial_printf(KRED "\tlane=%d; Input DQS: no eye center or eye too small (size=%d), use the original value %d\n",
				ilane, pEyeSize[ilane], l_PresetTrim[ilane] );

			pEyeCenter[ilane] = l_PresetTrim[ilane];

			serial_printf("\tlane= %d; eyesize= %d; eye center= 0x%x (was val= 0x%x; diff= %d)\n",
				ilane,
				pEyeSize[ilane],
				pEyeCenter[ilane],
				l_PresetTrim[ilane],
				(pEyeCenter[ilane] - l_PresetTrim[ilane]));
		}
		else
		{
			if (bSilent == FALSE)
				serial_printf("\tlane= %d; eyesize= %d; eye center= 0x%x (was val= 0x%x; diff= %d)\n",
					ilane,
					pEyeSize[ilane],
					pEyeCenter[ilane],
					l_PresetTrim[ilane],
					(pEyeCenter[ilane] - l_PresetTrim[ilane]));
		}

	}


	/*---------------------------------------------------------------------------------------------------------*/
	/*  Set new values                                                 */
	/*---------------------------------------------------------------------------------------------------------*/
	for (ilane = laneStart; ilane < laneStop ; ilane++)
	{
		serial_printf_debug("\tSweep DQS results: Lane%d, set %d to DQS (eye size is %d)\n", ilane,  pEyeCenter[ilane], pEyeSize[ilane]);
		REG_WRITE(PHY_LANE_SEL,(ilane*7)+(8<<8));
		REG_WRITE(IP_DQ_DQS_BITWISE_TRIM,(0x80 | pEyeCenter[ilane]) );
	}

	// dummy accsess to DDR (07/AUG/2016)
	IOW32 (0x1000, IOR32(0x1000));

	REG_WRITE(PHY_LANE_SEL, 0);

	/*-----------------------------------------------------------------------------------------*/
	/* Read leveling  (quick with no prints)                           */
	/*-----------------------------------------------------------------------------------------*/
	status = phy_leveling_l(PHY_RD_LVL_FAST);

	return status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Sweep_DQs_ECC_Trim_l                                   */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine sweep DQs delay and set to an optimized value in the middle of eye center */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS Sweep_DQs_ECC_Trim_l (BOOLEAN bInput)
{
	const static int startScan = -62;
	const static int stopScan  = 62 ;

	const UINT32     MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS = (stopScan - startScan + 1);
	const int    NUM_OF_LANES_SWEEP = 3;
	int      samplePoint = 0;
	UINT32       ilane ,ibit;
	UINT32       NewTrimReg;
	UINT16       l_DQs_ECC_BitStatus[MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS]; // 16bit for 16 bit of data. each bit is 0 for pass or 1 for fail.
	UINT32       l_DQs_ECC_PresetTrim[NUM_OF_LANES_SWEEP][7];
	int      l_DQs_ECC_BestTrim_EyeCenter[7], l_DQs_ECC_BestTrim_EyeSize[7];
	int      sum = 0;
	UINT32           ibit2 = 0;
	DEFS_STATUS  status = DEFS_STATUS_OK;



	if ( bInput == TRUE)
	{
		//startScan = -30 ; // -62;
		//stopScan  = 10 ;
		serial_printf("\n>Sweep ECC In DQs\n");
	}
	else
	{
		serial_printf("\n>Sweep ECC Out DQs\n");
	}

	memset(l_DQs_ECC_BitStatus,  0,      sizeof(l_DQs_ECC_BitStatus));
	memset(l_DQs_ECC_PresetTrim, 0,         sizeof(l_DQs_ECC_PresetTrim));
	memset(l_DQs_ECC_BestTrim_EyeCenter, 0, sizeof(l_DQs_ECC_BestTrim_EyeCenter));
	memset(l_DQs_ECC_BestTrim_EyeSize,   0, sizeof(l_DQs_ECC_BestTrim_EyeSize));


	/*---------------------------------------------------------------------------------------------------------*/
	/*  Get default values (of all lanes)                                      */
	/*---------------------------------------------------------------------------------------------------------*/
	for (ilane = 0 ; ilane < NUM_OF_LANES_SWEEP  ; ilane++)
	{
		for (ibit=0; ibit<7; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));

			// keep register value
			if ( bInput == TRUE)
				l_DQs_ECC_PresetTrim[ilane][ibit] = READ_REG_FIELD(IP_DQ_DQS_BITWISE_TRIM, IP_DQ_DQS_BITWISE_TRIM_ip_dq_dqs_bitwise_trim_reg);
			else
				l_DQs_ECC_PresetTrim[ilane][ibit] = READ_REG_FIELD(OP_DQ_DM_DQS_BITWISE_TRIM, OP_DQ_DM_DQS_BITWISE_TRIM_op_dq_dm_dqs_bitwise_trim_reg);
		}
	}

	ilane =  NUM_OF_LANES_SWEEP -1;


	for(ibit2 = 0 ; ibit2 < 7 ; ibit2++)
	{
		REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit2<<8));
		if ( bInput == TRUE)

			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit2]));
		else
				REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit2]));

			// Set DM too.
		if ( bInput == FALSE)
		{
				REG_WRITE(PHY_LANE_SEL, ilane*7 + 0x800);
				REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit2]));
		}
	}



	/*-----------------------------------------------------------------------------------------------------*/
	/* Abort will happen. Ignore it. Move vector table to RAM2 start and change abort handler              */
	/*-----------------------------------------------------------------------------------------------------*/

	// Copy Interrupt Vector to RAM2 address 0
	memcpy((void*)RAM2_BASE_ADDR, (void*)ROM_BASE_ADDR, 0x100);

	SET_REG_FIELD(FLOCKR1, FLOCKR1_RAMV, 1);

	*(UINT32 *)(RAM2_BASE_ADDR + 0x10) = 0xE25EF004 ; // 0xe25ef004 : SUBS PC, lr_abt, #4 : skip the failing command and continue after it.


	/*---------------------------------------------------------------------------------------------------------*/
	/*  Sweep                                                      */
	/*---------------------------------------------------------------------------------------------------------*/
	for (samplePoint = startScan ; samplePoint <= stopScan; samplePoint++)
	{
		NewTrimReg = IP_DQ_VALUE_TO_REG(samplePoint);

		if (((samplePoint - startScan) % 20) == 0) serial_printf("*");// heartbeat

		// Update trim value into DQ0...DQ7
		ilane = NUM_OF_LANES_SWEEP - 1;
		{
			for (ibit = 0; ibit < 7; ibit++)
			{
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));

				if ( bInput == TRUE)
					REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 |  (0x7F & NewTrimReg));
				else
					REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & NewTrimReg));

				// Set DM too.
				if ( bInput == FALSE)
				{
					REG_WRITE(PHY_LANE_SEL, ilane*7 + 0x800);
					REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & NewTrimReg));
				}

				/*-----------------------------------------------------------------------------------------*/
				/* Set other bits to previous values                               */
				/*-----------------------------------------------------------------------------------------*/
				for(ibit2 = 0 ; ibit2 < 7 ; ibit2++)
				{
					if (ibit2 != ibit) // all the other bits are set to preset value
					{
						REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit2<<8));
						if ( bInput == TRUE)
							REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit2]));
						else
							REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit2]));

						// Set DM too.
						if ( bInput == FALSE)
						{
							REG_WRITE(PHY_LANE_SEL, ilane*7 + 0x800);
							REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit2]));
						}
					}
				}
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));

				l_DQs_ECC_BitStatus[(samplePoint - startScan)%MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS] |= MC_MemStressTest(TRUE, FALSE);

				REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
				if ( bInput == TRUE)
					REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit]));
				else
					REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit]));

				// Set DM too.
				if ( bInput == FALSE)
				{
					REG_WRITE(PHY_LANE_SEL, ilane*7 + 0x800);
					REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & l_DQs_ECC_PresetTrim[ilane][ibit]));
				}

			}
		}


	}

	//-------------------------------------
	serial_printf("\n\t\t\t\t\tlane2  (ECC)\n\t\t\t\t\t6.....0\n");
	for (samplePoint = startScan; samplePoint < stopScan; samplePoint++)
	{
		if ( l_DQs_ECC_BitStatus[(samplePoint - startScan) % MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS] != 0xFFFF)
		{
			serial_printf("\treg= 0x%02X val= %3d bitwise ber= %d%d%d%d%d%d%d\n", IP_DQ_VALUE_TO_REG(samplePoint),  samplePoint,
			BINARY_PRINT7(l_DQs_ECC_BitStatus[(samplePoint - startScan)%MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS]));
		}
	}
	//-------------------------------------
	FindMidBiggestEyeBitwise_l(l_DQs_ECC_BitStatus, ARRAY_SIZE(l_DQs_ECC_BitStatus), l_DQs_ECC_BestTrim_EyeCenter, l_DQs_ECC_BestTrim_EyeSize, 7);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Add startScan as on offset to best trim (if the step is different than 1 need to change this too)   */
	/*-----------------------------------------------------------------------------------------------------*/
	for (ibit = 0; ibit < 7; ibit++)
	{
		if (l_DQs_ECC_BestTrim_EyeCenter[ibit] != (-1))
			l_DQs_ECC_BestTrim_EyeCenter[ibit] = l_DQs_ECC_BestTrim_EyeCenter[ibit] + startScan;
	}


	/*---------------------------------------------------------------------------------------------------------*/
	/* Check results and print info                                        */
	/*---------------------------------------------------------------------------------------------------------*/
	serial_printf("\n");
	ilane =  NUM_OF_LANES_SWEEP -1;

	for (ibit = 0; ibit < 7; ibit++)
	{

		/*---------------------------------------------------------------------------------------------*/
		/* Check if there is a valid eye (more then 5).  If not: use the original value:           */
		/*---------------------------------------------------------------------------------------------*/
		if  (l_DQs_ECC_BestTrim_EyeSize[ibit] < 5)
		{
			serial_printf(KNRM "\tlane= %d; bit= %d; DQs ECC no eye center or eye too small (%d), use the original value %d\n" KNRM,
				ilane,
				ibit,
				l_DQs_ECC_BestTrim_EyeSize[ibit],
				( IP_DQ_REG_TO_VALUE( l_DQs_ECC_PresetTrim[ilane][ibit])));

			l_DQs_ECC_BestTrim_EyeCenter[ibit] = (IP_DQ_REG_TO_VALUE( l_DQs_ECC_PresetTrim[ilane][ibit]) );
			status = DEFS_STATUS_HARDWARE_ERROR;
		}
		else
		{
			serial_printf("\tDQs lane= %d; bit= %d; eye size= %d; eye center= %d (was val= %d; diff= %d)\n",
				ilane,
				ibit,
				l_DQs_ECC_BestTrim_EyeSize[ibit],
				l_DQs_ECC_BestTrim_EyeCenter[ibit],
				(IP_DQ_REG_TO_VALUE( l_DQs_ECC_PresetTrim[ilane][ibit])),
				( l_DQs_ECC_BestTrim_EyeCenter[ibit] -  IP_DQ_REG_TO_VALUE( l_DQs_ECC_PresetTrim[ilane][ibit])));
		}
	}

	/*---------------------------------------------------------------------------------------------------------*/
	/*  Set new values for input trim                                          */
	/*---------------------------------------------------------------------------------------------------------*/
	ilane = NUM_OF_LANES_SWEEP -1 ;
	if ( bInput == TRUE)
	{
		sum = 0;
		for (ibit = 0; ibit < 7; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, (0x80 |  ( IP_DQ_VALUE_TO_REG(l_DQs_ECC_BestTrim_EyeCenter[ibit]))));
			// calc sum for average ( goes to DM):
			if (l_DQs_ECC_BestTrim_EyeCenter[ibit] != -1)
				sum +=     IP_DQ_VALUE_TO_REG(l_DQs_ECC_BestTrim_EyeCenter[ibit]);
		}
		sum = DIV_ROUND(sum, 7);       // sum is int, not UINT.. fixed around func; add +4  ( find the average of the bits)

		/*---------------------------------------------------------------------------------------------*/
		/* Rest of the bits: put the average of the first 4 bits                       */
		/*---------------------------------------------------------------------------------------------*/
		for (ibit = 7; ibit < 8; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, (0x80 |  sum));

		}
	}
	else // output sweep
	{
		// on output: set the average for DM since don't have per bit control:

		sum = 0;
		for (ibit = 0 ; ibit < 7; ibit++)
		{
			// Set the best values found by sweep back to the registers:
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
			REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  ( IP_DQ_VALUE_TO_REG(l_DQs_ECC_BestTrim_EyeCenter[ibit])));

			// calc sum for average ( goes to DM):
			sum +=   IP_DQ_VALUE_TO_REG(l_DQs_ECC_BestTrim_EyeCenter[ibit]);
		}
		sum = DIV_ROUND(sum, 7);       // sum is int, not UINT.. fixed around func; add +4  ( find the average of the bits)

		/*---------------------------------------------------------------------------------------------*/
		/* Rest of the bits: put the average of the first 4 bits + DM                      */
		/*---------------------------------------------------------------------------------------------*/
		for (ibit = 7; ibit < 9; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
			REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, (0x80 |  sum));
		}
	}

	// dummy accsess to DDR (07/AUG/2016)
	IOW32 (0x1000, IOR32(0x1000));

	REG_WRITE(PHY_LANE_SEL, 0);

	// return to ROM code vector table:
	SET_REG_FIELD(FLOCKR1, FLOCKR1_RAMV, 0);

	return DEFS_STATUS_OK;
}

//   ECC SWEEP END


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Sweep_DQs_Trim_l                                           */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine sweep DQs delay and set to an optimized value in the middle of eye center */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS Sweep_DQs_Trim_l (BOOLEAN bInput)
{
	const static int startScan = -62;
	const static int stopScan  = 62 ;

	const UINT32     MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS = (stopScan - startScan + 1);
	const int        NUM_OF_LANES_SWEEP = 2; // we do not support lane-2 (ECC) sweep.
	int          samplePoint = 0;
	UINT32       ilane ,ibit;
	UINT32       NewTrimReg;
	UINT16       g_Table_Y_BitStatus [MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS]; // 16bit for 16 bit of data. each bit is 0 for pass or 1 for fail.
	UINT32       g_PresetTrim[NUM_OF_LANES_SWEEP][8];
	int          g_Table_Y_BestTrim_EyeCenter[16], g_Table_Y_BestTrim_EyeSize[16];
	int          sum = 0;
	int      Separation;

	DEFS_STATUS      status = DEFS_STATUS_OK;

	if ( bInput == TRUE)
	serial_printf("\n>Sweep Input DQn:\n");
	else
	serial_printf("\n>Sweep Output DQn:\n");

	memset(g_Table_Y_BitStatus, 0xFF, sizeof(g_Table_Y_BitStatus));


	/*---------------------------------------------------------------------------------------------------------*/
	/*  Get default values (of all lanes)                                      */
	/*---------------------------------------------------------------------------------------------------------*/
	for (ilane = 0 ; ilane < NUM_OF_LANES_SWEEP  ; ilane++)
	{
		//serial_printf("lane%d:", ilane);
		for (ibit=0; ibit<8; ibit++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));

			// keep register value
			if ( bInput == TRUE)
			g_PresetTrim[ilane][ibit] = READ_REG_FIELD(IP_DQ_DQS_BITWISE_TRIM, IP_DQ_DQS_BITWISE_TRIM_ip_dq_dqs_bitwise_trim_reg);
			else
			g_PresetTrim[ilane][ibit] = READ_REG_FIELD(OP_DQ_DM_DQS_BITWISE_TRIM, OP_DQ_DM_DQS_BITWISE_TRIM_op_dq_dm_dqs_bitwise_trim_reg);

			// serial_printf(" %#010lx", g_PresetTrim[ilane][ibit] );
		}
		//serial_printf("\n");
	}

	/*---------------------------------------------------------------------------------------------------------*/
	/*  Sweep                                                      */
	/*---------------------------------------------------------------------------------------------------------*/
	for (samplePoint = startScan ; samplePoint <= stopScan; samplePoint++)
	{
		NewTrimReg = IP_DQ_VALUE_TO_REG(samplePoint);

		// Update trim value into DQ0...DQ7
		for (ilane = 0; ilane < NUM_OF_LANES_SWEEP ; ilane++)
		{
			for (ibit = 0; ibit < 8; ibit++)
			{
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));

				if ( bInput == TRUE)
				REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 |  (0x7F & NewTrimReg));
				else
				REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & NewTrimReg));
			}
			// Set DM too.
			if ( bInput == FALSE)
			{
				REG_WRITE(PHY_LANE_SEL, ilane*7 + 0x800);
				REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  (0x7F & NewTrimReg));
			}
		}

		// dummy accsess to DDR (07/AUG/2016)
		IOW32 (0x1000,IOR32(0x1000));

		g_Table_Y_BitStatus[(samplePoint - startScan)%MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS] = MC_MemStressTest(FALSE, FALSE);
	}

	//-------------------------------------
	serial_printf("\t\t\t\t\tlane1    lane0\n");
	serial_printf("\t\t\t\t\t7... ...0    7... ...0\n");
	Separation = TRUE;
	for (samplePoint = startScan; samplePoint < stopScan; samplePoint++)
	{
		if ( g_Table_Y_BitStatus[(samplePoint - startScan)%MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS] != 0xFFFF)
		{
			serial_printf("\treg= 0x%02X val= %3d bitwise ber= %d%d%d%d %d%d%d%d    %d%d%d%d %d%d%d%d\n", IP_DQ_VALUE_TO_REG(samplePoint),  samplePoint,
			BINARY_PRINT16(g_Table_Y_BitStatus[(samplePoint - startScan)%MC_DQ_DQS_BITWISE_TRIM_SAMPLING_POINTS]));
			Separation = FALSE;
		}
		else if (Separation==FALSE)
		{
			serial_printf("\treg= 0x%02X val= %3d -----\n", IP_DQ_VALUE_TO_REG(samplePoint),  samplePoint);
			Separation = TRUE;
		}
	}
	//-------------------------------------
	FindMidBiggestEyeBitwise_l(g_Table_Y_BitStatus, ARRAY_SIZE(g_Table_Y_BitStatus), g_Table_Y_BestTrim_EyeCenter, g_Table_Y_BestTrim_EyeSize, 16);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Add startScan as on offset to best trim (if the step is different than 1 need to change this too)   */
	/*-----------------------------------------------------------------------------------------------------*/
	for(ibit = 0; ibit < 16; ibit++)
	{
		g_Table_Y_BestTrim_EyeCenter[ibit] = g_Table_Y_BestTrim_EyeCenter[ibit] + startScan;
	}


	/*---------------------------------------------------------------------------------------------------------*/
	/* Check results and print info                                        */
	/*---------------------------------------------------------------------------------------------------------*/
	serial_printf("\n");
	ilane = 0;

	for(ibit = 0; ibit < 16; ibit++)
	{
		if( ibit == 8)
		{
			ilane++;
			serial_printf("\n");
		}

		/*---------------------------------------------------------------------------------------------*/
		/* Check if there is a valid eye (more then 5).  If not: use the original value:           */
		/*---------------------------------------------------------------------------------------------*/
		if  (g_Table_Y_BestTrim_EyeSize[ibit] < 5)
		{
			serial_printf(KRED "\tlane= %d; bit= %d; DQs: no eye center or eye too small, use the original value %d\n" KNRM, ilane, (ibit % 8), ( IP_DQ_REG_TO_VALUE( g_PresetTrim[ilane][ibit % 8])));
			g_Table_Y_BestTrim_EyeCenter[ibit] = (IP_DQ_REG_TO_VALUE( g_PresetTrim[ilane][ibit % 8]) );
			status = DEFS_STATUS_HARDWARE_ERROR;
		}
		else
		{
			serial_printf("\tlane= %d; bit= %d; eye size= %d; eye center= %d (was val= %d; diff= %d)\n",
			ilane,
			(ibit % 8),
			g_Table_Y_BestTrim_EyeSize[ibit],
			g_Table_Y_BestTrim_EyeCenter[ibit],
			(IP_DQ_REG_TO_VALUE( g_PresetTrim[ilane][ibit % 8])),
			(g_Table_Y_BestTrim_EyeCenter[ibit] -  IP_DQ_REG_TO_VALUE( g_PresetTrim[ilane][ibit % 8])));
		}
	}

	/*---------------------------------------------------------------------------------------------------------*/
	/*  Set new values for input trim                                          */
	/*---------------------------------------------------------------------------------------------------------*/
	if ( bInput == TRUE)
	{
		for (ilane = 0; ilane < NUM_OF_LANES_SWEEP ; ilane++)
		{
			for (ibit = 0; ibit < 8; ibit++)
			{
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
				REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, (0x80 |  ( IP_DQ_VALUE_TO_REG(g_Table_Y_BestTrim_EyeCenter[(ilane*8 + ibit) % sizeof(g_Table_Y_BestTrim_EyeCenter) ]))));
			}
		}
	}
	else // output sweep
	{
		// on output: set the average since don't have per bit control:
		for (ilane = 0; ilane < NUM_OF_LANES_SWEEP; ilane++)
		{
			sum = 0;
			for (ibit = 0 ; ibit < 8 ; ibit++)
			{
				// Set the best values found by sweep back to the registers:
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+(ibit<<8));
				REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 |  ( IP_DQ_VALUE_TO_REG(g_Table_Y_BestTrim_EyeCenter[(ilane*8 + ibit) % sizeof(g_Table_Y_BestTrim_EyeCenter) ])));

				// calc sum for average ( goes to DM):
				sum +=     IP_DQ_VALUE_TO_REG(g_Table_Y_BestTrim_EyeCenter[ilane*8 + ibit]);
			}
			sum = DIV_ROUND(sum, 8);           // sum is int, not UINT.. fixed around func; add +4  ( find the average of the bits)
			REG_WRITE(PHY_LANE_SEL, ilane*7 + 0x800);
			REG_WRITE(OP_DQ_DM_DQS_BITWISE_TRIM, 0x80 | sum);
		}
	}

	// dummy accsess to DDR (07/AUG/2016)
	IOW32 (0x1000, IOR32(0x1000));

	REG_WRITE(PHY_LANE_SEL, 0);

	return status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_MemStressTest                                       */
/*                                                     */
/* Parameters:      bECC : if true, mem test is for ECC lane. Can't sweep it directly, must check ber bit  */
/* Returns:     bitwise error in UINT16 (one UINT8 for each lane)                      */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine does memory test with a list of patterns. Used for sweep.         */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 MC_MemStressTest (BOOLEAN bECC, BOOLEAN bQuick )
{
	UINT16  ber = 0;


	const UINT32   aPtrn[6] = {0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA, 0x33333333, 0x99999999};
	const UINT32   iPtrnSize = 0x100;
	const UINT     iWalkers = 0x10; // lior - there is no point to shift 0x0001 more then 16 steps ;

	UINT32   iCnt1 = 0;
	UINT32   iCnt2 = 0;

#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
	extern UINT32 BOOTBLOCK_IMAGE_START;
	extern UINT32 BOOTBLOCK_IMAGE_END;
#endif

	volatile UINT16   *dstAddrStart = (UINT16 *)0x1000;

	volatile UINT32   *dstAddrStartLong = (UINT32 *)0x1000;

	/*-----------------------------------------------------------------------------------------------------*/
	/*                    Fixed pattern test                           */
	/*-----------------------------------------------------------------------------------------------------*/
	for (iCnt1 = 0; iCnt1 < ARRAY_SIZE(aPtrn); iCnt1++)
	{
		dstAddrStart = (UINT16 *)0x1000;
		dstAddrStartLong = (UINT32 *)0x1000;
		IOW32(MC_BASE_ADDR + 4*INT_ACK_ADDR, 0x7ffffff);
		IOW32(MC_BASE_ADDR + 4*INT_ACK_ADDR, 0x0);
		/*-----------------------------------------------------------------------------------------------------*/
		/*  Write pattern                                              */
		/*-----------------------------------------------------------------------------------------------------*/
		// memset((void *)dstAddrStart, (UINT8)aPtrn[iCnt1], iPtrnSize);
		for( iCnt2 = 0; iCnt2 <  iPtrnSize ; iCnt2 += 4)
		{
			*dstAddrStartLong = (UINT32)aPtrn[iCnt1];
			IOW32(MC_BASE_ADDR + 4*INT_ACK_ADDR, 0x7ffffff);
			IOW32(MC_BASE_ADDR + 4*INT_ACK_ADDR, 0x0);
			dstAddrStartLong++;
		}

		dstAddrStart = (UINT16 *)0x1000;
		dstAddrStartLong = (UINT32 *)0x1000;
		/*-----------------------------------------------------------------------------------------------------*/
		/* Verify pattern                                              */
		/*-----------------------------------------------------------------------------------------------------*/
		for( iCnt2 = 0; iCnt2 <  iPtrnSize ; iCnt2 += 2)
		{
			if (bECC == TRUE)
			{
				ber |= GetECCSyndrom(*dstAddrStart);
			}
			else
			{
				ber |= *dstAddrStart ^ aPtrn[iCnt1];
			}
			dstAddrStart++;
		}

		if((iCnt1 == 1)  && (bQuick == TRUE))
		{
			return ber;
		}

		if( ber == 0xFFFF)
		{
			return ber;
		}

	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Walking-1 pattern test                                          */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/*  Write pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		*dstAddrStart = (0x0001 << (iCnt2));
		dstAddrStart++;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Verify pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if (bECC == TRUE)
		{
			ber |= GetECCSyndrom(*dstAddrStart);
		}
		else
		{
			ber |= *dstAddrStart ^ (0x0001 << (iCnt2));
		}
		dstAddrStart++;
	}
	if( ber == 0xFFFF)
	{
		return ber;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Walking-0 pattern test                                            */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/*  Write pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		*dstAddrStart = 0xFFFF - (0x0001 << (iCnt2));
		dstAddrStart++;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Verify pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if (bECC == TRUE)
		{
			ber |= GetECCSyndrom(*dstAddrStart);
		}
		else
		{
			ber |= *dstAddrStart ^ (0xFFFF - (0x0001 << (iCnt2)));
		}
		dstAddrStart++;
	}
	if( ber == 0xFFFF)
	{
		return ber;
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/*                   Walking dead pattern test                     */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/*  Write pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if ((iCnt2 % 2) == 0)
		{
			*dstAddrStart = 0xFFFF - (0x0001 << (iCnt2/2));
		}
		else
		{
			*dstAddrStart = (0x0001 << (iCnt2/2));
		}
		dstAddrStart++;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Verify pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if (bECC == TRUE)
		{
			ber |= GetECCSyndrom(*dstAddrStart);
		}
		else
		{
			if ((iCnt2 % 2) == 0)
			{
				ber |= *dstAddrStart ^ (0xFFFF - (0x0001 << (iCnt2/2)));
			}
			else
			{
				ber |= *dstAddrStart ^ (0x0001 << (iCnt2/2));
			}
		}
		dstAddrStart++;
	}
	if( ber == 0xFFFF)
	{
		return ber;
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/*                    Walking dead1 pattern test                       */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/*  Write pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if ((iCnt2 % 2) == 0)
		{
			*dstAddrStart = 0xFFFF;
		}
		else
		{
			*dstAddrStart = (0x0001 << (iCnt2/2));
		}
		dstAddrStart++;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Verify pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if (bECC == TRUE)
		{
			ber |= GetECCSyndrom(*dstAddrStart);
		}
		else
		{
			if ((iCnt2 % 2) == 0)
			{
				ber |= *dstAddrStart ^ 0xFFFF;
			}
			else
			{
				ber |= *dstAddrStart ^ (0x0001 << (iCnt2/2));
			}
		}

		dstAddrStart++;
	}
	if( ber == 0xFFFF)
	{
		return ber;
	}



	/*-----------------------------------------------------------------------------------------------------*/
	/*                     Walking dead2 pattern test                      */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/*  Write pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if ((iCnt2 % 2) == 0)
		{
			*dstAddrStart = 0xFFFF - (0x0001 << (iCnt2/2));
		}
		else
		{
			*dstAddrStart = 0;
		}
		dstAddrStart++;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Verify pattern                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;
	for( iCnt2 = 0; iCnt2 <  iWalkers ; iCnt2++)
	{
		if (bECC == TRUE)
		{
			ber |= GetECCSyndrom(*dstAddrStart);
		}
		else
		{
			if ((iCnt2 % 2) == 0)
			{
				ber |= *dstAddrStart ^ (0xFFFF - (0x0001 << (iCnt2/2)));
			}
			else
			{
				ber |= *dstAddrStart ^ 0;
			}
		}
		dstAddrStart++;
	}
	if( ber == 0xFFFF)
	{
		return ber;
	}


	// Lior: move bootblock test to last test since this test take too much time (first it copy then it check)
#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
	/*-----------------------------------------------------------------------------------------------------*/
	/*  Use bootblock source code as a pseudo random pattern to test the DDR                   */
	/*  Copy BB code segment to the DDR                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;

	for( iCnt2 = 0; iCnt2 <  (&BOOTBLOCK_IMAGE_END - &BOOTBLOCK_IMAGE_START) ; iCnt2 += sizeof(UINT16))
	{
		*dstAddrStart = *(UINT16*)(&BOOTBLOCK_IMAGE_START + iCnt2);
		dstAddrStart++;
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Compare BB code segment to the DDR                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	dstAddrStart = (UINT16 *)0x1000;

	for( iCnt2 = 0; iCnt2 < (&BOOTBLOCK_IMAGE_END - &BOOTBLOCK_IMAGE_START) ; iCnt2 += sizeof(UINT16))
	{
		if (bECC == TRUE)
		{
			ber |= GetECCSyndrom(*(UINT16*)(&BOOTBLOCK_IMAGE_START + iCnt2));
		}
		else
		{
			ber |= *dstAddrStart ^ *(UINT16*)(&BOOTBLOCK_IMAGE_START + iCnt2);
		}

		// lior: save time and return as soos as posiable when there is total bits error (don't wait to verify to complete)
		if( ber == 0xFFFF)
		return ber;


		dstAddrStart++;

	}
#endif

	return ber;



}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FindMidBiggestEyeBitwise_l                                 */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine finds the middle of the eye on the array received from MC_MemStressTest    */
/*---------------------------------------------------------------------------------------------------------*/

// TBD: there was a bug in center calculation when there are several eyes !!!.
static void FindMidBiggestEyeBitwise_l (const UINT16 *SweepDataBuff, const int BuffSize, int *pEyeCenter, int *pEyeSize, int bits)
{
	int start   = -1;
	int stop    = -1;
	int eyeSize = -1;

	int  bufInd = 0;
	int  bitInd = 0;
	int  bitStep = 1;  // in bitwise sweep find the biggest eye size on every bit.
	// when sweeping a value that is common to entire lane , like DQS,
	// use bit zero and bit 8 value of the UINT16 of data buf.



	for(bitInd = 0; bitInd < bits; bitInd += bitStep)
	{
		start = -1;
		stop = -1;
		eyeSize = -1;

		for (bufInd = 0; bufInd < BuffSize; bufInd++)
		{
			if( (SweepDataBuff[bufInd] & ((UINT16)0x0001 << bitInd)) == 0)
			{// pass point
				if(start == (-1))
				{   // found a good start point
					start = bufInd;
					//serial_printf("[%d",bufInd);
				}
			}
			else
			{ // fail point
				if (start != (-1) )
				{   // found fail point (after found a good start point);
					//serial_printf("...%d]",bufInd);
					if ((bufInd - start) > eyeSize)
					{
						// new eye size is bigger then last found eye
						stop = bufInd - 1;  // last good point
						eyeSize = bufInd - start;
						//serial_printf("*");
					}
					start = -1;
				}
			}
		}

		/*----------------------------------------------------------------------------------------------*/
		/*      Close an unclosed eyes                                  */
		/*----------------------------------------------------------------------------------------------*/
		if ( start !=  -1)
		{
			// need to close at the end after found a good start point;
			serial_printf_debug("Close the unclosed eyes : bit %d bufInd=%d start=%d\n",bitInd, bufInd, start );
			if ( ((BuffSize-1) - start) >eyeSize)
			{
				// new eye size is bigger then last found eye
				stop    = (BuffSize-1) - 1;
				eyeSize = (BuffSize-1) - start;
				//serial_printf("*");
			}
			start = -1;
		}

		/*---------------------------------------------------------------------------------------------------------*/
		/*  write results to pEyeCenter and pEyeSize                                   */
		/*---------------------------------------------------------------------------------------------------------*/

		if (eyeSize == (-1))
		{
			/*------------------------------------------------------------------------------------------------*/
			/* No eye                                                                      */
			/*------------------------------------------------------------------------------------------------*/
			serial_printf_debug(KRED ">lane%d, bit%d: No eye !\n" KNRM , ((bitInd>7)?1:0), (bitInd %8));

			// Clear pEyeCenter and pEyeSize
			pEyeCenter[bitInd] = -1;
			pEyeSize[bitInd] =  0;
		}
		else
		{
			// calculate eye center
			pEyeSize[bitInd] = eyeSize;
			pEyeCenter[bitInd] = stop - DIV_CEILING(eyeSize, 2) + 1;

		}
		serial_printf_debug(">lane%d, bit%d: eye=%d center=%d\n", ((bitInd>7)?1:0), (bitInd %8),  pEyeSize[bitInd] , pEyeCenter[bitInd] );

	}

	return;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_CalcBestTrim_l                                      */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine find the gap between the dlls                         */
/*---------------------------------------------------------------------------------------------------------*/
static INT32 MC_CalcBestTrim_l (INT32 dlls_trim_l)
{
	UINT32 ilane;

	INT32  trim = 0x3F;  // start with max value.

	serial_printf(">Calc Best Trim:\n");

	for (ilane = 0 ; ilane < NUM_OF_LANES ; ilane++)
	{
		REG_WRITE(PHY_LANE_SEL,(ilane * 6));

		serial_printf("\tlane %d, trim_2 = %d\t", ilane, READ_REG_FIELD(PHY_DLL_TRIM_2, PHY_DLL_TRIM_2_dlls_trim_2));
		trim =  MIN(trim, (INT32)(READ_REG_FIELD(PHY_DLL_TRIM_2, PHY_DLL_TRIM_2_dlls_trim_2)));
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate DLL trim according to PHY_DLL_TRIM_2_dlls_trim_2                      */
	/*-----------------------------------------------------------------------------------------------------*/
	dlls_trim_l = 3 - (trim - dlls_trim_l);

	if(dlls_trim_l < 0)  dlls_trim_l = 0;

	dlls_trim_l = 0x3F & dlls_trim_l;

	serial_printf(" ==> %d \n", dlls_trim_l);

	return dlls_trim_l;

}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_Init_l                                          */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine init the memory controller                        */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS MC_Init_l (void)
{
	int Registerindex = 0;
	volatile UINT32 TimeOut = 0;
	UINT32 ddr_freq = CLK_GetPll1Freq();

	serial_printf(">Initializing Mem Controller.\n");
	for (Registerindex = 0; Registerindex < (sizeof(Denali_Ctl_Initialization)/sizeof(Denali_Ctl_Initialization[0])); Registerindex++)
	{
		IOW32(MC_BASE_ADDR + (Registerindex*4), Denali_Ctl_Initialization[Registerindex]);
	}


	// update PHY according to speed:
#ifndef DOUBLE_REFRESH_RATE
	IOW32(MC_BASE_ADDR + (51*4), (78*(ddr_freq/10000000))  );  // 7800 / T[ns]  :
#else
	IOW32(MC_BASE_ADDR + (51*4), (39*(ddr_freq/10000000))  );  // 7800 / T[ns]  :
#endif

	serial_printf(KGRN ">Refresh rate is %#010lx (%d * (ddr/10000000)\n" KNRM, IOR32(MC_BASE_ADDR + (51*4)), IOR32(MC_BASE_ADDR + (51*4))/(ddr_freq/10000000) );

	// Start Initialization Seq
	REG_SET_FIELD(MC_BASE_ADDR + ((UINT32)(START_ADDR) * 4), START_WIDTH, START_OFFSET, 1);

	// wait for MC initialization complete.
	TimeOut = 60000;
	MC_Sleep_l(1); // ~ 10usec
	while (((IOR32(MC_BASE_ADDR + ((UINT32)(INT_STATUS_ADDR) * 4))) & ((UINT32)0x01<<8)) == 0)
	{
		if (TimeOut == 0)
		{
			serial_printf(KRED ">LOG_ERROR_MC_INIT_TIMEOUT\n" KNRM);
			return DEFS_STATUS_FAIL;
		}
		TimeOut--;
	}

	return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_Init_test_l                                     */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine checks that the MC registers are set to the predefined values.        */
/*          in secondary reset , if the registers are not OK, the BB will restart the MC       */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS MC_Init_test_l (void)
{
	int Registerindex = 0;
	volatile UINT32 TimeOut = 0;
	UINT32 ddr_freq = CLK_GetPll1Freq();
	UINT32 read_value = 0;
	DEFS_STATUS ret_status = DEFS_STATUS_OK;

	serial_printf(">Testing Mem Controller init values\n");

	// This test will read registers to verify that they are still at their preset value.
	// Registers that contain status bits that are changed on the fly are skipped:
	for (Registerindex = 20; Registerindex < (sizeof(Denali_Ctl_Initialization)/sizeof(Denali_Ctl_Initialization[0])); Registerindex++)
	{
		read_value = IOR32(MC_BASE_ADDR + (Registerindex*4));
		if(read_value !=  Denali_Ctl_Initialization[Registerindex])
		{
			if (Registerindex == 51)
			{
#ifndef DOUBLE_REFRESH_RATE
				if (read_value != (78*(ddr_freq/10000000)))
#else
				if (read_value != (39*(ddr_freq/10000000)))
#endif
				{
					serial_printf(">_Wrong value at %#010lx, reg %d, Expected: %#010lx, read: %#010lx\n",
					MC_BASE_ADDR + (Registerindex*4), Registerindex,  read_value, Denali_Ctl_Initialization[Registerindex]);
					ret_status = DEFS_STATUS_AUTHENTICATION_FAIL;
				}
				else
				{
					serial_printf(">Refresh at %#010lx, reg %d, Expected: %#010lx, read: %#010lx\n",
					MC_BASE_ADDR + (Registerindex*4), Registerindex,  read_value, Denali_Ctl_Initialization[Registerindex]);

				}

			}
			else if ((Registerindex != 49 ) &&
					(Registerindex != 51 ) &&
					(Registerindex != 60 ) &&
					(Registerindex != 70 ) &&
					(Registerindex != 85 ) &&
					(Registerindex != 116) &&
					(Registerindex != 119) &&
					(Registerindex != 120) &&
					(Registerindex != 149) &&
					(Registerindex != 150) &&
					(Registerindex != 163) &&
					(Registerindex != 164) &&
					(Registerindex != 165) &&
					(Registerindex != 166) &&
					(Registerindex != 205) &&
					(Registerindex != 206) &&
					(Registerindex != 207) &&
					(Registerindex != 208) &&
					(Registerindex != 209) &&
					(Registerindex != 210) &&
					(Registerindex != 211) &&
					(Registerindex != 212) &&
					(Registerindex != 213) &&
					(Registerindex != 214))
			{

				serial_printf(">Wrong value at %#010lx. reg %d, Expected: %#010lx, read: %#010lx\n",
				MC_BASE_ADDR + (Registerindex*4), Registerindex, Denali_Ctl_Initialization[Registerindex], read_value);
				ret_status = DEFS_STATUS_AUTHENTICATION_FAIL;
			}
		}
	}

	if (ret_status == DEFS_STATUS_AUTHENTICATION_FAIL)
	{
		serial_printf("> MC registers test failed!\n");
#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
		CFG_HeartBeatToggle(3);    //  This function never returns! it blinks forever. This is for debug versions.
#endif
	}

	/* Perform a quick mem test on the DRAM . TODO: decide if to recover in case of failure. */

	if (MC_MemStressTest(FALSE, TRUE) != 0)
	{
		serial_printf("> Memory test failed!\n");
		ret_status = DEFS_STATUS_HARDWARE_ERROR;
#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
		CFG_HeartBeatToggle(4);    //  This function never returns! it blinks forever. This is for debug versions.
#endif
	}


	return ret_status;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_PHY_Init_l                                      */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:                                                */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine reset and init the memory controller and phy                  */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS MC_PHY_Init_l(void)
{
	DEFS_STATUS status ;

	// perform MC software reset
	CLK_ResetMC();

	status = phy_init_l();
	if (status != DEFS_STATUS_OK)
	return status;

	status = MC_Init_l();
	if (status != DEFS_STATUS_OK)
	return status;

	return status;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_IsDDRConfigured_l                                   */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:     TRUE:  if DDR is configured                                */
/*          FALSE: if DDR is not configured or need to be reinitialized                */
/* Side effects:    none                                           */
/* Description:                                                */
/*          Get the status of DDR Memory Controller initialization                 */
/*---------------------------------------------------------------------------------------------------------*/
static BOOLEAN MC_IsDDRConfigured_l (void)
{
	// if memory controller was already initialized do not reinitialize
	if ((1 == READ_REG_FIELD(INTCR2, INTCR2_MC_INIT)) &&
			(0 != REG_READ_FIELD(MC_BASE_ADDR + ((UINT32)(DRAM_CLASS_ADDR) * 4), DRAM_CLASS_WIDTH, DRAM_CLASS_OFFSET)))
	{
		return TRUE;
	}

	return FALSE;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_ConfigureDDR_l                                      */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:     status                                         */
/* Side effects:    none                                           */
/* Description:                                                */
/*          Set default configuration for the DDR Memory Controller                */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS MC_ConfigureDDR_l ( void )
{
	DEFS_STATUS       status =  DEFS_STATUS_OK;
	DEFS_STATUS   status_out_sweep =  DEFS_STATUS_OK;
	int       itrCnt_l = 1;
	UINT32        ilane = 0;
	int          l_BestEyeCenter[NUM_OF_LANES_MAX], l_BestEyeSize[NUM_OF_LANES_MAX];



	/*-----------------------------------------------------------------------------------------------------*/
	/* DLL trim: start at max value (26), calc the best trim, try it on the second iterration. If it fails,*/
	/* increas by 5 and retry till found a valid point.                            */
	/*-----------------------------------------------------------------------------------------------------*/
	dlls_trim_adrctl_clk  = 26; // value of 0 works in EB and SVB but not in Dell system, value of 5 works in Dell system.

	// Start with 3 lanes (with ECC)
	LANE_MASK = 7;
	NUM_OF_LANES = 3;

	if (g_mc_config & MC_CAPABILITY_IGNORE_ECC_DEVICE)
	{
		// Start with 2 lanes (no ECC training. No auto detection)
		serial_printf("Skip ECC training\n");
		LANE_MASK = 3;
		NUM_OF_LANES = 2;
	}

	do
	{
		serial_printf(">Iteration [%d]\n", itrCnt_l);

		// Init must pass at all time
		status = MC_PHY_Init_l();
		if (status != DEFS_STATUS_OK)
		return DEFS_STATUS_FAIL;

		// Write-leveling - if it fails on the second iteration, increase dlls_trim_adrctl_clk until dlls_trim_adrctl_clk return to it's start value (that pass at the first time !)
		status = phy_leveling_l(PHY_WR_LVL);
		if (status != DEFS_STATUS_OK )
		{
			if (itrCnt_l == 1)
			return DEFS_STATUS_FAIL; // failed on the very first iteration (with dlls at 26). Start again.
			else
			{
				// on every iteretion - try a higher value as starting point:
				dlls_trim_adrctl_clk = (dlls_trim_adrctl_clk + 5);
				if (dlls_trim_adrctl_clk > 26) // dll return to max. entire iteration has failed .
				return DEFS_STATUS_FAIL;
			}
		}

		if (itrCnt_l == 1)
		{
			//------------------------------------------------------------------
			// Reduce dlls_trim_adrctl_clk delay and update dlls_trim_2 accordingly
			//------------------------------------------------------------------
			// Find the shorter delay value of dlls_trim_adrctl_clk that can be use. This value depends on PCB CLK to DQSn skew for all active lanes.
			dlls_trim_adrctl_clk = (UINT32)MC_CalcBestTrim_l((INT32)dlls_trim_adrctl_clk);

			status = DEFS_STATUS_FAIL; // re-run second iteretion
		}

		itrCnt_l++;

	} while (status != DEFS_STATUS_OK);

	/*-------------------------------------------------------------------------------------------------*/
	/* VREF training and RW bit leveling                                   */
	/*-------------------------------------------------------------------------------------------------*/
	status = phy_leveling_l(PHY_VREF_TRNG|PHY_RW_BIT_LVL);


	/*--------------------------------------------------------------------*/
	/*      MC enhanced eye training                  */
	/*--------------------------------------------------------------------*/
	if(!(g_mc_config & MC_DISABLE_CAPABILITY_INPUT_DQS_ENHANCE_TRAINING))
	{
		if (status == DEFS_STATUS_OK)
		{
			Sweep_Input_DSQ_Enhanced_Training_l(FALSE);
		}

	} else {  // MC_DISABLE_CAPABILITY_INPUT_DQS_ENHANCE_TRAINING

		if (status == DEFS_STATUS_OK)
		{
			// continue with read-leveling
			status = phy_leveling_l(PHY_RD_LVL);
		}
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Run output and input delay sweep                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (status == DEFS_STATUS_OK)
	{
		status_out_sweep = Sweep_DQs_Trim_l(FALSE); // Run Parametric Sweep on WRITE side (both Output DQn and DM) and find the center for each DQn and average for each DM lane. (Note: assume DM length is average of data length)
		status = Sweep_DQs_Trim_l(TRUE); // Run Parametric Sweep on READ side and find the center for each DQn.

		// If output failed but input pass, maybe all we need is a second output sweep for fine tune the output after input was fine tuned.
		if ( (status == DEFS_STATUS_OK) && (status_out_sweep !=DEFS_STATUS_OK) )
			status = Sweep_DQs_Trim_l(FALSE);

		if (status == DEFS_STATUS_OK)
		{
			status = Sweep_Trim_Input_DQS_l(l_BestEyeCenter, l_BestEyeSize, FALSE, FALSE);
			if ((NUM_OF_LANES == NUM_OF_LANES_MAX) && (status == DEFS_STATUS_OK))
				status = Sweep_Trim_Input_DQS_l(l_BestEyeCenter, l_BestEyeSize, TRUE, FALSE);


			/*--------------------------------------------------------------------*/
			/*      MC enhanced eye training: results                             */
			/*--------------------------------------------------------------------*/

			if(!(g_mc_config & MC_DISABLE_CAPABILITY_INPUT_DQS_ENHANCE_TRAINING))
			{

				for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
				{
					serial_printf(KGRN "lane%d: inputDQS: eye size %d, DQS delay = %d\n" KNRM,
						ilane,
						l_BestEyeSize[ilane],
						l_BestEyeCenter[ilane]);

					// Check that the window after selecting is still big enough
					if (l_BestEyeSize[ilane] <  28)
					{
						serial_printf(KRED "ERROR: final inputDQS window is still too small despite the sweep\n" KNRM);
						status = DEFS_STATUS_FAIL ; // return everything from scartch.
					}
				}

				// Final set value is:
				for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
				{
					REG_WRITE(PHY_LANE_SEL,(ilane*7)+(8<<8));
					serial_printf(KGRN "lane%d: final input DQS : 0x%x\n" KNRM,
						ilane, READ_REG_FIELD(IP_DQ_DQS_BITWISE_TRIM, IP_DQ_DQS_BITWISE_TRIM_ip_dq_dqs_bitwise_trim_reg));
				}
			} // MC_DISABLE_CAPABILITY_INPUT_DQS_ENHANCE_TRAINING
		}
	}

	return status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Sweep_Input_DSQ_Enhanced_Training_l                        */
/*                                                     */
/* Parameters:      bECC                                           */
/* Returns:     status                                         */
/* Side effects:    none                                           */
/* Description:                                                */
/*          Enhanced training for Input DQS                            */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS Sweep_Input_DSQ_Enhanced_Training_l(BOOLEAN bECC)
{
	DEFS_STATUS     status = DEFS_STATUS_OK;
	UINT        laneStart = 0;
	UINT        laneStop = NUM_OF_LANES_MAX - 1;
	UINT        ilane = 0;
	UINT32      input_DQS_delay_cur;

	int     l_inputDelayTableEyeSize[NUM_OF_LANES_MAX][0x40];
	//int       l_bestInputDQS[NUM_OF_LANES_MAX] = {0,0,0};
	//int       l_bestWindowInputDQS[NUM_OF_LANES_MAX] = {0,0,0};

	int     l_InputDQS[NUM_OF_LANES_MAX] = {0,0,0};
	int     l_WindowInputDQS[NUM_OF_LANES_MAX] = {0,0,0};

	int          l_bestInputDQS[8*NUM_OF_LANES_MAX], l_bestWindowInputDQS[8*NUM_OF_LANES_MAX];

	if (bECC == TRUE)
	{
		laneStart  = NUM_OF_LANES_MAX - 1;
		laneStop   = NUM_OF_LANES_MAX;
		serial_printf("Enhanced sweep input DQS delay using read leveling for best eye size, ECC lane\n");
	}
	else
	{
		// first call to this function iswithou ECC.
		memset(l_inputDelayTableEyeSize, -1, sizeof(l_inputDelayTableEyeSize));
		memset(l_bestWindowInputDQS,     -1, sizeof(l_bestWindowInputDQS));
		memset(l_bestInputDQS,           -1, sizeof(l_bestInputDQS));
		serial_printf("Enhanced sweep input DQS delay using read leveling for best eye size\n");
	}



	/*---------------------------------------------------------------------------------------------*/
	/* Sweep input DQS delay against window size                                */
	/*---------------------------------------------------------------------------------------------*/
	for (input_DQS_delay_cur = 0x8; input_DQS_delay_cur < 0x40 ; input_DQS_delay_cur++)
	{

		/*-----------------------------------------------------------------------------------------*/
		/* Init DQS input delay                                                                */
		/*-----------------------------------------------------------------------------------------*/
		for (ilane = laneStart; ilane < laneStop; ilane++)
		{
			REG_WRITE(PHY_LANE_SEL,(ilane*7) + 0x800);
			REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 | (0x7F & input_DQS_delay_cur)); // input DQS
		}


		// dummy accsess to DDR
		IOW32 (0x1000,IOR32(0x1000));
		REG_WRITE(PHY_LANE_SEL, 0);

		/*-----------------------------------------------------------------------------------------*/
		/* Read leveling                                                                            */
		/*-----------------------------------------------------------------------------------------*/
		status = phy_leveling_l(PHY_RD_LVL_FAST);

		/*-----------------------------------------------------------------------------------------*/
		/* Sweep input DQS in order to get window size after read leveling                      */
		/*-----------------------------------------------------------------------------------------*/
		if (status == DEFS_STATUS_OK)
		{
			status = Sweep_Trim_Input_DQS_l(l_InputDQS, l_WindowInputDQS, bECC, TRUE);
			for (ilane = laneStart; ilane < laneStop; ilane++)
				serial_printf_debug("enh sweep res: lane%d bestin %d size %d\n",
					ilane, l_InputDQS[ilane], l_WindowInputDQS[ilane]);
		}

		else // no valid window:
		{
			serial_printf("Enhanced training: read leveling failed\n");
			for (ilane = laneStart; ilane < laneStop; ilane++)
				l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur] = 0;
		}


		// save the eye size results in l_inputDelayTableEyeSize
		for (ilane = laneStart; ilane < laneStop ; ilane++)
		{
			l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur] = l_WindowInputDQS[ilane];
			serial_printf_debug("enh sweep res set: lane%d keep %d val %d\n",
					ilane, l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur], l_bestWindowInputDQS[ilane]);
		}



		/*-----------------------------------------------------------------------------------------*/
		/* check what is best value for DQS: Check which input DQS value gives the maximum window   */
		/* Need to find 5 consequtive measures above 28 and after max                               */
		/*-----------------------------------------------------------------------------------------*/
		for (ilane = laneStart; ilane < laneStop; ilane++)
		{
			/* find max :*/
			if (l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur] > l_bestWindowInputDQS[ilane])
			{
				l_bestWindowInputDQS[ilane] = l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur];
				l_bestInputDQS[ilane] = input_DQS_delay_cur ;
			}
		}

		/*-----------------------------------------------------------------------------------------*/
		/* Read leveling  (quick with no prints)                                                    */
		/*-----------------------------------------------------------------------------------------*/
		status = phy_leveling_l(PHY_RD_LVL_FAST);
	}

	/*---------------------------------------------------------------------------------------------*/
	/* Check results and make sure we have 5 consequtive spots after max                            */
	/*---------------------------------------------------------------------------------------------*/
	for (ilane = laneStart; ilane < laneStop; ilane++)
	{
		for (input_DQS_delay_cur = l_bestInputDQS[ilane]; input_DQS_delay_cur < (0x40 - 5) ; input_DQS_delay_cur++)
		{
			/* 5 consecutive from max above 28 window (matches 350 ps jitter tolerance):*/
			if  (   (l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur] < 28) ||
					(l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur+1] < 28) ||
					(l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur+2] < 28) ||
					(l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur+3] < 28) ||
					(l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur+4] < 28))
			{
				serial_printf("move lane%d, iDQS = %d, eye size = %d\n", ilane, input_DQS_delay_cur, l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur]);
			}
			else // found it, take the middle:
			{
				l_bestInputDQS[ilane] = input_DQS_delay_cur + 2;
				l_bestWindowInputDQS[ilane] = l_inputDelayTableEyeSize[ilane][l_bestInputDQS[ilane]];
				serial_printf_debug("lane%d, final best iDQS = %d, eye size = %d\n", ilane, l_bestInputDQS[ilane], l_inputDelayTableEyeSize[ilane][l_bestInputDQS[ilane]]);
				break; // found it ( per lane ).
			}
		}
	}

	//Print input DQS delay vs. eye size sweep results
	serial_printf(KGRN "Input DQS and eye size (per lane):\n" KNRM "in delay:  " );
	for (input_DQS_delay_cur = 0x8; input_DQS_delay_cur < 0x40 ; input_DQS_delay_cur++)
	{
		serial_printf("%02d ", input_DQS_delay_cur);
	}

	for (ilane = laneStart; ilane < laneStop; ilane++)
	{
		serial_printf("\neyesize L%d:", ilane);
		for (input_DQS_delay_cur = 0x8; input_DQS_delay_cur < 0x40 ; input_DQS_delay_cur++)
		{
			if ( input_DQS_delay_cur == l_bestInputDQS[ilane])
				serial_printf(KGRN);
			serial_printf("%02d " KNRM, l_inputDelayTableEyeSize[ilane][input_DQS_delay_cur]);
		}
	}

	serial_printf("\n");

	/*---------------------------------------------------------------------------------------------*/
	/* Init DQS input delay to selected value + 2 : leave a gap from "shen masor"           */
	/*---------------------------------------------------------------------------------------------*/
	for (ilane = laneStart; ilane < laneStop; ilane++)
	{

		REG_WRITE(PHY_LANE_SEL,(ilane*7)+(8<<8));
		REG_WRITE(IP_DQ_DQS_BITWISE_TRIM, 0x80 | 0x7F & (l_bestInputDQS[ilane])); // input DQS
		serial_printf(KGRN "Enhanced results: Lane%d: Set input DQS delay %d\n" KNRM, ilane, l_bestInputDQS[ilane]);
	}

	// dummy accsess to DDR (07/AUG/2016)
	IOW32 (0x1000,IOR32(0x1000));
	REG_WRITE(PHY_LANE_SEL, 0);

	/*-----------------------------------------------------------------------------------------*/
	/* Read leveling                                    */
	/*-----------------------------------------------------------------------------------------*/
	status = phy_leveling_l(PHY_RD_LVL);



	return status;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_ConfigureDDR                                    */
/*                                                     */
/* Parameters:      UINT8 mc_config : value from  BOOTBLOCK_Get_MC_config (BB header)              */
/* Returns:     status                                         */
/* Side effects:    none                                           */
/* Description:                                                */
/*          Set default configuration for the DDR Memory Controller                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS MC_ConfigureDDR ( UINT8 mc_config )
{
	DEFS_STATUS   status =  DEFS_STATUS_OK;
	int       try;
	UINT32        dramSize = 0;
	UINT32        iCnt = 0;
	DEFS_STATUS   status_out_sweep =  DEFS_STATUS_OK;
	DEFS_STATUS   status_dqs_sweep =  DEFS_STATUS_OK;
	int          l_BestEyeCenter[8*NUM_OF_LANES_MAX], l_BestEyeSize[8*NUM_OF_LANES_MAX];


#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
	UINT32        iCnt2 = 0;
	extern UINT32 BOOTBLOCK_IMAGE_START;
	extern UINT32 BOOTBLOCK_IMAGE_END;
	volatile UINT16   *dstAddrStart = (UINT16 *)0x1000;
#endif

	g_mc_config = mc_config;

	// if memory controller was already initialized do not reinitialize
	if (MC_IsDDRConfigured_l())
	{
		serial_printf(">MC already configured.\n");

#ifdef DEBUG_COSMOS
		if((status = MC_Init_test_l()) != DEFS_STATUS_OK)
		{
			serial_printf(">MC registers corrupted! reinit DDR !!.\n");
		}
		else
		{
			return DEFS_STATUS_SYSTEM_NOT_INITIALIZED;
		}
#else
		return DEFS_STATUS_SYSTEM_NOT_INITIALIZED;
#endif
	}


	// mark that initialization was not finished
	SET_REG_FIELD(INTCR2, INTCR2_MC_INIT, 0);


	for (try = 1; try < 6; try++)
	{
		serial_printf("\n\n ****** DDR4 Init (Trial #%d) mc_config = 0x%x *******\n", try, mc_config);

		status = MC_ConfigureDDR_l();
		MC_PrintRegs();

		if (status==DEFS_STATUS_OK)
		{
			SDRAM_PrintRegs(); // Only when MC was init. When MC was not init, can't read/write MR registers.
			serial_printf("\n\n");
			break;
		}
	}


	if (status != DEFS_STATUS_OK)
	{
		serial_printf(KRED "****************************\n" KNRM);
		serial_printf(KRED "*** Your SDRAM is Faulty ***\n" KNRM);
		serial_printf(KRED "****************************\n" KNRM);
#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
		CFG_HeartBeatToggle(5);    //  This function never returns! it blinks forever. This is for debug versions.
#endif
		return (status);
	}

	// after initialization we set INTCR2.MC_Init_l to mark that initialization was finished

	dramSize = MC_CheckDramSize();
	MC_UpdateDramSize(dramSize);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Check if ECC device is selected  : init the memory size and enable ECC                  */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( LANE_MASK == LANE_MASK_MAX)
	{
#ifdef ECC_GLITCH_BYPASS
		UINT32 clksel;

		serial_printf(">Enable ECC (training ECC passed)\n");
		clksel = REG_READ(CLKSEL);  // save clock current settings
		SET_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL, CLKSEL_CPUCKSEL_CLKREF);  // set CPU  to CLKREF clock (25 MHz)
		CLK_Delay_MicroSec(10);  // some delay after clock MUX change
		IOW32(MC_BASE_ADDR + (ECC_EN_ADDR*4),           DENALI_CTL_93_DATA_ECC);  // Enable ECC
		REG_WRITE(CLKSEL, clksel); // restore clocks to normal
		CLK_Delay_MicroSec(10);  // some delay after clock MUX change
#else
		serial_printf(">Enable ECC (training ECC passed)\n");
		IOW32(MC_BASE_ADDR + (ECC_EN_ADDR*4),           DENALI_CTL_93_DATA_ECC);  // Enable ECC
#endif
		IOW32(MC_BASE_ADDR + (ECC_DISABLE_W_UC_ERR_ADDR*4), DENALI_CTL_94_DATA_ECC);

		IOW32(MC_BASE_ADDR + 4*INT_MASK_ADDR, 0x0FFFFFFF);    // enable only ECC interrupts


		serial_printf("\n\n\n");


#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
		Tock();


		/*---------------------------------------------------------------------------------------------*/
		/* zero the mem test area                                      */
		/*---------------------------------------------------------------------------------------------*/
		for( iCnt2 = 0; iCnt2 <  (&BOOTBLOCK_IMAGE_END - &BOOTBLOCK_IMAGE_START) ; iCnt2 += sizeof(UINT16))
		{
			*dstAddrStart = 0;
			dstAddrStart++;
		}
#endif


		/*---------------------------------------------------------------------------------------------------------*/
		/* ECC: Sweep ECC DQs (in and out and DQS)                                 */
		/*---------------------------------------------------------------------------------------------------------*/
		status_out_sweep = Sweep_DQs_ECC_Trim_l(FALSE); // Run Parametric Sweep on WRITE side (both Output DQn and DM) and find the center for each DQn and average for each DM lane. (Note: assume DM length is average of data length)
		status       = Sweep_DQs_ECC_Trim_l(TRUE); // Run Parametric Sweep on READ side and find the center for each DQn.

		// If output failed but input pass, maybe all we need is a second output sweep for fine tune the output after input was fine tuned.
		if ( (status == DEFS_STATUS_OK) && (status_out_sweep != DEFS_STATUS_OK) )
			status_out_sweep = Sweep_DQs_ECC_Trim_l(FALSE);


		/*--------------------------------------------------------------------*/
		/* ECC: enhanced eye training for lane (or simple sweep if enhanced is disabled) */
		/*--------------------------------------------------------------------*/
		if(!(g_mc_config & MC_DISABLE_CAPABILITY_INPUT_DQS_ENHANCE_TRAINING))
		{
			status_dqs_sweep = Sweep_Input_DSQ_Enhanced_Training_l(TRUE);
		}
		else
		{
			status_dqs_sweep = Sweep_Trim_Input_DQS_l(l_BestEyeCenter, l_BestEyeSize, TRUE, FALSE);
		}


		if ((status != DEFS_STATUS_OK) || (status_out_sweep != DEFS_STATUS_OK) || (status_dqs_sweep != DEFS_STATUS_OK))
		{
			serial_printf(KRED "***********************************\n" KNRM);
			serial_printf(KRED "*** Your ECC device is Faulty   ***\n" KNRM);
			serial_printf(KRED "***********************************\n" KNRM);
			return (status);
		}
		else
		{
			MC_PrintLeveling(TRUE, TRUE);

		}
#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
		Tock();
#endif
		serial_printf(">Zero the memory\n");

		/*---------------------------------------------------------------------------------------------*/
		/* Write zeros to the entire memory                                */
		/*---------------------------------------------------------------------------------------------*/
#pragma push
#pragma diag_suppress 2748  /* iCnt WILL point at 0 . I know what I'm doing . */
		for (iCnt = 0 ; iCnt <  dramSize ; iCnt += dramSize/16)
		{
			memset((void *)iCnt, 0, (dramSize/16));
			serial_printf(".");
		}
#pragma pop

		serial_printf("\n>Zero the memory done\n");

#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && defined(NPCM750))
		Tock();
#endif

		IOW32(MC_BASE_ADDR + 4*INT_MASK_ADDR, DENALI_CTL_118_DATA);
	}
	else
	{
		serial_printf(KCYN ">ECC is NOT enabled.\n" KNRM);
	}


	SET_REG_FIELD(INTCR2, INTCR2_MC_INIT, 1);

	return (status);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_UpdateDramSize                                      */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:     none                                           */
/* Side effects:                                               */
/* Description:                                                */
/*          Update refresh rate according to size                          */
/*---------------------------------------------------------------------------------------------------------*/
void MC_UpdateDramSize (UINT iDramSize)
{
	if (iDramSize == 0)
	{
		serial_printf(KRED "\n\n>ERROR: Can't detect SDRAM size.\n>SDRAM device is faulty\n");
		while(1);
	}
	if (iDramSize < _1GB_)
	{
		IOW32(MC_BASE_ADDR + (50*4), DENALI_CTL_50_DATA);
		IOW32(MC_BASE_ADDR + (107*4), DENALI_CTL_107_DATA);
	}
	else
	{
		IOW32(MC_BASE_ADDR + (50*4), DENALI_CTL_50_DATA_1GIB);
		IOW32(MC_BASE_ADDR + (107*4), DENALI_CTL_107_DATA_1GIB);
	}
}

#if  ((defined(DEBUG_LOG) || defined(DEV_LOG)) && (defined(NPCM750) || defined(NPCM850)))
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_PrintOutleveling                                    */
/*                                                     */
/* Parameters:      bIn - print input leveling   bOut - print output leveling                  */
/* Returns:     none                                           */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine prints the DQ, DM delays                          */
/*---------------------------------------------------------------------------------------------------------*/
static void MC_PrintLeveling (BOOLEAN bIn, BOOLEAN bOut)
{
	UINT32 ilane = 0;
	UINT32 ibit = 0;

	//---------------------------------------------------------------
	if (bOut)
	{
		serial_printf(">Data Output Delay:\n");
		for (ilane = 0; ilane < NUM_OF_LANES ; ilane++)
		{
			serial_printf("\t*Lane%lu: ", ilane );
			for (ibit = 0 ; ibit < 8 ; ibit++)
			{
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
				serial_printf("DQ%lu:0x%02X, ",ibit,(UINT8) (REG_READ(OP_DQ_DM_DQS_BITWISE_TRIM) & 0x7f));
			}

			ibit = 8;
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
			serial_printf("DM:0x%02X, ",(UINT8) (REG_READ(OP_DQ_DM_DQS_BITWISE_TRIM) & 0x7f) );

			ibit = 9;
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
			serial_printf("DQS:0x%02X, ",(UINT8) (REG_READ(OP_DQ_DM_DQS_BITWISE_TRIM) & 0x7f) );

			REG_WRITE(PHY_LANE_SEL,(ilane*6));
			if (((REG_READ(PHY_DLL_INCR_TRIM_1)>>ilane) & 0x01)==0x01)
			serial_printf("dlls_trim_1:+%u, ",(UINT8) (REG_READ(PHY_DLL_TRIM_1) & 0x3f)); // output dqs timing with respect to output dq/dm signals
			else
			serial_printf("dlls_trim_1:-%u, ",(UINT8) (REG_READ(PHY_DLL_TRIM_1) & 0x3f));

			REG_WRITE(PHY_LANE_SEL,(ilane*5));
			serial_printf("phase1:%u/64 cycle, ",(UINT8) ((REG_READ(UNQ_ANALOG_DLL_3) >>0) & 0x1f));  // DQS delayed on the write side

			REG_WRITE(PHY_LANE_SEL,(ilane*6));
			serial_printf("dlls_trim_2:%u ",(UINT8) (REG_READ(PHY_DLL_TRIM_2) & 0x3f)); //  adjust output dq/dqs/dm timing with respect to DRAM clock; This value is set by write-leveling

			REG_WRITE(PHY_LANE_SEL,(ilane*8));
			serial_printf("mas_dly:%u, ",(UINT8)((REG_READ(UNQ_ANALOG_DLL_2) >>24) & 0x7F)); // master delay line setting for this lane

			serial_printf("\n");
		}
	}
	//---------------------------------------------------------------
	if (bIn)
	{
		serial_printf(">Data Input Delay:\n");
		for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
		{
			serial_printf("\t*Lane%lu: ", ilane );
			for (ibit = 0 ; ibit < 8 ; ibit++)
			{
				REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
				serial_printf("DQ%lu:0x%02X, ",ibit,(UINT8) (REG_READ(IP_DQ_DQS_BITWISE_TRIM) & 0x7f));
			}

			ibit = 8;
			REG_WRITE(PHY_LANE_SEL,(ilane*7)+((UINT32)ibit<<8));
			serial_printf("     DQS:0x%02X, ",(UINT8) (REG_READ(IP_DQ_DQS_BITWISE_TRIM) & 0x7f));

			REG_WRITE(PHY_LANE_SEL,(ilane*6));
			if (((REG_READ(PHY_DLL_INCR_TRIM_3)>>ilane) & 0x01)==0x01)
			serial_printf("dlls_trim_3:+%u, ",(UINT8) (REG_READ(PHY_DLL_TRIM_3) & 0x3f));
			else
			serial_printf("dlls_trim_3:-%u, ",(UINT8) (REG_READ(PHY_DLL_TRIM_3) & 0x3f)); // input dqs timing with respect to input dq

			REG_WRITE(PHY_LANE_SEL,(ilane*5));
			serial_printf("phase2:%u/64 cycle, ",(UINT8) ((REG_READ(UNQ_ANALOG_DLL_3) >>8) & 0x1f));

			REG_WRITE(PHY_LANE_SEL,(ilane*8));
			serial_printf("data_capture_clk:0x%02X, ", (UINT8) (REG_READ(SCL_DCAPCLK_DLY) & 0xff)); // data_capture_clk edge used to transfer data from the input DQS clock domain to the PHY core clock domain // Automatically programmed by SCL

			REG_WRITE(PHY_LANE_SEL,(ilane*3));
			serial_printf("main_clk_delta_dly:0x%02X, ",(UINT8) (REG_READ(SCL_MAIN_CLK_DELTA) & 0x7));

			REG_WRITE(PHY_LANE_SEL,(ilane*8));
			serial_printf("cycle_cnt:0x%02X ",(UINT8)((REG_READ(SCL_START)>>20)&0xFF)); // number of delay elements required to delay the clock signal to align with the read DQS falling edge


			serial_printf("\n");
		}
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_PrintRegs                                       */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:     none                                           */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine prints the module registers                           */
/*---------------------------------------------------------------------------------------------------------*/
void MC_PrintRegs (void)
{
	UINT32 TmpReg32 = 0;
	UINT32 ilane = 0;
	UINT32 g_DllClockCycle = 0;

	serial_printf("\n************\n");
	serial_printf("* MC  Info *\n");
	serial_printf("************\n\n");

	//---------------------------------------------------------------
	TmpReg32 = REG_READ(SCL_START) /*cuml_scl_rslt*/ & REG_READ(WRLVL_CTRL) /*write-leveling status*/ & (~(REG_READ(DYNAMIC_WRITE_BIT_LVL)>>20)) /*bit_lvl_wr_failure_status*/ & (REG_READ(IP_DQ_DQS_BITWISE_TRIM)>>8) /*bit_lvl_done_status*/ & (~(REG_READ(DYNAMIC_BIT_LVL)>>14)) /*bit_lvl_failure_status*/ ;
	serial_printf(">Self-Conf Logic (SCL), Write-Leveling and Read/Write Bit-Leveling status:\n");
	for (ilane = 0;ilane<3;ilane++)
	{
		serial_printf("\t*lane%u:",ilane);
		if ((TmpReg32&(1<<ilane))==0)
		serial_printf("Failed.\n");
		else
		serial_printf("Passed.\n");
	}

	TmpReg32 = REG_READ(PHY_DLL_ADRCTRL);
	serial_printf(">PHY_DLL_ADRCTRL (0x4A) = %#010lx \n", TmpReg32);
	g_DllClockCycle = (UINT8)(TmpReg32>>24);
	serial_printf("\t*Number of delay elements corresponds to one clock cycle (dll_mas_dly): %lu.\n", g_DllClockCycle);
	serial_printf("\t*Number of delay elements corresponds to 1/4 clock cycle (dll_slv_dly_wire): %lu.\n", (TmpReg32>>16)&0xFF);
	/*serial_printf("\tNote: each delay element is ~30 psec.\n");*/
	if ((TmpReg32&0x100)==0)
	{
		serial_printf("\t*dlls_trim_adrctl: ");
		if ((TmpReg32&0x200)!=0)
		serial_printf("+"); // set to increment; limited by 1/4 clock (i.e., from dlls_trim_clk)
		else
		serial_printf("-"); // decrement; limited by dlls_trim_2 or 1/4 clock
		serial_printf("%lu.(pre-set)\n", (TmpReg32>>0)&0x3F);
	}
	//---------------------------------------------------------------
	TmpReg32 = REG_READ(PHY_DLL_RECALIB);
	serial_printf(">PHY_DLL_RECALIB (0x49) = %#010lx \n", TmpReg32);
	serial_printf("\t*dlls_trim_adrctrl_ma:");
	if ((TmpReg32&((UINT32)1<<27))!=0)
	serial_printf("+"); // set to increment; limited by 1/4 clock (i.e., from dlls_trim_clk)
	else
	serial_printf("-"); // decrement; limited by dlls_trim_2 or 1/4 clock
	serial_printf("%lu.(pre-set)\n", (TmpReg32>>0)&0x3F);

	//---------------------------------------------------------------
	REG_WRITE(PHY_LANE_SEL,0);
	TmpReg32 = REG_READ(PHY_DLL_TRIM_CLK);
	serial_printf(">PHY_DLL_TRIM_CLK (0x69) = %#010lx \n", TmpReg32);
	serial_printf("\t*dlls_trim_clk: ");
	if ((TmpReg32&0x40)!=0)
	serial_printf("+"); // set to increment;
	else
	serial_printf("-"); // decrement. (limited by the value of dlls_trim_2)
	serial_printf("%lu.(pre-set)\n", (TmpReg32>>0)&0x3F);


	//---------------------------------------------------------------
	serial_printf(">PHY_DLL_RISE_FALL (0x63) = %#010lx \n\t*rise_cycle_cnt: = %lu\n",
	REG_READ(PHY_DLL_RISE_FALL), READ_REG_FIELD(PHY_DLL_RISE_FALL, PHY_DLL_RISE_FALL_rise_cycle_cnt));

	//---------------------------------------------------------------
	TmpReg32 = REG_READ(PHY_PAD_CTRL);
	serial_printf(">PHY_PAD_CTRL (0x48) = %#010lx \n",TmpReg32);
	serial_printf("\t*DQ and DQS input dynamic term(ODT):");
	switch (TmpReg32&0x7)
	{
	case 0: serial_printf("No\n"); break;
	case 1: serial_printf("240R\n"); break;
	case 2: serial_printf("120R\n"); break;
	case 3: serial_printf("80R\n"); break;
	case 4: serial_printf("60R\n"); break;
	case 5: serial_printf("48R\n"); break;
	default: serial_printf("Reserved\n");
	}
	serial_printf("\t*DQ, DM and DQS output drive strength: ");
	if ( (TmpReg32&((UINT32)1<<4)) == 0)
	serial_printf("34R\n");
	else
	serial_printf("48R\n");
	serial_printf("\t*Addr and ctrl output drive strength: ");
	if ( (TmpReg32&((UINT32)1<<16)) == 0)
	serial_printf("34R\n");
	else
	serial_printf("48R\n");
	serial_printf("\t*Clock out drive strength: ");
	if ( (TmpReg32&((UINT32)1<<20)) == 0)
	serial_printf("34R.\n");
	else
	serial_printf("48R.\n");
	serial_printf("\t*preamble_dly = ");
	switch((TmpReg32>>29)&0x3)
	{
	case 0: serial_printf("2"); break;
	case 1: serial_printf("1.5"); break;
	case 2: serial_printf("1"); break;
	default:
		serial_printf("Inv\n");
	}


	serial_printf(" cycles.\n>SCL_LATENCY (0x43) = %#010lx \n", REG_READ(SCL_LATENCY));
	serial_printf("\t*capture_clk_dly: = %lu\n", READ_REG_FIELD(SCL_LATENCY,SCL_LATENCY_capture_clk_dly));
	serial_printf("\t*main_clk_dly: = %lu\n", READ_REG_FIELD(SCL_LATENCY,SCL_LATENCY_main_clk_dly));
	//---------------------------------------------------------------

	MC_PrintLeveling(TRUE, TRUE);


	//---------------------------------------------------------------
	serial_printf(">PHY VREF:\n");
	for (ilane = 0; ilane < NUM_OF_LANES; ilane++)
	{
		serial_printf("\t*Lane%lu:",ilane);
		REG_WRITE(PHY_LANE_SEL,(ilane*7));
		serial_printf("0x%02X\n", READ_REG_FIELD(VREF_TRAINING, VREF_TRAINING_vref_value));
	}
	serial_printf(">UNIQUIFY_IO_1 %#010lx  \n", REG_READ(UNIQUIFY_IO_1));
	serial_printf(">UNIQUIFY_IO_2 %#010lx  \n", REG_READ(UNIQUIFY_IO_2));
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SDRAM_PrintRegs                                      */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:     none                                           */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine prints the DDR device information                     */
/*---------------------------------------------------------------------------------------------------------*/
static void SDRAM_PrintRegs (void)
{
	DEFS_STATUS status;
	UINT8 MPR_Data[4];
	UINT16 MPR0_Readout [8*4];
	UINT16 *pMPR0_Readout = MPR0_Readout;
	UINT8 x_row, y_row;

	const UINT8 CL[16] = {9,10,11,12,13,14,15,16,18,20,22,24,0,0,0,0};
	const UINT8 CWL[8] = {9,10,11,12,14,16,18,0};

	serial_printf("\n***************\n");
	serial_printf("* SDRAM  Info *\n");
	serial_printf("***************\n\n");

	//-------------
	serial_printf(">Readout MPR Page 0:");
	MPR_Readout_RawData (0, MPR0_Readout);
	for (y_row = 0; y_row<4; y_row++)
	{
		serial_printf("\n\t%u: ",y_row);
		for (x_row = 0; x_row<8; x_row++)
		{
			serial_printf("0x%04X ",*pMPR0_Readout);
			pMPR0_Readout++;
		}
	}
	//-------------
	status = MPR_Page_Readout (2, MPR_Data);
	serial_printf("\n\n>Readout MPR Page 2 =>0: 0x%02X, 1: 0x%02X, 2: 0x%02X, 3: 0x%02X. ",MPR_Data[0],MPR_Data[1],MPR_Data[2],MPR_Data[3]);
	if (status != DEFS_STATUS_OK)
	{
		serial_printf(" (values may be invalid)");
		return;
	}

	//--------------------------------
	serial_printf("\n\t*VREF DQ: ");
	serial_printf("0x%02X, ", (MPR_Data[1]>>1)&0x3F);
	serial_printf("range %u.\n",(MPR_Data[1]>>7) + 1);

	serial_printf("\t*Output drive strength: ");
	switch (MPR_Data[3]&0x03)
	{
	case 0:  serial_printf("34R"); break;
	case 1:  serial_printf("48R"); break;
	default: serial_printf("Reserved"); break;
	}
	serial_printf("\n\t*RTT_NOM: ");
	switch (MPR_Data[3]>>5)
	{
	case 0: serial_printf("Off"); break;
	case 1: serial_printf("60R"); break;
	case 2: serial_printf("120R"); break;
	case 3: serial_printf("40R"); break;
	case 4: serial_printf("240R"); break;
	case 5: serial_printf("48R"); break;
	case 6: serial_printf("80R"); break;
	case 7: serial_printf("34R"); break;
	default: serial_printf("Reserved");
	}
	serial_printf("\n\t*RTT_PARK: ");
	switch ((MPR_Data[3]>>2) & 0x7)
	{
	case 0: serial_printf("Off"); break;
	case 1: serial_printf("60R"); break; // RZQ/4
	case 2: serial_printf("120R"); break; // RZQ/2
	case 3: serial_printf("40R"); break; // RZQ/6
	case 4: serial_printf("240R"); break; // RZQ/1
	case 5: serial_printf("48R"); break; // RZQ/5
	case 6: serial_printf("80R"); break; // RZQ/3
	case 7: serial_printf("34R"); break; // RZQ/7
	default: serial_printf("Reserved");
	}
	serial_printf("\n\t*RTT_WR: ");
	switch ((MPR_Data[0]>>0) & 0x3)
	{
	case 0: serial_printf("Off"); break;
	case 1: serial_printf("120R"); break; // RZQ/2
	case 2: serial_printf("240R"); break; // RZQ/1
	default: serial_printf("Reserved");
	}
	serial_printf("\n\t*CAS Latency (CL): 0x%x (%u) \n",MPR_Data[2]>>4, CL[MPR_Data[2]>>4]);  // 09/03/2016: added more info
	serial_printf("\t*CAS Write Latency (CWL): 0x%x (%u) \n", MPR_Data[2]&0x07, CWL[MPR_Data[2]&0x07]); // 09/03/2016: fixed a bug in the shift and added more info
	serial_printf("\n");

	//--------------------------------
	status = MPR_Page_Readout (3, MPR_Data);
	serial_printf(">Readout MPR Page 3 =>0: 0x%02X, 1: 0x%02X, 2: 0x%02X, 3: 0x%02X. ",MPR_Data[0],MPR_Data[1],MPR_Data[2],MPR_Data[3]);
	if (status != DEFS_STATUS_OK)
	serial_printf(" (values may be invalid) \n");
	serial_printf("\n\n");
	//--------------------------------
	return;
}
//---------------------------------------------------------------------------------------------------------------------


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_PrintVersion                                    */
/*                                                     */
/* Parameters:      none                                           */
/* Returns:     none                                           */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine prints the module version                         */
/*---------------------------------------------------------------------------------------------------------*/
void MC_PrintVersion (void)
{
	serial_printf("MC          = %d\n", MC_MODULE_TYPE);
}

#else // no printouts ( used for the monitor).

static void MC_PrintLeveling (BOOLEAN bIn, BOOLEAN bOut)
{
	return;
}

void MC_PrintRegs (void)
{
	return;
}

static void SDRAM_PrintRegs (void)
{
	return;
}

void MC_PrintVersion (void)
{
	return;
}

#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MPR_Readout_RawData                                    */
/*                                                     */
/* Parameters:      Page: 0 to 3                                       */
/*          *MPR_RawData: return value (array of 8 * 4)                        */
/* Returns:     status pass or fail                                    */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine read DDR Multi Purpose Registers (MPR) page raw data              */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS MPR_Readout_RawData (UINT8 Page, /*OUT*/ UINT16 *MPR_RawData)
{
	UINT32 MC_MR3;
	UINT32 ByteOffset, BitOffset;
	DEFS_STATUS status;
	volatile UINT16 *pAddr;

	memset (MPR_RawData,0xcc,2*8*4);

	MC_MR3 = IOR32(MC_BASE_ADDR + (86*4)) & 0x1FFFF ; // ENALI_CTL_86.MR3_DATA_F0_0 // get MR3 value assuming MR3_DATA_F0_0 is correct value
	MC_MR3 = MC_MR3 & ~(UINT32)0x7 & ~((UINT32)3<<11); // remove MPR operation (bit 2), MPR selection (bits 1:0) and MPR Read Format (bits 12:11)

	status = WriteModeReg(3, MC_MR3 | (UINT32)0<<11 /*Readout serial mode*/ | (UINT32)1<<2 /*Enable MPR*/ | (UINT32)Page<<0 /*Page number*/);
	if (status!=DEFS_STATUS_OK) // 05/10/2015
	return (DEFS_STATUS_FAIL);

	MC_Sleep_l (10); // some delay to the DDR device to enter MPR mode

	// We assuming ECC is disabled at this stage
	// We assuming data-cache is disabled at this state otherwise return data is invalid.

	for (ByteOffset=0;ByteOffset<4; ByteOffset++)
	{
		pAddr = (UINT16 *)(ByteOffset<<12); // BA[1:0] set register index inside a page of MPR. CPU adress to DDR address:  [ ....  ;BA[1:0] ; Column[9:0]; byte/word]
		for (BitOffset=0; BitOffset<8; BitOffset++)
		{
			*MPR_RawData = *pAddr;
			MPR_RawData++;
			pAddr++;
		}
	}

	// Disable MPR
	WriteModeReg(3, MC_MR3);

	return (DEFS_STATUS_OK);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MPR_Page_Readout                                       */
/*                                                     */
/* Parameters:      Page: 0 to 3                                       */
/*          *MPR: return value (array of 4)                            */
/* Returns:     status pass or fail                                    */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine read DDR Multi Purpose Registers (MPR) page                   */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS MPR_Page_Readout (UINT8 Page, /*OUT*/ UINT8 *MPR)
{
	UINT8 tmp_data;
	UINT32 bit_index;
	UINT16 MPR_RawData [8*4];
	UINT32 ByteOffset;
	UINT16 *pMPR_RawData = MPR_RawData;

	memset (MPR,0xcc,4);

	if (MPR_Readout_RawData (Page, MPR_RawData) != DEFS_STATUS_OK) // read MRP page raw data (in serial mode)
	return (DEFS_STATUS_FAIL);

	for (ByteOffset=0; ByteOffset<4; ByteOffset++)
	{
		tmp_data = 0;
		bit_index=0x80;
		while (bit_index!=0)
		{
			if (*pMPR_RawData == 0xFFFF)
			tmp_data |= bit_index;
			else if (*pMPR_RawData != 0x0000)
			return (DEFS_STATUS_FAIL); // in serial mode all data bus must be the same value (0 or 1). If not, there is be a signal integrity issue.
			// TBD: else count the amount of 1 and 0 in *pMPR_RawData value; set value to 1 or 0 according to majority. We have signal integrity issue but most likely can read DDR info.
			// e.g. up to 4 anomaly bits from 16 bits can concider as data valid.
			bit_index = bit_index >>1;
			pMPR_RawData++;
		}
		*MPR = tmp_data;
		MPR++;
	}
	return (DEFS_STATUS_OK);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        WriteModeReg                                       */
/*                                                     */
/* Parameters:      Index: DDR device register index 0..6;  Data: data to write (only 12 LSB are used)     */
/* Returns:     status pass or fail                                    */
/* Side effects:                                               */
/* Description:                                                */
/*          This routine wirte to DDR device configuraion registers (there are 7 of those in DDR4) */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS WriteModeReg(UINT8 Index, UINT32 Data)
{
	UINT32 WRITE_MODEREG;
	volatile UINT32 TimeOut;

	IOW32(MC_BASE_ADDR + (INT_ACK_ADDR*4), (UINT32)(-1)); // DENALI_CTL_117 // clear all status bits
	IOW32(MC_BASE_ADDR + (85*4), Data); // DENALI_CTL_85.MRSINGLE_DATA_0 // write data
	WRITE_MODEREG = (UINT32)1<<25 | (UINT32)1<<23 | (UINT32)Index; // write to CS 0, write to MR specify in bits 7:0
	IOW32(MC_BASE_ADDR + (70*4), WRITE_MODEREG); // DENALI_CTL_70

	TimeOut = 60000;
	MC_Sleep_l(1); // ~ 10usec
	while (1)
	{
		if ( ((IOR32(MC_BASE_ADDR + (INT_STATUS_ADDR*4))>>25) & 0x01) == 0x01)  // DENALI_CTL_116.25
		{
			if  ( (IOR32(MC_BASE_ADDR + (71*4)) & 0xFF) == 0x01 ) // DENALI_CTL_71.MRW_STATUS
			return (DEFS_STATUS_FAIL);
			else
			return (DEFS_STATUS_OK);
		}

		if (TimeOut==0)
		{
			serial_printf(KRED "\t>WriteModeReg::TimeOut\n" KNRM);
			return (DEFS_STATUS_FAIL);
		}

		TimeOut--;
	}

}
//-------------------------------------------------------------------------------

static UINT16 GetECCSyndrom(UINT32 readVal)
{
	UINT16 status= 0xFF80; // we can only test lower nibble. Rest are assumed error. status is bitwise.
	UINT32 status_correctable = 0;
	UINT32 status_uncorrectable = 0;
	UINT32 int_status = 0;
	UINT32 volatile data = 0;

	int_status = IOR32(MC_BASE_ADDR + 4*INT_STATUS_ADDR);
	IOW32(MC_BASE_ADDR + 4*INT_ACK_ADDR, 0x7ffffff);
	IOW32(MC_BASE_ADDR + 4*INT_ACK_ADDR, 0x0);

	if ( (int_status & (1<<6) ) == (1<<6))
	{
		serial_printf_debug("6-Mult uncorrectable detected.\n");
		return 0xFFFF;
	}

	if ( (int_status & (1<<5) ) == (1<<5))
	{
		serial_printf_debug("5-An uncorrectable detected\t");
		return 0xFFFF;
	}

	if ( (int_status & (1<<4) ) == (1<<4))
	{
		serial_printf_debug("4-mult correctable detected.\t");
		status = 4;
	}

	if ( (int_status & (1<<3) ) == (1<<3))
	{
		serial_printf_debug("3-A correctable detected.\t");
		status = 3;
	}

	// no ECC error.
	if (status == 0xFF80)
		return status;

	data = IOR32(MC_BASE_ADDR + 4*ECC_C_DATA_ADDR);


	if ( status != 0)
	{
		status_correctable =   IOR32(MC_BASE_ADDR + 4*ECC_C_SYND_ADDR);
		status_uncorrectable = IOR32(MC_BASE_ADDR + 4*ECC_U_SYND_ADDR);

		if(status_correctable != 0)
			serial_printf_debug("C synd=%#08lx add=%#08lx data=%#08lx id=%#08lx\t",
			status_correctable,
			IOR32(MC_BASE_ADDR + 4*ECC_C_ADDR_ADDR),
			IOR32(MC_BASE_ADDR + 4*ECC_C_DATA_ADDR),
			IOR32(MC_BASE_ADDR + 4*ECC_C_ID_ADDR));

		if(status_uncorrectable != 0)
			serial_printf_debug("Un synd=%#08lx add=%#08lx data=%#08lx id=%#08lx\t",
			status_uncorrectable,
			IOR32(MC_BASE_ADDR + 4*ECC_U_ADDR_ADDR),
			IOR32(MC_BASE_ADDR + 4*ECC_U_DATA_ADDR),
			IOR32(MC_BASE_ADDR + 4*ECC_U_ID_ADDR));

	}



	/*-----------------------------------------------------------------------------------------------------*/
	/* Syndrome Incorrect Bit                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	switch (status_correctable)
	{
	case 0x00000000: status = 0xFF80 ; break;   // No Error
	case 0x00000001: status = 0xFF81 ; break;   // Check [0]
	case 0x00000002: status = 0xFF82 ; break;   // Check [1]
	case 0x00000004: status = 0xFF84 ; break;   // Check [2]
	case 0x00000008: status = 0xFF88 ; break;   // Check [3]
	case 0x0000000b: status = 0xFFFF ; break;   // Data [31]
	case 0x0000000e: status = 0xFFFF ; break;   // Data [30]
	case 0x00000010: status = 0xFF90 ; break;   // Check [4]
	case 0x00000013: status = 0xFFFF ; break;   // Data [29]
	case 0x00000015: status = 0xFFFF ; break;   // Data [28]
	case 0x00000016: status = 0xFFFF ; break;   // Data [27]
	case 0x00000019: status = 0xFFFF ; break;   // Data [26]
	case 0x0000001a: status = 0xFFFF ; break;   // Data [25]
	case 0x0000001c: status = 0xFFFF ; break;   // Data [24]
	case 0x00000020: status = 0xFFA0 ; break;   // Check [5]
	case 0x00000023: status = 0xFFFF ; break;   // Data [23]
	case 0x00000025: status = 0xFFFF ; break;   // Data [22]
	case 0x00000026: status = 0xFFFF ; break;   // Data [21]
	case 0x00000029: status = 0xFFFF ; break;   // Data [20]
	case 0x0000002a: status = 0xFFFF ; break;   // Data [19]
	case 0x0000002c: status = 0xFFFF ; break;   // Data [18]
	case 0x00000031: status = 0xFFFF ; break;   // Data [17]
	case 0x00000034: status = 0xFFFF ; break;   // Data [16]
	case 0x00000040: status = 0xFFC0 ; break;   // Check [6]
	case 0x0000004a: status = 0xFFFF ; break;   // Data [15]
	case 0x0000004f: status = 0xFFFF ; break;   // Data [14]
	case 0x00000052: status = 0xFFFF ; break;   // Data [13]
	case 0x00000054: status = 0xFFFF ; break;   // Data [12]
	case 0x00000057: status = 0xFFFF ; break;   // Data [11]
	case 0x00000058: status = 0xFFFF ; break;   // Data [10]
	case 0x0000005b: status = 0xFFFF ; break;   // Data [9]
	case 0x0000005e: status = 0xFFFF ; break;   // Data [8]
	case 0x00000062: status = 0xFFFF ; break;   // Data [7]
	case 0x00000064: status = 0xFFFF ; break;   // Data [6]
	case 0x00000067: status = 0xFFFF ; break;   // Data [5]
	case 0x00000068: status = 0xFFFF ; break;   // Data [4]
	case 0x0000006b: status = 0xFFFF ; break;   // Data [3]
	case 0x0000006d: status = 0xFFFF ; break;   // Data [2]
	case 0x00000070: status = 0xFFFF ; break;   // Data [1]
	case 0x00000075: status = 0xFFFF ; break;   // Data [0]
	default:
		serial_printf_debug("\nsynd unknown C=0x%x, U=0x%x\n", status_correctable, status_uncorrectable);
	}


	serial_printf_debug("stat %#010lx\n", status);


	return status;

}

#endif  // !defined _PALLADIUM_


