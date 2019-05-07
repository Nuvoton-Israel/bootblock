/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   defs_internal.h                                                          */
/* This file contains internal auxililary macros used in common NTIL definitions */
/* Project:                                                                   */
/*            SWC DEFS                                                        */
/*----------------------------------------------------------------------------*/

#ifndef __DEFS_INTERNAL_H__
#define __DEFS_INTERNAL_H__


/*---------------------------------------------------------------------------------------------------------*/
/* Auxiliary macros (not part of the interface)                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define _REG_READ_AUX_(addr,type,size)              _REG_ACCESS_FORM1(type,R,size,addr)
#define _REG_WRITE_AUX_(val,addr,type,size)         _REG_ACCESS_FORM2(type,W,size,addr,val)

#define _REG_ACCESS_FORM1(type,rw,size,arg)         type##rw##size((arg))
#define _REG_ACCESS_FORM2(type,rw,size,arg1,arg2)   type##rw##size((arg1), (arg2))
#define _CASTING_FROM(size)                         _CASTING_FROM2(UINT,size)
#define _CASTING_FROM2(type,size)                   type##size

#define _GET_REG(addr,accesstype,size)              addr,accesstype,size
#define _GET_REG_ADDR(addr,accesstype,size)         addr
#define _GET_REG_ACCESSTYPE(addr,accesstype,size)   accesstype
#define _GET_REG_SIZE(addr,accesstype,size)         size

#define _GET_FIELD(pos,size)                        pos,size
#define _GET_FIELD_POS(pos,size)                    pos
#define _GET_FIELD_SIZE(pos,size)                   size


#define _READ_FIELD(reg, reg_size, size, position) \
    (((reg)>>(position)) & ((((_CASTING_FROM(reg_size))1)<<(size))-1))

#define _SET_REG_FIELD(reg, size, position, value)    \
    REG_WRITE(_GET_REG(reg),(((REG_READ(_GET_REG(reg))) &(~((((_CASTING_FROM(_GET_REG_SIZE(reg)))1<<(size))-1)<<(position))))\
    | (((((_CASTING_FROM(_GET_REG_SIZE(reg)))1<<(size))-1) & (value))<<(position))))

#define  _SET_REG_FIELD_SIZE(reg,size,position,value)                                                                   \
    if (size == 1)                                                                                                      \
    {                                                                                                                   \
        if (value == 1)                                                                                                 \
        {                                                                                                               \
            _SET_REG_BIT_FIELD(_GET_REG(reg), position);                                                                \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            _CLEAR_REG_BIT_FIELD(_GET_REG(reg), position);                                                              \
        }                                                                                                               \
   }                                                                                                                    \
   else                                                                                                                 \
   {                                                                                                                    \
      _SET_REG_FIELD(_GET_REG(reg),size,position,value);                                                                \
   }

#ifdef CPU_64_BIT
#define _SET_VAR_FIELD(var,size,position,value)                                                                                                 \
   ((sizeof(var) == 8) ?   ((var) = ((var) & (~((((UINT64)1<<(size))-1)<<(position))))  | (((((UINT64)1<<(size))-1) & (value))<<(position))) :  \
   ((sizeof(var) == 4) ?   ((var) = ((var) & (~((((UINT32)1<<(size))-1)<<(position))))  | (((((UINT32)1<<(size))-1) & (value))<<(position))) :  \
   ((sizeof(var) == 2) ?   ((var) = ((var) & (~((((UINT16)1<<(size))-1)<<(position))))  | (((((UINT16)1<<(size))-1) & (value))<<(position))) :  \
                           ((var) = ((var) & (~(((        1<<(size))-1)<<(position))))  | ((((        1<<(size))-1) & (value))<<(position))))))
#else
#define _SET_VAR_FIELD(var,size,position,value)                                                                                                 \
   ((sizeof(var) == 2) ?   ((var) = ((var) & (~((((UINT16)1<<(size))-1)<<(position))))  | (((((UINT16)1<<(size))-1) & (value))<<(position))) :  \
   ((sizeof(var) == 4) ?   ((var) = ((var) & (~((((UINT32)1<<(size))-1)<<(position))))  | (((((UINT32)1<<(size))-1) & (value))<<(position))) :  \
                           ((var) = ((var) & (~(((        1<<(size))-1)<<(position))))  | ((((        1<<(size))-1) & (value))<<(position)))))
#endif						   
						   

#define _SET_REG_BIT_FIELD(reg, position)    \
    REG_WRITE(_GET_REG(reg),((REG_READ(_GET_REG(reg))) |((_CASTING_FROM(_GET_REG_SIZE(reg)))1<<(position))))

#define _READ_REG_BIT_FIELD(reg, position)    \
    (((reg)>>(position)) & (_CASTING_FROM(_GET_REG_SIZE(reg))1)

#define _CLEAR_REG_BIT_FIELD(reg, position)    \
    REG_WRITE(_GET_REG(reg),((REG_READ(_GET_REG(reg))) & (~((_CASTING_FROM(_GET_REG_SIZE(reg)))1<<(position)))))

#define _MASK_FIELD(size, position)   \
    (((1UL<<(size))-1)<<(position))

#define _BUILD_FIELD_VAL(size, position, value)   \
    ((((1UL<<(size))-1) & (value)) << (position))



#endif

