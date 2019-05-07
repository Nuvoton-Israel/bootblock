/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   shm_if.h                                                                 */
/*            This file contains Shared Memory (SHM) interface                */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _SHM_IF_H
#define _SHM_IF_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_IF()

#if defined (SHM_MODULE_TYPE)
#include __MODULE_HEADER(shm, SHM_MODULE_TYPE)
#endif

#if defined (FIU_MODULE_TYPE)
#include __MODULE_HEADER(fiu, FIU_MODULE_TYPE)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* SHM window number                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {
    SHM_WINDOW_ONE      = 0,
    SHM_WINDOW_TWO      = 1,
#ifdef SHM_CAPABILITY_EXTENDED_SHM
    SHM_WINDOW_THREE    = 2,
    SHM_WINDOW_FOUR     = 3,
#endif
    SHM_WINDOW_NUMBER
} SHM_WINDOW_T;


/*---------------------------------------------------------------------------------------------------------*/
/* SHM Host error response                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {
    SHM_NO_SYNC          = 0,  /* EC does not return SYNC (default). */
    SHM_RETURN_0_ON_READ = 1,  /* EC returns 00h data on read transactions. */
    SHM_LONG_WAIT_SYNC   = 2   /* EC returns Long Wait SYNC until the block protection is removed */
} SHM_HOST_ERROR_RESPONSE_T;


/*---------------------------------------------------------------------------------------------------------*/
/* SHM offset interrupts (core)                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {
    SHM_INT_ON_HOST_RD_PROTECT_ADDR     = 0x00000001,
    SHM_INT_ON_HOST_WR_PROTECT_ADDR     = 0x00000002,
    SHM_INT_ON_HOST_WR_TO_INDIRECT_SEM  = 0x00000008,
    SHM_INT_ON_HOST_WR_WIN1_SEM         = 0x00000010,
    SHM_INT_ON_HOST_WR_WIN2_SEM         = 0x00000020,
    SHM_INT_ON_ANY_HOST_SHM_WRITE       = 0x00000040,
#ifdef SHM_CAPABILITY_OFFSET
    SHM_INT_ON_HOST_RD_WIN1_CORE_OFFSET = 0x00000100,
    SHM_INT_ON_HOST_WR_WIN1_HOST_OFFSET = 0x00000200,
    SHM_INT_ON_HOST_RD_WIN2_CORE_OFFSET = 0x00000400,
    SHM_INT_ON_HOST_WR_WIN2_HOST_OFFSET = 0x00000800,
#endif
#ifdef SHM_CAPABILITY_EXTENDED_SHM
    SHM_INT_ON_HOST_WR_WIN3_SEM         = 0x00100000,
    SHM_INT_ON_HOST_WR_WIN4_SEM         = 0x00200000,
#ifdef SHM_CAPABILITY_OFFSET
    SHM_INT_ON_HOST_RD_WIN3_CORE_OFFSET = 0x01000000,
    SHM_INT_ON_HOST_WR_WIN3_HOST_OFFSET = 0x02000000,
    SHM_INT_ON_HOST_RD_WIN4_CORE_OFFSET = 0x04000000,
    SHM_INT_ON_HOST_WR_WIN4_HOST_OFFSET = 0x08000000,
#endif
#endif /* SHM_CAPABILITY_EXTENDED_SHM */
} SHM_INT_TYPE_T;

typedef enum {
    SHM_OFFSET_INT__ON_HOST_RD_CORE_OFFSET = 0,  //Enable interrupt on host read from core offset
    SHM_OFFSET_INT__ON_HOST_WR_HOST_OFFSET = 1   //Enable interrupt on host write to host offset
} SHM_OFFSET_INT_T;


/*---------------------------------------------------------------------------------------------------------*/
/* SHM offset interrupts (host)                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define E_IRQ_CORE_RD_FROM_HOST_OFFSET       MASK_BIT(0)  //Enable Host IRQ on core read from host offset
#define E_IRQ_CORE_WR_TO_CORE_OFFSET         MASK_BIT(1)  //Enable Host IRQ on core read from host offset
#define E_SMI_CORE_RD_FROM_HOST_OFFSET       MASK_BIT(4)  //Enable Host IRQ on core read from host offset
#define E_SMI_CORE_WR_TO_CORE_OFFSET         MASK_BIT(5)  //Enable Host IRQ on core read from host offset


/*---------------------------------------------------------------------------------------------------------*/
/* SHM Callback function                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef SHM_CAPABILITY_OFFSET
#define SHM_INT_STATUS_HOST_READ_ACCESS_ERROR        MASK_BIT(0)
#define SHM_INT_STATUS_HOST_SEM_INDIRECT_WRITE       MASK_BIT(3)
#define SHM_INT_STATUS_HOST_SEM1_WRITE_ACCESS        MASK_BIT(4)
#define SHM_INT_STATUS_HOST_SEM2_WRITE_ACCESS        MASK_BIT(5)
#define SHM_INT_STATUS_HOST_SHM_ACCESS               MASK_BIT(6)
#define SHM_INT_STATUS_HOST_WIN1_CORE_OFFSET_READ    MASK_BIT(8)
#define SHM_INT_STATUS_HOST_WIN1_HOST_OFFSET_WRITE   MASK_BIT(9)
#define SHM_INT_STATUS_HOST_WIN2_CORE_OFFSET_READ    MASK_BIT(10)
#define SHM_INT_STATUS_HOST_WIN2_HOST_OFFSET_WRITE   MASK_BIT(11)
#ifdef SHM_CAPABILITY_EXTENDED_SHM
#define SHM_INT_STATUS_HOST_SEM3_WRITE_ACCESS        MASK_BIT(20)
#define SHM_INT_STATUS_HOST_SEM4_WRITE_ACCESS        MASK_BIT(21)
#define SHM_INT_STATUS_HOST_WIN3_CORE_OFFSET_READ    MASK_BIT(24)
#define SHM_INT_STATUS_HOST_WIN3_HOST_OFFSET_WRITE   MASK_BIT(25)
#define SHM_INT_STATUS_HOST_WIN4_CORE_OFFSET_READ    MASK_BIT(26)
#define SHM_INT_STATUS_HOST_WIN4_HOST_OFFSET_WRITE   MASK_BIT(27)
#endif  /* SHM_CAPABILITY_EXTENDED_SHM */
#endif  /* SHM_CAPABILITY_OFFSET */

typedef void (*SHM_CALLBACK_T)(UINT32 sts);

#ifdef ESPI_MODULE_TYPE
typedef void (*SHM_ESPI_CALLBACK_T)(void);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_Init                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  shm_callback - Function to be called upon shared memory interrupt                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Shared memory hardware module elements that are relevant for  */
/*                  both Shared Memory RAM windows.                                                        */
/*                  Note that flash-related initializations which are expected to be done by the boot ROM  */
/*                  are not done here (e.g. flash size, read burst size, memory protection,                */
/*                  HOSTWAIT clearing).                                                                    */
/*                                                                                                         */
/* Usage:           in order to init the shm call 4 function:                                              */
/*                   1. SHM_Init: sets the callback                                                        */
/*                   2. SHM_Config: sets SMC_CTL, unless using default values.                             */
/*                   3. SHM_ReleaseHostWait: host can start                                                */
/*                   4. SHM_HostAccessEnable(TRUE): host can start using the SHM.                          */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_Init (SHM_CALLBACK_T shm_callback);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_Config                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  hostErrorResponse     - Host Error Response.                                           */
/*                  bEnableIntByHostError - Enable Interrupt by Host Access Errors.                        */
/*                  bEnableIntHostAccShm  - Enable Interrupt by Host Access to Shared Memory.              */
/*                  bPrefetchEnable       - Prefetch Enable.                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configs Shared memory host handling: interrupts, error response etc.      */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_Config (   SHM_HOST_ERROR_RESPONSE_T  hostErrorResponse,
                    BOOLEAN                    bEnableIntByHostError,
                    BOOLEAN                    bEnableIntHostAccShm,
                    BOOLEAN                    bPrefetchEnable);

#ifdef ESPI_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EspiRegisterCallback                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  shm_espiCallback - Espi callback                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine registers callback for ESPI module interrupts                             */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EspiRegisterCallback (SHM_ESPI_CALLBACK_T shm_espiCallback);
#endif // ESPI_MODULE_TYPE

#ifdef SHM_CAPABILITY_SEMAPHORE_SMI_GENERATION
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_HostSemaphoreConfig                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num  - Shared Memory Window to be configured.                                   */
/*                  bEnableIRQ  - TRUE:  Enable generation of IRQ on Core write to the semaphore register  */
/*                                FALSE: Disable IRQ on Core write to the semaphore register               */
/*                  bEnableSMI  - TRUE:  Enable generation of SMI on Core write to the semaphore register  */
/*                                FALSE: Disable SMI on Core write to the semaphore register               */
/*                  bEnableSemOnWindow -                                                                   */
/*                                TRUE: enable a semaphore access through the first address byte of        */
/*                                      the RAM Window. In othere words this will cuase the semaphore to   */
/*                                      be located in offset 0 of the SHM RAM window.                      */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Shared memory Semaphore behavior                              */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_HostSemaphoreConfig ( SHM_WINDOW_T window_num,
                               BOOLEAN      bEnableIRQ,
                               BOOLEAN      bEnableSMI,
                               BOOLEAN      bEnableSemOnWindow);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_WindowConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num  - Shared Memory Window to be configured.                                   */
/*                  window_base - Core base address of RAM window.                                         */
/*                  window_size - Exponent of 2 - e.g. 4 designate RAM window of 16 (2**4).                */
/*                                Valid values are 3-12.                                                   */
/*                  window_prot - bitMask of Shared RAM Window, Access Protection                          */
/*                                if a bit is set to 1 - the specific aread is protected.                  */
/*                                if a bit is set to 0 - the specific aread is not protected               */
/*                                ===========================================================              */
/*                                For uRider12 (and below)/SIO14/SIO15 the bits meaning are:               */
/*                                ===========================================================              */
/*                                - Bit 0 = Low Half of window is Read Protected                           */
/*                                - Bit 1 = Low Half of window is Write Protected                          */
/*                                - Bit 2 = High Half of window is Read Protected                          */
/*                                - Bit 3 = High Half of window is Write Protected                         */
/*                                ===========================================================              */
/*                                For MRider15 the bits meaning are:                                       */
/*                                ===========================================================              */
/*                                - Bit 0  = Write Protect the first (lower) 1/8 of the RAM window         */
/*                                - Bit 1  = Write Protect the second 1/8 of the RAM window                */
/*                                - Bit 2  = Write Protect the 3rd 1/8 of the RAM window                   */
/*                                - Bit 3  = Write Protect the 4th 1/8 of the RAM window                   */
/*                                - Bit 4  = Write Protect the 5th 1/8 of the RAM window                   */
/*                                - Bit 5  = Write Protect the 6th 1/8 of the RAM window                   */
/*                                - Bit 6  = Write Protect the 7th 1/8 of the RAM window                   */
/*                                - Bit 7  = Write Protect the 8th 1/8 of the RAM window                   */
/*                                - Bit 8  = Read Protect the first (lower) 1/8 of the RAM window          */
/*                                - Bit 9  = Read Protect the second 1/8 of the RAM window                 */
/*                                - Bit 10 = Read Protect the 3rd 1/8 of the RAM window                    */
/*                                - Bit 11 = Read Protect the 4th 1/8 of the RAM window                    */
/*                                - Bit 12 = Read Protect the 5th 1/8 of the RAM window                    */
/*                                - Bit 13 = Read Protect the 6th 1/8 of the RAM window                    */
/*                                - Bit 14 = Read Protect the 7th 1/8 of the RAM window                    */
/*                                - Bit 15 = Read Protect the 8th 1/8 of the RAM window                    */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Shared memory hardware module elements that are relevant for  */
/*                  the WCB protocol implementation.                                                       */
/*                  Note that flash-related initializations which are expected to be done by the boot ROM  */
/*                  are not done here (e.g. flash size, read burst size, memory protection,                */
/*                  HOSTWAIT clearing).                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_WindowConfig (SHM_WINDOW_T window_num, UINT32 window_base, UINT8 window_size, UINT16 window_prot);

#ifdef SHM_CAPABILITY_EXTENDED_SHM
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableHostEventRegisters                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window number.                                              */
/*                  enbale     - TRUE: enable, FALSE disable                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable/disable the Host Event Registers                                   */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableHostEventRegisters (SHM_WINDOW_T window_num, BOOLEAN  enable);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableSemaphoreInterrupt                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window number.                                              */
/*                  enbale     - TRUE: enable, FALSE disable                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable/disable the interrupt on host access to the Semaphore  register    */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableSemaphoreInterrupt (SHM_WINDOW_T  window_num, BOOLEAN enable);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SemaphoreRead                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be read.                                          */
/*                                                                                                         */
/* Returns:         Semaphore value                                                                        */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads the semaphore in a given Shared Memory Window.                      */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 SHM_SemaphoreRead (SHM_WINDOW_T window_num);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SemaphoreWrite                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  value      - Value to be written to sempaphore.                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes to the semaphore in a given Shared Memory Window.                  */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_SemaphoreWrite (SHM_WINDOW_T window_num, UINT8 value);

#if defined FIU_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_FlashConfig                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  flash_size       - The flash size.                                                     */
/*                  read_burst_size  - Flash read burst size (1/16 bytes).                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the size of the connected flash for SHM operations.            */
/*                  The Flash size must be configured to the same value as in FIU_CFG register.            */
/*                  It also selects the maximum size of a read burst transaction, which is supported by    */
/*                  the flash.                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_FlashConfig (FIU_DEV_SIZE_T flash_size, FIU_R_BURST_T read_burst_size);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_HostAccessEnable                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/stalls host read transactions:                                    */
/*                  - When transactions are enabled, all transactions are served automatically by the      */
/*                    hardware, including transactions that are pending from the period when the           */
/*                    transactions were stalled.                                                           */
/*                    It is intended to be called when flash write or erase operation is completed.        */
/*                  - When transactions are stalled, no transactions are served and interrupts (notifying  */
/*                    the core of a pending transaction) are not enabled.                                  */
/*                    It is intented to be called whenever a flash write or erase operation is initiated - */
/*                    the SHM transaction are stalled until read and write operation is completed.         */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_HostAccessEnable (BOOLEAN enable);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_HostAccessState                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         TRUE if host read transactions are stalled; FALSE otherwise.                           */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the stall host read transactions state.                         */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN SHM_HostAccessState (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_ReleaseHostWait                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine releases the LPC bus stall, allowing host to communicate with the device. */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_ReleaseHostWait (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_ClearInterrupts                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*        window_num - Shared Memory Window to be written.                                                 */
/*        val (bit mask)     - 0x001 (SHM_INT_ON_HOST_RD_PROTECT_ADDR), clear pending interrupt on         */
/*                                    host read from SHM protected address.                                */
/*                           - 0x002 (SHM_INT_ON_HOST_WR_PROTECT_ADDR), clear pending interrupt on         */
/*                                    host write to SHM protected address.                                 */
/*                           - 0x008 (SHM_INT_ON_HOST_WR_TO_INDIRECT_SEM), clear pending interrupt on      */
/*                                    host write to the indirect semaphore                                 */
/*                           - 0x010 (SHM_INT_ON_HOST_WR_WIN1_SEM), clear pending interrupt on             */
/*                                    host write to SHM win1 semaphore.                                    */
/*                           - 0x020 (SHM_INT_ON_HOST_WR_WIN2_SEM), clear pending interrupt on             */
/*                                    host write to SHM win2 semaphore.                                    */
/*                           - 0x020 (SHM_INT_ON_ANY_HOST_SHM_WRITE), clear pending interrupt on           */
/*                                    host write to any unprotected SHM address.                           */
/*                           - 0x100 (SHM_INT_ON_HOST_RD_WIN1_CORE_OFFSET), clear pending interrupt on     */
/*                                    host read from win1 core offset.                                     */
/*                           - 0x200 (SHM_INT_ON_HOST_WR_WIN1_HOST_OFFSET), clear pending interrupt on     */
/*                                    host write to win1 host offset.                                      */
/*                           - 0x400 (SHM_INT_ON_HOST_RD_WIN2_CORE_OFFSET), clear pending interrupt on     */
/*                                    host read from win2 core offset.                                     */
/*                           - 0x800 (SHM_INT_ON_HOST_WR_WIN2_HOST_OFFSET), clear pending interrupt on     */
/*                                    host write to win2 host offset.                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine clear pending Core interrupt on host access to one of the                 */
/*                  pre-configured offsets                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_ClearInterrupts (SHM_INT_TYPE_T val);

#ifdef SHM_CAPABILITY_OFFSET

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SetCoreOffset                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  offset     -  offset from the windows base.                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Set the "Core offset" from the windows base.                                           */
/*                  A core WRITE to this offset can generate an IRQ/SMI to the host                        */
/*                  A host READ from this offset can generate an Interrupt to the core                     */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_SetCoreOffset (SHM_WINDOW_T window_num, UINT16 offset);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableCoreOffsetInterrupts                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  val       -  0 (SHM_OFFSET_INT__ON_HOST_RD_CORE_OFFSET), Enable/Disable interrupt on   */
/*                                  host read from core offset.                                            */
/*                            -  1 (SHM_OFFSET_INT__ON_HOST_WR_HOST_OFFSET), Enable/Disable interrupt on   */
/*                                  host write to host offset.                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable/disable the Core interrupt on host access to one of the            */
/*                  pre-configured offsets                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableCoreOffsetInterrupts (SHM_WINDOW_T window_num, SHM_OFFSET_INT_T val, BOOLEAN enable);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_SetHostOffset                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  offset     -  offset from the windows base.                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  uses the SIB to access the Host registers in order to set the "Host offset"            */
/*                  A host WRITE to this offset can generate an Interrupt to the core                      */
/*                  A core READ from this offset can generate an IRQ/SMI to the host                       */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_SetHostOffset (SHM_WINDOW_T window_num, UINT16 offset);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_EnableHostOffsetInterrupts                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  window_num - Shared Memory Window to be written.                                       */
/*                  mask      -  bit 0 (E_IRQ_CORE_RD_FROM_HOST_OFFSET), when set, Enable IRQ on           */
/*                                      core read from host offset.                                        */
/*                            -  bit 1 (E_IRQ_CORE_WR_TO_CORE_OFFSET), when set, Enable IRQ on             */
/*                                      core write to core offset.                                         */
/*                            -  bit 2,3 not used                                                          */
/*                            -  bit 4 (E_SMI_CORE_RD_FROM_HOST_OFFSET), when set, Enable SIM on           */
/*                                      core read from host offset.                                        */
/*                            -  bit 5 (E_SMI_CORE_WR_TO_CORE_OFFSET), when set, Enable SMI on             */
/*                                      core write to core offset.                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  uses the SIB to access the Host registers in order to enable/disable the Host          */
/*                  IRQ or SMI on core access to one of the pre-configured offsets                         */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_EnableHostOffsetInterrupts (SHM_WINDOW_T window_num, UINT8 mask);

#endif //SHM_CAPABILITY_OFFSET

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_PrintRegs (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHM_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SHM_PrintVersion (void);

#endif /* _SHM_IF_H */

