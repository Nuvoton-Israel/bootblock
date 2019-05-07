/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   clk_regs.h                                                               */
/*            This file contains definitions of CLK registers                 */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef CLK_REGS_H
#define CLK_REGS_H

#include __CHIP_H_FROM_DRV()


/**************************************************************************************************************************/
/*   Clock Enable 1 Register (CLKEN1) (Changed in )                                                                       */
/**************************************************************************************************************************/
#define  CLKEN1                         (CLK_BASE_ADDR + 0x00) , CLK_ACCESS, 32         /* Offset: CLK_BA + 00h */
#define  CLKEN1_SMB1                     31 , 1              /* 31 SMB1 (SMBus Interface 1 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_SMB0                     30 , 1              /* 30 SMB0 (SMBus Interface 0 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_SMB7                     29 , 1              /* 29 SMB7 (SMBus Interface 7 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_SMB6                     28 , 1              /* 28 SMB6 (SMBus Interface 6 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_ADC                      27 , 1              /* 27 ADC (ADC Clock Enable Bit). Eng. note: uses APB1 clock and ADC clock (24 MHz).                                     */
#define  CLKEN1_WDT                      26 , 1              /* 26 WDT (All Watchdog Timers Clock Enable Bit). Eng. note: uses APB1 clock and Timer clock (24 MHz).                   */
#define  CLKEN1_USBDEV3                  25 , 1              /* 25 USBDEV3 (USB Device 3 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN1_USBDEV6                  24 , 1              /* 24 USBDEV6 (USB Device 6 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN1_USBDEV5                  23 , 1              /* 23 USBDEV5 (USB Device 5 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN1_USBDEV4                  22 , 1              /* 22 USBDEV4 (USB Device 4 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN1_EMC2                     21 , 1              /* 21 EMC2 (EMC2 Clock Enable Bit). Eng. note: uses AHB8 clock and RMII2 clock.                                          */
#define  CLKEN1_TIMER5_9                 20 , 1              /* 20 TIMER5-9 (Timer 5 to Timer 9 and Watchdog 1 (Timer Module 1) Clock Enable Bit). Eng. note: uses APB1               */
#define  CLKEN1_TIMER0_4                 19 , 1              /* 19 TIMER0-4 (Timer 0 to Timer 4 and Watchdog 0 (Timer Module 0) Clock Enable Bit). Eng. note: uses APB1               */
#define  CLKEN1_PWM0                     18 , 1              /* 18 PWM0 (PWM0 module Clock Enable Bit). Eng. note: uses APB3 clock.                                                   */
#define  CLKEN1_HUART                    17 , 1              /* 17 HUART (HUART Clock Enable Bit). Eng. note: uses UART clock (24/48MHz).                                             */
#define  CLKEN1_SMB5                     16 , 1              /* 16 SMB5 (SMBus Interface 5 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_SMB4                     15 , 1              /* 15 SMB4 (SMBus Interface 4 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_SMB3                     14 , 1              /* 14 SMB3 (SMBus Interface 3 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_SMB2                     13 , 1              /* 13 SMB2 (SMBus Interface 2 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN1_MC                       12 , 1              /* 12 MC (DDR4 Memory Controller Clock Enable Bit). Eng. note: uses MC-PHY clock and MC clock.                           */
#define  CLKEN1_UART01                   11 , 1              /* 11 UART01 (UART0 and UART1 Clock Enable Bit). Eng. note: uses UART clock (24/48 MHz).                                 */
#define  CLKEN1_AES                      10 , 1              /* 10 AES (AES Clock Enable Bit). Eng. note: uses AHB8 clock.                                                            */
#define  CLKEN1_PECI                     9 , 1               /* 9 PECI (PECI Clock Enable Bit). Eng. note: uses APB3 clock.                                                           */
#define  CLKEN1_USBDEV2                  8 , 1               /* 8 USBDEV2 (USB Device 2 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                     */
#define  CLKEN1_UART23                   7 , 1               /* 7 UART23 (UART2 and UART3 Clock Enable Bit). Eng. note: uses UART clock (24/48 MHz).                                  */
#define  CLKEN1_EMC1                     6 , 1               /* 6 EMC1 (EMC1 Clock Enable Bit). Eng. note: uses AHB8 clock and RMII1 clock.                                           */
#define  CLKEN1_USBDEV1                  5 , 1               /* 5 USBDEV1 (USB Device 1 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                     */
#define  CLKEN1_SHM                      4 , 1               /* 4 SHM (SHM Clock Enable Bit). Eng. note: uses AHB3 clock.                                                             */
#define  CLKEN1_GDMA0                    3 , 1               /* 3 GDMA0 (GDMA0 Clock Enable Bit). Eng. note: uses AHB8 clock and AHB17 clock.                                         */
#define  CLKEN1_KCS                      2 , 1               /* 2 KCS (KCS Clock Enable Bit). Eng. note: uses APB1 clock.                                                             */
#define  CLKEN1_SPI3                     1 , 1               /* 1 SPI3 (FIU3 Clock Enable Bit). Eng. note: uses AHB3 clock and SPI3 divided clock.                                    */
#define  CLKEN1_SPI0                     0 , 1               /* 0 SPI0 (FIU0 Clock Enable Bit). Eng. note: uses AHB18 clock and SPI0 divided clock.                                   */

/**************************************************************************************************************************/
/*   Clock Enable 2 Register (CLKEN2) (Changed in )                                                                       */
/**************************************************************************************************************************/
#define  CLKEN2                         (CLK_BASE_ADDR + 0x28) , CLK_ACCESS, 32         /* Offset: CLK_BA + 28h */
#define  CLKEN2_CP                       31 , 1              /* 31 CP (Coprocessor Clock Enable Bit). Eng. note: uses AHB9 clock.                                                     */
#define  CLKEN2_TOCK                     30 , 1              /* 30 TOCK (TOCK Clock Enable Bit). Eng. note: uses its own divider.                                                     */
#define  CLKEN2_GMAC1                    28 , 1              /* 28 GMAC1 (Gigabit MAC 1 Module Clock Enable Bit). Eng. note: uses AHB8 clock, AHB12 and RGMII1 clocks.                */
#define  CLKEN2_USBIF                    27 , 1              /* 27 USBIF (HUB, UTMI and OHCI Clock Enable Bit). When this bit is low, USB devices 0-9 and USB Host clocks             */
#define  CLKEN2_USBHOST                  26 , 1              /* 26 USBHOST (USB Host Clock Enable Bit). Eng. note: uses AHB8 clock, AHB7 clock and UTMI clock.                        */
#define  CLKEN2_GMAC2                    25 , 1              /* 25 GMAC2 (Gigabit MAC 2 Module Clock Enable Bit). Eng. note: uses AHB8 clock, AHB13 and RGMII2 clocks.                */
#define  CLKEN2_PSPI2                    23 , 1              /* 23 PSPI2 (Peripheral SPI 2 Clock Enable Bit). Eng. note: uses APB5 clock.                                             */
#define  CLKEN2_PSPI1                    22 , 1              /* 22 PSPI1 (Peripheral SPI 1 Clock Enable Bit). Eng. note: uses APB5 clock.                                             */
#define  CLKEN2_SIOX2                    19 , 1              /* 19 SIOX2 (Serial GPIO Expander 2 Clock Enable Bit). Eng. note: uses APB3 clock                                        */
#define  CLKEN2_SIOX1                    18 , 1              /* 18 SIOX1 (Serial GPIO Expander 1 Clock Enable Bit). Eng. note: uses APB3 clock                                        */
#define  CLKEN2_FUSE                     16 , 1              /* 16 FUSE (Fuse Module Clock Enable Bit). Eng. note: uses APB4 clock                                                    */
#define  CLKEN2_VCD                      14 , 1              /* 14 VCD (Video Capture and Differentiate Module Clock Enable Bit). Eng. note: uses AHB8, AHB4 and AHB5                 */
#define  CLKEN2_ECE                      13 , 1              /* 13 ECE (Encoding Compression Module Clock Enable Bit). Eng. note: uses AHB8 and AHB6 clocks.                          */
#define  CLKEN2_VDMA                     12 , 1              /* 12 VDMA (VDM DMA Clock Enable Bit). Eng. note: uses AHB8 and AHB17 clocks.                                            */
#define  CLKEN2_AHBPCIBRG                11 , 1              /* 11 AHBPCIBRG (AHB-to-PCI Bridge Clock Enable Bit).                                                                    */
#define  CLKEN2_GFXSYS                   10 , 1              /* 10 GFXSYS (Graphics System Clock Enable Bit).                                                                         */
#define  CLKEN2_SDHC                     9 , 1               /* 9 SDHC (SD Host Controller Clock Enable Bit). Eng. note: uses AHB8 and AHB7 clocks and interface clock.               */
#define  CLKEN2_MMC                      8 , 1               /* 8 MMC (Multimedia Card Host Controller Clock Enable Bit). Eng. note: uses AHB8 and AHB2 clocks and                    */
#define  CLKEN2_MFT7_0                   0 , 8              /* 7-0 MFT7-0 (Tachometer Module 7-0 Clock Enable Bits). Eng. note: uses APB4 clock                                      */

/**************************************************************************************************************************/
/*   Clock Enable 3 Register (CLKEN3) (New in )                                                                           */
/**************************************************************************************************************************/
#define  CLKEN3                         (CLK_BASE_ADDR + 0x30) , CLK_ACCESS, 32         /* Offset: CLK_BA + 30h */
#define  CLKEN3_GPIOM7                   31 , 1              /* 31 GPIOM7 (GPIO Module 7 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_GPIOM6                   30 , 1              /* 30 GPIOM6 (GPIO Module 6 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_GPIOM5                   29 , 1              /* 29 GPIOM5 (GPIO Module 5 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_GPIOM4                   28 , 1              /* 28 GPIOM4 (GPIO Module 4 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_GPIOM3                   27 , 1              /* 27 GPIOM3 (GPIO Module 3 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_GPIOM2                   26 , 1              /* 26 GPIOM2 (GPIO Module 2 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_GPIOM1                   25 , 1              /* 25 GPIOM1 (GPIO Module 1 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_GPIOM0                   24 , 1              /* 24 GPIOM0 (GPIO Module 0 Clock Enable Bit). Eng. note: uses APB1 clock                                                */
#define  CLKEN3_ESPI                     23 , 1              /* 23 ESPI (ESPI Module Clock Enable Bit). Eng. note: uses APB2 clock                                                    */
#define  CLKEN3_SMB11                    22 , 1              /* 22 SMB11 (SMBus Interface 11 Clock Enable Bit). Eng. note: uses APB2 clock.                                           */
#define  CLKEN3_SMB10                    21 , 1              /* 21 SMB10 (SMBus Interface 10 Clock Enable Bit). Eng. note: uses APB2 clock.                                           */
#define  CLKEN3_SMB9                     20 , 1              /* 20 SMB9 (SMBus Interface 9 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN3_SMB8                     19 , 1              /* 19 SMB8 (SMBus Interface 8 Clock Enable Bit). Eng. note: uses APB2 clock.                                             */
#define  CLKEN3_SMB15                    18 , 1              /* 18 SMB15 (SMBus Interface 15 Clock Enable Bit). Eng. note: uses APB2 clock.                                           */
#define  CLKEN3_RNG                      17 , 1              /* 17 RNG (Random Number Generator Clock Enable Bit). Eng. note: uses APB1 clock.                                        */
#define  CLKEN3_TIMER10_14               16 , 1              /* 16 TIMER10-14 (Timer 10 to Timer 14 and Watchdog 2 (Timer module 2) Clock Enable Bit). Eng. note: uses                */
#define  CLKEN3_PCIERC                   15 , 1              /* 15 PCIERC (PCI Express Root Complex Clock Enable Bit). Eng. note: uses AHB14 clock, PIPE clock and MC                 */
#define  CLKEN3_SECECC                   14 , 1              /* 14 SECECC. (RAS, ECC and MODP accelerator Clock Enable Bit). Eng. note: use AHB8 clock.                               */
#define  CLKEN3_SHA                      13 , 1              /* 13 SHA. (SHA-1 and SHA-256 Module Clock Enable Bit). Eng. note: use AHB8 clock.                                       */
#define  CLKEN3_SMB14                    12 , 1              /* 12 SMB14 (SMBus Interface 14 Clock Enable Bit). Eng. note: uses APB2 clock.                                           */
#define  CLKEN3_GDMA2                    11 , 1              /* 11 GDMA2. (General Purpose DMA 2 Clock Enable bit). Eng. note: uses AHB8 clock and AHB17 clock.                       */
#define  CLKEN3_GDMA1                    10 , 1              /* 10 GDMA1. (General Purpose DMA 1 Clock Enable Bit). Eng. note: uses AHB8 clock and AHB17 clock.                       */
#define  CLKEN3_PCIMBX                   9 , 1               /* 9 PCIMBX. (PCI Mailbox Clock Enable Bit). Eng. note: use AHB8 clock and PCI clock.                                    */
#define  CLKEN3_USBDEV9                  7 , 1               /* 7 USBDEV9. (USB Device 9 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN3_USBDEV8                  6 , 1               /* 6 USBDEV8. (USB Device 8 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN3_USBDEV7                  5 , 1               /* 5 USBDEV7. (USB Device 7 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN3_USBDEV0                  4 , 1               /* 4 USBDEV0. (USB Device 0 Clock Enable Bit). Eng. note: uses AHB8 clock, AHB2 clock and UTMI clock.                    */
#define  CLKEN3_SMB13                    3 , 1               /* 3 SMB13 (SMBus Interface 13 Clock Enable Bit). Eng. note: uses APB2 clock.                                            */
#define  CLKEN3_SPIX                     2 , 1               /* 2 SPIX (SPIX Clock Enable Bit). Eng. note: uses AHB18 clock and SPIX divided clock.                                   */
#define  CLKEN3_SMB12                    1 , 1               /* 1 SMB12 (SMBus Interface 12 Clock Enable Bit). Eng. note: uses APB2 clock.                                            */
#define  CLKEN3_PWM1                     0 , 1               /* 0 PWM1. (PWM1 Module Clock Enable Bit). Eng. note: uses APB3 clock.                                                   */

/**************************************************************************************************************************/
/*   Clock Select Register (CLKSEL) (Changed in Poleg)                                                                    */
/**************************************************************************************************************************/
#define  CLKSEL                         (CLK_BASE_ADDR + 0x04) , CLK_ACCESS, 32         /* Location: CLK_BA+04h */
#define  CLKSEL_DVCSSEL                  23 , 2             /* 24-23 DVCSSEL (DVC System Clock Source Select Bit). Typically, a divided PLL2 (by 4): 240 MHz.                        */
#define  CLKSEL_GFXMSEL                  21 , 2             /* 22-21 GFXMSEL (Graphics Memory Clock Source Select Bit). Typically, a divided PLL2 (by 3): 320 MHz.                   */
#define  CLKSEL_CLKOUTSEL                18 , 3             /* 20-18 CLKOUTSEL (CLKOUT signal Clock Source Select Bit).                                                              */
#define  CLKSEL_GFXCKSEL                 16 , 2             /* 17-16 GFXCKSEL (Graphics System Clock Source Select Bit). Typically, the same source of the CPU clock.                */
#define  CLKSEL_TIMCKSEL                 14 , 2             /* 15-14 TIMCKSEL (Timer Clock Source Select Bit).                                                                       */
#define  CLKSEL_MCCKSEL                  12 , 2             /* 13-12 MCCKSEL (Memory Controller Clock Source Select Bit).                                                            */
#define  CLKSEL_SUCKSEL                  10 , 2             /* 11-10 SUCKSEL (USB Serial Clock Source Select Bit). For UTMI2UTMI and OHCI logic. Should select a clock of            */
#define  CLKSEL_UARTCKSEL                8 , 2              /* 9-8 UARTCKSEL (Core and Host UART Clock Source Select Bit).                                                           */
#define  CLKSEL_SDCKSEL                  6 , 2              /* 7-6 SDCKSEL (SDHC Clock Source Select Bit).                                                                           */
#define  CLKSEL_PIXCKSEL                 4 , 2              /* 5-4 PIXCKSEL (Pixel Clock Source Select Bit).                                                                         */
#define  CLKSEL_GPRFSEL                  2 , 2              /* 3-2 GPRFSEL (Graphics PLL Reference Clock Source Select Bit).                                                         */
#define  CLKSEL_CPUCKSEL                 0 , 2              /* 1-0 CPUCKSEL (CPU/AMBA/MC Clock Source Select Bit).                                                                   */

/**************************************************************************************************************************/
/*   Clock Divider Control Register 1 (CLKDIV1) (Changed in )                                                             */
/**************************************************************************************************************************/
#define  CLKDIV1                        (CLK_BASE_ADDR + 0x08) , CLK_ACCESS, 32         /* Offset: CLK_BA + 08h */
#define  CLKDIV1_ADCCKDIV                28 , 3             /* 30-28 ADCCKDIV (ADC CLK2ADC Clock Divider Control). Sets the division factor from the Timer clock (24 MHz). The       */
#define  CLKDIV1_CLK4DIV                 26 , 2             /* 27-26 CLK4DIV (AMBA AHB Clock Divider Control).                                                                       */
#define  CLKDIV1_TIMCKDIV                21 , 5             /* 25-21 TIMCKDIV (Timer Clock Source Divider Control). Default is divide by 1. The division factor is                   */
#define  CLKDIV1_UARTDIV                 16 , 5             /* 20-16 UARTDIV (UART Clock Source Divider Control). This resulting clock must be 24 MHz for UARTs proper operation.    */
#define  CLKDIV1_MMCCKDIV                11 , 5             /* 15-11 MMCCKDIV (MMC Controller (SDHC2) Clock Divider Control). Sets the division factor from the clock selected       */
#define  CLKDIV1_AHB3CKDIV               6 , 5              /* 10-6 AHB3CKDIV (SPI3 Clock Divider Control). Sets the division factor from AHB clock (CLK4) to AHB3 and SPI3          */
#define  CLKDIV1_PCICKDIV                2 , 4              /* 5-2 PCICKDIV (Internal PCI Clock Divider Control). Sets the division factor from the clock selected by the CLKSEL.    */
#define  CLKDIV1_CLK2DIV                 0 , 1               /* 0 CLK2DIV (AMBA AXI Clock Divider Control).                                                                           */

/**************************************************************************************************************************/
/*   Clock Divider Control Register 2 (CLKDIV2) (Changed in )                                                             */
/**************************************************************************************************************************/
#define  CLKDIV2                        (CLK_BASE_ADDR + 0x2C) , CLK_ACCESS, 32         /* Offset: CLK_BA + 2Ch */
#define  CLKDIV2_APB4CKDIV               30 , 2             /* 31-30 APB4CKDIV (AMBA APB4 Clock Divider Control). APB4 clock frequency is up to 67 MHz.                              */
#define  CLKDIV2_APB3CKDIV               28 , 2             /* 29-28 APB3CKDIV (AMBA APB3 Clock Divider Control). APB3 clock frequency is up to 67 MHz.                              */
#define  CLKDIV2_APB2CKDIV               26 , 2             /* 27-26 APB2CKDIV (AMBA APB2 Clock Divider Control). APB2 clock frequency is up to 67 MHz.                              */
#define  CLKDIV2_APB1CKDIV               24 , 2             /* 25-24 APB1CKDIV (AMBA APB1 Clock Divider Control). APB1 clock must be at least 24 MHz, for UARTs to function.          */
#define  CLKDIV2_APB5CKDIV               22 , 2             /* 23-22 APB5CKDIV (AMBA APB5 Clock Divider Control). APB5 clock must be high to enable PSPI1-2 high SPI clock           */
#define  CLKDIV2_CLKOUTDIV               16 , 5             /* 20-16 CLKOUTDIV (CLKOUT Pin Divider Control). Sets the division factor from the clock selected by CLKOUTSEL to        */
#define  CLKDIV2_GFXCKDIV                13 , 3             /* 15-13 GFXCKDIV (Graphics System Clock Divider Control). Sets the division factor from the clock selected by the       */
#define  CLKDIV2_SUCKDIV                 8 , 5              /* 12-8 SUCKDIV (‘Serial’ USB UTMI Bridge Clock Divider Control). Sets the division factor from the clock selected by    */
#define  CLKDIV2_SU48CKDIV               4 , 4              /* 7-4 SU48CKDIV (USB Host TBD OHCI Clock Divider Control). Sets the division factor from the clock selected by          */
#define  CLKDIV2_SD1CKDIV                0 , 4              /* 3-0 SD1CKDIV (SDHC1 Clock Divider Control). Sets the division factor from the clock selected by SDCKSEL. The          */

/**************************************************************************************************************************/
/*   Clock Divider Control Register 3 (CLKDIV3) (Added in )                                                               */
/**************************************************************************************************************************/
#define  CLKDIV3                        (CLK_BASE_ADDR + 0x58) , CLK_ACCESS, 32         /* Offset: CLK_BA + 58h */
#define  CLKDIV3_SPI0CKDV                6 , 5              /* 10-6 SPI0CKDV (SPI0 Clock Divider Control). Sets the division factor from AHB clock to SPI0 clock. The division factor*/
#define  CLKDIV3_SPIXCKDV                1 , 5              /* 5-1 SPIXCKDV (SPIX Clock Divider Control). Sets the division factor from AHB clock to SPIX clock. The division factor */

/**************************************************************************************************************************/
/*   PLL Control Register 0 (PLLCON0) PLL Section Assumes Silicon Creation PLLGF40LPFRAC                                  */
/**************************************************************************************************************************/
#define  PLLCON0                        (CLK_BASE_ADDR + 0x0C) , CLK_ACCESS, 32         /* Offset: CLK_BA + 0Ch */

/**************************************************************************************************************************/
/*   PLL Control Register 1 (PLLCON1)                                                                                     */
/**************************************************************************************************************************/
#define  PLLCON1                        (CLK_BASE_ADDR + 0x10) , CLK_ACCESS, 32         /* Offset: CLK_BA + 10h */

/**************************************************************************************************************************/
/*   PLL Control Register 2 (PLLCON2) (New in Poleg)                                                                      */
/**************************************************************************************************************************/
#define  PLLCON2                        (CLK_BASE_ADDR + 0x54) , CLK_ACCESS, 32         /* Location: CLK_BA+54h */
#define  PLLCONn_LOKI                    31 , 1              /* 31 LOKI (Lock Indication ).                                                                                           */
#define  PLLCONn_LOKS                    30 , 1              /* 30 LOKS (Unlock Indication Sticky bit ).                                                                              */
#define  PLLCONn_FBDV                    16 , 12            /* 27-16 FBDV (PLL VCO Output Clock Feedback Divider). The feedback divider divides the output clock from                */
#define  PLLCONn_OTDV2                   13 , 3             /* 15-13 OTDV2 (PLL Output Clock Divider 2). The output divider divides the VCO clock output. The divide value           */
#define  PLLCONn_PWDEN                   12 , 1              /* 12 PWDEN (Power-Down Mode Enable).                                                                                    */
#define  PLLCONn_OTDV1                   8 , 3              /* 10-8 OTDV1 (PLL Output Clock Divider 1). The output divider divides the VCO clock output. The divide value            */
#define  PLLCONn_INDV                    0 , 6              /* 5-0 INDV (PLL Input Clock Divider). The input divider divides the input reference clock into the PLL. The di-         */

/**************************************************************************************************************************/
/*   Software Reset Register (SWRSTR) (Changed in )                                                                       */
/**************************************************************************************************************************/
#define  SWRSTR                         (CLK_BASE_ADDR + 0x14) , CLK_ACCESS, 32         /* Offset: CLK_BA + 14h */
#define  SWRSTR_SWRST4                   6 , 1               /* 6 SWRST4 (Software Reset Control Bit 4). Generates an internal reset pulse to the modules defined in                  */
#define  SWRSTR_SWRST3                   5 , 1               /* 5 SWRST3 (Software Reset Control Bit 3). Generates an internal reset pulse to the modules defined in                  */
#define  SWRSTR_SWRST2                   4 , 1               /* 4 SWRST2 (Software Reset Control Bit 2). Generates an internal reset pulse to the modules defined in                  */
#define  SWRSTR_SWRST1                   3 , 1               /* 3 SWRST1 (Software Reset Control Bit 1). Generates an internal reset pulse to the modules defined in                  */

/**************************************************************************************************************************/
/*   IRQ Wake-Up Control Register (IRQWAKECON)                                                                            */
/**************************************************************************************************************************/
#define  IRQWAKECON                     (CLK_BASE_ADDR + 0x18) , CLK_ACCESS, 32         /* Offset: CLK_BA + 18h */
#define  IRQWAKECON_IRQWAKEUPPOL         16 , 16            /* 31-16 IRQWAKEUPPOL (IRQ Wake-Up Polarity for GPIOE15-0).                                                              */
#define  IRQWAKECON_IRQWAKEUPEN          0 , 16             /* 15-0 IRQWAKEUPEN (Wake-Up Enable for GPIOE15-0).                                                                      */

/**************************************************************************************************************************/
/*   IRQ Wake-Up Flag Register (IRQWAKEFLAG)                                                                              */
/**************************************************************************************************************************/
#define  IRQWAKEFLAG                    (CLK_BASE_ADDR + 0x1C) , CLK_ACCESS, 32         /* Offset: CLK_BA + 1Ch */
#define  IRQWAKEFLAG_IRQWAKEFLAG         0 , 16             /* 15-0 IRQWAKEFLAG (Wake-Up Flag for GPIOE15-0).                                                                        */

/**************************************************************************************************************************/
/*   IP Software Reset Register 1 (IPSRST1) (Changed in )                                                          */
/**************************************************************************************************************************/
#define  IPSRST1                        (CLK_BASE_ADDR + 0x20) , CLK_ACCESS, 32         /* Offset: CLK_BA + 20h */
#define  IPSRST1_SMB1                    31 , 1              /* 31 SMB1 (SMBus1 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_SMB0                    30 , 1              /* 30 SMB0 (SMBus0 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_SMB7                    29 , 1              /* 29 SMB7 (SMBus7 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_SMB6                    28 , 1              /* 28 SMB6 (SMBuS6 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_ADC                     27 , 1              /* 27 ADC (ADC Software Reset Control Bit).                                                                              */
#define  IPSRST1_USBDEV3                 25 , 1              /* 25 USBDEV3 (USB Device 3 Controller Software Reset Control Bit).                                                      */
#define  IPSRST1_USBDEV6                 24 , 1              /* 24 USBDEV6 (USB Device 6 Controller Software Reset Control Bit).                                                      */
#define  IPSRST1_USBDEV5                 23 , 1              /* 23 USBDEV5 (USB Device 5 Controller Software Reset Control Bit).                                                      */
#define  IPSRST1_USBDEV4                 22 , 1              /* 22 USBDEV4 (USB Device 4 Controller Software Reset Control Bit).                                                      */
#define  IPSRST1_EMC2                    21 , 1              /* 21 EMC2 (EMC2 Software Reset Control Bit).                                                                            */
#define  IPSRST1_TIM5_9                  20 , 1              /* 20 TIM5-9 (Timer 5-9 and Watchdog 1 (Timer module 1) Software Reset Control Bit).                                     */
#define  IPSRST1_TIM0_4                  19 , 1              /* 19 TIM0-4 (Timer 0-4 and Watchdog 0 (Timer module 0) Software Reset Control Bit).                                     */
#define  IPSRST1_PWM                     18 , 1              /* 18 PWM (PWM Modules Software Reset Control Bit).                                                                      */
#define  IPSRST1_SMB5                    16 , 1              /* 16 SMB5 (SMBus5 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_SMB4                    15 , 1              /* 15 SMB4 (SMBus4 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_SMB3                    14 , 1              /* 14 SMB3 (SMBus3 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_SMB2                    13 , 1              /* 13 SMB2 (SMBus2 Interface Software Reset Control Bit).                                                                */
#define  IPSRST1_MC                      12 , 1              /* 12 MC (DDR4 Memory Controller Software Reset Control Bit).                                                            */
#define  IPSRST1_UART01                  11 , 1              /* 11 UART01 (UARTs 0 and 1 Software Reset Control Bit).                                                                 */
#define  IPSRST1_AES                     10 , 1              /* 10 AES (AES Module Software Reset Control Bit).                                                                       */
#define  IPSRST1_PECI                    9 , 1               /* 9 PECI (PECI Module Software Reset Control Bit).                                                                      */
#define  IPSRST1_USBDEV2                 8 , 1               /* 8 USBDEV2 (USB Device 2 Controller Software Reset Control Bit).                                                       */
#define  IPSRST1_UART23                  7 , 1               /* 7 UART23 (UART 2 and UART 3 Software Reset Control Bit).                                                              */
#define  IPSRST1_EMC1                    6 , 1               /* 6 EMC1 (EMC1 Software Reset Control Bit).                                                                             */
#define  IPSRST1_USBDEV1                 5 , 1               /* 5 USBDEV1 (USB Device 1 Controller Software Reset Control Bit).                                                       */
#define  IPSRST1_GDMA0                   3 , 1               /* 3 GDMA0 (GDMA0 Software Reset Control Bit).                                                                           */
#define  IPSRST1_SPI3                    1 , 1               /* 1 SPI3 (FIU3 and AHB3 (including SHM part connected to AHB3) Software Reset Control Bit).                             */
#define  IPSRST1_SPI0                    0 , 1               /* 0 SPI0 (FIU0 Software Reset Control Bit).                                                                             */

/**************************************************************************************************************************/
/*   IP Software Reset Register 2 (IPSRST2) (Changed in )                                                                 */
/**************************************************************************************************************************/
#define  IPSRST2                        (CLK_BASE_ADDR + 0x24) , CLK_ACCESS, 32         /* Offset: CLK_BA + 24h */
#define  IPSRST2_CP                      31 , 1              /* 31 CP (Coprocessor Reset Control Bit).  This bit resets the CP registers but not registers that are marked as         */
                                                             /* reset only by VSB power-up reset. Before setting this bit, the BMC CPU should reset the CP2BST register, and          */
                                                             /* check bit 31 after the reset, that indicates if the CP had a watchdog reset at the same time.                         */
                                                             /* BMC ROM Code clears this bit according to inverted value of CP_FUSTRAP1.                                              */
#define  IPSRST2_GMAC1                   28 , 1              /* 28 GMAC1 (Gigabit MAC 1 Software Reset Control Bit)                                                                   */
#define  IPSRST2_USBHOST                 26 , 1              /* 26 USBHOST (USB Host Software Reset Control Bit)                                                                      */
#define  IPSRST2_GMAC2                   25 , 1              /* 25 GMAC2 (Gigabit MAC 2 Software Reset Control Bit)                                                                   */
#define  IPSRST2_PSPI2                   23 , 1              /* 23 PSPI2 (Peripheral SPI 2 Software Reset Control Bit)                                                                */
#define  IPSRST2_PSPI1                   22 , 1              /* 22 PSPI1 (Peripheral SPI 1 Software Reset Control Bit)                                                                */
#define  IPSRST2_SIOX2                   19 , 1              /* 19 SIOX2 (Serial GPIO Expander 2 Reset Control Bit)                                                                   */
#define  IPSRST2_SIOX1                   18 , 1              /* 18 SIOX1 (Serial GPIO Expander 1 Reset Control Bit)                                                                   */
#define  IPSRST2_OTP                     16 , 1              /* 16 OTP (One Time Programmable Module Reset Control Bit). Resets the state machines and registers.                     */
#define  IPSRST2_VCD                     14 , 1              /* 14 VCD (Video Capture and Differentiate Module Reset Control Bit).                                                    */
#define  IPSRST2_ECE                     13 , 1              /* 13 ECE (Encoding Compression Module Reset Control Bit).                                                               */
#define  IPSRST2_VDMA                    12 , 1              /* 12 VDMA (VDM DMA Module Reset Control Bit).                                                                           */
#define  IPSRST2_AHBPCIBRG               11 , 1              /* 11 AHBPCIBRG (AHB-to-PCI Bridge Reset Control Bit).                                                                   */
#define  IPSRST2_GFXSYS                  10 , 1              /* 10 GFXSYS (Graphics System Reset Control Bit). Should be used only if graphics system is under BMC control.           */
#define  IPSRST2_SDHC                    9 , 1               /* 9 SDHC (SD Host Controller Reset Control Bit).                                                                        */
#define  IPSRST2_MMC                     8 , 1               /* 8 MMC (Multimedia Card Host Controller Reset Control Bit).                                                            */
#define  IPSRST2_MFT7_0                  0 , 8              /* 7-0 MFT7-0 (Tachometer Module 7-0 Reset Control Bits).                                                                */

/**************************************************************************************************************************/
/*   IP Software Reset Register 3 (IPSRST3) (New in )                                                                     */
/**************************************************************************************************************************/
#define  IPSRST3                        (CLK_BASE_ADDR + 0x34) , CLK_ACCESS, 32         /* Offset: CLK_BA + 34h */
#define  IPSRST3_USBPHY2                 25 , 1              /* 25 USBPHY2 (USB PHY 2 Software Reset Control Bit). Must be used before initializing USB port 2 (Host/Device).         */
#define  IPSRST3_USBPHY1                 24 , 1              /* 24 USBPHY1 (USB PHY 1 Software Reset Control Bit). Must be used before initializing USB port 1 (Hub).                 */
#define  IPSRST3_ESPI                    23 , 1              /* 23 ESPI (eSPI Module Software Reset Control Bit). Must not be used when eSPI is active.                               */
#define  IPSRST3_SMB11                   22 , 1              /* 22 SMB11 (SMBus11 Interface Software Reset Control Bit).                                                              */
#define  IPSRST3_SMB10                   21 , 1              /* 21 SMB10 (SMBus10 Interface Software Reset Control Bit).                                                              */
#define  IPSRST3_SMB9                    20 , 1              /* 20 SMB9 (SMBus9 Interface Software Reset Control Bit).                                                                */
#define  IPSRST3_SMB8                    19 , 1              /* 19 SMB8 (SMBus8 Interface Software Reset Control Bit).                                                                */
#define  IPSRST3_SMB15                   18 , 1              /* 18 SMB15 (SMBus15 Interface Software Reset Control Bit).                                                              */
#define  IPSRST3_RNG                     17 , 1              /* 17 RNG (Random Number Generator Software Reset Control Bit).                                                          */
#define  IPSRST3_TIMER10_14              16 , 1              /* 16 TIMER10-14 (Timers 10-14 and Watchdog 2 (Timer module 2) Software Reset Control Bit).                              */
#define  IPSRST3_PCIERC                  15 , 1              /* 15 PCIERC (PCI Express Root Complex Reset Control Bit.                                                                */
#define  IPSRST3_SECECC                  14 , 1              /* 14 SECECC (RAS, ECC and MODP Accelerator Module Reset Control Bit).                                                   */
#define  IPSRST3_SHA                     13 , 1              /* 13 SHA (SHA-1 and SHA-256 Module Reset Control Bit).                                                                  */
#define  IPSRST3_SMB14                   12 , 1              /* 12 SMB14 (SMBus14 Interface Software Reset Control Bit).                                                              */
#define  IPSRST3_GDMA2                   11 , 1              /* 11 GDMA2 (General Purpose DMA Module 2 Reset Control Bit).                                                            */
#define  IPSRST3_GDMA1                   10 , 1              /* 10 GDMA1 (General Purpose DMA Module 1 Reset Control Bit).                                                            */
#define  IPSRST3_SPCIMBX                 9 , 1               /* 9 SPCIMBX (PCI Mailbox Reset Control Bit).                                                                            */
#define  IPSRST3_USBHUB                  8 , 1               /* 8 USBHUB. (USB Hub)                                                                                                   */
#define  IPSRST3_USBDEV9                 7 , 1               /* 7 USBDEV9 (USB Device 9 Reset Control Bits).                                                                          */
#define  IPSRST3_USBDEV8                 6 , 1               /* 6 USBDEV8 (USB Device 8 Reset Control Bits).                                                                          */
#define  IPSRST3_USBDEV7                 5 , 1               /* 5 USBDEV7 (USB Device 7 Reset Control Bits).                                                                          */
#define  IPSRST3_USBDEV0                 4 , 1               /* 4 USBDEV0 (USB Device 0 Reset Control Bits).                                                                          */
#define  IPSRST3_SMB13                   3 , 1               /* 3 SMB13 (SMBus13 Interface Software Reset Control Bit).                                                               */
#define  IPSRST3_SPIX                    2 , 1               /* 2 SPIX (SOIX Reset Control Bits).                                                                                     */
#define  IPSRST3_SMB12                   1 , 1               /* 1 SMB12 (SMBus12 Interface Software Reset Control Bit).                                                               */
#define  IPSRST3_PWM1                    0 , 1               /* 0 PWM1 (PWM Module 1 Reset Control Bits).                                                                             */

/**************************************************************************************************************************/
/*   Watchdog 0 Reset Control Register (WD0RCR) (New in )                                                                 */
/**************************************************************************************************************************/
#define  WD0RCR                         (CLK_BASE_ADDR + 0x38) , CLK_ACCESS, 32         /* Offset: CLK_BA + 38h */
#define  WD0RCR_LPCESPI                  31 , 1              /* 31 LPCESPI (LPC or ESPI Watchdog Reset Control Bit). All eSPI or LPC modules, eSPI interface.                         */
#define  WD0RCR_PCIE                     30 , 1              /* 30 PCIE (PCI Express and PCI Peripherals Watchdog Reset Control Bit). All PCI modules.                                */
#define  WD0RCR_SHMKCS                   29 , 1              /* 29 SHMKCS (SHM and KCS modules Watchdog Reset Control Bit). SHM, KCS0-3, Post Code FIFO modules.                      */
#define  WD0RCR_PWM                      28 , 1              /* 28 PWM (PWM Modules Watchdog Reset Control Bit). PWM0-1 modules.                                                      */
#define  WD0RCR_SPER                     27 , 1              /* 27 SPER (Slow Peripherals Watchdog Reset Control Bit). UART0-3, SMB0-11, PECI, ADC modules.                           */
#define  WD0RCR_SPI                      26 , 1              /* 26 SPI (SPI interfaces Watchdog Reset Control Bit). FIU0, FIU3, FIUX, PSPI1-2, MFT0-7 modules.                        */
#define  WD0RCR_SIOX2                    25 , 1              /* 25 SIOX2 (Serial IO Expander 2 Watchdog Reset Control Bit).                                                           */
#define  WD0RCR_SIOX1                    24 , 1              /* 24 SIOX1 (Serial IO Expander 1 Watchdog Reset Control Bit).                                                           */
#define  WD0RCR_GPIOM7                   23 , 1              /* 23 GPIOM7 (GPIO Module 7 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_GPIOM6                   22 , 1              /* 22 GPIOM6 (GPIO Module 6 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_GPIOM5                   21 , 1              /* 21 GPIOM5 (GPIO Module 5 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_GPIOM4                   20 , 1              /* 20 GPIOM4 (GPIO Module 4 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_GPIOM3                   19 , 1              /* 19 GPIOM3 (GPIO Module 3 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_GPIOM2                   18 , 1              /* 18 GPIOM2 (GPIO Module 2 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_GPIOM1                   17 , 1              /* 17 GPIOM1 (GPIO Module 1 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_GPIOM0                   16 , 1              /* 16 GPIOM0 (GPIO Module 0 Watchdog Reset Control Bit).                                                                 */
#define  WD0RCR_TIMER                    15 , 1              /* 15 TIMER (Timer Modules Watchdog Reset Control Bit).                                                                  */
#define  WD0RCR_PCIMBX                   14 , 1              /* 14 PCIMBX (PCI Mailbox Watchdog Reset Control Bit).                                                                   */
#define  WD0RCR_AHB2PCI                  13 , 1              /* 13 AHB2PCI (AHB to PCI Bridge Watchdog Reset Control Bit).                                                            */
#define  WD0RCR_SD                       12 , 1              /* 12 SD (SD Host Controller Watchdog Reset Control Bit). modules.                                                       */
#define  WD0RCR_MMC                      11 , 1              /* 11 MMC (Multimedia Card Controller Watchdog Reset Control Bit).                                                       */
#define  WD0RCR_DMA                      10 , 1              /* 10 DMA (DMA Modules Watchdog Reset Control Bit). GDMA0-2, VDMA modules.                                               */
#define  WD0RCR_USBHST                   9 , 1               /* 9 USBHST (USB Host Watchdog Reset Control Bit).                                                                       */
#define  WD0RCR_USBDEV                   8 , 1               /* 8 USBDEV (USB Devices Watchdog Reset Control Bit). USBDEV0-9 modules.                                                 */
#define  WD0RCR_ETH                      7 , 1               /* 7 ETH (Ethernet Controllers Watchdog Reset Control Bit). GMAC1, GMAC2, EMC1-2 modules.                                */
#define  WD0RCR_CLKS                     6 , 1               /* 6 CLKS (Clock Control Watchdog Reset Control Bit). Resets the clock control to power-on reset values.                 */
#define  WD0RCR_MC                       5 , 1               /* 5 MC (DDR4 Memory Controller Watchdog Reset Control Bit).                                                             */
#define  WD0RCR_RV                       4 , 1               /* 4 RV (Remote Video Watchdog Reset Control Bit). ECE, VCD, DVC modules                                                 */
#define  WD0RCR_SEC                      3 , 1               /* 3 SEC (Security Peripherals Watchdog Reset Control Bit). SECECC, 3DES, AES, RNG, SHA, OTP modules.                    */
#define  WD0RCR_CP                       2 , 1               /* 2 CP (Coprocessor Watchdog Reset Control Bit).                                                                        */
#define  WD0RCR_A9DBG                    1 , 1               /* 1 A9DBG (Cortex A9 Debugger (CoreSight) Watchdog Reset Control Bit).                                                  */
#define  WD0RCR_CA9C                     0 , 1               /* 0 CA9C (Cortex A9 Cores Watchdog Reset Control Bit).                                                                  */

/**************************************************************************************************************************/
/*   Watchdog 1 Reset Control Register (WD1RCR) (New in )                                                                 */
/**************************************************************************************************************************/
#define  WD1RCR                         (CLK_BASE_ADDR + 0x3C) , CLK_ACCESS, 32         /* Offset: CLK_BA + 3Ch */

/**************************************************************************************************************************/
/*   Watchdog 2 Reset Control Register (WD2RCR) (New in )                                                                 */
/**************************************************************************************************************************/
#define  WD2RCR                         (CLK_BASE_ADDR + 0x40) , CLK_ACCESS, 32         /* Offset: CLK_BA + 40h */

/**************************************************************************************************************************/
/*   Software Reset Control Register 1 (SWRSTC1) (New in )                                                                */
/**************************************************************************************************************************/
#define  SWRSTC1                        (CLK_BASE_ADDR + 0x44) , CLK_ACCESS, 32         /* Offset: CLK_BA + 44h */

/**************************************************************************************************************************/
/*   Software Reset Control Register 2 (SWRSTC2) (New in )                                                                */
/**************************************************************************************************************************/
#define  SWRSTC2                        (CLK_BASE_ADDR + 0x48) , CLK_ACCESS, 32         /* Offset: CLK_BA + 48h */

/**************************************************************************************************************************/
/*   Software Reset Control Register 3 (SWRSTC3) (New in )                                                                */
/**************************************************************************************************************************/
#define  SWRSTC3                        (CLK_BASE_ADDR + 0x4C) , CLK_ACCESS, 32         /* Offset: CLK_BA + 4Ch */

/**************************************************************************************************************************/
/*   Software Reset Control Register 4 (SWRSTC4) (New in )                                                                */
/**************************************************************************************************************************/
#define  SWRSTC4                        (CLK_BASE_ADDR + 0x50) , CLK_ACCESS, 32         /* Offset: CLK_BA + 50h */

/**************************************************************************************************************************/
/*   CORST Control Register (CORSTC) (New in )                                                                            */
/**************************************************************************************************************************/
#define  CORSTC                         (CLK_BASE_ADDR + 0x5C) , CLK_ACCESS, 32         /* Offset: CLK_BA + 5Ch */

/**************************************************************************************************************************/
/*   PLL Control Register GFX (PLLCONG) (New in )                                                                         */
/**************************************************************************************************************************/
#define  PLLCONG                        (CLK_BASE_ADDR + 0x60) , CLK_ACCESS, 32         /* Offset: CLK_BA + 60h */
#define  PLLCONG_LOKI                    31 , 1              /* 31 LOKI (Lock Indication ).                                                                                           */
#define  PLLCONG_LOKS                    30 , 1              /* 30 LOKS (Unlock Indication Sticky Bit ).                                                                              */
#define  PLLCONG_GPLLTST                 29 , 1              /* 29 GPLLTST (Graphics PLL Test Mode).                                                                                  */
#define  PLLCONG_FBDV                    16 , 12            /* 27-16 FBDV (PLL VCO Output Clock Feedback Divider). The feedback divider divides the output clock from                */
#define  PLLCONG_OTDV2                   13 , 3             /* 15-13 OTDV2 (PLL Output Clock Divider 2). The output divider divides the VCO clock output. The divide value           */
#define  PLLCONG_PWDEN                   12 , 1              /* 12 PWDEN (Power-Down Mode Enable).                                                                                    */
#define  PLLCONG_OTDV1                   8 , 3              /* 10-8 OTDV1 (PLL Output Clock Divider 1). The output divider divides the VCO clock output. The divide value            */
#define  PLLCONG_INDV                    0 , 6              /* 5-0 INDV (PLL Input Clock Divider). The input divider divides the input reference clock into the PLL. The divide      */

/**************************************************************************************************************************/
/*   AHB Clock Frequency Information Register (AHBCKFI)                                                                   */
/**************************************************************************************************************************/
#define  AHBCKFI                        (CLK_BASE_ADDR + 0x64) , CLK_ACCESS, 32         /* Offset: CLK_BA + 64h */
#define  AHBCKFI_TST1S                   31 , 1              /* 31 TST1S (Test Seconds Counter). Defines the test mode for the SECCNT register when set. In this mode, the            */
#define  AHBCKFI_AHB_CLK_FRQ             0 , 8              /* 7-0 AHB_CLK_FRQ (AHB Clock Frequency). Defines the AHB clock frequency in MHz. The configuration software             */

/**************************************************************************************************************************/
/*   Seconds Counter Register (SECCNT)                                                                                    */
/**************************************************************************************************************************/
#define  SECCNT                         (CLK_BASE_ADDR + 0x68) , CLK_ACCESS, 32         /* Offset: CLK_BA + 68h */
#define  SECCNT_SEC_CNT                  0 , 32             /* 31-0 SEC_CNT (Seconds Count). This register is reset by only VSB power-up reset. The value of this register is        */

/**************************************************************************************************************************/
/*   25M Counter Register (CNTR25M)                                                                                        */
/**************************************************************************************************************************/
#define  CNTR25M                         (CLK_BASE_ADDR + 0x6C) , CLK_ACCESS, 32
#define  CNTR25M_COUNT                    0 , 25   /* 25MCNT (25 MHz Count). This register is reset by only VSB power-up reset. The value of this register
                                                        represents a counter with a 25 MHz clock, used to update the SECCNT register. This field is updated every
                                                        640ns. The 4 LSB of this field are always 0. When this field reaches a value of 25,000,000 it goes to 0 and
                                                        SEC_CNT field is updated.*/
#define  CNTR25M_ACCURECY                 EXT_CLOCK_FREQUENCY_MHZ  /* minimum accurecy 1us which is 5 cycles */



/*---------------------------------------------------------------------------------------------------------*/
/* GFXMSEL                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_GFXMSEL_CLKREF             0x02          /*  1 0    : CLKREF clock (25 MHz, default).      */
#define CLK_SEL_GFXMSEL_PLL2               0x03          /*  1 1    : PLL2 clock, frequency divided by 3.  */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKOUTSEL                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_CLKOUTSEL_PLL0             0x00          /*   0 0 0 : PLL0 clock.                          */
#define CLK_SEL_CLKOUTSEL_PLL1             0x01          /*   0 0 1 : PLL1 clock.                          */
#define CLK_SEL_CLKOUTSEL_CLKREF           0x02          /*   0 1 0 : CLKREF input (25 MHz, default).      */
#define CLK_SEL_CLKOUTSEL_PLLG             0x03          /*   0 1 1 : Graphics PLL output clock, divided by 2 */
#define CLK_SEL_CLKOUTSEL_PLL2             0x04          /*   1 0 0 : PLL2 clock divided by 2.             */

/*---------------------------------------------------------------------------------------------------------*/
/* GFXCKSEL                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_GFXCKSEL_PLL0              0x00          /*   0 0   : PLL0 clock.                          */
#define CLK_SEL_GFXCKSEL_PLL1              0x01          /*   0 1   : PLL1 clock.                          */
#define CLK_SEL_GFXCKSEL_CLKREF            0x02          /*   1 0   : CLKREF clock (25 MHz, default).      */
#define CLK_SEL_GFXCKSEL_PLL2              0x03          /*   1 1   : PLL2 clock divided by 2.             */

/*---------------------------------------------------------------------------------------------------------*/
/* TIMCKSEL                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_TIMCKSEL_PLL0              0x00          /*   0 0   : PLL0 clock.                          */
#define CLK_SEL_TIMCKSEL_PLL1              0x01          /*   0 1   : PLL1 clock.                          */
#define CLK_SEL_TIMCKSEL_CLKREF            0x02          /*   1 0   : CLKREF clock (25 MHz, default).      */
#define CLK_SEL_TIMCKSEL_PLL2              0x03          /*   1 1   : PLL2 clock divided by 2.             */

/*---------------------------------------------------------------------------------------------------------*/
/* MCCKSEL                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_MCCKSEL_PLL1               0x00          /*  0 0    : PLL1 clock.                          */
#define CLK_SEL_MCCKSEL_CLKREF             0x02          /*  1 0    : CLKREF clock (25 MHz, default).      */
#define CLK_SEL_MCCKSEL_MCBPCK             0x03          /*  1 1    : MCBPCK clock input.                  */

/*---------------------------------------------------------------------------------------------------------*/
/* SUCKSEL                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_SUCKSEL_PLL0               0x00          /*  0 0    : PLL0 clock.                          */
#define CLK_SEL_SUCKSEL_PLL1               0x01          /*  0 1    : PLL1 clock.                          */
#define CLK_SEL_SUCKSEL_CLKREF             0x02          /*  1 0    : CLKREF clock (25 MHz, default).      */
#define CLK_SEL_SUCKSEL_PLL2               0x03          /*  1 1    : PLL2 clock divided by 2.             */

/*---------------------------------------------------------------------------------------------------------*/
/* UARTCKSEL                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_UARTCKSEL_PLL0             0x00          /*  0 0    : PLL0 clock.                          */
#define CLK_SEL_UARTCKSEL_PLL1             0x01          /*  0 1    : PLL1 clock.                          */
#define CLK_SEL_UARTCKSEL_CLKREF           0x02          /*  1 0    : CLKREF clock (25 MHz, default).      */
#define CLK_SEL_UARTCKSEL_PLL2             0x03          /*  1 1    : PLL2 clock divided by 2.             */

/*---------------------------------------------------------------------------------------------------------*/
/* SDCKSEL                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_SDCKSEL_PLL0               0x00          /*   0 0   : PLL0 clock.                          */
#define CLK_SEL_SDCKSEL_PLL1               0x01          /*   0 1   : PLL1 clock.                          */
#define CLK_SEL_SDCKSEL_CLKREF             0x02          /*   1 0   : CLKREF clock (25 MHz, default).      */
#define CLK_SEL_SDCKSEL_PLL2               0x03          /*   1 1   : PLL2 clock divided by 2.             */

/*---------------------------------------------------------------------------------------------------------*/
/* PIXCKSEL                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_PIXCKSEL_PLLG              0x00          /*   0 0   : PLL GFX clock after divide to 2.     */
#define CLK_SEL_PIXCKSEL_CLKOUT            0x01          /*   0 1   : CLKOUT/GPIO160 pin as input (MFSEL1.21 and GPIO160 controls should be left at default state).*/
#define CLK_SEL_PIXCKSEL_CLKREF            0x02          /*   1 0   : CLKREF input. (default)              */

/*---------------------------------------------------------------------------------------------------------*/
/* GPRFSEL                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_GPRFSEL_GFXBYPCK           0x00          /*   0 0   : GFXBYPCK pin.                        */
#define CLK_SEL_GPRFSEL_USB                0x01          /*   0 1   : USB OHCI Clock (48 MHz).             */
#define CLK_SEL_GPRFSEL_CLKREF             0x02          /*   1 0   : CLKREF input. (default)              */

/*---------------------------------------------------------------------------------------------------------*/
/* CPUCKSEL                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_CPUCKSEL_PLL0              0x00          /*   0 0   : PLL0 clock.                          */
#define CLK_SEL_CPUCKSEL_PLL1              0x01          /*   0 1   : PLL1 clock.                          */
#define CLK_SEL_CPUCKSEL_CLKREF            0x02          /*   1 0   : CLKREF input (25 MHz, default).      */
#define CLK_SEL_CPUCKSEL_SYSBPCK           0x03          /*   1 1   : Bypass clock from pin SYSBPCK.       */

/*---------------------------------------------------------------------------------------------------------*/
/* ADCCKDIV                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_ADCCKDIV1                  0x00          /*    0 0 0: /1 (should not be used).             */
#define CLK_SEL_ADCCKDIV2                  0x01          /*    0 0 1: /2.                                  */
#define CLK_SEL_ADCCKDIV4                  0x02          /*    0 1 0: /4.                                  */
#define CLK_SEL_ADCCKDIV8                  0x03          /*    0 1 1: /8.                                  */
#define CLK_SEL_ADCCKDIV16                 0x04          /*    1 0 0: /16                                  */
#define CLK_SEL_ADCCKDIV32                 0x05          /*    1 0 1: /32 (default)                        */

/*---------------------------------------------------------------------------------------------------------*/
/* APBxCKDIV                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_SEL_APB_DIV1                   0x00          /*     0 0 : AHB1CLK/1.                           */
#define CLK_SEL_APB_DIV2                   0x01          /*     0 1 : AHB1CLK/2. (default for APB5 )       */
#define CLK_SEL_APB_DIV4                   0x02          /*     1 0 : AHB1CLK/4. (default, except APB5)    */
#define CLK_SEL_APB_DIV8                   0x03          /*     1 1 : AHB1CLK/8.                           */





/***********************************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/* DVCSSEL (DVC System Clock Source Select Bit). Typically, a divided PLL2 (by 4): 240 MHz.                */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_DVCSSEL_CLKREF       0x02   /*   1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_DVCSSEL_PLL2         0x03   /*   1 1: PLL2 clock, frequency divided by 4.  */



/*---------------------------------------------------------------------------------------------------------*/
/* GFXMSEL (Graphics Memory Clock Source Select Bit). Typically it would be divided PLL2, 320MHz.          */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_GFXMSEL_CLKREF       0x02   /*   1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_GFXMSEL_PLL2         0x03   /*   1 1: PLL2    clock, frequency divided by 3.  */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKOUTSEL (CLKOUT signal Clock Source Select Bit).                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_CLKOUTSEL_PLL0       0x00   /*  0 0 0: PLL0    clock.  */
#define   CLKSEL_CLKOUTSEL_PLL1       0x01   /*  0 0 1: PLL1    clock.  */
#define   CLKSEL_CLKOUTSEL_CLKREF     0x02   /*  0 1 0: CLKREF input (25 MHz, default).  */
#define   CLKSEL_CLKOUTSEL_PLLG       0x03   /*  0 1 1: Graphics PLL output clock, divided by 2 .  */
#define   CLKSEL_CLKOUTSEL_PLL2       0x04   /*  1 0 0: PLL2    clock divided by 2.  */

/*---------------------------------------------------------------------------------------------------------*/
/* GFXCKSEL (Graphics System Clock Source Select Bit). Typically it would be the same source of the CPU    */
/* clock.                                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_GFXCKSEL_PLL0       0x00   /*  0 0: PLL0    clock.  */
#define   CLKSEL_GFXCKSEL_PLL1       0x01   /*  0 1: PLL1    clock.  */
#define   CLKSEL_GFXCKSEL_CLKREF     0x02   /*  1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_GFXCKSEL_PLL2       0x03   /*  1 1: PLL2    clock divided by 2.  */
/* Note: Before changing this field, assure a delay of 200 clock cycles from last change of GSCKDIV and PCICKDIV fields in CLKDIVx register.  */

/*---------------------------------------------------------------------------------------------------------*/
/* TIMCKSEL (Timer Clock Source Select Bit).                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_TIMCKSEL_PLL0       0x00   /*  0 0: PLL0    clock.  */
#define   CLKSEL_TIMCKSEL_PLL1       0x01   /*  0 1: PLL1    clock.  */
#define   CLKSEL_TIMCKSEL_CLKREF     0x02   /*  1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_TIMCKSEL_PLL2       0x03   /*  1 1: PLL2    clock divided by 2.  */
/* Note: Before changing this field, assure a delay of 200 clock cycles from last change of TIMCKDIV field in CLKDIV register.  */

/*---------------------------------------------------------------------------------------------------------*/
/* MCCKSEL (Memory Controller Clock Source Select Bit).                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_MCCKSEL_PLL1        0x00   /*  0 0: PLL1    clock.  */
#define   CLKSEL_MCCKSEL_CLKREF      0x02   /*  1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_MCCKSEL_MCBPCK      0x03   /*  1 1: MCBPCK clock input.  */

/*---------------------------------------------------------------------------------------------------------*/
/* SUCKSEL (USB Serial Clock Source Select Bit). For UTMI2UTMI and OHCI logic. Should select a clock of 480*/
/* MHZ.                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_SUCKSEL_PLL0        0x00   /*  0 0: PLL0    clock.  */
#define   CLKSEL_SUCKSEL_PLL1        0x01   /*  0 1: PLL1    clock.  */
#define   CLKSEL_SUCKSEL_CLKREF      0x02   /*  1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_SUCKSEL_PLL2        0x03   /*  1 1: PLL2    clock divided by 2.  */
/* Note: Before changing this field, assure a delay of 200 clock cycles from the last change of SUCKDIV field in CLKDIV register.  */

/*---------------------------------------------------------------------------------------------------------*/
/* UARTCKSEL (Core and Host UART Clock Source Select Bit).                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_UARTCKSEL_PLL0      0x00   /*  0 0: PLL0    clock.  */
#define   CLKSEL_UARTCKSEL_PLL1      0x01   /*  0 1: PLL1    clock.  */
#define   CLKSEL_UARTCKSEL_CLKREF    0x02   /*  1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_UARTCKSEL_PLL2      0x03   /*  1 1: PLL2    clock divided by 2.  */
/* Note: Before changing this field, assure a delay of 200 clock cycles from last change of UARTDIV field in CLKDIV register.  */


/*---------------------------------------------------------------------------------------------------------*/
/* SDCKSEL (SDHC Clock Source Select Bit).                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_SDCKSEL_PLL0        0x00   /* 0 0: PLL0    clock.  */
#define   CLKSEL_SDCKSEL_PLL1        0x01   /* 0 1: PLL1    clock.  */
#define   CLKSEL_SDCKSEL_CLKREF      0x02   /* 1 0: CLKREF clock (25 MHz, default).  */
#define   CLKSEL_SDCKSEL_PLL2        0x03   /* 1 1: PLL2    clock divided by 2.  */
/* Note: Before changing this field, assure a delay of 200 clock cycles from last change of SDCKDIV field in CLKDIV register. */

/*---------------------------------------------------------------------------------------------------------*/
/* PIXCKSEL (Pixel Clock Source Select Bit).                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_PIXCKSEL_PLLG       0x00   /* 0 0: PLL GFX clock after divide to 2.  */
#define   CLKSEL_PIXCKSEL_CLKOUT     0x01   /* 0 1: CLKOUT/GPIO160 pin as input (MFSEL1.21 and GPIO160 controls should be left at default state).  */
#define   CLKSEL_PIXCKSEL_CLKREF     0x02   /* 1 0: CLKREF input. (default)  */


/*---------------------------------------------------------------------------------------------------------*/
/* GPRFSEL (Graphics PLL Reference Clock Source Select Bit).                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_GPRFSEL_GFXBYPCK     0x00   /* 0 0: GFXBYPCK pin.  */
#define   CLKSEL_GPRFSEL_USB_OHCI     0x01   /* 0 1: USB OHCI Clock (48 MHz).  */
#define   CLKSEL_GPRFSEL_CLKREF       0x02   /* 1 0: CLKREF input. (default)  */

/*---------------------------------------------------------------------------------------------------------*/
/* CPUCKSEL (CPU/AMBA/MC Clock Source Select Bit).                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define   CLKSEL_CPUCKSEL_PLL0        0x00   /* 0 0: PLL0    clock.  */
#define   CLKSEL_CPUCKSEL_PLL1        0x01   /* 0 1: PLL1    clock.  */
#define   CLKSEL_CPUCKSEL_CLKREF      0x02   /* 1 0: CLKREF input (25 MHz, default).  */
#define   CLKSEL_CPUCKSEL_SYSBPCK     0x03   /* 1 1: Bypass clock from pin SYSBPCK.  */
/* Note: Before changing this field, assure a delay of 200 (selected) clock cycles from last change of CLKDIV register. */






/*---------------------------------------------------------------------------------------------------------*/
/* clock division field values:                                                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_ADCCKDIV                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_ADCCKDIV1     0 /* 0 0 0: /1 (should not be used). */
#define  CLKDIV1_ADCCKDIV2     1 /* 0 0 1: /2. */
#define  CLKDIV1_ADCCKDIV4     2 /* 0 1 0: /4. */
#define  CLKDIV1_ADCCKDIV8     3 /* 0 1 1: /8. */
#define  CLKDIV1_ADCCKDIV16    4 /* 1 0 0: /16 */
#define  CLKDIV1_ADCCKDIV32    5 /* 1 0 1: /32 (default) */


/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_CLK4DIV (AMBA AHB Clock Divider Control)                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_CLK4DIV1    0 /* 0: CLK2 Clock is used for CLK4. */
#define  CLKDIV1_CLK4DIV2    1 /* 1: CLK2 Clock is divided by 2 for CLK4 (default). */
#define  CLKDIV1_CLK4DIV3    2 /* 2: CLK2 Clock is divided by 3 for CLK4. */
#define  CLKDIV1_CLK4DIV4    3 /* 3: CLK2 Clock is divided by 4 for CLK4. */
 /* After changing this field, assure a delay of 200 clock cycles before changing CPUCKSEL field in CLKSEL register. */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_TIMCKDIV (Timer Clock Source Divider Control). Default is divide by 20. The division factor is  */
/* (TIMCKDIV +1), where TIMCKDIV value is 0-31.                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_TIMCK_DIV(n)   ( (n) - 1)     /* • TIMCKDIV value is 0-31. */
/* Note: After changing this field, assure a delay of 200 selected clock cycles before the timer is used. */


/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_UARTDIV (UART Clock Source Divider Control). This resulting clock must be 24 MHz for UARTs proper*/
/* operation                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_UART_DIV(n)   ( (n) - 1)     /* Default is divide by 20. */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_MMCCKDIV (MMC Controller (SDHC2) Clock Divider Control). Sets the division factor from the clock*/
/* selected by SDCKSEL.                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_MMCCK_DIV(n)   ( (n) - 1)     /* The division factor is (MMCCKDIV+1), where MMCCKDIV is 0 to 31. Default is to divide by 32. */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_AHB3CKDIV (SPI3 Clock Divider Control). Sets the division factor from AHB clock (CLK4) to AHB3  */
/* and SPI3                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_AHB3CK_DIV(n)   ( (n) - 1)     /* clock. The division factor is (AHB3CKDIV+1), where AHB3CKDIV is 0 to 31. Default is to divide by 2. */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_PCICKDIV (Internal PCI Clock Divider Control). Sets the division factor from the clock selected */
/* by the CLKSEL                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_PCICK_DIV(n)   ( (n) - 1)     /* GFXCKSEL field. The division factor is (PCICKDIV+1), where PCICKDIV is 0 to 15. */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV1_CLK2DIV (AMBA AXI Clock Divider Control).                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV1_CLK2DIV1    0 /* 0: CPU Clock is used for AXI16 (CLK2). */
#define  CLKDIV1_CLK2DIV2    1 /* 1: CPU Clock is divided by 2 for AXI16 (CLK2)(default). */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV2_APBxCKDIV (AMBA APBx Clock Divider Control).                                                    */
/* APB1 clock must be at least 24 MHz, for UARTs to function. APB clock frequency is up to 67 MHz          */
/* APB2 clock frequency is up to 67 MHz.                                                                   */
/* APB3 clock frequency is up to 67 MHz                                                                    */
/* APB4 clock frequency is up to 67 MHz                                                                    */
/* APB5 clock must be high to enable PSPI1-2 high SPI clock rate. The preferred setting is divide by 2     */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV2_APBxCKDIV1    0 /* 0 0: AHB1CLK/1. */
#define  CLKDIV2_APBxCKDIV2    1 /* 0 1: AHB1CLK/2. */
#define  CLKDIV2_APBxCKDIV4    2 /* 1 0: AHB1CLK/4 (default). */
#define  CLKDIV2_APBxCKDIV8    3 /* 1 1: AHB1CLK/8. */
 /* Note: After changing this field, assure a delay of 200 clock cycles before changing CPUCKSEL field in CLKSEL */


/*---------------------------------------------------------------------------------------------------------*/
/* GFXCKDIV (Graphics System Clock Divider Control). Sets the division factor from the clock selected by   */
/* the CLKSEL.GFXCKSEL field. The division factor is (GFXCKDIV+1), where GFXCKDIV is 0 to 7.               */
/* Default is divide by 5.                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV2_GFXCKDIV_DIV(n)   ( (n) - 1)     /* is (SPI0CKDIV+1), where SPI0CKDIV is 0 to 31. Default is to divide by 1. */
/* After changing this field, ensure a delay of 200 selected clock cycles before changing GFXCKSEL field in CLKSEL register. */



/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV3_SPI0CKDV (SPI0 Clock Divider Control). Sets the division factor from AHB clock to SPI0 clock.   */
/* The division factor is (SPI0CKDIV+1)                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV3_SPI0CKDV_DIV(n)   ( (n) - 1)     /* is (SPI0CKDIV+1), where SPI0CKDIV is 0 to 31. Default is to divide by 1. */

/*---------------------------------------------------------------------------------------------------------*/
/* CLKDIV3_SPIXCKDV (SPIX Clock Divider Control). Sets the division factor from AHB clock to SPIX clock.   */
/* The division factor is (SPIXCKDIV+1)                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define  CLKDIV3_SPIXCKDV_DIV(n)   ( (n) - 1)     /* is (SPIXCKDIV+1), where SPIXCKDIV is 0 to 31. Default is to divide by 1. */


/*---------------------------------------------------------------------------------------------------------*/
/* PWDEN:  PLL power down values                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define  PLLCONn_PWDEN_NORMAL            0
#define  PLLCONn_PWDEN_POWER_DOWN        1



#endif // CLK_REGS_H

