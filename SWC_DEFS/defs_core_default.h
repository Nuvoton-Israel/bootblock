/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   defs_core_default.h                                                      */
/*            This file contains stub default definitions for core access     */
/* Project:                                                                   */
/*            SWC DEFS                                                        */
/*----------------------------------------------------------------------------*/

#ifndef _DEFS_DEFAULT_H_
#define _DEFS_DEFAULT_H_


/*---------------------------------------------------------------------------------------------------------*/
/* Default REG definitions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef REG8
#define REG8                volatile UINT8
#endif

#ifndef REG16
#define REG16               volatile UINT16
#endif

#ifndef REG32
#define REG32               volatile UINT32
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Default PTR definitions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef PTR8
#define PTR8                REG8 *
#endif

#ifndef PTR16
#define PTR16               REG16 *
#endif

#ifndef PTR32
#define PTR32               REG32*
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent MEM definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef MEMR8
#define MEMR8(a)            (*(PTR8) (a))
#endif

#ifndef MEMR16
#define MEMR16(a)           (*(PTR16)(a))
#endif

#ifndef MEMR32
#define MEMR32(a)           (*(PTR32)(a))
#endif

#ifndef MEMW8
#define MEMW8(a,v)          (*((PTR8)  (a))) = ((UINT8)(v))
#endif

#ifndef MEMW16
#define MEMW16(a,v)         (*((PTR16) (a))) = ((UINT16)(v))
#endif

#ifndef MEMW32
#define MEMW32(a,v)         (*((PTR32) (a))) = ((UINT32)(v))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent IO definitions                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef IOR8
#define IOR8(a)             MEMR8(a)
#endif

#ifndef IOR16
#define IOR16(a)            MEMR16(a)
#endif

#ifndef IOR32
#define IOR32(a)            MEMR32(a)
#endif

#ifndef IOW8
#define IOW8(a,v)           MEMW8(a, v)
#endif

#ifndef IOW16
#define IOW16(a,v)          MEMW16(a, v)
#endif

#ifndef IOW32
#define IOW32(a,v)          MEMW32(a, v)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupts macros                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ENABLE_INTERRUPTS
#define ENABLE_INTERRUPTS()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Disable interrupts                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DISABLE_INTERRUPTS
#define DISABLE_INTERRUPTS()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Preserve interrupt state and disable                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef INTERRUPTS_SAVE_DISABLE
#define INTERRUPTS_SAVE_DISABLE(var)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Restore saved interrupt state - E bit only                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef INTERRUPTS_RESTORE
#define INTERRUPTS_RESTORE(var)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Set vector table pointer                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef INTERRUPTS_SET_VECTOR_TABLE
#define INTERRUPTS_SET_VECTOR_TABLE(table)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ICACHE_SAVE_DISABLE
#define ICACHE_SAVE_DISABLE(var)
#endif

#ifndef ICACHE_SAVE_ENABLE
#define ICACHE_SAVE_ENABLE(var)
#endif

#ifndef ICACHE_RESTORE
#define ICACHE_RESTORE(var)
#endif

#ifndef ICACHE_INVALIDATE
#define ICACHE_INVALIDATE()
#endif

#ifndef DCACHE_SAVE_DISABLE
#define DCACHE_SAVE_DISABLE(var)
#endif

#ifndef DCACHE_SAVE_ENABLE
#define DCACHE_SAVE_ENABLE(var)
#endif

#ifndef DCACHE_RESTORE
#define DCACHE_RESTORE(var)
#endif

#ifndef DCACHE_INVALIDATE
#define DCACHE_INVALIDATE()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Call a function at addr                                                                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef FUNC_CALL
#define FUNC_CALL(addr)     (((void (*)(void))(addr))())
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CPU power management                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef CPU_IDLE
#define CPU_IDLE()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Assertion macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ASSERT
#ifndef ASSERT
#define ASSERT(cond)
#endif
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*                                   CORE AND COMPILER SPECIFIC KEYWORDS                                   */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _MEM_TYPE_BIT_
#define _MEM_TYPE_BIT_
#endif

#ifndef _MEM_TYPE_FAST_
#define _MEM_TYPE_FAST_
#endif

#ifndef _MEM_TYPE_MEDFAST_
#define _MEM_TYPE_MEDFAST_
#endif

#ifndef _MEM_TYPE_MEDSLOW_
#define _MEM_TYPE_MEDSLOW_
#endif

#ifndef _MEM_TYPE_SLOW_
#define _MEM_TYPE_SLOW_
#endif

#ifndef _CONST_TYPE_
#define _CONST_TYPE_            const
#endif

#ifndef _INLINE_
#define _INLINE_                inline
#endif

#ifndef _ALIGN_
#define _ALIGN_(x, decl)        decl
#endif

#ifndef _PACK_
#define _PACK_(decl)            decl
#endif

#ifndef _ZI_
#define _ZI_(decl)              decl
#endif

#ifndef _SECTION_
#define _SECTION_(secname,decl) decl
#endif

#ifndef _AT_
#define _AT_(addr, decl)        decl
#endif

#ifndef _NORET_
#define _NORET_(decl)           decl
#endif

#ifndef _NORET_SPEC_
#define _NORET_SPEC_(decl)      decl
#endif

#endif /*_DEFS_DEFAULT_H_ */
