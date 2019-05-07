/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   mc_if.h                                                                  */
/*            This file contains MC module interface                          */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _MC_IF_H
#define _MC_IF_H

#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "hal.h"




/*---------------------------------------------------------------------------------------------------------*/
/* Priority type                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    MC_PRIO_HIGH,
    MC_PRIO_MIDDLE,
    MC_PRIO_LOW,
} MC_PRIO_T;




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_ConfigureDDR                                                                        */
/*                                                                                                         */
/* Parameters:      UINT8 mc_config : value from  BOOTBLOCK_Get_MC_config (BB header)                      */
/* Returns:         status                                                                                 */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  Set default configuration for the DDR Memory Controller                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS MC_ConfigureDDR (UINT8 mc_config);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_SetPortPriority                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  portNum     - Number of the Memory port to set the priority to                         */
/*                  fixedPrio   - Indicates if to set fixed priority or auto priority                      */
/*                  priority    - In fixed mode, setting fixed priority                                    */
/*                                In auto mode, setting initial priority                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine priority configuration for Memory controller ports                        */
/*---------------------------------------------------------------------------------------------------------*/
void MC_SetPortPriority (UINT portNum, BOOLEAN fixedPrio, MC_PRIO_T priority);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_ReconfigureODT                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Transfers the PCTL to CFG state, writes to ODTCFG register and                         */
/*                  returns to ACTIVE state.                                                               */
/*                  Assumes: PCTL has already been configured.                                             */
/*                  This function serves the Booter to bypass Z1 ROM configuration issue.                  */
/*---------------------------------------------------------------------------------------------------------*/
void MC_ReconfigureODT (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_EnableCoherency                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Enables the Coherency Reorder feature in Memory Controller                             */
/*---------------------------------------------------------------------------------------------------------*/
void MC_EnableCoherency (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_UpdateDramSize                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Update refresh rate according to size                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void MC_UpdateDramSize (UINT iDramSize);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_CheckDramSize                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  measure the DRAM size                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 MC_CheckDramSize (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_MemStressTest                                                                        */
/*                                                                                                         */
/* Parameters:      bECC : if true, mem test is for ECC lane. Can't sweep it directly, must check ber bit  */
/* Returns:         bitwise error in UINT16 (one UINT8 for each lane)                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine does memory test with a list of patterns. Used for sweep.                 */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 MC_MemStressTest (BOOLEAN bECC, BOOLEAN bQuick );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_Init_Denali                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*          This routine init the memory controller                                                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS MC_Init_Denali (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_PrintRegs                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void MC_PrintRegs (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MC_PrintVersion                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void MC_PrintVersion (void);


#endif //_MC_IF_H

