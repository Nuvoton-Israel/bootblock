/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   hal_internal.h                                                           */
/*            This file contains internal HAL macros                          */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef __HAL_INTERNAL_H__
#define __HAL_INTERNAL_H__

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             INCLUDE MACROS                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Include external files                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define __EXTERNAL_FILE(name)                        STRINGX(name)

#if !defined SWC_HAL_FLAT

/*---------------------------------------------------------------------------------------------------------*/
/* Include drivers interfaces from hal                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_IF(moduleType)                      STRINGX(ModuleDrivers/moduleType/moduleType##_if.c)
#define __LOGICAL_IF(logicalType)                    STRINGX(LogicalDrivers/logicalType/logicalType##_if.c)
#define __PERIPHERAL_IF(peripheralType)              STRINGX(PeripheralDrivers/peripheralType/peripheralType##_if.c)

/*---------------------------------------------------------------------------------------------------------*/
/* Include drivers interfaces headers from hal                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_IF_HEADER(moduleType)               STRINGX(ModuleDrivers/moduleType/moduleType##_if.h)
#define __LOGICAL_IF_HEADER(logicalType)             STRINGX(LogicalDrivers/logicalType/logicalType##_if.h)
#define __PERIPHERAL_IF_HEADER(peripheralType)       STRINGX(PeripheralDrivers/peripheralType/peripheralType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include module drivers interfaces headers from driver/interface/chip                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_IF_HEADER_FROM_DRV(moduleType)      STRINGX(../../../ModuleDrivers/moduleType/moduleType##_if.h)
#define __MODULE_IF_HEADER_FROM_IF(moduleType)       STRINGX(../../ModuleDrivers/moduleType/moduleType##_if.h)
#define __MODULE_IF_HEADER_FROM_CHIP_DRV(moduleType) STRINGX(../../ModuleDrivers/moduleType/moduleType##_if.h)
#define __MODULE_IF_HEADER_FROM_COMMON(moduleType)   STRINGX(../ModuleDrivers/moduleType/moduleType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include logical drivers interfaces headers from driver/interface                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define __LOGICAL_IF_HEADER_FROM_DRV(logicalType)    STRINGX(../../../LogicalDrivers/logicalType/logicalType##_if.h)
#define __LOGICAL_IF_HEADER_FROM_IF(logicalType)     STRINGX(../../LogicalDrivers/logicalType/logicalType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include Peripheral drivers interfaces headers from driver/interface                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define __PERIPHERAL_IF_HEADER_FROM_DRV(peripheralType)    STRINGX(../../../PeripheralDrivers/peripheralType/peripheralType##_if.h)
#define __PERIPHERAL_IF_HEADER_FROM_IF(peripheralType)     STRINGX(../../PeripheralDrivers/peripheralType/peripheralType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include chip header from driver/interface/chip                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define __CHIP_H_FROM_CHIP()                         STRINGX(../chip.h)
#define __CHIP_H_FROM_IF()                           STRINGX(../../Chips/chip.h)
#define __CHIP_H_FROM_DRV()                          STRINGX(../../../Chips/chip.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include drivers from interface                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_DRIVER(moduleType, moduleVer)       STRINGX(../moduleType/moduleVer/moduleType##_drv.c)
#define __MODULE_HEADER(moduleType, moduleVer)       STRINGX(../moduleType/moduleVer/moduleType##_drv.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include module drivers registers from hal_regs                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_REGS_HEADER(moduleType, moduleVer)  STRINGX(ModuleDrivers/moduleType/moduleVer/moduleType##_regs.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include chip files                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define __CHIP_H_FILE_NAME(name)                     STRINGX(name/name.h)
#define __CHIP_C_FILE_NAME(name)                     STRINGX(name/name.c)
#define __CHIP_IF_FILE_NAME(name)                    STRINGX(name/CONCAT2(name, _if.h))
#define __CHIP_REGS_FILE_NAME(name)                  STRINGX(name/CONCAT2(name, _regs.h))

/*---------------------------------------------------------------------------------------------------------*/
/* Include common files                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define __COMMON_HEADER(name)                        STRINGX(../Common/name)

#else

/*---------------------------------------------------------------------------------------------------------*/
/* Include drivers interfaces from hal                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_IF(moduleType)                      STRINGX(moduleType##_if.c)
#define __LOGICAL_IF(logicalType)                    STRINGX(logicalType##_if.c)
#define __PERIPHERAL_IF(peripheralType)              STRINGX(peripheralType##_if.c)

/*---------------------------------------------------------------------------------------------------------*/
/* Include drivers interfaces headers from hal                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_IF_HEADER(moduleType)               STRINGX(moduleType##_if.h)
#define __LOGICAL_IF_HEADER(logicalType)             STRINGX(logicalType##_if.h)
#define __PERIPHERAL_IF_HEADER(peripheralType)       STRINGX(peripheralType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include module drivers interfaces headers from driver/interface/chip                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_IF_HEADER_FROM_DRV(moduleType)      STRINGX(moduleType##_if.h)
#define __MODULE_IF_HEADER_FROM_IF(moduleType)       STRINGX(moduleType##_if.h)
#define __MODULE_IF_HEADER_FROM_CHIP_DRV(moduleType) STRINGX(moduleType##_if.h)
#define __MODULE_IF_HEADER_FROM_COMMON(moduleType)   STRINGX(moduleType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include logical drivers interfaces headers from driver/interface                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define __LOGICAL_IF_HEADER_FROM_DRV(logicalType)    STRINGX(logicalType##_if.h)
#define __LOGICAL_IF_HEADER_FROM_IF(logicalType)     STRINGX(logicalType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include Peripheral drivers interfaces headers from driver/interface                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define __PERIPHERAL_IF_HEADER_FROM_DRV(peripheralType)    STRINGX(peripheralType##_if.h)
#define __PERIPHERAL_IF_HEADER_FROM_IF(peripheralType)     STRINGX(peripheralType##_if.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include chip header from driver/interface/chip                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define __CHIP_H_FROM_CHIP()                         STRINGX(chip.h)
#define __CHIP_H_FROM_IF()                           STRINGX(chip.h)
#define __CHIP_H_FROM_DRV()                          STRINGX(chip.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include drivers from interface                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_DRIVER(moduleType, moduleVer)       STRINGX(moduleType##_drv.c)
#define __MODULE_HEADER(moduleType, moduleVer)       STRINGX(moduleType##_drv.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include module drivers registers from hal_regs                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define __MODULE_REGS_HEADER(moduleType, moduleVer)  STRINGX(moduleType##_regs.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Include chip files                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define __CHIP_H_FILE_NAME(name)                     STRINGX(name.h)
#define __CHIP_C_FILE_NAME(name)                     STRINGX(name.c)
#define __CHIP_IF_FILE_NAME(name)                    STRINGX(CONCAT2(name, _if.h))
#define __CHIP_REGS_FILE_NAME(name)                  STRINGX(CONCAT2(name, _regs.h))

/*---------------------------------------------------------------------------------------------------------*/
/* Include common files                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define __COMMON_HEADER(name)                        STRINGX(name)

#endif  /* SWC_HAL_FLAT */

#endif //__HAL_INTERNAL_H__

