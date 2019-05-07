/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   clk_drv.c                                                                */
/*            This file contains CLK module implementation                    */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_DRV()

#include "hal.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Include driver header                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#include "clk_drv.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Include driver registers                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#include "../3/clk_regs.h"


#if  defined(DEBUG_LOG) || defined(DEV_LOG)
#include "../../../../Src/Apps/serial_printf/serial_printf.h"
#else
#define         serial_printf(x,...)                   (void)0
#define         serial_printf_debug(x,...)             (void)0
#endif



/*---------------------------------------------------------------------------------------------------------*/
/* Spec predefined values                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* PLLCON 0 possible values:                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_125MHZ_PLLCON0_REG_CFG    0x00284401
#define CLK_333MHZ_PLLCON0_REG_CFG    0x00A02403
#define CLK_500MHZ_PLLCON0_REG_CFG    0x00282201
#define CLK_600MHZ_PLLCON0_REG_CFG    0x00302201
#define CLK_666MHZ_PLLCON0_REG_CFG    0x00A02203
#define CLK_700MHZ_PLLCON0_REG_CFG    0x001C2101
#define CLK_720MHZ_PLLCON0_REG_CFG    0x00902105
#define CLK_750MHZ_PLLCON0_REG_CFG    0x001E2101
#define CLK_800MHZ_PLLCON0_REG_CFG    0x00202101  /* PLL1 setting for 800 MHz in Z2 and later will have to be 0040_2101h (instead of 0040_2201h for Z1). */
#define CLK_825MHZ_PLLCON0_REG_CFG    0x00212101
#define CLK_850MHZ_PLLCON0_REG_CFG    0x00222101
#define CLK_888MHZ_PLLCON0_REG_CFG    0x03782119
#define CLK_900MHZ_PLLCON0_REG_CFG    0x00242101
#define CLK_950MHZ_PLLCON0_REG_CFG    0x00262101
#define CLK_1000MHZ_PLLCON0_REG_CFG   0x00282101
#define CLK_1066MHZ_PLLCON0_REG_CFG   0x00802103

/*---------------------------------------------------------------------------------------------------------*/
/* PLLCON 1 possible values (notice that PLL1 in Z2 has a divider /2, so OTDV1 is smaller in half          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_333MHZ_PLLCON1_REG_CFG    0x00A02203


#define CLK_444MHZ_PLLCON1_REG_CFG       0x00A02303
#define CLK_500MHZ_PLLCON1_REG_CFG       0x00282101
#define CLK_600MHZ_PLLCON1_REG_CFG       0x00302101
#define CLK_666MHZ_PLLCON1_REG_CFG_Z1    0x00A02203 /* for Z1 */
#define CLK_666MHZ_PLLCON1_REG_CFG       0x00A02103 /* for Z2: change OTDV1 */

#define CLK_700MHZ_PLLCON1_REG_CFG    0x00382101
#define CLK_720MHZ_PLLCON1_REG_CFG    0x01202105
#define CLK_750MHZ_PLLCON1_REG_CFG    0x003C2101 /* change FBDV */
#define CLK_775MHZ_PLLCON1_REG_CFG    0x003E2101
#define CLK_787_5MHZ_PLLCON1_REG_CFG  0x003F2101// 787.5 MHz (PLLCON1 = 003F_2101h)
#define CLK_800MHZ_PLLCON1_REG_CFG    0x00402101 /* change OTDV1 : PLL1 setting for 800 MHz in Z2 and later will have to be 0040_2101h (instead of 0040_2201h for Z1). */
#define CLK_825MHZ_PLLCON1_REG_CFG    0x00422101 /* change FBDV */
#define CLK_850MHZ_PLLCON1_REG_CFG    0x00442101 /* change FBDV */
#define CLK_900MHZ_PLLCON1_REG_CFG    0x00482101 /* change FBDV */
#define CLK_950MHZ_PLLCON1_REG_CFG    0x004C2101 /* change FBDV */
#define CLK_1000MHZ_PLLCON1_REG_CFG   0x00502101 /* change FBDV */
#define CLK_1066MHZ_PLLCON1_REG_CFG   0x01002103


#define CLK_800MHZ_PLLCON0_REG_CFG_BB  0x00402201


/*---------------------------------------------------------------------------------------------------------*/
/* PLLCON 2 possible values                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_960MHZ_PLLCON2_REG_CFG    0x00C02105


#define LOK_TIMEOUT  100000  /* 4ms if 25 MHz */


/*---------------------------------------------------------------------------------------------------------*/
/* Local definitions                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/* SD Clock Target frequency */
#define SD_CLK_TARGET_FREQ          (48 * _1MHz_)
#define SU48_DESIRED_FREQUENCY      48  // MHz (dont use _1MHz_)
#define SU_DESIRED_FREQUENCY        30  // MHz (dont use _1MHz_)



/*---------------------------------------------------------------------------------------------------------*/
/* Local Functions                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  CLK_CalculatePLLFrequency (UINT32 pllVal);




/*---------------------------------------------------------------------------------------------------------*/
/* Functions Implementation                                                                                */
/*---------------------------------------------------------------------------------------------------------*/

DEFS_STATUS WaitForPllLock (PLL_MODULE_T pll)
{
	volatile UINT32 loki_bit = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read LOKI till PLL is locked or timeout expires                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	while (1)
	{

		switch (pll)
		{
			case PLL0:
				loki_bit = READ_REG_FIELD(PLLCON0, PLLCONn_LOKI);
				break;
			case PLL1:
				loki_bit = READ_REG_FIELD(PLLCON1, PLLCONn_LOKI);
				break;
			case PLL2:
				loki_bit = READ_REG_FIELD(PLLCON2, PLLCONn_LOKI);
				break;
			case PLL_GFX:
				loki_bit = READ_REG_FIELD(PLLCONG, PLLCONG_LOKI);
				break;
			default:
				return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
		}

		/*-------------------------------------------------------------------------------------------------*/
		/* PLL is locked, clear and break                                                                  */
		/*-------------------------------------------------------------------------------------------------*/
		if (loki_bit == 1)
		{

			switch (pll)
			{
				case PLL0:
					SET_REG_FIELD(PLLCON0, PLLCONn_LOKS, 1);
					break;
				case PLL1:
					SET_REG_FIELD(PLLCON1, PLLCONn_LOKS, 1);
					break;
				case PLL2:
					SET_REG_FIELD(PLLCON2, PLLCONn_LOKS, 1);
					break;
				case PLL_GFX:
					SET_REG_FIELD(PLLCONG, PLLCONn_LOKS, 1);
					break;
				default:
					return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
			}

			break;
		}


	}

	return DEFS_STATUS_OK;

}

#if defined (STRP_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureClocks                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  straps -  strap values from the PWRON register (flipped according to FUSTRAP register) */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the chip PLLs and selects clock sources for modules            */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ConfigureClocks (void)
{

	/*-----------------------------------------------------------------------------------------------------*/
	/* pll configuration local values                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32  PLLCON0_l  = 0;
	UINT32  PLLCON1_l  = 0;
	UINT32  PLLCON2_l  = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* clk diveiders local values                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32  CLKDIV1_l  = 0;
	UINT32  CLKDIV2_l  = 0;
	UINT32  CLKDIV3_l  = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* clk mux local values                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32  CLKSEL_l   = 0;

	UINT32 CLKSEL_CLKREF_l = 0;

	UINT32 polegVersion = CHIP_Get_Version();

	/*-----------------------------------------------------------------------------------------------------*/
	/* Z1                                                                                                  */
	/* 2   1   0   DDR     CORE    AXI     AHB     AHB3    SPI0    APB5     APB2,3    APB1,4     PCI Z1  GFX Z1           PCI Z2        GFX Z2   */
	/* Straps (Z1)                                                                                                */
	/* 3   2   1                                                                                              */
	/* 0   0   0   MCBPCK  FT      FT/2    FT/4    FT/4    FT/4    FT/4     FT/4      FT/4       PLL0/5  PLL0/4 */
	/* 0   0   1   25      25      12.5    6.25    3.1     6.25    3.125    1.5       1.5        4.17    5                  4.17          5      */
	/* 0   1   0   1066    1000    500     250     19.23   19.23   125      62.5      31.25      142.8   167                133.3         152.3  */ //1066 /8 /7
	/* 0   1   1   667     500     250     125     17.85   17.85   62.5     62.5      31.25      166.7   167                120           160    */  //480 /4 /3
	/* 1   0   0   850     800     400     200     20      20      100      50        25         133     160                133           160    */  //800 /6 /5
	/* 1   0   1   900     888     444     222     18.5    18.5    111      55.5      27.75      148     177.6              128.6         150    */  //900 /7 /6
	/* 1   1   0   800     850     425     212.5   19.3    19.3    106.25   53.125    26.6       170     170                133           160    */  //800 /6 /5
	/* 1   1   1   800     800     400     200     20      20      100      50        25         133     160                133           160    */  //800 /6 /5
	/*-----------------------------------------------------------------------------------------------------*/


	/*-----------------------------------------------------------------------------------------------------*/
	/* Z2                                                                                                  */
	/*  2   1   0   DDR CORE    AXI AHB AHB3    SPI0    APB5    APB2,3  APB1,4  \PCI    GFX                */
	/*  Straps  \                                                                                               */
	/*  3   2   1   \                                                                                           */
	/*  0   0   0   MCBPCK  FT  FT/2    FT/4    FT/4    FT/4    FT/4    FT/4    FT/4                            */
	/*  0   0   1   25  25  12.5    6.25    3.1     6.25    3.125   1.5 1.5 4.17    5                           */
	/*  0   1   0   667 500 250 125 17.85   17.85   62.5    62.5    31.25   120 160 480 PLL2    4   3   120 160 */
	/*  0   1   1   667 500 250 125 17.85   17.85   62.5    62.5    31.25   120 160 480 PLL2    4   3   120 160 */
	/*  1   0   0   667 500 250 125 17.85   17.85   62.5    62.5    31.25   120 160 480 PLL2    4   3   120 160 */
	/*  1   0   1   667 500 250 125 17.85   17.85   62.5    62.5    31.25   120 160 480 PLL2    4   3   120 160 */
	/*  1   1   0   667 500 250 125 17.85   17.85   62.5    62.5    31.25   120 160 480 PLL2    4   3   120 160 */
	/*  1   1   1   800 800 400 200 20      20      100 50  25  133 160 800 PLL0\1  6   5   133.3333333 160     */
	/*-----------------------------------------------------------------------------------------------------*/


	/*-----------------------------------------------------------------------------------------------------*/
	/* Common configurations for the clk mux. will be changes later according to straps 1-3                */
	/*-----------------------------------------------------------------------------------------------------*/
	CLKSEL_l           = REG_READ(CLKSEL);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read CLKDIV default configuration                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	CLKDIV1_l          = REG_READ(CLKDIV1);
	CLKDIV2_l          = REG_READ(CLKDIV2);
	CLKDIV3_l          = REG_READ(CLKDIV3);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Spec: PLL0 is the system clock PLL. Output frequency range is 800 to 1000 (800) MHz.                */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_PLL0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Spec: PLL1 is the Memory Controller clock PLL. Output frequency range is 667 to 1066 (800) MHz.     */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_PLL1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* PLL2 is for UARTs, timers, USB. Output frequency is 960 MHz.                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_DVCSSEL,   CLKSEL_DVCSSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_SDCKSEL,   CLKSEL_SDCKSEL_PLL2);

	SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL0);

	SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXMSEL,   CLKSEL_GFXMSEL_PLL2);

	/*-----------------------------------------------------------------------------------------------------*/
	/* GFX PLL used for graphics                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_GPRFSEL,   CLKSEL_GPRFSEL_CLKREF);  // PLLG source clock can be clkref (like other plls)\ GFXBPCK \ USB OHCI
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_PIXCKSEL,  CLKSEL_GPRFSEL_GFXBYPCK);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Clock configuration - TIMER connection should change in spec to allow timer to be connected to the  */
	/* external reference clock by defaultm                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);


	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting the Timer divisor to 2                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_TIMCKDIV, CLKDIV1_TIMCK_DIV(1));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read clock speed from straps                                                                        */
	/* Values interpretation is according to FUSTRAP spec and not PWRON spec                               */
	/* STRP_Init() must be run prior to calling this function                                              */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/* Z2 and above:                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
		switch (STRP_Ckfrq_Get())
		{
			case STRP_CLKFRQ_OPTION0:   /** Z1: DDR=MCBPCK  CORE=FT       AXI=FT/2  AHB=FT/4     AHB3=FT/2   SPI0=FT/2   APB5=FT/4   APB2,3=FT/4     APB1,4=FT/4     PCI=FT/5    GFX=FT/4 */
										/** Z2: DDR=MCBPCK  CORE=FT       AXI=FT/2  AHB=FT/4     AHB3=FT/4   SPI0=FT/4   APB5=FT/4   APB2,3=FT/4     APB1,4=FT/4     PCI=PLL0/5  GFX=PLL0/4 */
				/* PLLs on default values: */

				PLLCON0_l  = CLK_800MHZ_PLLCON0_REG_CFG;     // Configure PLL0 with value divided by 24 for UART
				PLLCON1_l  = CLK_800MHZ_PLLCON1_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(5));
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(4));

				/* clk selection is for this bypass: */
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL, CLKSEL_CPUCKSEL_SYSBPCK);
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_MCCKSEL,  CLKSEL_MCCKSEL_MCBPCK);

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(1));  /* AHB3 clock div = 4 */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(1));  /* SPI clock div = 4, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(1));  /* SPI clock div = 4. take it slow.  */

				FUSE_Init(7); /* Set APBRT to APB4 frequency + 1. */
				break;

			case STRP_CLKFRQ_OPTION2:   /** Z1: DDR=1066   CORE=1000   AXI=500   AHB=250   AHB3=19.23  SPI0=19.23    APB5=125      APB2,3=62.5      APB1,4=31.25    PCI=142.8   GFX=167 */
										/** Z2: DDR=667    CORE=500    AXI=250   AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5      APB1,4=31.25    PCI=120     GFX=160 */

			case STRP_CLKFRQ_OPTION3:   /** Z1: DDR=667    CORE=500    AXI=250   AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5      APB1,4=31.25    PCI=166.7   GFX=167 */
										/** Z2: DDR=667    CORE=500    AXI=250   AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5      APB1,4=31.25    PCI=120     GFX=160 */

			case STRP_CLKFRQ_OPTION4:   /** Z1: DDR=850    CORE=800    AXI=400   AHB=200   AHB3=20     SPI0=20       APB5=100      APB2,3=50        APB1,4=25       PCI=133     GFX=160 */
										/** Z2: DDR=667    CORE=500    AXI=250   AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5      APB1,4=31.25    PCI=120     GFX=160  */

			case STRP_CLKFRQ_OPTION5:   /** Z1: DDR=900    CORE=888    AXI=444   AHB=222   AHB3=18.5   SPI0=18.5     APB5=111      APB2,3=55.5      APB1,4=27.75    PCI=148     GFX=177.6 */
										/** Z2: DDR=667    CORE=500    AXI=250   AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5      APB1,4=31.25    PCI=120     GFX=160  */

			case STRP_CLKFRQ_OPTION6:   /** Z1: DDR=800    CORE=850    AXI=425   AHB=212.5 AHB3=19.3   SPI0=19.3     APB5=106.25   APB2,3=53.125    APB1,4=26.6     PCI=170     GFX=170 */
										/** Z2: DDR=667    CORE=500    AXI=250   AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5      APB1,4=31.25    PCI=120     GFX=160 */

				PLLCON0_l  = CLK_500MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_666MHZ_PLLCON1_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL2);
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(4));
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(3));

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(7));  /* AHB3 clock div = 7*2 */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 2, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(7));  /* SPI clock div = 7, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				FUSE_Init(32); /* Set APBRT to APB4 frequency + 1. */

				break;


			case STRP_CLKFRQ_OPTION1:   /** Z1: DDR=25      CORE=25     AXI=12.5    AHB=6.25    AHB3=3.1    SPI0=6.25   APB5=3.125  APB2,3=3.125    APB1,4=1.5      PCI=4.17    GFX=5 */
										/** Z2: DDR=25      CORE=25     AXI=12.5    AHB=6.25    AHB3=3.1    SPI0=6.25   APB5=3.125  APB2,3=1.5      APB1,4=1.5      PCI=4.167   GFX=5           */

			case STRP_CLKFRQ_OPTION7:   /** Z1:  DDR=800    CORE=800    AXI=400     AHB=200     AHB3=20     SPI0=20     APB5=100    APB2,3=50       APB1,4=25       PCI=133     GFX=160 */
										/** Z2:  DDR=800    CORE=800    AXI=400     AHB=200     AHB3=20     SPI0=20     APB5=100    APB2,3=50       APB1,4=25       PCI=133     GFX=160                         */

				/* PLLs on difault values: */
				PLLCON0_l  = CLK_800MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_800MHZ_PLLCON1_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL0);
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(6));
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(5));

				/* since CORE == DDR clock. connect them both to same PLL */
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL, CLKSEL_CPUCKSEL_PLL1);
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL1); // move to PLL1 too.

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(10));  /* AHB3 clock div = 10*2 */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(10));  /* SPI clock div = 10, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				FUSE_Init(26); /* Set APBRT to APB4 frequency + 1. */

				break;

			default:
				ASSERT(0);
		}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Switch clock sources to external clock for all muxes (while initing the PLLs)                       */
	/*-----------------------------------------------------------------------------------------------------*/
	// Switch the clock source to another PLL or external clock (25 MHz)
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_DVCSSEL,   CLKSEL_DVCSSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_SDCKSEL,   CLKSEL_SDCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_GFXMSEL,   CLKSEL_GFXMSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_GPRFSEL,   CLKSEL_GPRFSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_PIXCKSEL,  CLKSEL_PIXCKSEL_CLKREF);

	REG_WRITE(CLKSEL, CLKSEL_CLKREF_l);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Change PLL configuration (leave PLL in reset mode):                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	// Set PWDEN bit
	SET_VAR_FIELD(PLLCON0_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);
	SET_VAR_FIELD(PLLCON1_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);
	SET_VAR_FIELD(PLLCON2_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);

	/* set to power down == 1 */
	REG_WRITE(PLLCON0, PLLCON0_l);
	REG_WRITE(PLLCON1, PLLCON1_l);
	REG_WRITE(PLLCON2, PLLCON2_l);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait 10usec for PLL:                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	//3. Wait 2 É›s or more;
	CLK_Delay_MicroSec(10);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable PLLs after configuring. Delays to prevent power issues.                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(PLLCON0, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
	CLK_Delay_MicroSec(10);
	SET_REG_FIELD(PLLCON1, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
	CLK_Delay_MicroSec(10);
	SET_REG_FIELD(PLLCON2, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
	CLK_Delay_MicroSec(10);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait 750usec for PLL to stabilize:                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	WaitForPllLock(PLL0);
	WaitForPllLock(PLL1);
	WaitForPllLock(PLL2);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Change CLKDIV configuration:                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(CLKDIV1, CLKDIV1_l);
	REG_WRITE(CLKDIV2, CLKDIV2_l);
	REG_WRITE(CLKDIV3, CLKDIV3_l);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv change and clkSel change:                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(400);

	/*----------------------------------------------------------------------------------------------------*/
	/* Change clock source according to configuration:                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	/* in Z1 clksel all at once: REG_WRITE(CLKSEL, CLKSEL_l);                                              */
	/*                                                                                                     */
	/* in z2 in order to avoid power surge clksel is st gradualy, according to this scema:                 */
	/* [ brackets] mean do it in the same step                                                             */
	/*                                                                                                     */
	/* MC, [DVC,   CPU], GFXMSEL, GPRFSEL,  GFXCKSEL, [PIXCKSEL, CLKOUTSEL, SUCKSEL, TIMCKSEL, UARTCKSEL,  */
	/* SDCKSEL]                                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/

	SET_REG_FIELD(CLKSEL, CLKSEL_MCCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_MCCKSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_DVCSSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_DVCSSEL));
	SET_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_CPUCKSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_GFXMSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_GFXMSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_GPRFSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_GPRFSEL ));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_GFXCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_GFXCKSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_PIXCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_PIXCKSEL));
	SET_REG_FIELD(CLKSEL, CLKSEL_CLKOUTSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_CLKOUTSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_SUCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_SUCKSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_TIMCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_TIMCKSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_UARTCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_UARTCKSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_SDCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_SDCKSEL ));
	CLK_Delay_MicroSec(20);


	/* Wait 10usec for PLL:   */
	CLK_Delay_MicroSec(10);

	return;

}
#endif // defined (STRP_MODULE_TYPE)


#if defined (USB_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureUSBClock                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the USB clock to 60MHz by checking which PLL is                             */
/*                  dividable by 60 and setting correct SEL and DIV values.                                */
/*                  Assumes that one of the PLLs is dividable by 60                                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS  CLK_ConfigureUSBClock (void)
{
	UINT32  choosenPllFreq;
	UINT32  su60Divider;
	UINT32  suDivider;

	choosenPllFreq = CLK_GetPll2Freq();

	if (choosenPllFreq % SU48_DESIRED_FREQUENCY == 0)
	{
		SET_REG_FIELD(CLKSEL, CLKSEL_SUCKSEL, CLKSEL_SUCKSEL_PLL2);
	}
	else
	{
		choosenPllFreq = CLK_GetPll0Freq();
		if(choosenPllFreq % SU48_DESIRED_FREQUENCY == 0)
		{
			SET_REG_FIELD(CLKSEL, CLKSEL_SUCKSEL, CLKSEL_SUCKSEL_PLL0);
		}
		else
		{
			return DEFS_STATUS_INVALID_PARAMETER;
		}
	}

	su60Divider = choosenPllFreq / (SU48_DESIRED_FREQUENCY*_1MHz_);
	SET_REG_FIELD(CLKDIV2, CLKDIV2_SU48CKDIV, su60Divider - 1);
	CLK_Delay_Cycles(200);
	suDivider   = choosenPllFreq / (SU_DESIRED_FREQUENCY*_1MHz_);
	SET_REG_FIELD(CLKDIV2, CLKDIV2_SUCKDIV, suDivider - 1);
	CLK_Delay_Cycles(200);

	return DEFS_STATUS_OK;

}
#endif // defined (USB_MODULE_TYPE)


#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureUartClock                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the Uart clock source to be closest to 24MHz by                */
/*                  modifying the UART divider.                                                            */
/*                  In _PALLADIUM_ bypass mode the UART input frequency is set to be highest possible -    */
/*                  same as APB frequency                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_ConfigureUartClock(void)
{
	UINT32 uart_clk; //Hz

#ifdef _PALLADIUM_
	UINT32 apb_divisor;
#endif
	/*-------------------------------------------------------------------------------------------------*/
	/* Set UART to 24MHz. Source is PLL2 (960Mhz), which is divided by 2, so we get 24Mhz = 960/2/20   */
	/*-------------------------------------------------------------------------------------------------*/

#ifndef _PALLADIUM_
	UINT32 uartDesiredFreq  = 24*_1MHz_; //Hz
	UINT32 clkStrp = STRP_Ckfrq_Get();

	if ( (STRP_CLKFRQ_T)clkStrp != STRP_CLKFRQ_OPTION0)
	/*-----------------------------------------------------------------------------------------------------*/
	/* Normal configuration - UART from PLL0 with divider calculated from PLL0 configuration to get 24MHz  */
	/*-----------------------------------------------------------------------------------------------------*/
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* Calculate the divider given PLL2 output and desired frequency:                                  */
		/*-------------------------------------------------------------------------------------------------*/
		UINT32 pllFreq = CLK_GetPll2Freq();
		UINT32 uartDiv = pllFreq/uartDesiredFreq;
		uart_clk = pllFreq / uartDiv;

		/*-------------------------------------------------------------------------------------------------*/
		/* Set divider:                                                                                    */
		/*-------------------------------------------------------------------------------------------------*/
		SET_REG_FIELD(CLKDIV1, CLKDIV1_UARTDIV, CLKDIV1_UART_DIV(uartDiv));

		/*-------------------------------------------------------------------------------------------------*/
		/* Choose PLL2 as a source:                                                                        */
		/*-------------------------------------------------------------------------------------------------*/
		SET_REG_FIELD(CLKSEL, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_PLL2);
	}
	else
	/*-----------------------------------------------------------------------------------------------------*/
	/* Bypass configuration - UART from ref clock which is 25MHz with no divider                           */
	/*-----------------------------------------------------------------------------------------------------*/
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* Set divider to one:                                                                             */
		/*-------------------------------------------------------------------------------------------------*/
		SET_REG_FIELD(CLKDIV1, CLKDIV1_UARTDIV, CLKDIV1_UART_DIV(1));

		/*-------------------------------------------------------------------------------------------------*/
		/* Choose CLKREF as a source:                                                                      */
		/*-------------------------------------------------------------------------------------------------*/
		SET_REG_FIELD(CLKSEL, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_CLKREF);

		uart_clk = EXT_CLOCK_FREQUENCY_HZ;
	}

#else //_PALLADIUM_
	//perror("this code must be fixed for palladium");

	apb_divisor      = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/*                                          Palladium bypass                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	#define PALLADIUM_ACTUAL_PLL0       (*((UINT32*)0xffff3fc4))

	/*-----------------------------------------------------------------------------------------------------*/
	/* Retriving apb_devisor from PLL0                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	// apb_divisor = CLK_GetPLL0toAPBdivisor(APB1);

	 apb_divisor = 20;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting the same clock source as APB (APB is under CPU):                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	//SET_REG_FIELD(CLKSEL, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_PLL2);

	CLK_Delay_Cycles(200);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting UART devisor as APB devisor. This is the maximum UART clock we can achieve                  */
	/*-----------------------------------------------------------------------------------------------------*/
	//SET_REG_FIELD(CLKDIV1, CLKDIV1_UARTDIV, apb_divisor - 1);


	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculating the actual UART clock                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	uart_clk = PALLADIUM_ACTUAL_PLL0*1000 / apb_divisor;


#endif //#ifndef _PALLADIUM_

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv change and clkSel change:                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	return uart_clk;

}
#endif // defined (UART_MODULE_TYPE)


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPLL0toAPBdivisor                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the value achieved by dividing PLL0 frequency to APB frequency    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPLL0toAPBdivisor (APB_CLK apb)
{
	UINT32 apb_divisor = 1;

	apb_divisor = apb_divisor * (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV) + 1);       // AXI divider ( div by 1\2)
	apb_divisor = apb_divisor * (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV) + 1);       // AHBn divider (div by 1\2\3\4)

	switch (apb)
	{
		case APB1:
			apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB1CKDIV));     // APB divider
			break;
		case APB2:
			apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB2CKDIV));     // APB divider
			break;
		case APB3:
			apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB3CKDIV));     // APB divider
			break;
		case APB4:
			apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB4CKDIV));     // APB divider
			break;
		case APB5:
			apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB5CKDIV));     // APB divider
			break;
		case SPI0:
			apb_divisor = apb_divisor * (1 + READ_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV));       // SPI0 divider
			break;
		case SPI3:
			apb_divisor = apb_divisor * (1 + READ_REG_FIELD(CLKDIV1, CLKDIV1_AHB3CKDIV));       // SPI3 divider
			break;
		default:
			apb_divisor = 0xFFFFFFFF;
			break;
	}

	return apb_divisor;
}






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_Cycles                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cycles -  num of cycles to delay                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs delay in number of cycles (delay in C code).                     */
/*                  For a more accurate delay, use : CLK_Delay_MicroSec                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_Cycles (UINT32 cycles)
{
	UINT            cacheState = 0;
	volatile UINT   i          = 0;
	volatile UINT32 iterations = 0;


	/*-----------------------------------------------------------------------------------------------------*/
	/* The measurements were done on PD over 50 cycles, fetches from ROM:                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	const UINT CYCLES_IN_ONE_ITERATION_CACHE_DISABLED  =   145;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate number of iterations                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	iterations = cycles/CYCLES_IN_ONE_ITERATION_CACHE_DISABLED + 1;

	/*-----------------------------------------------------------------------------------------------------*/
	/* The actual wait loop:                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	ICACHE_SAVE_DISABLE(cacheState);
	for (i = 0; i < iterations; i++);
	ICACHE_RESTORE(cacheState);

	return iterations;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetTimeStamp                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         Current time stamp                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void  CLK_GetTimeStamp (UINT32 time_quad[2])
{
	UINT32 Seconds;
	UINT32 RefClocks;

	do
	{
		Seconds = REG_READ(SECCNT);
		RefClocks = REG_READ(CNTR25M);
	} while (REG_READ(SECCNT) != Seconds);

	time_quad[0] = RefClocks;
	time_quad[1] = Seconds;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_MicroSec                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSec -  number of microseconds to delay                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs a busy delay (empty loop)                                        */
/*                  the number of iterations is based on current CPU clock calculation and cache           */
/*                  enabled/disabled                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_MicroSec (UINT32 microSec)
{

/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* This register is reset by only VSB power-up reset. The value of this register                           */
/* represents a counter with a 25 MHz clock, used to update the SECCNT register. This field is updated every*/
/* 640ns. The 4 LSB of this field are always 0. When this field reaches a value of 25,000,000 it goes to 0 */
/* and SEC_CNT field is updated.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

	/* not testing that microSec < 33 sec (2^25bit) us */

	UINT32 iUsCnt1[2], iUsCnt2[2];
	UINT32 delay;  // Acctual delay generated by FW
	UINT32 minimum_delay = (microSec * EXT_CLOCK_FREQUENCY_MHZ) + CNTR25M_ACCURECY; /* this is equivalent to microSec/0.64 + minimal tic length.*/

	CLK_GetTimeStamp(iUsCnt1);

	do
	{
		CLK_GetTimeStamp(iUsCnt2);
		delay =  ((EXT_CLOCK_FREQUENCY_MHZ * _1MHz_) * (iUsCnt2[1] - iUsCnt1[1])) + (iUsCnt2[0] - iUsCnt1[0]);
	}
	while(delay < minimum_delay);


	return (UINT32)(delay / EXT_CLOCK_FREQUENCY_MHZ);

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_Since                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSecDelay -  number of microseconds to delay since t0_time. if zero: no delay.     */
/*                  t0_time       - start time , to measure time from.                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  get a time stamp, delay microSecDelay from it. If microSecDelay has already passed     */
/*                  since the time stamp , then no delay is executed. returns the time that elapsed since  */
/*                  t0_time .                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_Since (UINT32 microSecDelay, UINT32 t0_time[2])
{

	UINT32 iUsCnt2[2];
	UINT32 timeElapsedSince;  // Acctual delay generated by FW
	UINT32 minimum_delay = (microSecDelay * EXT_CLOCK_FREQUENCY_MHZ) + CNTR25M_ACCURECY; /* this is equivalent to microSec/0.64 + minimal tic length.*/


	do
	{
		CLK_GetTimeStamp(iUsCnt2);
		timeElapsedSince =  ((EXT_CLOCK_FREQUENCY_MHZ * _1MHz_) * (iUsCnt2[1] - t0_time[1])) + (iUsCnt2[0] - t0_time[0]);
	}
	while(timeElapsedSince < minimum_delay);

	/*-----------------------------------------------------------------------------------------------------*/
	/* return elapsed time                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	return (UINT32)(timeElapsedSince / EXT_CLOCK_FREQUENCY_MHZ);
}



#if defined (TMC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureTimerClock                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Timer clock configuration                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_ConfigureTimerClock (void)
{

	SET_REG_FIELD(CLKEN1, CLKEN1_TIMER0_4, 1);
	SET_REG_FIELD(CLKEN1, CLKEN1_TIMER5_9, 1);
	SET_REG_FIELD(CLKEN3, CLKEN3_TIMER10_14, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv or clkSel change:                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting the Timer divisor to 1                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(CLKDIV1, CLKDIV1_TIMCKDIV, CLKDIV1_TIMCK_DIV(1));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv or clkSel change:                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Sellecting 25MHz clock source                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(CLKSEL, CLKSEL_TIMCKSEL, CLKSEL_TIMCKSEL_CLKREF);

	/* Wait 10usec for PLL:   */
	CLK_Delay_MicroSec(10);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Returning the clock frequency                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	return EXT_CLOCK_FREQUENCY_MHZ*_1MHz_;
}
#endif // defined (TMC_MODULE_TYPE)


#if defined (EMC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureEMCClock                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ethNum -  ethernet module number                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures EMC clocks                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void    CLK_ConfigureEMCClock(UINT32 ethNum)
{
	if (ethNum == 0)                          // ETH0 - EMC1
	{
	   SET_REG_FIELD(CLKEN1, CLKEN1_EMC1, 1);
	}
	else if (ethNum == 1)                     // ETH1 - EMC2
	{
	   SET_REG_FIELD(CLKEN1, CLKEN1_EMC2, 1);
	}
}
#endif // #if defined (EMC_MODULE_TYPE)



#if defined (GMAC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureGMACClock                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ethNum -  ethernet module number                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures GMAC clocks                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void    CLK_ConfigureGMACClock(UINT32 ethNum)
{
	if (ethNum == 2)                          // ETH2 - GMAC1
	{
		SET_REG_FIELD(CLKEN2, CLKEN2_GMAC1, 1);
	}
	else if (ethNum == 3)                     // ETH3 - GMAC2
	{
		SET_REG_FIELD(CLKEN2, CLKEN2_GMAC2, 1);
	}
}
#endif //#if defined (GMAC_MODULE_TYPE)



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureADCClock                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  adc_freq -  adc frequency in Hz.                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets ADC clock in Hzs                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void    CLK_ConfigureADCClock (UINT32 adc_freq)
{
	UINT32  adc_div = DIV_ROUND(CLK_GetTimerFreq() , adc_freq);
	UINT32  adc_div_val = 5;  // value to set to register. If needed, round to the nearest divider.

		 if (adc_div <= 1)   adc_div_val = 0;
	else if (adc_div <= 2)   adc_div_val = 1;
	else if (adc_div <= 4)   adc_div_val = 2;
	else if (adc_div <= 8)   adc_div_val = 3;
	else if (adc_div <= 16)  adc_div_val = 4;

	SET_REG_FIELD(CLKDIV1, CLKDIV1_ADCCKDIV, adc_div_val);

	/* Wait 10usec for PLL:   */
	CLK_Delay_MicroSec(10);

	return;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetADCClock                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:          adc frequency in Hz.                                                                  */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets ADC clock in Hz                                                      */
/*---------------------------------------------------------------------------------------------------------*/
int    CLK_GetADCClock (void)
{
	return  CLK_GetTimerFreq() / (1 << READ_REG_FIELD(CLKDIV1, CLKDIV1_ADCCKDIV));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureSDClock                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum -  SD module number                                                              */
/*                                                                                                         */
/* Returns:         SD clock frequency                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the SD clock to frequency closest to 48MHz from beneath                     */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_ConfigureSDClock(UINT32 sdNum)
{
	UINT32  divider;
	UINT32  pll0Freq;   //In Hz
	UINT32  sdhci_clock;   //In Hz
	UINT32  target_freq;   //In Hz


	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum >= SD_NUM_OF_MODULES)
	{
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
	}

	pll0Freq = CLK_GetPll0Freq();
	HAL_PRINT("clk_drv: %s pll0Freq =%d \n",__FUNCTION__, pll0Freq);

#ifndef _PALLADIUM_
	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate rounded up divider to produce closest to Target output clock                              */
	/*-----------------------------------------------------------------------------------------------------*/
	target_freq = SD_CLK_TARGET_FREQ_A1;

	divider = (pll0Freq % target_freq == 0) ? (pll0Freq / target_freq) :          \
			(pll0Freq / target_freq) + 1;
#endif
	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting clock divider                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum == SD1_DEV)  // SD Card
	{
#ifdef _PALLADIUM_
		divider = 2;     /* Set maximum freq for Palladium (in reality around 600KHz) */
#endif
		SET_REG_FIELD(CLKDIV2, CLKDIV2_SD1CKDIV, (divider/2)-1);
		HAL_PRINT("clk_drv: %s CLKDIV2_SD1CKDIV =%d sdhci =%d\n",__FUNCTION__, READ_REG_FIELD(CLKDIV2, CLKDIV2_SD1CKDIV), sdNum);
	}
	else if (sdNum == SD2_DEV)  // eMMC
	{
#ifdef _PALLADIUM_
		divider = 1;     /* Set maximum freq for Palladium (in reality around 600KHz) */
#endif
		SET_REG_FIELD(CLKDIV1, CLKDIV1_MMCCKDIV, divider-1);
		HAL_PRINT("clk_drv: %s CLKDIV1_MMCCKDIV =%d sdhci =%d\n",__FUNCTION__, READ_REG_FIELD(CLKDIV1, CLKDIV1_MMCCKDIV),sdNum);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait to the divider to stabilize (according to spec)                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	/*-----------------------------------------------------------------------------------------------------*/
	/* SD clock uses always PLL0                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	if (CLK_GetPll0Freq() == 800000000)
	{
		SET_REG_FIELD(CLKSEL, CLKSEL_SDCKSEL, CLKSEL_SDCKSEL_PLL0);
	}
	else
	{
		SET_REG_FIELD(CLKSEL, CLKSEL_SDCKSEL, CLKSEL_SDCKSEL_PLL1);
	}

	sdhci_clock = pll0Freq/divider;
	HAL_PRINT("clk_drv: %s sdhci_clock =%d sdhci =%d\n",__FUNCTION__, sdhci_clock, sdNum);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure Clock Enable Register                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum == SD1_DEV)
	{
		SET_REG_FIELD(CLKEN2, CLKEN2_SDHC, 1);
	}
	else if (sdNum == SD2_DEV)
	{
		SET_REG_FIELD(CLKEN2, CLKEN2_MMC, 1);
	}

	return sdhci_clock;
}


#if defined (EMC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetEth                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of EMC module                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetEMC(UINT32 deviceNum)
{
	if (deviceNum == 0)
	{
		SET_REG_FIELD(IPSRST1, IPSRST1_EMC1, 1);
		SET_REG_FIELD(IPSRST1, IPSRST1_EMC1, 0);
	}
	else
	{
		SET_REG_FIELD(IPSRST1, IPSRST1_EMC2, 1);
		SET_REG_FIELD(IPSRST1, IPSRST1_EMC2, 0);
	}
}
#endif // #if defined (EMC_MODULE_TYPE)


#if defined (FIU_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetFIU                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of FIU                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetFIU (void)
{
	SET_REG_FIELD(IPSRST1, IPSRST1_SPI0, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_SPI0, 0);

	SET_REG_FIELD(IPSRST1, IPSRST1_SPI3, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_SPI3, 0);

	SET_REG_FIELD(IPSRST3, IPSRST3_SPIX, 1);
	SET_REG_FIELD(IPSRST3, IPSRST3_SPIX, 0);
}
#endif //#if defined (FIU_MODULE_TYPE)

#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetUART                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of all UARTs. 0 and 1 have shared reset. 2 and 3 too.   */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetUART (void)
{
	 SET_REG_FIELD(IPSRST1, IPSRST1_UART01, 1);
	 SET_REG_FIELD(IPSRST1, IPSRST1_UART01, 0);

	 SET_REG_FIELD(IPSRST1, IPSRST1_UART23, 1);
	 SET_REG_FIELD(IPSRST1, IPSRST1_UART23, 0);
}
#endif // #if defined (UART_MODULE_TYPE)


#if defined (AES_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetAES                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of AES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetAES (void)
{
	SET_REG_FIELD(IPSRST1, IPSRST1_AES, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_AES, 0);
}
#endif //if defined (AES_MODULE_TYPE)


#if defined (MC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetMC                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of MC                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetMC (void)
{
	serial_printf(">MC reset\n");
	SET_REG_FIELD(IPSRST1, IPSRST1_MC, 1);
	CLK_Delay_MicroSec(100);
	SET_REG_FIELD(IPSRST1, IPSRST1_MC, 0);
	CLK_Delay_MicroSec(100);


	/* Force re-training of DDR (because DDR module is reinitialized*/
	SET_REG_FIELD(INTCR2, INTCR2_MC_INIT, 0);
}
#endif // #if defined (MC_MODULE_TYPE)


#if defined (TMC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetTIMER                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of Timer                                                */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetTIMER (UINT32 deviceNum)
{
	if (deviceNum <= 4)
	{
		SET_REG_FIELD(IPSRST1, IPSRST1_TIM0_4, 1);
		SET_REG_FIELD(IPSRST1, IPSRST1_TIM0_4, 0);
	}
	else if (deviceNum <= 9)
	{
		SET_REG_FIELD(IPSRST1, IPSRST1_TIM5_9, 1);
		SET_REG_FIELD(IPSRST1, IPSRST1_TIM5_9, 0);
	}
	else
	{
		SET_REG_FIELD(IPSRST3, IPSRST3_TIMER10_14, 1);
		SET_REG_FIELD(IPSRST3, IPSRST3_TIMER10_14, 0);
	}

}
#endif // #if defined (TMC_MODULE_TYPE)


#if defined (SD_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetSD                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum -  SD module number                                                              */
/*                                                                                                         */
/* Returns:         DEFS_STATUS Error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of SD                                                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_ResetSD (UINT32 sdNum)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum >= SD_NUM_OF_MODULES)
	{
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
	}

	if (sdNum == SD1_DEV)
	{
		SET_REG_FIELD(IPSRST2, IPSRST2_SDHC, 1);
		SET_REG_FIELD(IPSRST2, IPSRST2_SDHC, 0);
	}
	else if (sdNum == SD2_DEV)
	{
		SET_REG_FIELD(IPSRST2, IPSRST2_SDHC, 1);
		SET_REG_FIELD(IPSRST2, IPSRST2_SDHC, 0);
	}

	return DEFS_STATUS_OK;
}
#endif // #if defined (SD_MODULE_TYPE)


#if defined (GMAC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetGMAC                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of GMAC                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetGMAC (UINT32 deviceNum)
{
	if (deviceNum == 2)
	{
		SET_REG_FIELD(IPSRST2, IPSRST2_GMAC1, 1);
		SET_REG_FIELD(IPSRST2, IPSRST2_GMAC1, 0);
	}
	else if (deviceNum == 3)
	{
		SET_REG_FIELD(IPSRST2, IPSRST2_GMAC2, 1);
		SET_REG_FIELD(IPSRST2, IPSRST2_GMAC2, 0);
	}

}
#endif // #if defined (GMAC_MODULE_TYPE)

#if defined (PSPI_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetPSPI                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of PSPI                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_ResetPSPI(UINT32 deviceNum)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (deviceNum >= PSPI_NUM_OF_MODULES)
	{
		return HAL_ERROR_BAD_DEVNUM;
	}

	if (deviceNum == PSPI1_DEV)
	{
		SET_REG_FIELD(IPSRST2, IPSRST2_PSPI1, 1);
		SET_REG_FIELD(IPSRST2, IPSRST2_PSPI1, 0);
	}
	else if (deviceNum == PSPI2_DEV)
	{
		SET_REG_FIELD(IPSRST2, IPSRST2_PSPI2, 1);
		SET_REG_FIELD(IPSRST2, IPSRST2_PSPI2, 0);
	}

	return DEFS_STATUS_OK;
}
#endif // #if defined (PSPI_MODULE_TYPE)

//Calculates the PLL frequency in Hz given PLL register value
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_CalculatePLLFrequency                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  pllVal    -                                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the PLL frequency in Hz                                           */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  CLK_CalculatePLLFrequency (UINT32 pllVal)
{
	UINT32  FIN         = EXT_CLOCK_FREQUENCY_KHZ; // 25MHz in KHz units
	UINT32  FOUT        = 0;
	UINT32  NR          = 0;
	UINT32  NF          = 0;
	UINT32  NO          = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Extract PLL fields:                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	NR = READ_VAR_FIELD(pllVal, PLLCONn_INDV);   /* PLL Input Clock Divider */
	NF = READ_VAR_FIELD(pllVal, PLLCONn_FBDV);   /* PLL VCO Output Clock Feedback Divider). */
	NO = (READ_VAR_FIELD(pllVal, PLLCONn_OTDV1)) * (READ_VAR_FIELD(pllVal, PLLCONn_OTDV2));   /* PLL Output Clock Divider 1 */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate PLL frequency in Hz:                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	FOUT = ((10*FIN*NF)/(NO*NR));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Notice: for better accurecy we multiply the "MONE" in 10, and later in 100 to get to Hz units.      */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/* Returning value in Hertz:                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	return  FOUT*100 ;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll0Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL0 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPll0Freq (void)
{
	UINT32  pllVal      = 0;

	pllVal = REG_READ(PLLCON0);
	return CLK_CalculatePLLFrequency(pllVal);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll1Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL1 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPll1Freq (void)
{
	UINT32  pllVal      = 0;

	pllVal = REG_READ(PLLCON1);

	return (CLK_CalculatePLLFrequency(pllVal)/2);

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll2Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL2 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPll2Freq (void)
{
	UINT32  pllVal      = 0;

	pllVal = REG_READ(PLLCON2);

	return (CLK_CalculatePLLFrequency(pllVal)/2);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetCPUFreq                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates CPU frequency in Hz                                            */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetCPUFreq (void)
{
	UINT32  FOUT        = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate CPU clock:                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (READ_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL) == CLKSEL_CPUCKSEL_PLL0)
	{
		FOUT = CLK_GetPll0Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL) == CLKSEL_CPUCKSEL_PLL1)
	{
		FOUT = CLK_GetPll1Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL) == CLKSEL_CPUCKSEL_CLKREF)
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* Reference clock 25MHz:                                                                          */
		/*-------------------------------------------------------------------------------------------------*/
		FOUT = EXT_CLOCK_FREQUENCY_HZ; //FOUT is specified in MHz
	}
	else
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* External clock, assume low freq ref clock (25MHz):                                              */
		/*-------------------------------------------------------------------------------------------------*/
		FOUT = EXT_CLOCK_FREQUENCY_HZ; //FOUT is specified in MHz
	}


#ifdef _DEBUG_
		*(UINT32*)(PCIMBX_PHYS_BASE_ADDR+0x24)  = FOUT; /* ROM_MAILBOX_DEBUG9 */
#endif

	/*-----------------------------------------------------------------------------------------------------*/
	/* Returing value in Hertz                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	return FOUT;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetMCFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates MC frequency                                                   */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetMCFreq (void)
{
	UINT32  FOUT        = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate MC clock:                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	if (READ_REG_FIELD(CLKSEL, CLKSEL_MCCKSEL) == CLKSEL_MCCKSEL_PLL1)
	{
		FOUT = CLK_GetPll1Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_MCCKSEL) == CLKSEL_MCCKSEL_MCBPCK)
	{
		FOUT = 0; // external output on MCBPCK pin. Unknown frequency.
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_MCCKSEL) == CLKSEL_MCCKSEL_CLKREF)
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* Reference clock 25MHz:                                                                          */
		/*-------------------------------------------------------------------------------------------------*/
		FOUT = EXT_CLOCK_FREQUENCY_HZ; //FOUT is specified in MHz
	}
	else
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* External clock, assume low freq ref clock (25MHz):                                              */
		/*-------------------------------------------------------------------------------------------------*/
		FOUT = EXT_CLOCK_FREQUENCY_HZ; //FOUT is specified in MHz
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Returing value in Hertz                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	return FOUT;

}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetAPBFreq                                                                         */
/*                                                                                                         */
/* Parameters:      apb number,1 to 5                                                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns APB frequency  in Hz                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetAPBFreq (APB_CLK apb)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Avalilable APBs between 1 to 5                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_COND_CHECK((apb <= SPI3) && (apb >= APB1) , DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

	/*-----------------------------------------------------------------------------------------------------*/
	/* In Yarkon APB frequency is CPU frequency divided by AHB0 Clock dividor, AHB1 Clock dividor and APB  */
	/* Clock divider                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/

	return  CLK_GetCPUFreq()  / CLK_GetPLL0toAPBdivisor(apb);  // UINT32  CLK_GetPLL0toAPBdivisor (APB_CLK apb)
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetCPFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns CP frequency in Hz                                                */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetCPFreq (void)
{
	UINT32  clk2Div = 0;

	UINT32  clk4Div = 0;

	clk2Div = (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV) + 1);

	clk4Div = (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV) + 1);


	/*-----------------------------------------------------------------------------------------------------*/
	/* In Poleg APB frequency is CPU frequency divided by AHB0 Clock dividor, AHB1 Clock dividor and APB  */
	/* Clock divider                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	return  (CLK_GetCPUFreq()  / (clk2Div * clk4Div)) ;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetTimerFreq                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates tiemr frequency in Hz                                          */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetTimerFreq (void)
{
	UINT32  FOUT        = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate CPU clock:                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if      (READ_REG_FIELD(CLKSEL, CLKSEL_TIMCKSEL) == CLKSEL_TIMCKSEL_PLL0)
	{
		FOUT = CLK_GetPll0Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_TIMCKSEL) == CLKSEL_TIMCKSEL_PLL1)
	{
		FOUT = CLK_GetPll1Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_TIMCKSEL) == CLKSEL_TIMCKSEL_PLL2)
	{
		FOUT = CLK_GetPll2Freq();
	}
	else
	{
		/*-------------------------------------------------------------------------------------------------*/
		/* External clock, assume low freq ref clock (25MHz):                                              */
		/*-------------------------------------------------------------------------------------------------*/
		FOUT = EXT_CLOCK_FREQUENCY_HZ; //FOUT is specified in MHz
	}

	FOUT = FOUT / (READ_REG_FIELD(CLKDIV1, CLKDIV1_TIMCKDIV) + 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Returing value in Hertz                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	return FOUT;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_SetCPFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets CP frequency in Hz                                                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_SetCPFreq (UINT32  cpFreq)
{
	UINT32  clkDiv = CLK_GetCPUFreq() / cpFreq;

	switch (clkDiv)
	{
		case 1:
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV1);
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV1);
			break;
		case 2:
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV2);
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV1);
			break;
		case 3:
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV3);
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV1);
			break;
		case 4:
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV2);
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV2);
			break;
		case 6:
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV3);
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV2);
			break;
		case 8:
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV4);
			SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV2);
			break;
		default:
			return DEFS_STATUS_INVALID_PARAMETER;
	}

	CLK_Delay_MicroSec(20);

	return  DEFS_STATUS_OK;

}




#if defined (SD_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetSDClock                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum -  SD module number                                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the SD base clock frequency in Hz                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetSDClock (UINT32 sdNum)
{
	UINT32  divider;
	UINT32  pll0Freq;   //In Hz

	pll0Freq = CLK_GetPll0Freq();
	if ((SD_DEV_NUM_T)sdNum == SD1_DEV)
	{
		divider = 1 + READ_REG_FIELD(CLKDIV2, CLKDIV2_SD1CKDIV);
	}
	else if (sdNum == SD2_DEV)
	{
		divider = 1 + READ_REG_FIELD(CLKDIV1, CLKDIV1_MMCCKDIV);
	}
	else
	{
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
	}

	return (pll0Freq/divider);
}
#endif  //#if defined (SD_MODULE_TYPE)


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigurePCIClock                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs configuration of PCI clock depending on                          */
/*                  presence of VGA BIOS as specified by STRAP13                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ConfigurePCIClock (void)
{

	// Need to pgm the PCI clock to 96 MHz.

	// Done in following steps:
	// 1.  Delay 2us
	// 2.  Change CLKSEL1.GFXCKSEL ( bits 17-16)  from 1h to 3h (select PLL2)
	// 3.  Delay 2 us
	// 4.  Change CLKDIV1.PCICKDIV (bits 5-2) from 02h to 04h (divide by 5)
	// 5.  Delay 2us

	CLK_Delay_MicroSec(2);
	SET_REG_FIELD(CLKSEL, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL2);
	CLK_Delay_MicroSec(2);

	SET_REG_FIELD(CLKDIV1, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(4));
	SET_REG_FIELD(CLKDIV2, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(3));

	CLK_Delay_MicroSec(2);

	return;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPCIClock                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns configuration of PCI clock                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetPCIClock (void)
{

	// Need to pgm the PCI clock to 96 MHz.
	UINT32 pci_source = 0;
	UINT32 pci_sel =  READ_REG_FIELD(CLKSEL, CLKSEL_GFXCKSEL);

	if (pci_sel == 0)
		pci_source = CLK_GetPll0Freq();
	if (pci_sel == 1)
		pci_source = CLK_GetPll1Freq();
	if (pci_sel == 2)
		pci_source = EXT_CLOCK_FREQUENCY_HZ;
	if (pci_sel == 3)
		pci_source = CLK_GetPll2Freq();


	return (pci_source / (1 + (UINT32)READ_REG_FIELD(CLKDIV1, CLKDIV1_PCICKDIV)));

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetGFXClock                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                 This routine returns configuration of GFX clock                                         */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetGFXClock (void)
{

	// Need to pgm the PCI clock to 96 MHz.
	UINT32 pci_source = 0;
	UINT32 pci_sel =  READ_REG_FIELD(CLKSEL, CLKSEL_GFXCKSEL);

	if (pci_sel == 0)
		pci_source = CLK_GetPll0Freq();
	if (pci_sel == 1)
		pci_source = CLK_GetPll1Freq();
	if (pci_sel == 2)
		pci_source = EXT_CLOCK_FREQUENCY_HZ;
	if (pci_sel == 3)
		pci_source = CLK_GetPll2Freq();


	return (pci_source / (1 + (UINT32)READ_REG_FIELD(CLKDIV2, CLKDIV2_GFXCKDIV)));

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Configure_CPU_MC_Clock                                                             */
/*                                                                                                         */
/* Parameters:      mcFreq - frequency in Hz of MC                                                         */
/* Parameters:      cpuFreq - frequency in Hz of CPU                                                       */
/* Parameters:      pll0Freq - frequency in Hz of PLL0, if it's not used by CPU we can set it to something else*/
/* Returns:         status                                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs configuration of MC and CPU Clocks                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_Configure_CPU_MC_Clock (UINT32 mcFreq , UINT32 cpuFreq, UINT32 pll0_freq)
{
	UINT32 PLLCON1_l = 0;
	UINT32 PLLCON0_l = 0;
	UINT32 clksel_l = 0;

	UINT32 clksel_clkref_l = 0;
	UINT32 pll0_freq_tmp = 0;

	UINT32 PLLCON0_current_reg_value = 0;
	UINT32 PLLCON1_current_reg_value = 0;

	// If clk already set to this frequency, skip this func ( round down the lowest word):
	if ((MSW(mcFreq) == MSW(CLK_GetMCFreq())) &&
	    (MSW(cpuFreq) == MSW(CLK_GetCPUFreq())) &&
	    (pll0_freq == 0))
	{
		return DEFS_STATUS_SYSTEM_NOT_INITIALIZED;
	}

	PLLCON0_current_reg_value = REG_READ(PLLCON0);
	PLLCON1_current_reg_value = REG_READ(PLLCON1);

	clksel_l           = REG_READ(CLKSEL);
	clksel_clkref_l    = clksel_l;

	PLLCON0_l = PLLCON0_current_reg_value;
	PLLCON1_l = PLLCON1_current_reg_value;

	if(pll0_freq != 0)
		pll0_freq_tmp =  pll0_freq;
	else
		pll0_freq_tmp = cpuFreq;

	/*---------------------------------------------------------------------------------------------------------*/
	/* PLLCON 1 possible values (notice that PLL1 in Z2 has a divider /2, so OTDV1 is smaller in half	   */
	/*---------------------------------------------------------------------------------------------------------*/
	if	( mcFreq <=  333333334)    PLLCON1_l =	CLK_333MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  444444445)    PLLCON1_l =	CLK_444MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  500000000)    PLLCON1_l =	CLK_500MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  600000000)    PLLCON1_l =	CLK_600MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  666666667)    PLLCON1_l =	CLK_666MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  700000000)    PLLCON1_l =	CLK_700MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  720000000)    PLLCON1_l =	CLK_720MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  750000000)    PLLCON1_l =	CLK_750MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  775000000)    PLLCON1_l =	CLK_775MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  787500000)    PLLCON1_l =	CLK_787_5MHZ_PLLCON1_REG_CFG	 ;
	else if ( mcFreq <=  800000000)    PLLCON1_l =	CLK_800MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  825000000)    PLLCON1_l =	CLK_825MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  850000000)    PLLCON1_l =	CLK_850MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  900000000)    PLLCON1_l =	CLK_900MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <=  950000000)    PLLCON1_l =	CLK_950MHZ_PLLCON1_REG_CFG     ;
	else if ( mcFreq <= 1000000000)    PLLCON1_l =	CLK_1000MHZ_PLLCON1_REG_CFG    ;
	else if ( mcFreq <= 1060000000)    PLLCON1_l =	CLK_1066MHZ_PLLCON1_REG_CFG    ;
	else   return DEFS_STATUS_CLK_ERROR;


	/*---------------------------------------------------------------------------------------------------------*/
	/* PLLCON 0 possible values (notice that PLL1 in Z2 has a divider /2, so OTDV1 is smaller in half	   */
	/*---------------------------------------------------------------------------------------------------------*/
	if	( pll0_freq_tmp <=  125000000)        PLLCON0_l =  CLK_125MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  333333334)    PLLCON0_l =  CLK_333MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  500000000)    PLLCON0_l =  CLK_500MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  600000000)    PLLCON0_l =  CLK_600MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  666666667)    PLLCON0_l =  CLK_666MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  700000000)    PLLCON0_l =  CLK_700MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  720000000)    PLLCON0_l =  CLK_720MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  750000000)    PLLCON0_l =  CLK_750MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  800000000)    PLLCON0_l =  CLK_800MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  825000000)    PLLCON0_l =  CLK_825MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  850000000)    PLLCON0_l =  CLK_850MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  900000000)    PLLCON0_l =  CLK_900MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <=  950000000)    PLLCON0_l =  CLK_950MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <= 1000000000)    PLLCON0_l =  CLK_1000MHZ_PLLCON0_REG_CFG	;
	else if ( pll0_freq_tmp <= 1060000000)    PLLCON0_l =  CLK_1066MHZ_PLLCON0_REG_CFG	;
	else   return DEFS_STATUS_CLK_ERROR;

	/*-----------------------------------------------------------------------------------------------------*/
	/* CLKSEL handling: Check if MC freq != CPU freq. If so, need to set CPU to different PLL.             */
	/*-----------------------------------------------------------------------------------------------------*/
	if (mcFreq != cpuFreq)
	{
		/* ERROR: can't select 3 different settings */
		if (pll0_freq != 0)
			return DEFS_STATUS_FAIL;

		/*-----------------------------------------------------------------------------------------------------*/
		/* MC and CPU use different PLLs . CPU on PLL0, MC on PLL1                                             */
		/*-----------------------------------------------------------------------------------------------------*/
		SET_VAR_FIELD(clksel_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_PLL0);
		SET_VAR_FIELD(clksel_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_PLL1);
	}
	else // if (mcFreq == cpuFreq)
	{

		/*-----------------------------------------------------------------------------------------------------*/
		/* MC and CPU use same PLL1                                                                            */
		/*-----------------------------------------------------------------------------------------------------*/
		SET_VAR_FIELD(clksel_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_PLL1);
		SET_VAR_FIELD(clksel_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_PLL1);
	}



	/*-----------------------------------------------------------------------------------------------------*/
	/* PLL0 handling : Check if PLL is set to the value CPU is set: Notice:even if MC freq == CPU freq,    */
	/* both will be connected to PLL1, and PLL0 will be turned off.                                 	   */
	/*-----------------------------------------------------------------------------------------------------*/
	if (((MSW(CLK_CalculatePLLFrequency(PLLCON0_current_reg_value)) != MSW(cpuFreq)) &&
	    (cpuFreq != mcFreq))

		// for the case where PLL0 is used for other means then setting CPU0 freq:
		|| (pll0_freq != 0))
	{
		/*-----------------------------------------------------------------------------------------------------*/
		/* Switch clock sources to external clock for all muxes (only for PLL0 users)     	               */
		/*-----------------------------------------------------------------------------------------------------*/
		if (READ_VAR_FIELD(clksel_l, CLKSEL_CPUCKSEL) == CLKSEL_CPUCKSEL_PLL0)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_UARTCKSEL) == CLKSEL_UARTCKSEL_PLL0)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_CLKOUTSEL) == CLKSEL_CLKOUTSEL_PLL0)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_TIMCKSEL) == CLKSEL_TIMCKSEL_PLL0)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_SUCKSEL) == CLKSEL_SUCKSEL_PLL0)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_SUCKSEL,	 CLKSEL_SUCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_SDCKSEL) == CLKSEL_SDCKSEL_PLL0)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_SDCKSEL,	 CLKSEL_SDCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_GFXCKSEL) == CLKSEL_GFXCKSEL_PLL0)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_CLKREF);

		if (pll0_freq != 0)
		{
			SET_VAR_FIELD(clksel_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_PLL0); // TOCK comes from PLL0 !
			SET_VAR_FIELD(clksel_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);
			SET_VAR_FIELD(clksel_l, CLKSEL_SUCKSEL,	 CLKSEL_SUCKSEL_PLL2);
			SET_VAR_FIELD(clksel_l, CLKSEL_SDCKSEL,	 CLKSEL_SDCKSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL1);
		}

		REG_WRITE(CLKSEL, clksel_clkref_l);
		CLK_Delay_MicroSec(20);

		/*-----------------------------------------------------------------------------------------------------*/
		/* Change PLL configuration (leave PLL in reset mode):                                                 */
		/*-----------------------------------------------------------------------------------------------------*/
		// Set PWDEN bit
		SET_VAR_FIELD(PLLCON0_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);

		/* set to power down == 1 */
		REG_WRITE(PLLCON0, PLLCON0_l);

		/*-----------------------------------------------------------------------------------------------------*/
		/* Wait 10usec for PLL:                                                                                */
		/*-----------------------------------------------------------------------------------------------------*/
		//3. Wait 20us or more;
		CLK_Delay_MicroSec(20);

		/*-----------------------------------------------------------------------------------------------------*/
		/* Enable PLLs after configuring. Delays to prevent power issues.                                      */
		/*-----------------------------------------------------------------------------------------------------*/
		SET_VAR_FIELD(PLLCON0_l, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
		REG_WRITE(PLLCON0, PLLCON0_l);
		CLK_Delay_MicroSec(20);


		/*-----------------------------------------------------------------------------------------------------*/
		/* Wait 750usec for PLL to stabilize:                                                                  */
		/*-----------------------------------------------------------------------------------------------------*/
		WaitForPllLock(PLL0);
		/* Wait 10usec for PLL:   */
		CLK_Delay_MicroSec(60);

		// return All that was on PLL0 back:
		REG_WRITE(CLKSEL, clksel_l);
		CLK_Delay_MicroSec(20);

	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* PLL1 handling                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	if (MSW(mcFreq) != MSW(CLK_GetMCFreq()))
	{
		/*-----------------------------------------------------------------------------------------------------*/
		/* if both freq are same, move everything to PLL1 and shutdown PLL0: Notice clksel_l is used for final value for CLKSEL reg !*/
		/*-----------------------------------------------------------------------------------------------------*/
		if ((mcFreq == cpuFreq)|| (pll0_freq != 0))
		{
			clksel_clkref_l = clksel_l;

			/*-----------------------------------------------------------------------------------------------------*/
			/* MC and CPU use same PLL1                                                                            */
			/*-----------------------------------------------------------------------------------------------------*/
			SET_VAR_FIELD(clksel_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);
			SET_VAR_FIELD(clksel_l, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_PLL2);
			SET_VAR_FIELD(clksel_l, CLKSEL_SDCKSEL,   CLKSEL_SDCKSEL_PLL1);
			SET_VAR_FIELD(clksel_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL1);

			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_CLKREF);
		}
		else
		{
			SET_VAR_FIELD(clksel_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_PLL0);
		 	SET_VAR_FIELD(clksel_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_PLL1);
		}

		/*-----------------------------------------------------------------------------------------------------*/
		/* check if other modules are using PLL1. If so, move them someplace else while the PLL1 is configuring*/
		/* Notice ! here we decide on clksel_clkref_l which is for the time of te switching only.              */
		/*-----------------------------------------------------------------------------------------------------*/


		if (READ_VAR_FIELD(clksel_l, CLKSEL_UARTCKSEL) == CLKSEL_UARTCKSEL_PLL1)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_CLKOUTSEL) == CLKSEL_CLKOUTSEL_PLL1)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_TIMCKSEL) == CLKSEL_TIMCKSEL_PLL1)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_SUCKSEL) == CLKSEL_SUCKSEL_PLL1)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_SDCKSEL) == CLKSEL_SDCKSEL_PLL1)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_SDCKSEL,   CLKSEL_SDCKSEL_CLKREF);

		if (READ_VAR_FIELD(clksel_l, CLKSEL_GFXCKSEL) == CLKSEL_GFXCKSEL_PLL1)
			SET_VAR_FIELD(clksel_clkref_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_CLKREF);


		/*-----------------------------------------------------------------------------------------------------*/
		/*  "hold" the clocks while PLL1 is reconfiured.                                                       */
		/*-----------------------------------------------------------------------------------------------------*/
		REG_WRITE(CLKSEL, clksel_clkref_l);
		CLK_Delay_MicroSec(60);

		/*-----------------------------------------------------------------------------------------------------*/
		/* Now reconfigure PLL1:                                                                               */
		/*-----------------------------------------------------------------------------------------------------*/
		SET_VAR_FIELD(PLLCON1_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);
		REG_WRITE(PLLCON1, PLLCON1_l);
		CLK_Delay_MicroSec(60);

		SET_VAR_FIELD(PLLCON1_l, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
		REG_WRITE(PLLCON1, PLLCON1_l);
		CLK_Delay_MicroSec(60);

		WaitForPllLock(PLL1);
		CLK_Delay_MicroSec(60);

		/*-----------------------------------------------------------------------------------------------------*/
		/* return everything back to PLL1, includeing the CPU                                                  */
		/*-----------------------------------------------------------------------------------------------------*/
		REG_WRITE(CLKSEL, clksel_l);
		CLK_Delay_MicroSec(60);

		if (mcFreq == cpuFreq)
		{
			// shutdown PLL0, not used any more:
			SET_VAR_FIELD(PLLCON0_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);
	    		REG_WRITE(PLLCON0, PLLCON0_l);
		}

		/* Force re-training of DDR (because clock is reinitialized*/
		SET_REG_FIELD(INTCR2, INTCR2_MC_INIT, 0);


	}

   	CLK_Verify_and_update_dividers();

	return DEFS_STATUS_OK;


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Verify_and_update_dividers                                                         */
/*                                                                                                         */
/* Parameters:      None                                                                                   */
/* Returns:         DEFS_STATUS                                                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine fix all the dividers after PLL change                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_Verify_and_update_dividers (void)
{
	UINT32 clkDiv;

	UINT32 clk4Freq = CLK_GetCPUFreq()/ (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV) + 1) / (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV) + 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Fix APB1 to be above 24MHz                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	clkDiv = (clk4Freq / (24*_1MHz_));
		if (clkDiv >= 8)
		clkDiv = CLKDIV2_APBxCKDIV8;
	else if (clkDiv >= 4)
		clkDiv = CLKDIV2_APBxCKDIV4;
	else if (clkDiv >=2 )
		clkDiv = CLKDIV2_APBxCKDIV2;
	else
		clkDiv = CLKDIV2_APBxCKDIV1;

	SET_REG_FIELD(CLKDIV2, CLKDIV2_APB1CKDIV, clkDiv);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Fix APB2-3 to be above 40MHz and below 50Mhz                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	clkDiv = (clk4Freq / (40 *_1MHz_));
	if (clkDiv >= 8)
		clkDiv = CLKDIV2_APBxCKDIV8;
	else if (clkDiv >= 4)
		clkDiv = CLKDIV2_APBxCKDIV4;
	else if (clkDiv >=2 )
		clkDiv = CLKDIV2_APBxCKDIV2;
	else
		clkDiv = CLKDIV2_APBxCKDIV1;

	SET_REG_FIELD(CLKDIV2, CLKDIV2_APB2CKDIV, clkDiv);

	if ((CLK_GetAPBFreq(APB2) > (50 * _1MHz_)) && (clkDiv < CLKDIV2_APBxCKDIV8))
		clkDiv++;

	SET_REG_FIELD(CLKDIV2, CLKDIV2_APB2CKDIV, clkDiv);
	SET_REG_FIELD(CLKDIV2, CLKDIV2_APB3CKDIV, clkDiv);


	/*-----------------------------------------------------------------------------------------------------*/
	/* Fix APB4 to be above 25MHz                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	clkDiv = (clk4Freq / (25 * _1MHz_));
	if (clkDiv >= 8)
		clkDiv = CLKDIV2_APBxCKDIV8;
	else if (clkDiv >= 4)
		clkDiv = CLKDIV2_APBxCKDIV4;
	else if (clkDiv >=2 )
		clkDiv = CLKDIV2_APBxCKDIV2;
	else
		clkDiv = CLKDIV2_APBxCKDIV1;

	SET_REG_FIELD(CLKDIV2, CLKDIV2_APB4CKDIV, clkDiv);

	if ((CLK_GetAPBFreq(APB4) < (25 * _1MHz_)) && (clkDiv > CLKDIV2_APBxCKDIV1))
		clkDiv--;
	if ((CLK_GetAPBFreq(APB4) > (50 * _1MHz_)) && (clkDiv < CLKDIV2_APBxCKDIV8))
		clkDiv++;

	SET_REG_FIELD(CLKDIV2, CLKDIV2_APB4CKDIV, clkDiv);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Fix APB5 to be above 50MHz and below 100Mhz                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	clkDiv = (clk4Freq / (100 *_1MHz_));
	if (clkDiv >= 8)
		clkDiv = CLKDIV2_APBxCKDIV8;
	else if (clkDiv >= 4)
		clkDiv = CLKDIV2_APBxCKDIV4;
	else if (clkDiv >=2 )
		clkDiv = CLKDIV2_APBxCKDIV2;
	else
		clkDiv = CLKDIV2_APBxCKDIV1;

	SET_REG_FIELD(CLKDIV2, CLKDIV2_APB5CKDIV, clkDiv);

		if (CLK_GetAPBFreq(APB5) < (50 * _1MHz_))
			clkDiv--;
		if (CLK_GetAPBFreq(APB5) > (100 * _1MHz_))
			clkDiv++;

		SET_REG_FIELD(CLKDIV2, CLKDIV2_APB5CKDIV, clkDiv);


	/*-----------------------------------------------------------------------------------------------------*/
	/* Fix SPI0,3 to be above 40MHz and below 50Mhz                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	clkDiv = (clk4Freq / (40 *_1MHz_) );
	SET_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV, CLKDIV3_SPI0CKDV_DIV(clkDiv));

	if (CLK_GetAPBFreq(SPI0) <= (40 * _1MHz_))
		clkDiv--;
	if (CLK_GetAPBFreq(SPI0) > (51 * _1MHz_))
		clkDiv++;


	SET_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV, CLKDIV3_SPI0CKDV_DIV(clkDiv));
	SET_REG_FIELD(CLKDIV3, CLKDIV3_SPIXCKDV, CLKDIV3_SPIXCKDV_DIV(clkDiv));
	SET_REG_FIELD(CLKDIV1, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(clkDiv));

	CLK_ConfigurePCIClock();
	CLK_ConfigureADCClock(25 * _1MHz_);

	return DEFS_STATUS_OK;


}


#if defined (FIU_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureFIUClock                                                                  */
/*                                                                                                         */
/* Parameters:      UINT8  fiu, UINT8 clkDiv                                                               */
/* Returns:         DEFS_STATUS                                                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config the FIU clock (according to the header )                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_ConfigureFIUClock(FIU_MODULE_T  fiu, UINT8 clkDiv)
{

	 /*----------------------------------------------------------------------------------------------------*/
	 /* Defines the clock divide ratio from AHB to FIU0 clock.                                             */
	 /*----------------------------------------------------------------------------------------------------*/
	 UINT32  ratio = 0;

	 /*----------------------------------------------------------------------------------------------------*/
	 /* Ignored if FIU_Clk_Divider is either 0 or 0FFh.                                                    */
	 /*----------------------------------------------------------------------------------------------------*/
	 if ( (clkDiv == 0) || (clkDiv == 0xFF))
		return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;

	 /* set SPIn clk div */
	 switch (fiu)
	 {
		case FIU_MODULE_0:
			SET_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV,  (CLKDIV3_SPI0CKDV_DIV(clkDiv) & 0x1F));
			break;
		case  FIU_MODULE_3:
			SET_REG_FIELD(CLKDIV1, CLKDIV1_AHB3CKDIV, (CLKDIV1_AHB3CK_DIV(clkDiv)   & 0x1F));
			break;
		case  FIU_MODULE_X:
			SET_REG_FIELD(CLKDIV3, CLKDIV3_SPIXCKDV,  (CLKDIV3_SPIXCKDV_DIV(clkDiv) & 0x1F));
			break;
		default:
			return DEFS_STATUS_INVALID_PARAMETER;


	 }

	 /*----------------------------------------------------------------------------------------------------*/
	 /* After changing this field, ensure a delay of 25 SPI0 clock cycles before changing CPUCKSEL field in*/
	 /* CLKSEL register or accessing the AHB18 bus.                                                        */
	 /*----------------------------------------------------------------------------------------------------*/
	 ratio = READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV) * READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV) * clkDiv;

	 /* delay is according to ratio. Take some buffer too */
	 CLK_Delay_Cycles(50 * ratio);

	 return DEFS_STATUS_OK;
}
#endif //  defined (FIU_MODULE_TYPE)


#if defined (FIU_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetFIUClockDiv                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config the FIU clock (according to the header )                           */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 CLK_GetFIUClockDiv (FIU_MODULE_T  fiu)
{

	 /*----------------------------------------------------------------------------------------------------*/
	 /* Defines the clock divide ratio from AHB to FIU0 clock.1                                            */
	 /*----------------------------------------------------------------------------------------------------*/
	 switch (fiu)
	 {
		case FIU_MODULE_0:
			return READ_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV) + 1;
		case  FIU_MODULE_3:
			return READ_REG_FIELD(CLKDIV1, CLKDIV1_AHB3CKDIV) + 1;
		case  FIU_MODULE_X:
			return READ_REG_FIELD(CLKDIV3, CLKDIV3_SPIXCKDV) + 1;
		default:
			return 0xFF;
	 }


}
#endif// defined (FIU_MODULE_TYPE)


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module instance registers                                      */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_PrintRegs (void)
{
	HAL_PRINT("CLK:\n");
	HAL_PRINT("------\n");
	HAL_PRINT("CLKEN1              = 0x%08X\n",    REG_READ(CLKEN1  ));
	HAL_PRINT("CLKEN2              = 0x%08X\n",    REG_READ(CLKEN2  ));
	HAL_PRINT("CLKEN3              = 0x%08X\n",    REG_READ(CLKEN3  ));
	HAL_PRINT("CLKSEL              = 0x%08X\n",    REG_READ(CLKSEL  ));
	HAL_PRINT("CLKDIV1             = 0x%08X\n",    REG_READ(CLKDIV1 ));
	HAL_PRINT("CLKDIV2             = 0x%08X\n",  REG_READ(CLKDIV2       ));
	HAL_PRINT("CLKDIV3             = 0x%08X\n",  REG_READ(CLKDIV3       ));
	HAL_PRINT("PLLCON0             = 0x%08X\n",  REG_READ(PLLCON0       ));
	HAL_PRINT("PLLCON1             = 0x%08X\n",  REG_READ(PLLCON1       ));
	HAL_PRINT("PLLCON2             = 0x%08X\n",  REG_READ(PLLCON2       ));
	HAL_PRINT("SWRSTR              = 0x%08X\n",  REG_READ(SWRSTR        ));
	HAL_PRINT("IRQWAKECON          = 0x%08X\n",  REG_READ(IRQWAKECON    ));
	HAL_PRINT("IRQWAKEFLAG         = 0x%08X\n",  REG_READ(IRQWAKEFLAG   ));
	HAL_PRINT("IPSRST1             = 0x%08X\n",  REG_READ(IPSRST1       ));
	HAL_PRINT("IPSRST2             = 0x%08X\n",  REG_READ(IPSRST2       ));
	HAL_PRINT("IPSRST3             = 0x%08X\n",  REG_READ(IPSRST3       ));
	HAL_PRINT("WD0RCR              = 0x%08X\n",  REG_READ(WD0RCR        ));
	HAL_PRINT("WD1RCR              = 0x%08X\n",  REG_READ(WD1RCR        ));
	HAL_PRINT("WD2RCR              = 0x%08X\n",  REG_READ(WD2RCR        ));
	HAL_PRINT("SWRSTC1             = 0x%08X\n",  REG_READ(SWRSTC1       ));
	HAL_PRINT("SWRSTC2             = 0x%08X\n",  REG_READ(SWRSTC2       ));
	HAL_PRINT("SWRSTC3             = 0x%08X\n",  REG_READ(SWRSTC3       ));
	HAL_PRINT("SWRSTC4             = 0x%08X\n",  REG_READ(SWRSTC4       ));
	HAL_PRINT("CORSTC              = 0x%08X\n",  REG_READ(CORSTC        ));
	HAL_PRINT("PLLCONG             = 0x%08X\n",  REG_READ(PLLCONG       ));
	HAL_PRINT("AHBCKFI             = 0x%08X\n",  REG_READ(AHBCKFI       ));
	HAL_PRINT("SECCNT              = 0x%08X\n",  REG_READ(SECCNT        ));
	HAL_PRINT("CNTR25M             = 0x%08X\n",  REG_READ(CNTR25M       ));


	HAL_PRINT("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_PrintVersion (void)
{
	HAL_PRINT("CLK        = %d\n", CLK_MODULE_TYPE);
}

