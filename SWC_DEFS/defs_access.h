/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   defs_access.h                                                            */
/* This file contains NTIL hardware registers access and field manipulation macros */
/* Project:                                                                   */
/*            SWC DEFS                                                        */
/*----------------------------------------------------------------------------*/

#ifndef __ACCESS_H__
#define __ACCESS_H__

#include "defs_internal.h"

/*---------------------------------------------------------------------------------------------------------*/
/*                      REGISTERS AND VARIABLES ACCESS, BIT AND FIELD OPERATION MACROS                     */
/*---------------------------------------------------------------------------------------------------------*/
/* Generic registers access macros assuming registers database defined as follows: (see Coding Standards   */
/* document Chapter 12 for detailed description)                                                           */
/*                                                                                                         */
/* Module defintition:                                                                                     */
/* ------------------                                                                                      */
/*                                                                                                         */
/* #define <MODULE>_BASE_ADDR          (<module_base_addr> )             these definition should be kept   */
/*                                                                       in  a <chip>.h file, since it is  */
/*                                                                       chip specific                     */
/*                                                                                                         */
/* #define <MODULE>_ACCESS             (<module_access_type> )           same name as access macros        */
/*                                                                       name,i.e MEM, IO, INDEXDATA, etc. */
/*                                                                       The rest of the definitions       */
/*                                                                       should be kept in a <hw_module>.h */
/*                                                                       file since they are module        */
/*                                                                       specific                          */
/*                                                                                                         */
/* Regsiter defintition:                                                                                   */
/* --------------------                                                                                    */
/*                                                                                                         */
/* #define <MY_REG_1>              (<MODULE>_BASE_ADDR + <offset>), <ACCESS TYPE>, <8/16/32>               */
/*                                                                                                         */
/* Fields defintition:                                                                                     */
/* -------------------                                                                                     */
/*                                                                                                         */
/* #define <MY_REG_1_FIELD_1>   <field 1 position>, <field 1 size>                                         */
/* #define <MY_REG_1_FIELD_2>   <field 2 position>, <field 2 size>                                         */
/* ...                                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* When there is a definition of registers as described above, manipulations on registers and fields can   */
/* be done in the following ways:                                                                          */
/*                                                                                                         */
/* 1. The registers can be accessed directly using the following macros:                                   */
/*                                                                                                         */
/*          REG_READ(reg)                                                                                  */
/*          REG_WRITE(reg,val)                                                                             */
/*                                                                                                         */
/* 2. Registers field can be accessed using the following macros, whose internal implementation access the */
/*    register using REG_READ and REG_WRITE macors (implicit read-modify-write)                            */
/*                                                                                                         */
/*                                                                                                         */
/*          READ_REG_FIELD(reg, field)                                                                     */
/*          SET_REG_FIELD(reg, field, value)                                                               */
/*                                                                                                         */
/*                                                                                                         */
/* 3. For registers field manipulation via explicit read-modify-write procedure on a variable, use the     */
/*    following macros:                                                                                    */
/*                                                                                                         */
/*          READ_VAR_FIELD(var, field)                                                                     */
/*          SET_VAR_FIELD(var, field, value)                                                               */
/*                                                                                                         */
/*                                                                                                         */
/* 4. For the cases when the fields are single bits and are stored in a variable rather than in a constant,*/
/*    the following sets of macros can be used:                                                            */
/*                                                                                                         */
/*          SET_REG_BIT(reg, bit_no)                                                                       */
/*          READ_REG_BIT(reg, bit_no)                                                                      */
/*          CLEAR_REG_BIT(reg, bit_no)                                                                     */
/*                                                                                                         */
/*          SET_VAR_BIT(var, bit_no)                                                                       */
/*          READ_VAR_BIT(var, bit_no)                                                                      */
/*          CLEAR_VAR_BIT(var, bit_no                                                                      */
/*                                                                                                         */
/* 5. For the cases where masks that should be written to the registers are variables, one may use the     */
/*    the following macros. These macros should be used rarely, only in the cases where the usual method   */
/*    of fields defitinions can not be used:                                                               */
/*                                                                                                         */
/*          SET_VAR_MASK(var, bit_mask)                                                                    */
/*          READ_VAR_MASK(var, bit_mask)                                                                   */
/*          CLEAR_VAR_MASK(var, bit_mask)                                                                  */
/*                                                                                                         */
/*          SET_REG_MASK(reg, bit_mask)                                                                    */
/*          READ_REG_MASK(reg, bit_mask)                                                                   */
/*          CLEAR_REG_MASK(reg, bit_mask)                                                                  */
/*                                                                                                         */
/*                                                                                                         */
/* 6. Sometimes it is required to just get a mask (filled with 1b) that fits a certain field without       */
/*    writing a value to it. For this purpose use the macro                                                */
/*                                                                                                         */
/*          MASK_FIELD(field)                                                                              */
/*                                                                                                         */
/* 7. For building a value of a mask without writing to actual register or value, use:                     */
/*                                                                                                         */
/*          BUILD_FIELD_VAL(field, value)                                                                  */
/*                                                                                                         */
/*    This macro is useful when several fields of register/variable are required to be set at a time.      */
/*    In this case, an ORed chain of BUILD_FIELD_VAL invocations will be written to the                    */
/*    register/variable                                                                                    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Direct register access                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define      REG_READ(reg)                                                                                 \
            _REG_READ_AUX_(_GET_REG_ADDR(reg), _GET_REG_ACCESSTYPE(reg), _GET_REG_SIZE(reg))

#define      REG_WRITE(reg,val)                                                                            \
            _REG_WRITE_AUX_((val),_GET_REG_ADDR(reg),_GET_REG_ACCESSTYPE(reg), _GET_REG_SIZE(reg))


/*---------------------------------------------------------------------------------------------------------*/
/* Get field of a register / variable                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_REG_FIELD(reg, field)                                                                         \
    _READ_FIELD(REG_READ(_GET_REG(reg)), _GET_REG_SIZE(reg), _GET_FIELD_SIZE(field), _GET_FIELD_POS(field))

#ifdef CPU_64_BIT
#define READ_VAR_FIELD(var, field)                                                                         \
   ((sizeof(var) == 8) ? (_READ_FIELD(var, 64, _GET_FIELD_SIZE(field), _GET_FIELD_POS(field))) :           \
   ((sizeof(var) == 4) ? (_READ_FIELD(var, 32, _GET_FIELD_SIZE(field), _GET_FIELD_POS(field))) :           \
   ((sizeof(var) == 2) ? (_READ_FIELD(var, 16, _GET_FIELD_SIZE(field), _GET_FIELD_POS(field))) :           \
                         (_READ_FIELD(var,  8, _GET_FIELD_SIZE(field), _GET_FIELD_POS(field))))))
#else
#define READ_VAR_FIELD(var, field)                                                                         \
   ((sizeof(var) == 4) ? (_READ_FIELD(var, 32, _GET_FIELD_SIZE(field), _GET_FIELD_POS(field))) :           \
   ((sizeof(var) == 2) ? (_READ_FIELD(var, 16, _GET_FIELD_SIZE(field), _GET_FIELD_POS(field))) :           \
                         (_READ_FIELD(var,  8, _GET_FIELD_SIZE(field), _GET_FIELD_POS(field)))))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Set field of a register / variable                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_REG_FIELD(reg, field, value)                                                                   \
    _SET_REG_FIELD_SIZE(_GET_REG(reg),_GET_FIELD_SIZE(field),_GET_FIELD_POS(field),(value))


#define SET_VAR_FIELD(var, field, value)                                                                   \
    _SET_VAR_FIELD(var,_GET_FIELD_SIZE(field), _GET_FIELD_POS(field), (value))


/*---------------------------------------------------------------------------------------------------------*/
/* Variable bit operation macros                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef CPU_64_BIT
#define SET_VAR_BIT(var, nb)                                                                               \
   ((sizeof(var) == 8) ? ((var) |= ((UINT64)0x1<<(nb))):                                                   \
   ((sizeof(var) == 4) ? ((var) |= ((UINT32)0x1<<(nb))):                                                   \
   ((sizeof(var) == 2) ? ((var) |= ((UINT16)0x1<<(nb))):                                                   \
                         ((var) |= (        0x1<<(nb))))))
#else
#define SET_VAR_BIT(var, nb)                                                                               \
   ((sizeof(var) == 4) ? ((var) |= ((UINT32)0x1<<(nb))):                                                   \
   ((sizeof(var) == 2) ? ((var) |= ((UINT16)0x1<<(nb))):                                                   \
                         ((var) |= (        0x1<<(nb)))))

#endif

#define READ_VAR_BIT(var, nb)      (((var) >> (nb)) & 0x1)

#ifdef CPU_64_BIT
#define CLEAR_VAR_BIT(var, nb)                                                                             \
    ((sizeof(var) == 8) ? ((var) &= (~((UINT64)0x1<<(nb)))):                                               \
    ((sizeof(var) == 4) ? ((var) &= (~((UINT32)0x1<<(nb)))):                                               \
    ((sizeof(var) == 2) ? ((var) &= (~((UINT16)0x1<<(nb)))):                                               \
                          ((var) &= (~(        0x1<<(nb)))))))
#else
#define CLEAR_VAR_BIT(var, nb)                                                                             \
    ((sizeof(var) == 4) ? ((var) &= (~((UINT32)0x1<<(nb)))):                                               \
    ((sizeof(var) == 2) ? ((var) &= (~((UINT16)0x1<<(nb)))):                                               \
                          ((var) &= (~(        0x1<<(nb))))))
#endif
						  

#define MASK_BIT(nb)               (1UL<<(nb))

/*---------------------------------------------------------------------------------------------------------*/
/* Register bit operation macros                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_REG_BIT(reg, nb)                                                                               \
    REG_WRITE(_GET_REG(reg),((REG_READ(_GET_REG(reg))) | ((_CASTING_FROM(_GET_REG_SIZE(reg)))0x1<<(nb))))

#define READ_REG_BIT(reg, nb)                                                                              \
    (((REG_READ(_GET_REG(reg))>>(nb)) & ((_CASTING_FROM(_GET_REG_SIZE(reg)))0x1)))

#define CLEAR_REG_BIT(reg, nb)                                                                             \
    REG_WRITE(_GET_REG(reg),((REG_READ(_GET_REG(reg))) & (~((_CASTING_FROM(_GET_REG_SIZE(reg)))0x1<<(nb)))))

/*---------------------------------------------------------------------------------------------------------*/
/* Variable mask operation macros                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_VAR_MASK(var, bit_mask)       ((var) |= (bit_mask))

#define READ_VAR_MASK(var, bit_mask)      ((var) &  (bit_mask))

#define CLEAR_VAR_MASK(var, bit_mask)     ((var) &= (~(bit_mask)))

/*---------------------------------------------------------------------------------------------------------*/
/* Register mask operation macros                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_REG_MASK(reg, bit_mask)                                                                        \
    REG_WRITE(_GET_REG(reg),((REG_READ(_GET_REG(reg))) | (bit_mask)))

#define READ_REG_MASK(reg, bit_mask)                                                                       \
    (REG_READ(_GET_REG(reg)) & (bit_mask))

#define CLEAR_REG_MASK(reg, bit_mask)                                                                      \
    REG_WRITE(_GET_REG(reg),((REG_READ(_GET_REG(reg))) & (~(bit_mask))))

/*---------------------------------------------------------------------------------------------------------*/
/* Build a mask of a register / variable field                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define MASK_FIELD(field)                                                                                  \
    _MASK_FIELD(_GET_FIELD_SIZE(field), _GET_FIELD_POS(field))

/*---------------------------------------------------------------------------------------------------------*/
/* Expand the value of the given field into its correct position                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define BUILD_FIELD_VAL(field, value)                                                                      \
    _BUILD_FIELD_VAL(_GET_FIELD_SIZE(field), _GET_FIELD_POS(field), (value))


/*---------------------------------------------------------------------------------------------------------*/
/* Getting info about registers and fields                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define  REG_ADDR(reg)                _GET_REG_ADDR(reg)
#define  GET_FIELD(pos,size)           pos,size

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                SFR ACCESS,BIT AND FIELD OPERATION MACROS                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* SFR Handling.                                                                                           */
/* KEIL and COP8 handle SFRs as VARs, other tools may handle them as REGs                                  */
/*---------------------------------------------------------------------------------------------------------*/

#if (defined (__C51__) && defined (__KEIL__)) || defined (__COP8_IAR__)     /* KEIL / COP8                 */

/*---------------------------------------------------------------------------------------------------------*/
/* Direct SFR access                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define SFR_READ(sfr)                           (sfr)
#define SFR_WRITE(sfr, val)                      sfr = (val)

/*---------------------------------------------------------------------------------------------------------*/
/* SFR fields manipulation                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_SFR_FIELD(sfr, field)              READ_VAR_FIELD(sfr, _GET_FIELD(field))
#define SET_SFR_FIELD(sfr, field, value)        SET_VAR_FIELD(sfr, _GET_FIELD(field), (value))

/*---------------------------------------------------------------------------------------------------------*/
/* SFR non-constant bit operation macros                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_SFR_BIT(sfr, nb)                    SET_VAR_BIT(sfr, nb)
#define READ_SFR_BIT(sfr, nb)                   READ_VAR_BIT(sfr, nb)
#define CLEAR_SFR_BIT(sfr, nb)                  CLEAR_VAR_BIT(sfr, nb)

/*---------------------------------------------------------------------------------------------------------*/
/* SFR mask operation macros                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_SFR_MASK(sfr, bit_mask)             SET_VAR_MASK(sfr, bit_mask)
#define READ_SFR_MASK(sfr, bit_mask)            READ_VAR_MASK(sfr, bit_mask)
#define CLEAR_SFR_MASK(sfr, bit_mask)           CLEAR_VAR_MASK(sfr, bit_mask)

#else
/*---------------------------------------------------------------------------------------------------------*/
/* Direct SFR access                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define SFR_READ(sfr)                           REG_READ(_GET_REG(sfr))
#define SFR_WRITE(sfr, val)                     REG_WRITE(_GET_REG(sfr), (val))

/*---------------------------------------------------------------------------------------------------------*/
/* SFR fields manipulation                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_SFR_FIELD(sfr, field)              READ_REG_FIELD(_GET_REG(sfr), _GET_FIELD(field))
#define SET_SFR_FIELD(sfr, field, value)        SET_REG_FIELD(_GET_REG(sfr), _GET_FIELD(field), (value))

/*---------------------------------------------------------------------------------------------------------*/
/* SFR non-constant bit operation macros                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_SFR_BIT(sfr, nb)                    SET_REG_BIT(_GET_REG(sfr), nb)
#define READ_SFR_BIT(sfr, nb)                   READ_REG_BIT(_GET_REG(sfr), nb)
#define CLEAR_SFR_BIT(sfr, nb)                  CLEAR_REG_BIT(_GET_REG(sfr), nb)

/*---------------------------------------------------------------------------------------------------------*/
/* SFR mask operation macros                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_SFR_MASK(sfr, bit_mask)             SET_REG_MASK(_GET_REG(sfr), bit_mask)
#define READ_SFR_MASK(sfr, bit_mask)            READ_REG_MASK(_GET_REG(sfr), bit_mask)
#define CLEAR_SFR_MASK(sfr, bit_mask)           CLEAR_REG_MASK(_GET_REG(sfr), bit_mask)

#endif



#endif /* __ACCESS_H__ */
