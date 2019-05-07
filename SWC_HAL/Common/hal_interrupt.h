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

#ifndef __HAL_INTERRUPT_ABSTRACTION_H__
#define __HAL_INTERRUPT_ABSTRACTION_H__

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#if defined ICU_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_COMMON(icu)
#endif

#if defined MIWU_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_COMMON(miwu)
#endif

#if defined NVIC_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_COMMON(nvic)
#endif

#if defined GIC_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_COMMON(gic)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt provider types                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
#if defined ICU_MODULE_TYPE
    INTERRUPT_PROVIDER_ICU,
#endif

#if defined MIWU_MODULE_TYPE
    INTERRUPT_PROVIDER_MIWU,
#endif

#if defined NVIC_MODULE_TYPE
    INTERRUPT_PROVIDER_NVIC,
#endif

#if defined GIC_MODULE_TYPE
    INTERRUPT_PROVIDER_GIC,
#endif


    INTERRUPT_PROVIDER_LAST
} INTERRUPT_PROVIDER_TYPE;


/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler type                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*INTERRUPT_HANDLER)(UINT16 intNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt polarity types                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    INTERRUPT_POLARITY_RISING_EDGE  = 0,
    INTERRUPT_POLARITY_FALLING_EDGE,
    INTERRUPT_POLARITY_BOTH_EDGES,
    INTERRUPT_POLARITY_LEVEL_HIGH,
    INTERRUPT_POLARITY_LEVEL_LOW
} INTERRUPT_POLARITY_TYPE;

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt callbacks types                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef void        (*INTERRUPT_ENABLE_T)               (UINT16 intNum, BOOLEAN enable);
typedef void        (*INTERRUPT_REGISTER_HANDLER_T)     (UINT16 intNum, INTERRUPT_HANDLER handler);
typedef void        (*INTERRUPT_CLEAR_T)                (UINT16 intNum);
typedef BOOLEAN     (*INTERRUPT_PENDING_T)              (UINT16 intNum);
typedef void        (*INTERRUPT_SET_POLARITY_T)         (UINT16 intNum, INTERRUPT_POLARITY_TYPE polarity);
typedef BOOLEAN     (*INTERRUPT_ENABLED_T)              (UINT16 intNum);
typedef void        (*INTERRUPT_SET_PRIORITY_T)         (UINT16 intNum, UINT8 priority);

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt provider type                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    INTERRUPT_ENABLE_T              EnableInt;
    INTERRUPT_REGISTER_HANDLER_T    RegisterHandler;
    INTERRUPT_CLEAR_T               ClearInt;
    INTERRUPT_PENDING_T             PendingInt;
    INTERRUPT_SET_POLARITY_T        SetPolarity;
    INTERRUPT_ENABLED_T             InterruptEnabled;
    INTERRUPT_SET_PRIORITY_T        SetPriority;
} INTERRUPT_PROVIDER_STRUCT;





/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  CONST                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#if defined INTERRUPT_PROVIDER_DYNAMIC
extern const INTERRUPT_PROVIDER_STRUCT INTERRUPT_PROVIDER[INTERRUPT_PROVIDER_LAST];
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               DEFINITIONS                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Common macros                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define INTERRUPT_REGISTER_AND_ENABLE(provider, int, handler, polarity, priority)   \
{                                                                                   \
        INTERRUPT_ENABLE(provider, int, FALSE);                                     \
        INTERRUPT_REGISTER_HANDLER(provider, int, handler);                         \
        INTERRUPT_SET_POLARITY(provider, int, polarity);                            \
        INTERRUPT_SET_PRIORITY(provider, int, priority);                            \
        INTERRUPT_CLEAR(provider, int);                                             \
        INTERRUPT_ENABLE(provider, int, TRUE);                                      \
}

#define INTERRUPT_SAVE_DISABLE(provider, int, var)                                  \
{                                                                                   \
    var = INTERRUPT_ENABLED(provider, int);                                         \
    INTERRUPT_ENABLE(provider, int, FALSE);                                         \
}

#define INTERRUPT_RESTORE(provider, int, var)                                       \
{                                                                                   \
    INTERRUPT_ENABLE(provider, int, (BOOLEAN)var);                                  \
}

#if defined INTERRUPT_PROVIDER_DYNAMIC

/*---------------------------------------------------------------------------------------------------------*/
/*                                       Dynamic Interrupt Provider                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define INTERRUPT_ENABLE(provider, int, enable)                     INTERRUPT_PROVIDER[provider].EnableInt(int, enable)
#define INTERRUPT_REGISTER_HANDLER(provider, int, handler)          INTERRUPT_PROVIDER[provider].RegisterHandler(int, handler)
#define INTERRUPT_CLEAR(provider, int)                              INTERRUPT_PROVIDER[provider].ClearInt(int)
#define INTERRUPT_PENDING(provider, int)                            INTERRUPT_PROVIDER[provider].PendingInt(int)
#define INTERRUPT_SET_POLARITY(provider, int, polarity)             INTERRUPT_PROVIDER[provider].SetPolarity(int, polarity)
#define INTERRUPT_ENABLED(provider, int)                            INTERRUPT_PROVIDER[provider].InterruptEnabled(int)
#define INTERRUPT_SET_PRIORITY(provider, int, priority)             INTERRUPT_PROVIDER[provider].SetPriority(int, priority)

#else

/*---------------------------------------------------------------------------------------------------------*/
/*                                        Static Interrupt Provider                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Interface Interrupt Abstraction Macros                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define INTERRUPT_ENABLE(provider, int, enable)                     CONCAT2(provider,_ENABLE(int, enable))
#define INTERRUPT_REGISTER_HANDLER(provider, int, handler)          CONCAT2(provider,_REGISTER_HANDLER(int, handler))
#define INTERRUPT_CLEAR(provider, int)                              CONCAT2(provider,_CLEAR(int))
#define INTERRUPT_PENDING(provider, int)                            CONCAT2(provider,_PENDING(int))
#define INTERRUPT_SET_POLARITY(provider, int, polarity)             CONCAT2(provider,_SET_POLARITY(int, polarity))
#define INTERRUPT_ENABLED(provider, int)                            CONCAT2(provider,_ENABLED(int))
#define INTERRUPT_SET_PRIORITY(provider, int, priority)             CONCAT2(provider,_SET_PRIORITY(int, priority))

/*---------------------------------------------------------------------------------------------------------*/
/* ICU Interrupt support                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ICU_MODULE_TYPE
#define INTERRUPT_PROVIDER_ICU_ENABLE(int, enable)                ICU_EnableInt       ((ICU_INT_SRC_T)int, enable)
#define INTERRUPT_PROVIDER_ICU_REGISTER_HANDLER(int, handler)     ICU_InstallSwHandler((ICU_INT_SRC_T)int, (SW_HANDLER_T)handler)
#define INTERRUPT_PROVIDER_ICU_CLEAR(int)                         ICU_ClearInt        ((ICU_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_ICU_PENDING(int)                       ICU_PendingInt      ((ICU_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_ICU_SET_POLARITY(int, polarity)
#define INTERRUPT_PROVIDER_ICU_ENABLED(int)                       ICU_IntEnabled      ((ICU_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_ICU_SET_PRIORITY(int, priority)
#else
#define INTERRUPT_PROVIDER_ICU_ENABLE(int, enable)
#define INTERRUPT_PROVIDER_ICU_REGISTER_HANDLER(int, handler)
#define INTERRUPT_PROVIDER_ICU_CLEAR(int)
#define INTERRUPT_PROVIDER_ICU_PENDING(int)
#define INTERRUPT_PROVIDER_ICU_SET_POLARITY(int, polarity)
#define INTERRUPT_PROVIDER_ICU_ENABLED(int)
#define INTERRUPT_PROVIDER_ICU_SET_PRIORITY(int, priority)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* MIWU Interrupt support                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef MIWU_MODULE_TYPE
#define INTERRUPT_PROVIDER_MIWU_ENABLE(int, enable)               MIWU_EnableChannel    ((MIWU_SRC_T)int, enable)
#define INTERRUPT_PROVIDER_MIWU_REGISTER_HANDLER(int, handler)    MIWU_InstallHandler   ((MIWU_SRC_T)int, (MIWU_HANDLER_T)handler)
#define INTERRUPT_PROVIDER_MIWU_CLEAR(int)                        MIWU_ClearChannel     ((MIWU_SRC_T)int)
#define INTERRUPT_PROVIDER_MIWU_PENDING(int)                      MIWU_PendingChannel   ((MIWU_SRC_T)int)
#define INTERRUPT_PROVIDER_MIWU_SET_POLARITY(int, polarity)       MIWU_ConfigEdgeChannel((MIWU_SRC_T)int, (MIWU_EDGE_T)polarity)
#define INTERRUPT_PROVIDER_MIWU_ENABLED(int)                      MIWU_ChannelEnabled   ((MIWU_SRC_T)int)
#define INTERRUPT_PROVIDER_MIWU_SET_PRIORITY(int, priority)
#else
#define INTERRUPT_PROVIDER_MIWU_ENABLE(int, enable)
#define INTERRUPT_PROVIDER_MIWU_REGISTER_HANDLER(int, handler)
#define INTERRUPT_PROVIDER_MIWU_CLEAR(int)
#define INTERRUPT_PROVIDER_MIWU_PENDING(int)
#define INTERRUPT_PROVIDER_MIWU_SET_POLARITY(int, polarity)
#define INTERRUPT_PROVIDER_MIWU_ENABLED(int)
#define INTERRUPT_PROVIDER_MIWU_SET_PRIORITY(int, priority)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* NVIC Interrupt support                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef NVIC_MODULE_TYPE
#define INTERRUPT_PROVIDER_NVIC_ENABLE(int, enable)               NVIC_EnableInt       ((NVIC_INT_SRC_T)int, enable)
#define INTERRUPT_PROVIDER_NVIC_REGISTER_HANDLER(int, handler)    NVIC_InstallSwHandler((NVIC_INT_SRC_T)int, (SW_HANDLER_T)handler)
#define INTERRUPT_PROVIDER_NVIC_CLEAR(int)                        NVIC_ClearInt        ((NVIC_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_NVIC_PENDING(int)                      NVIC_PendingInt      ((NVIC_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_NVIC_SET_POLARITY(int, polarity)
#define INTERRUPT_PROVIDER_NVIC_ENABLED(int)                      NVIC_IntEnabled      ((NVIC_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_NVIC_SET_PRIORITY(int, priority)       NVIC_ConfigPriority  ((NVIC_INT_SRC_T)int, priority)
#else
#define INTERRUPT_PROVIDER_NVIC_ENABLE(int, enable)
#define INTERRUPT_PROVIDER_NVIC_REGISTER_HANDLER(int, handler)
#define INTERRUPT_PROVIDER_NVIC_CLEAR(int)
#define INTERRUPT_PROVIDER_NVIC_PENDING(int)
#define INTERRUPT_PROVIDER_NVIC_SET_POLARITY(int, polarity)
#define INTERRUPT_PROVIDER_NVIC_ENABLED(int)
#define INTERRUPT_PROVIDER_NVIC_SET_PRIORITY(int, priority)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GIC Interrupt support                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef GIC_MODULE_TYPE
#define INTERRUPT_PROVIDER_GIC_ENABLE(int, enable)               GIC_EnableInt        ((GIC_INT_SRC_T)int, enable)
#define INTERRUPT_PROVIDER_GIC_REGISTER_HANDLER(int, handler)    GIC_InstallHandler   ((GIC_INT_SRC_T)int, (HANDLER_T)handler)
#define INTERRUPT_PROVIDER_GIC_CLEAR(int)                        GIC_ClearInt         ((GIC_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_GIC_PENDING(int)                      (void)0     /*GIC_PendingInt       ((GIC_INT_SRC_T)int)*/
#define INTERRUPT_PROVIDER_GIC_SET_POLARITY(int, polarity)       (void)0     /*((GIC_INT_SRC_T)int, polarity)*/
#define INTERRUPT_PROVIDER_GIC_ENABLED(int)                      GIC_IntEnabled       ((GIC_INT_SRC_T)int)
#define INTERRUPT_PROVIDER_GIC_SET_PRIORITY(int, priority)       GIC_ConfigPriority   ((GIC_INT_SRC_T)int, priority)
#else
#define INTERRUPT_PROVIDER_GIC_ENABLE(int, enable)
#define INTERRUPT_PROVIDER_GIC_REGISTER_HANDLER(int, handler)
#define INTERRUPT_PROVIDER_GIC_CLEAR(int)
#define INTERRUPT_PROVIDER_GIC_PENDING(int)
#define INTERRUPT_PROVIDER_GIC_SET_POLARITY(int, polarity)
#define INTERRUPT_PROVIDER_GIC_ENABLED(int)
#define INTERRUPT_PROVIDER_GIC_SET_PRIORITY(int, priority)
#endif

#endif

#endif //__HAL_INTERRUPT_ABSTRACTION_H__

