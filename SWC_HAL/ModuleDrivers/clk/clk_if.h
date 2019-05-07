/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   clk_if.h                                                                 */
/*            This file contains CLK  (clock) module interface                */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef CLK_IF_H
#define CLK_IF_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#include __CHIP_H_FROM_IF()

#include "hal.h"


typedef enum PLL_tag
{
    PLL0    = 0,
    PLL1    = 1,
    PLL2    = 2,
    PLL_GFX = 3,

} PLL_MODULE_T ;


typedef enum APB_CLK
{
    APB1  = 1,
    APB2  = 2,
    APB3  = 3,
    APB4  = 4,
    APB5  = 5,
	SPI0  = 6,
	SPI3  = 7
} APB_CLK;


/*---------------------------------------------------------------------------------------------------------*/
/* SD device definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    SD1_DEV = 0,
    SD2_DEV = 1,
} SD_DEV_NUM_T;


#if defined (STRP_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureClocks                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs general clocks configuration                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ConfigureClocks (void);
#endif // defined (STRP_MODULE_TYPE)

#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureUartClock                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs UART clock configuration                                         */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_ConfigureUartClock (void);
#endif // defined (UART_MODULE_TYPE)

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
DEFS_STATUS CLK_ConfigureUSBClock (void);
#endif //  defined (USB_MODULE_TYPE)


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureAPBClock                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         APB freq in Hz                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures APB divider to 2, return the frequency of APB in Hz                         */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_ConfigureAPBClock (APB_CLK apb);

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
UINT32 CLK_ConfigureTimerClock (void);
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
void CLK_ConfigureEMCClock (UINT32 ethNum);
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
void CLK_ConfigureGMACClock (UINT32 ethNum);
#endif //#if defined (GMAC_MODULE_TYPE)

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureSDClock                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum       -  SD module number                                                        */
/*                                                                                                         */
/* Returns:         SD clock frequency                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the SD clock to frequency closest to the target clock from beneath          */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_ConfigureSDClock (UINT32 sdNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureADCClock                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  adc_freq -  adc frequency in Hz.                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets ADC clock in Hz                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void    CLK_ConfigureADCClock (UINT32 adc_freq);


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
int    CLK_GetADCClock (void);


#if defined (EMC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetEMC                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of EMC module                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetEMC (UINT32 deviceNum);
#endif // #if defined (EMC_MODULE_TYPE)

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
void CLK_ResetGMAC (UINT32 deviceNum);
#endif // #if defined (GMAC_MODULE_TYPE)

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
void CLK_ResetFIU (void);
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
void CLK_ResetUART (void);
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
void CLK_ResetAES (void);
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
void CLK_ResetMC (void);
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
void CLK_ResetTIMER (UINT32 deviceNum);
#endif // #if defined (TMC_MODULE_TYPE)

#if defined (SD_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetSD                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum -  SD module number                                                              */
/*                                                                                                         */
/* Returns:         SWC HAL Error code                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of SD                                                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_ResetSD (UINT32 sdNum);
#endif // #if defined (SD_MODULE_TYPE)

#if defined (PSPI_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetPSPI                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -  PSPI module number                                                        */
/*                                                                                                         */
/* Returns:         BMC HAL Error code                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of PSPI                                                 */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS CLK_ResetPSPI(UINT32 deviceNum);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPLL0toAPBdivisor                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the value achieved by dividing PLL0 frequency to APB frequency    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetPLL0toAPBdivisor (APB_CLK apb);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_MicroSec                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSec -  number of microseconds to delay                                            */
/*                                                                                                         */
/* Returns:         Number of iterations executed                                                          */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs a busy delay (empty loop)                                        */
/*                  the number of iterations is based on current CPU clock calculation and cache           */
/*                  enabled/disabled                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_MicroSec (UINT32 microSec);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_Since                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSec -  number of microseconds to delay                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  get a time stamp, delay microSec from it. If microSec has allready passed since the    */
/*                  time stamp , then no delay is needed.                                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_Since (UINT32 microSec, UINT32 time_quad[2]);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Time_MicroSec                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         Current time stamp                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void  CLK_GetTimeStamp (UINT32 time_quad[2]);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_Cycles                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cycles -                                                                               */
/*                                                                                                         */
/* Returns:         Number of iterations executed                                                          */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs delay in number of cycles                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_Cycles (UINT32 cycles);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetCPUFreq                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates CPU frequency                                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetCPUFreq (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetMCFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates MC frequency                                                   */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetMCFreq (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetAPBFreq                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns APB frequency                                                     */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetAPBFreq (APB_CLK apb);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetCPFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns CP frequency                                                      */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetCPFreq (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetTimerFreq                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates tiemr frequency in Hz                                          */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetTimerFreq (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_SetCPFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets CP frequency                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_SetCPFreq (UINT32  cpFreq);



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
UINT32 CLK_GetSDClock (UINT32 sdNum);
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
void CLK_ConfigurePCIClock (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPCIClock                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets PCI clock frequency                                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetPCIClock (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetGFXClock                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets GFX clock frequency                                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetGFXClock (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll0Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL0 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPll0Freq (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll1Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL1 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPll1Freq (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll2Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL2 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPll2Freq (void);



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
DEFS_STATUS CLK_Configure_CPU_MC_Clock (UINT32 mcFreq , UINT32 cpuFreq, UINT32 pll0_freq);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Verify_and_update_dividers                                                         */
/*                                                                                                         */
/* Parameters:      None                                                                                   */
/* Returns:         DEFS_STATUS                                                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine fix all the dividers after PLL change                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_Verify_and_update_dividers (void);

#if defined (FIU_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureFIUClock                                                                  */
/*                                                                                                         */
/* Parameters:      fiu - module (0, 3, X).                                                                */
/*                  clkDiv - actual number to write to reg. The value is clkdDiv + 1)                      */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config the FIU clock (according to the header )                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS CLK_ConfigureFIUClock (FIU_MODULE_T  fiu, UINT8 clkDiv);
#endif // #if defined (FIU_MODULE_TYPE)

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
UINT8 CLK_GetFIUClockDiv (FIU_MODULE_T  fiu);
#endif // defined (FIU_MODULE_TYPE)

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_PrintRegs (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_PrintVersion (void);

#endif // CLK_IF_H

