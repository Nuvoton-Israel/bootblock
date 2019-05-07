/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   hal_interrupt.h                                                          */
/*            This file contains Interrupt Provider abstraction to HAL        */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "hal_interrupt.h"


#if defined INTERRUPT_PROVIDER_DYNAMIC

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                      FORWARD FUNCTION DECLARATION                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ICU_MODULE_TYPE
void    _ICU_SET_POLARITY(UINT16 intNum, INTERRUPT_POLARITY_TYPE polarity);
void    _ICU_SET_PRIORITY(UINT16 intNum, UINT8 priority);
#endif

#ifdef MIWU_MODULE_TYPE
void    _MIWU_SET_PRIORITY(UINT16 intNum, UINT8 priority);
#endif

#ifdef NVIC_MODULE_TYPE
void    _NVIC_SET_POLARITY(UINT16 intNum, INTERRUPT_POLARITY_TYPE polarity);
#endif

#ifdef GIC_MODULE_TYPE
void    _GIC_SET_POLARITY(UINT16 intNum, INTERRUPT_POLARITY_TYPE polarity);
void    _GIC_SET_PRIORITY(UINT16 intNum, UINT8 priority);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  CONST                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
const INTERRUPT_PROVIDER_STRUCT INTERRUPT_PROVIDER[INTERRUPT_PROVIDER_LAST] =
{
#ifdef ICU_MODULE_TYPE
    {
        (INTERRUPT_ENABLE_T)ICU_EnableInt,
        (INTERRUPT_REGISTER_HANDLER_T)ICU_InstallSwHandler,
        (INTERRUPT_CLEAR_T)ICU_ClearInt,
        (INTERRUPT_PENDING_T)ICU_PendingInt,
        _ICU_SET_POLARITY,
        (INTERRUPT_ENABLED_T)ICU_IntEnabled,
        _ICU_SET_PRIORITY,
    },
#endif
#ifdef MIWU_MODULE_TYPE
    {
        (INTERRUPT_ENABLE_T)MIWU_EnableChannel,
        (INTERRUPT_REGISTER_HANDLER_T)MIWU_InstallHandler,
        (INTERRUPT_CLEAR_T)MIWU_ClearChannel,
        (INTERRUPT_PENDING_T)MIWU_PendingChannel,
        (INTERRUPT_SET_POLARITY_T)MIWU_ConfigEdgeChannel,
        (INTERRUPT_ENABLED_T)MIWU_ChannelEnabled,
        _MIWU_SET_PRIORITY,
    },
#endif
#ifdef NVIC_MODULE_TYPE
        (INTERRUPT_ENABLE_T)NVIC_EnableInt,
        (INTERRUPT_REGISTER_HANDLER_T)NVIC_InstallSwHandler,
        (INTERRUPT_CLEAR_T)NVIC_ClearInt,,
        (INTERRUPT_PENDING_T)NVIC_PendingInt,
        _NVIC_SET_POLARITY,
        (INTERRUPT_ENABLED_T)NVIC_IntEnabled,
        (INTERRUPT_SET_PRIORITY_T)NVIC_ConfigPriority,
    },
#endif
#ifdef GIC_MODULE_TYPE
        (INTERRUPT_ENABLE_T)GIC_EnableInt,
        (INTERRUPT_REGISTER_HANDLER_T)GIC_InstallHandler,
        (INTERRUPT_CLEAR_T)GIC_ClearInt,,
        (INTERRUPT_PENDING_T)GIC_PendingInt,
        _GIC_SET_POLARITY,
        (INTERRUPT_ENABLED_T)GIC_IntEnabled,
        (INTERRUPT_SET_PRIORITY_T)GIC_ConfigPriority,
    },
#endif

};



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                         FUNCTION IMPLEMENTATION                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#ifdef ICU_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        _ICU_SET_POLARITY                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intNum      - Interrupt number                                                         */
/*                  polarity    - Interrupt polarity                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets ICU interrupt polarity                                               */
/*lint -e{715}      Suppress 'intNum'/'polarity' not referenced                                            */
/*---------------------------------------------------------------------------------------------------------*/
void _ICU_SET_POLARITY(UINT16 intNum, INTERRUPT_POLARITY_TYPE polarity)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* ICU contain hard-coded polarities, nothing to do                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        _MIWU_SET_PRIORITY                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intNum      - Interrupt number                                                         */
/*                  priority    - Interrupt priority                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets ICU interrupt priority                                               */
/*lint -e{715}      Suppress 'intNum'/'priority' not referenced                                            */
/*---------------------------------------------------------------------------------------------------------*/
void _ICU_SET_PRIORITY(UINT16 intNum, UINT8 priority)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* ICU does not contains priorities, nothing to do                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
}
#endif

#ifdef MIWU_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        _MIWU_SET_PRIORITY                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intNum      - Interrupt number                                                         */
/*                  priority    - Interrupt priority                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets MIWU interrupt priority                                              */
/*lint -e{715}      Suppress 'intNum'/'priority' not referenced                                            */
/*---------------------------------------------------------------------------------------------------------*/
void _MIWU_SET_PRIORITY(UINT16 intNum, UINT8 priority)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* MIWU does not contains priorities, nothing to do                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
}
#endif

#ifdef NVIC_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        _NVIC_SET_POLARITY                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intNum      - Interrupt number                                                         */
/*                  polarity    - Interrupt polarity                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine clears NVIC interrupt number int_no                                       */
/*lint -e{715}      Suppress 'intNum'/'polarity' not referenced                                            */
/*---------------------------------------------------------------------------------------------------------*/
void _NVIC_SET_POLARITY(UINT16 intNum, INTERRUPT_POLARITY_TYPE polarity)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* NVIC contain hard-coded polarities, nothing to do                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
}
#endif



#ifdef GIC_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        _GIC_SET_POLARITY                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intNum      - Interrupt number                                                         */
/*                  polarity    - Interrupt polarity                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine clears GIC interrupt number int_no                                        */
/*lint -e{715}      Suppress 'intNum'/'polarity' not referenced                                            */
/*---------------------------------------------------------------------------------------------------------*/
void _GIC_SET_POLARITY(UINT16 intNum, INTERRUPT_POLARITY_TYPE polarity)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* GIC contain hard-coded polarities, nothing to do                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        _GIC_SET_PRIORITY                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intNum      - Interrupt number                                                         */
/*                  priority    - Interrupt priority                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets GIC interrupt priority                                               */
/*lint -e{715}      Suppress 'intNum'/'priority' not referenced                                            */
/*---------------------------------------------------------------------------------------------------------*/
void _GIC_SET_PRIORITY(UINT16 intNum, UINT8 priority)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* GIC does not contains priorities, nothing to do                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
}
#endif


#endif

