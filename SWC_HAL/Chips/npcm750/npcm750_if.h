/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   npcm750_if.h                                                             */
/*            This file contains chip dependent interface                     */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef __NPCM750_IF_H__
#define __NPCM750_IF_H__

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_CHIP()

/*---------------------------------------------------------------------------------------------------------*/
/* Module Dependencies                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (SMB_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(smb)
#endif

#if defined (ADC_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(adc)
#endif

#if defined (PWM_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(pwm)
#endif

#if defined (FIU_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(fiu)
#endif

#if defined (SHM_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(shm)
#endif

#if defined (MFT_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(mft)
#endif

#if defined (DAC_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(dac)
#endif

#if defined (PS2_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(ps2)
#endif

#if defined (GPIO_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(gpio)
#endif

#if defined (PECI_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(peci)
#endif

#if defined (CLK_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(clk)
#endif

#if defined (SPI_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(spi)
#endif

#if defined (SD_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(sd)
#endif

#if defined (UART_MODULE_TYPE)
#include __MODULE_IF_HEADER_FROM_CHIP_DRV(uart)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

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
void CHIP_SMBPullUp (SMB_MODULE_T module_num, BOOLEAN enable);


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
DEFS_STATUS  CHIP_SMB_GetGpioNumbers (SMB_MODULE_T smb_module, UINT segment, INT* ret_scl_gpio, INT* ret_sda_gpio );



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
DEFS_STATUS CHIP_SMB_GetSegment (SMB_MODULE_T smb_module, UINT* segment);


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
/*---------------------------------------------------------------------------------------------------------*/
#if defined (SMB_MODULE_TYPE)
void CHIP_MuxSMB (SMB_MODULE_T smb_module, BOOLEAN bEnable);
#endif



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
/*                  This routine selects the SMBx segment to use.                                          */
/*                  It does not mux the SMB! use CHIP_MuxSMB in addition                                   */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_Segment (SMB_MODULE_T smb_module, UINT32 segment, BOOLEAN bEnable);

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
BOOLEAN CHIP_MuxPECI  (void);
#endif


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
BOOLEAN CHIP_MuxESPI  (UINT driveStrength_mA);
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
BOOLEAN CHIP_MuxLPC  (UINT driveStrength_mA);


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
void CHIP_MuxADC (ADC_INPUT_T adc_module);
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
void CHIP_MuxPWM (PWM_MODULE_T pwm_module);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxFIU                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum  - SPI Flash device number                                                      */
/*                  csXEnable - enable CS.                                                                 */
/*                  fiuMode -  FIU mode (simgle\dual\quad)                                                 */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects FIU muxing                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (FIU_MODULE_TYPE)
DEFS_STATUS CHIP_MuxFIU (   FIU_MODULE_T      devNum,
                            BOOLEAN           cs0Enable,
                            BOOLEAN           cs1Enable,
                            BOOLEAN           cs2Enable,
                            BOOLEAN           cs3Enable,
                            BOOLEAN           quadMode);

#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxSPI                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - The PSPI module to be selected.                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the PSPI function to the corresponding pin.                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (SPI_MODULE_TYPE)
void CHIP_MuxSPI (SPI_MODULE_T spi_module);
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
void CHIP_MuxMFT (MFT_MODULE_T mft_module, MFT_TIMER_T  mft_timer);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxGPIO                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio - GPIO number to mux                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine mux chip pin to function as GPIO                                          */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (GPIO_MODULE_TYPE)
void CHIP_MuxGPIO (UINT8 gpio);
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
/*---------------------------------------------------------------------------------------------------------*/
#if defined (GPIO_MODULE_TYPE)
void CHIP_MuxGPIOPort (UINT8 port);
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
void CHIP_MuxDAC (DAC_INPUT_T dac_input);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxPS2                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ps2_channel - PS2 channel number                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the PS2 channel interface on multiplexed pin.                     */
/*                  The function should be called prior to any PS2 operation.                              */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (PS2_MODULE_TYPE)
void CHIP_MuxPS2 (PS2_CHANNEL_T ps2_channel);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MuxCIRLED                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  enable - TRUE to select IR_RX signal to MFT5 Timer 5; FALSE to select TACH10 input.    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables or disables connection of the IR_RX signal to MFT 5 function      */
/*                  Timer B (when this connection is disabled, TACH10 signal is connected instead).        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (CIRLED_MODULE_TYPE)
void CHIP_MuxCIRLED (BOOLEAN enable);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_CIRJacksStatus                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         Status of CIR Jacks Detection                                                          */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the CIR emitter jacks detection, i.e. the value of ^CIRDT1 and    */
/*                  ^CIRDT2 pins, which is reflected in SPBCTL register.                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (CIR_MODULE_TYPE)
UINT8 CHIP_CIRJacksStatus  (void);
#endif









/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_SD                                                                             */
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
DEFS_STATUS CHIP_Mux_SD(SD_DEV_NUM_T sdNum, SD_DEV_TYPE_T sdType);
#endif //  ( SD_MODULE_TYPE)

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootTimeConfig                                                                 */
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
void CHIP_EmmcBootTimeConfig (UINT32 clk);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootStart                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ack - TRUE means expect the boot acknowledge data, FALSE otherwise.                    */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initiates a EMMC boot sequence.                                           */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ( SD_MODULE_TYPE)
void CHIP_EmmcBootStart (BOOLEAN ack);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootInvalid                                                                    */
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
BOOLEAN CHIP_EmmcBootInvalid  (void);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EmmcBootEnd                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ends a EMMC boot sequence, either when the boot sequence is done,         */
/*                  or an error is discovered.                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ( SD_MODULE_TYPE)
void CHIP_EmmcBootEnd  (void);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SetVCDInput                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  headNum - Number of the head (1 or 2)                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the intput for the VCD                                            */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (VCD_MODULE_TYPE)
void CHIP_SetVCDInput(/* TODO CHIP_VCD_HEAD_T headNum */ void);
#endif //  VCD_MODULE_TYPE


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_SetGFXInput                                                                        */
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
void CHIP_SetGFXConfig(/* TODO VCD_CONFIG_T config */ void);
#endif // VCD_MODULE_TYPE


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_ConfigUSBBurstSize                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the USB burst size in USB test register                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_ConfigUSBBurstSize (void);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_GSPI                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs muxing for Graphics Core SPI Signals                             */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_GSPI (BOOLEAN bOn);



#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_Uart                                                                           */
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
DEFS_STATUS CHIP_Mux_Uart (UART_MUX_T redirection_mode, BOOLEAN CoreSP, BOOLEAN sp1, BOOLEAN sp2);
#endif // UART_MODULE_TYPE



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_ConfigUSBBurstSize                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the USB burst size in USB test register                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_ConfigUSBBurstSize (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_ResetTypeIsPor                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns true if reset source was POR (powerOn) reset                                   */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN CHIP_ResetTypeIsPor (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_ResetTypeIsCor                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns TRUE if and only if a Core reset is performed                                  */
/*                  and FALSE on any other reset type.                                                     */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN CHIP_ResetTypeIsCor (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_MarkCfgDone                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Sets a flag in scratch register specifying that CLK/DDR configuration were performed   */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_MarkCfgDone (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_CfgWasDone                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns true if a flag in scratch register indicates                                   */
/*                  that CLK/DDR configuration was already done                                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN CHIP_CfgWasDone (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_EnableJtag                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Enable Jtag access                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_EnableJtag (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_DisableJtag                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Disable Jtag access                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_DisableJtag (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_DisableModules                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  mdlr -  Value to write to MDLR register                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Writes to the MDLR register which determines which modules to disable                  */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_DisableModules(UINT32 mdlr);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Mux_SIOXH                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  enable -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SIOXH mux                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_Mux_SIOXH(BOOLEAN enable);


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
UINT32   CHIP_Get_Version (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_Disable_CPU1                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable second core for singel core solution  (used in Z2 and above)      */
/*---------------------------------------------------------------------------------------------------------*/
void   CHIP_Disable_CPU1 (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CHIP_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the chip registers                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CHIP_PrintRegs (void);

#endif //__npcm750_IF_H__

