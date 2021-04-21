/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   espi_drv.c                                                               */
/* This file contains Enhanced Serial Peripheral Interface (eSPI) driver implementation */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_DRV()

#include "espi_drv.h"
#include "espi_regs.h"
#include <string.h>
#ifdef ESPI_PC_BM_SELF_TEST
#include <stdlib.h>
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Module Dependencies                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined ESPI_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_DRV(espi)
#endif

#if defined GDMA_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_DRV(gdma)
#endif

#if defined SHM_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_DRV(shm)
#endif

#if defined MIWU_MODULE_TYPE
#include __MODULE_IF_HEADER_FROM_DRV(miwu)
#endif

#ifdef ESPI_CAPABILITY_SAF
#if defined FLASH_DEV_MODULE_TYPE
#include __LOGICAL_IF_HEADER_FROM_DRV(flash_dev)
#endif
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*                                                 GENERAL                                                 */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Flash/PC BM completion codes                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_SUCCESSFUL_COMPLETION_WO_DATA    = 0x06,
    ESPI_UNSUCCESSFUL_COMPLETION_WO_DATA  = 0x0E,
    ESPI_SUCCESSFUL_COMPLETION_WITH_DATA  = 0x0F,
} ESPI_COMPLETION_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Transfer header tagPlusLength fields                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define HEADER_LENGTH                           0,  12
#define HEADER_TAG                              12,  4

/*---------------------------------------------------------------------------------------------------------*/
/* Preserve ESPI interrupt state and disable                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_INTERRUPT_SAVE_DISABLE(var)                    \
{                                                           \
    var = REG_READ(ESPI_ESPIIE);                            \
    ESPI_IntEnable(ESPI_CHANNEL_ALL, FALSE);                \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Restore ESPI interrupt state                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_INTERRUPT_RESTORE(var)                         \
{                                                           \
    REG_WRITE(ESPI_ESPIIE, var);                            \
}


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   PC                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Message codes                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_PC_BM_MSG_CODE_LTR = 1,
} ESPI_PC_BM_MSG_CODE;

/*---------------------------------------------------------------------------------------------------------*/
/* Message types                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_PC_MSG_READ,
    ESPI_PC_MSG_WRITE,
    ESPI_PC_MSG_LTR
} ESPI_PC_MSG_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Peripheral Channel Maximum Payload Size                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_PC_BM_MAX_PAYLOAD_SIZE_64B  = 1,
    ESPI_PC_BM_MAX_PAYLOAD_SIZE_128B,
    ESPI_PC_BM_MAX_PAYLOAD_SIZE_256B
} ESPI_PC_BM_MAX_PAYLOAD_SIZE;

/*---------------------------------------------------------------------------------------------------------*/
/* Peripheral Channel Maximum Read Request Size                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_PC_BM_MAX_READ_REQ_SIZE_64B = 1,
    ESPI_PC_BM_MAX_READ_REQ_SIZE_128B,
    ESPI_PC_BM_MAX_READ_REQ_SIZE_256B,
    ESPI_PC_BM_MAX_READ_REQ_SIZE_512B,
    ESPI_PC_BM_MAX_READ_REQ_SIZE_1024B,
    ESPI_PC_BM_MAX_READ_REQ_SIZE_2048B,
    ESPI_PC_BM_MAX_READ_REQ_SIZE_4096B
} ESPI_PC_BM_MAX_READ_REQ_SIZE;

/*---------------------------------------------------------------------------------------------------------*/
/* Cycle type structure                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8 _32bAddrCycType;
    UINT8 _64bAddrCycType;
} ESPI_PC_BM_CYC_T;

/*---------------------------------------------------------------------------------------------------------*/
/* LTR message header struct                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef _PACK_(struct)
{
    _PACK_(UINT8    rqLsLv8_9); // Reserved for read/write request
    _PACK_(UINT8    lv0_7);
    _PACK_(UINT8    reserved[2]);
} ESPI_PC_BM_LTR_MSG_HDR_T;

/*---------------------------------------------------------------------------------------------------------*/
/* User request information on non polling mode request                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT32*                     buffer;
    UINT32                      size;
    UINT32                      currSize;
    UINT32                      buffOffset;
    UINT64                      RamOffset;
    ESPI_PC_MSG_T               reqType;
    DEFS_STATUS                 status;
    BOOLEAN                     strpHdr;
    BOOLEAN                     manualMode;
    BOOLEAN                     useDMA;
    UINT16                      buffTransferred;
} ESPI_PC_USR_REQ_INFO;

/*---------------------------------------------------------------------------------------------------------*/
/* PC request header struct                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef _PACK_(struct)
{
    _PACK_(UINT8        msgCode);
    _PACK_(UINT8        cycleType);
    _PACK_(UINT16       tagPlusLength);
    _PACK_(union)
    {
        _PACK_(UINT32   addr32b);
        _PACK_(UINT32   msgSpec);
        _PACK_(UINT64   addr64b);
    }addr;
} ESPI_PC_REQ_HDR_T;


#define ESPI_PC_MAX_PAYLOAD_SIZE                64
#define ESPI_PC_PAUTO_MAX_TRANS_COUNT           256
#define ESPI_PC_READ_REP_HDR_SIZE               4
#define ESPI_PC_32_BIT_HDR_SIZE                 8
#define ESPI_PC_64_BIT_HDR_SIZE                 12
#define ESPI_PC_MSG_HDR_SIZE                    8
#define ESPI_PC_READ_SECTOR_SIZE                _4KB_
#define ESPI_PC_AUTO_MODE(on)                   SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMBRSTEN, on)
#define ESPI_PC_AUTO_MODE_IS_ON                 READ_REG_FIELD(ESPI_PERCTL, PERCTL_BMBRSTEN)

/*---------------------------------------------------------------------------------------------------------*/
/* Cycle types                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_CYC_TYPE_MEM_READ_32         0
#define ESPI_PC_BM_CYC_TYPE_MEM_READ_64         2
#define ESPI_PC_BM_CYC_TYPE_MEM_WRITE_32        1
#define ESPI_PC_BM_CYC_TYPE_MEM_WRITE_64        3
#define ESPI_PC_BM_CYC_TYPE_MSG_MASK            0xF1
#define ESPI_PC_BM_CYC_TYPE_MSG                 0x10
#define ESPI_PC_BM_CYC_TYPE_MSG_WITH_DATA       0x11

/*---------------------------------------------------------------------------------------------------------*/
/* ESPIERR relevant bits mask                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_ESPIERR_MASK                 0xFFFFFFFF // TODO: change

/*---------------------------------------------------------------------------------------------------------*/
/* Messages routing                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_MSG_ROUTE_LTR                (0 << 1)

/*---------------------------------------------------------------------------------------------------------*/
/* rqLsLv8_9 fields                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_LTR_LV                0, 2
#define ESPI_PC_BM_LTR_LS                2, 3
#define ESPI_PC_BM_LTR_RQ                7, 1

/*---------------------------------------------------------------------------------------------------------*/
/* Latency value fields                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_LTR_LV8_9             8, 2
#define ESPI_PC_BM_LTR_LV0_7             0, 8

/*---------------------------------------------------------------------------------------------------------*/
/* Preserve PC BM receive interrupt state and disable                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_RX_INTERRUPT_SAVE_DISABLE(var)            \
{                                                            \
    var = READ_REG_FIELD(ESPI_ESPIIE, ESPIIE_PBMRXIE);       \
    SET_REG_FIELD(ESPI_ESPIIE, ESPIIE_PBMRXIE, FALSE);       \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Restore PC BM receive interrupt state                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_RX_INTERRUPT_RESTORE(var)                 \
{                                                            \
    SET_REG_FIELD(ESPI_ESPIIE, ESPIIE_PBMRXIE, var);         \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Preserve PC BM transfer done interrupt state and disable                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_TXDONE_INTERRUPT_SAVE_DISABLE(var)        \
{                                                            \
    var = READ_REG_FIELD(ESPI_ESPIIE, ESPIIE_BMTXDONEIE);    \
    SET_REG_FIELD(ESPI_ESPIIE, ESPIIE_BMTXDONEIE, FALSE);    \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Restore PC BM transfer done interrupt state                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_TXDONE_INTERRUPT_RESTORE(var)             \
{                                                            \
    SET_REG_FIELD(ESPI_ESPIIE, ESPIIE_BMTXDONEIE, var);      \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Macro to handle TAG incrementation                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_PC_BM_GET_NEXT_TAG(tag)    ((tag + 1) % (1 << 4))


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   VW                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* VW Index range                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8           start;
    UINT8           end;
} ESPI_VW_INDEX_RANGE_T;

#define ESPI_VW_NON_FATAL_ERR_TIMEOUT          0XFFFFFF // TODO: verify value is not too long/short


/*---------------------------------------------------------------------------------------------------------*/
/*                                                  OOB                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* OOB (Tunnelled SMBus) Message cycle Type                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_MESSAGE                        0x21

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Channel SMBus Slave Destination Address Definitions                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_SMB_SLAVE_DEST_ADDR_ESPI_HW    0x02
#define ESPI_OOB_SMB_SLAVE_DEST_ADDR_CSME_FW    0x10
#define ESPI_OOB_SMB_SLAVE_DEST_ADDR_PMC_FW     0x20

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Channel SMBus Slave Source Address Definitions                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_SMB_SLAVE_SRC_ADDR_EC          0x0F

/*---------------------------------------------------------------------------------------------------------*/
/* CSME FW commands                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_USB_CMD                        0x08

/*---------------------------------------------------------------------------------------------------------*/
/* PMC FW commands                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_PECI_CMD                       0x01

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Channel Maximum Payload Size:                                                                       */
/* - MCTP (over SMBus) payload (64B)                                                                       */
/* - SMBus header (4B)                                                                                     */
/* - MCTP header (4B)                                                                                      */
/* - PEC (1B)                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_MAX_PAYLOAD                    73

/*---------------------------------------------------------------------------------------------------------*/
/* OOB RX Timeout                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_RX_TIMEOUT                     0x200000

/*---------------------------------------------------------------------------------------------------------*/
/* ESPIERR relevant bits mask                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_ESPIERR_MASK                   0xFFFFFFFF // TODO: change

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Retry bit                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_PECI_RETRY                     0, 1

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Maximum Retry Trials                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_PECI_RETRY_MAX_COUNT           3

/*---------------------------------------------------------------------------------------------------------*/
/* PECI  Zero-data transaction                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define NO_PECI_DATA                            0L

/*---------------------------------------------------------------------------------------------------------*/
/* PECI maximum data size                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_PECI_DATA_SIZE                 ((ESPI_OOB_PECI_DATA_SIZE_DWORD * 2) + 1)

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Command type                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {                /* do not change - code relies on enum value */
    ESPI_OOB_CMD_HW = 0,
    ESPI_OOB_CMD_CSME,
    ESPI_OOB_CMD_PMC,
    ESPI_OOB_CMD_CUR,
    ESPI_OOB_CMD_LAST
} ESPI_OOB_CMD_T;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Data                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef union OOB_DATA {
    UINT32 value;
    struct
    {
        UINT8 lsb0;
        UINT8 lsb1;
        UINT8 lsb2;
        UINT8 lsb3;
    } bytes;
} OOB_DATA;

/*---------------------------------------------------------------------------------------------------------*/
/* Transfer header                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   pktLen;
    UINT8   type;
    UINT16  tagPlusLength;
} ESPI_OOB_TRANS_HDR;

/*---------------------------------------------------------------------------------------------------------*/
/* Transfer payload                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   destAddr;
    UINT8   cmdCode;
    UINT8   byteCount;
    UINT8   srcAddr;
} ESPI_OOB_MCTP_PKT;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Command Information                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   srcAddr;
    UINT8   destAddr;
    UINT8   cmdCode;
    UINT8   nWrite;
    UINT8*  writeBuf;
    UINT8   nRead;
    UINT8*  readBuf;
} ESPI_OOB_CMD_INFO;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Request Information                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   nWrite;
    UINT8*  writeBuf;
    UINT8   nRead;
    UINT8*  readBuf;
} ESPI_OOB_REQ_INFO;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB PECI message                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   trgAddr;
    UINT8   writeLen;
    UINT8   readLen;
    UINT8   cmd;
    UINT8   data[ESPI_OOB_PECI_DATA_SIZE];
} ESPI_OOB_PECI_MSG;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB USB message                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   usage;
    UINT8   data[3];
} ESPI_OOB_USB_MSG;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Host ID                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_PECI_HOST_ID(host_id)          (UINT32)(host_id << 1 | 0)

/*---------------------------------------------------------------------------------------------------------*/
/* USB usage responce mask                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_USB_USAGE_RES_MASK(x)          (x | 0x10)

/*---------------------------------------------------------------------------------------------------------*/
/* Preserve OOB interrupt state and disable                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_INTERRUPT_SAVE_DISABLE(var)               \
{                                                          \
    var = READ_REG_FIELD(ESPI_ESPIIE, ESPIIE_OOBRXIE);     \
    ESPI_IntEnable(ESPI_CHANNEL_OOB, FALSE);               \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Restore OOB interrupt state                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_INTERRUPT_RESTORE(var)                    \
{                                                          \
    ESPI_IntEnable(ESPI_CHANNEL_OOB, var);                 \
}


/*---------------------------------------------------------------------------------------------------------*/
/*                                                  FLASH                                                  */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Transfer header                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   pktLen;
    UINT8   type;
    UINT16  tagPlusLength;
} ESPI_FLASH_TRANS_HDR;

/*---------------------------------------------------------------------------------------------------------*/
/* User request information on non polling mode request                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT32*                 buffer;
    UINT32                  size;
    UINT32                  currSize;
    UINT32                  buffOffset;
    UINT32                  flashOffset;
    ESPI_FLASH_REQ_ACC_T    reqType;
    DEFS_STATUS             status;
    BOOLEAN                 strpHdr;
    BOOLEAN                 manualMode;
    BOOLEAN                 useDMA;
} ESPI_FLASH_USR_REQ_INFO;


#define ESPI_FLASH_MAX_READ_REQ_SIZE            64
#define ESPI_FLASH_AUTO_MODE_MAX_TRANS_SIZE     256
#define ESPI_FLASH_PROGRAM_PAGE_DEFAULT_SIZE    256
#define ESPI_FLASH_TRANS_DATA_PAYLOAD_MAX_SIZE  16
#define ESPI_FLASH_REQ_BUFF_CURR_LOC(reqType)   (ESPI_FLASH_currReqInfo_L.counter * (ESPI_FLASH_MaxTransSize[reqType]))
#define ESPI_FLASH_REQ_BUFF_SIZE_LEFT(reqType)  (ESPI_FLASH_currReqInfo_L.size - (ESPI_FLASH_currReqInfo_L.counter * ESPI_FLASH_MaxTransSize[reqType]))
#define ESPI_FLASH_REQ_BUFF_CURR_SIZE(reqType)  (MIN(ESPI_FLASH_REQ_BUFF_SIZE_LEFT(reqType), ESPI_FLASH_MaxTransSize[reqType]))
#define ESPI_FLASH_AUTO_MODE(on)                SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_AMTEN, on)
#define ESPI_FLASH_IS_AUTO_MODE_ON              (READ_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_AMTEN))
#define ESPI_FLASH_READ_SECTOR_SIZE             _4KB_
#define ESPI_FLASH_AUTO_AMDONE_LOOP_COUNT       0xFFFFFF
#ifdef ESPI_CAPABILITY_SAF
#define ESPI_FLASH_MAX_READ_REQ_SIZE_SUPP       ESPI_FLASH_SafMaxReadSize[READ_REG_FIELD(ESPI_FLASHCFG,FLASHCFG_FLASHREQSUP)]
#define ESPI_FLASH_MAX_PAYLOAD_REQ_SIZE         _64B_
#define ESPI_MIDDLE_COMPLETION_MASK             0xF9
#define ESPI_FIRST_COMPLETION_MASK              0xFB
#define ESPI_LAST_COMPLETION_MASK               0xFD
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ESPIERR relevant bits mask                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_ESPIERR_MASK                 0xFFFFFFFF // TODO: change

/*---------------------------------------------------------------------------------------------------------*/
/* Macro to handle TAG incrementation                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_FLASH_AUTO_MODE_TAG_CHANGE
#define ESPI_FLASH_GET_NEXT_TAG(tag)            ((tag + 1) % (1 << 4))
#else
/*---------------------------------------------------------------------------------------------------------*/
/* In Auto mode the tag is toggled, thus the range in use is 0/1 to supports manual and auto mode          */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_GET_NEXT_TAG(tag)            ((tag + 1) % 2)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Flash RX Timeout                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_RX_TIMEOUT                   0x200000

/*---------------------------------------------------------------------------------------------------------*/
/* Preserve FLASH receive interrupt state and disable                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_RX_INTERRUPT_SAVE_DISABLE(var)              \
{                                                              \
    var = READ_REG_FIELD(ESPI_ESPIIE, ESPIIE_FLASHRXIE);       \
    SET_REG_FIELD(ESPI_ESPIIE, ESPIIE_FLASHRXIE, FALSE);       \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Restore FLASH receive interrupt state                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_RX_INTERRUPT_RESTORE(var)                   \
{                                                              \
    SET_REG_FIELD(ESPI_ESPIIE, ESPIIE_FLASHRXIE, var);         \
}

#ifdef ESPI_CAPABILITY_SAF
/*---------------------------------------------------------------------------------------------------------*/
/* Flash access channel mode                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_MODE_MAF  = 0,
    ESPI_FLASH_MODE_SAF = 1,
} ESPI_FLASH_MODE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash SAF host requests                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_SAF_REQ_READ  = 0,
    ESPI_FLASH_SAF_REQ_WRITE = 1,
    ESPI_FLASH_SAF_REQ_ERASE = 2,
    ESPI_FLASH_SAF_REQ_RPMC_OP1  = 3,
    ESPI_FLASH_SAF_REQ_RPMC_OP2  = 4,
    ESPI_FLASH_SAF_REQ_NUM,
} ESPI_FLASH_SAF_REQ_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash SAF erase sizes                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_ERASE_4K  = 0,
    ESPI_FLASH_ERASE_16K = 1,
    ESPI_FLASH_ERASE_64K = 2,
} ESPI_FLASH_ERASE_T;

typedef struct
{
    ESPI_FLASH_SAF_REQ_T    reqType;
    DEFS_STATUS             status;
    UINT                    outBufferSize;
    UINT8                   tag;
    UINT32                  buffer[(_64B_)/sizeof(UINT32)];
    UINT8                   currSize;
    UINT32                  offset;
    UINT32                  size;
    UINT32                  reminder;
    UINT32                  bytesRead;
} ESPI_FLASH_SAF_REQ_INFO;

#endif // ESPI_CAPABILITY_SAF

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL VARIABLES                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*                                                 GENERAL                                                 */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Firmware Boot Load indication                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static BOOLEAN                          ESPI_bootLoad;

/*---------------------------------------------------------------------------------------------------------*/
/* eSPI Error Mask                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32                           ESPI_errorMask;

/*---------------------------------------------------------------------------------------------------------*/
/* eSPI Config Update Mask                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8                            ESPI_configUpdateEnable;
static UINT32                           ESPI_configUpdateMask;

/*---------------------------------------------------------------------------------------------------------*/
/* Main eSPI user callback                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_INT_HANDLER                 ESPI_userIntHandler_L;


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   PC                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
static ESPI_PC_BM_MSG_HANDLER           ESPI_PC_BM_userMsgHandler_L;
static ESPI_INT_HANDLER                 ESPI_PC_BM_userIntHandler_L;
static BOOLEAN                          ESPI_PC_BM_sysRAMis64bAddr_L;
static ESPI_PC_USR_REQ_INFO             ESPI_PC_BM_usrReqInfo_L;
static DEFS_STATUS                      ESPI_PC_BM_status_L;
static UINT8                            ESPI_PC_BM_currReqTag_L;
static BOOLEAN                          ESPI_PC_BM_RXIE_L;
static BOOLEAN                          ESPI_PC_BM_TXDONE_L;
#ifdef ESPI_PC_BM_SELF_TEST
static DEFS_STATUS                      ESPI_PC_BM_selfTestStatus_L;
#endif
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   VW                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI VW Index Ranges, per type                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static const ESPI_VW_INDEX_RANGE_T  ESPI_VW_indexRange[ESPI_VW_TYPE_NUM] =
{
    { 0,    1},     /* ESPI_VW_TYPE_INT_EV  */
    { 2,    7},     /* ESPI_VW_TYPE_SYS_EV  */
    { 64,   127},   /* ESPI_VW_TYPE_PLT     */
#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    { 128,  255},   /* ESPI_VW_TYPE_GPIO    */
#endif
};

/*---------------------------------------------------------------------------------------------------------*/
/* Virtual Wire user callback                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_VW_HANDLER_T            ESPI_VW_Handler_L;


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   OOB                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Command/Request Information                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_CMD_INFO*           ESPI_OOB_currOutCmdInfo_L;
static ESPI_OOB_CMD_INFO*           ESPI_OOB_currInCmdInfo_L;
static ESPI_OOB_CMD_INFO            ESPI_OOB_cmdInfo[ESPI_OOB_CMD_LAST];

/*---------------------------------------------------------------------------------------------------------*/
/* Local buffer to store OOB Channel SMBus request & response                                              */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8                        ESPI_OOB_InBuf_L[ESPI_OOB_MAX_PAYLOAD];
static UINT8                        ESPI_OOB_OutBuf_L[ESPI_OOB_MAX_PAYLOAD];

/*---------------------------------------------------------------------------------------------------------*/
/* OOB current request Tag                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8                        ESPI_OOB_currReqTag_L;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB current status                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS                  ESPI_OOB_Status_L;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI messages buffer (maximum read buffer size = <maximum data size> + <cc> + <2 FCS>)                  */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8                        ESPI_OOB_peciReadBuff_L[ESPI_OOB_PECI_DATA_SIZE_QWORD + 3];

/*---------------------------------------------------------------------------------------------------------*/
/* PECI client address                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8                        ESPI_OOB_peciClientAddress_L;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI messages completion code                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8                        ESPI_OOB_peciCc_L;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI callback                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_PECI_CALLBACK_T     ESPI_OOB_peciCallback_L;

/*---------------------------------------------------------------------------------------------------------*/
/* Current PECI Command being handled                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_PECI_COMMAND_T      ESPI_OOB_peciCurrentCommand_L;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Retry Counter                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8                        ESPI_OOB_peciRetryCount_L;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI message                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_PECI_MSG            ESPI_OOB_peciMsg_L;

/*---------------------------------------------------------------------------------------------------------*/
/* USB message                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_USB_MSG             ESPI_OOB_usbMsg_L;

/*---------------------------------------------------------------------------------------------------------*/
/* USB callback                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_USB_CALLBACK_T      ESPI_OOB_usbCallback_L;

/*---------------------------------------------------------------------------------------------------------*/
/* CRASHLOG callback                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_CRASHLOG_CALLBACK_T ESPI_OOB_crashLogCallback_L;

/*---------------------------------------------------------------------------------------------------------*/
/* HW callback                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
static ESPI_OOB_HW_CALLBACK_T       ESPI_OOB_hwCallback_L;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI CRC8 table (Polynomial: x8 + x2 + x + 1)                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static const UINT8 ESPI_OOB_PECI_crc8_table[256] =
{
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};


/*---------------------------------------------------------------------------------------------------------*/
/*                                                  FLASH                                                  */
/*---------------------------------------------------------------------------------------------------------*/

static UINT8                    ESPI_FLASH_currReqTag_L;
static ESPI_FLASH_USR_REQ_INFO  ESPI_FLASH_currReqInfo_L;
static volatile DEFS_STATUS     ESPI_FLASH_status_L;
static BOOLEAN                  ESPI_FLASH_RXIE_L;
static UINT32                   ESPI_FLASH_WritePageSize_L;
static ESPI_INT_HANDLER         ESPI_FLASH_userIntHandler_L = NULL;
static const UINT8              ESPI_FLASH_MaxTransSize[] = {ESPI_FLASH_MAX_READ_REQ_SIZE,
                                                             ESPI_FLASH_TRANS_DATA_PAYLOAD_MAX_SIZE,
                                                             ESPI_FLASH_BLOCK_ERASE_NUM}; // DO NOT change order, according to ESPI_FLASH_REQ_ACC_T
#ifdef ESPI_CAPABILITY_SAF
static BOOLEAN                  ESPI_FLASH_SafPendingIncomingReq_L;
static BOOLEAN                  ESPI_FLASH_SafPendingIncomingRes_L;
static const UINT               ESPI_FLASH_SafMaxReadSize[] = {_64B_, _64B_, _128B_, _256B_, _512B_,
                                                               _1KB_, _2KB_, _4KB_}; // DO NOT change order, according to ESPI_FLASH_REQ_ACC_T
static FLASH_DEV_FP_T           ESPI_FLASH_SafFlashParams_L;
static ESPI_FLASH_SAF_REQ_INFO  ESPI_FLASH_SafReqInfo_L;
#endif // ESPI_CAPABILITY_SAF


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                   LOCAL FUNCTIONS FORWARD DECLERATION                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*                                                 GENERAL                                                 */
/*---------------------------------------------------------------------------------------------------------*/

#if defined (MIWU_MODULE_TYPE) && defined (ESPI_MIWU_WAKEUP_SUPPORT)
static void         ESPI_WakeUpHandler_l            (MIWU_SRC_T miwu_src);
#endif
static void         ESPI_Reset_l                    (void);
static void         ESPI_GetError_l                 (UINT32 mask);
static void         ESPI_ResetConnfigUpdate_l       (void);
static BOOLEAN      ESPI_IsChannelSlaveEnable_l     (ESPI_CHANNEL_TYPE_T channel);
static void         ESPI_ConfigUpdate_l             (void);


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   PC                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
static DEFS_STATUS  ESPI_PC_BM_GetCurrReqSize_l             (ESPI_PC_MSG_T reqType, UINT64 ramOffset, UINT32 size,
                                                             UINT32* currSize);
static DEFS_STATUS  ESPI_PC_BM_SetRequest_l                 (ESPI_PC_MSG_T reqType, UINT64 offset, UINT32 size,
                                                             const UINT32* buffer, UINT8 tag);
static DEFS_STATUS  ESPI_PC_BM_HandleInputPacket_l          (ESPI_PC_MSG_T req, UINT32* buffer, BOOLEAN strpHdr,
                                                             UINT32 size);
static DEFS_STATUS  ESPI_PC_BM_ReceiveDataBuffer_l          (ESPI_PC_MSG_T reqType, UINT32* buffer, BOOLEAN strpHdr,
                                                             UINT32 size);
static DEFS_STATUS  ESPI_PC_BM_ReqManual_l                  (ESPI_PC_MSG_T msgType, UINT64 offset, UINT32 size,
                                                             BOOLEAN pollingMode, UINT32* buffer, BOOLEAN strpHdr);
static DEFS_STATUS  ESPI_PC_BM_ReadReqAuto_l                (UINT64 offset, UINT32 size, BOOLEAN polling,
                                                             UINT32* buffer, BOOLEAN strpHdr, BOOLEAN useDMA);
static void         ESPI_PC_BM_HandleReqRes_l               (void);
static void         ESPI_PC_BM_HandleInMessage_l            (void);
static void         ESPI_PC_BM_AutoModeTransDoneHandler_l   (UINT32 status, UINT32 intEnable);
#ifdef GDMA_MODULE_TYPE
static void         ESPI_PC_BM_GdmaIntHandler_l             (UINT8  module, UINT8 channel, DEFS_STATUS status, UINT32 transferLen);
#endif
static void         ESPI_PC_BM_ExitAutoReadRequest_l        (DEFS_STATUS status, BOOLEAN useDMA);
static void         ESPI_PC_BM_HandleBurstErr_l             (BOOLEAN useDMA);
static DEFS_STATUS  ESPI_PC_BM_EnqueuePacket_l              (ESPI_PC_MSG_T msgType);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   VW                                                    */
/*---------------------------------------------------------------------------------------------------------*/

static DEFS_STATUS  ESPI_VW_GetType_l               (UINT8 index, ESPI_VW_TYPE_T* type);
static DEFS_STATUS  ESPI_VW_GetReg_l                (UINT8 index, ESPI_VW_DIR_T dir, UINT8* reg);
#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
static DEFS_STATUS  ESPI_VW_GetGpio_l               (UINT8 index, ESPI_VW_DIR_T dir, UINT8* gpio);
#endif
static void         ESPI_VW_HandleInputEvent_l      (void);
#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
static void         ESPI_VW_HandleInputGpio_l       (void);
#endif
#if defined (ESPI_CAPABILITY_FLASH_NON_FATAL_ERR_IND) || defined (ESPI_CAPABILITY_PC_NON_FATAL_ERR_IND) || defined (ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT)
static void         ESPI_VW_SmNonFatalError_l      (void);
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*                                                  FLASH                                                  */
/*---------------------------------------------------------------------------------------------------------*/

static void         ESPI_FLASH_Init_l               (void);
static DEFS_STATUS  ESPI_FLASH_ReqManual_l          (ESPI_FLASH_REQ_ACC_T reqType, UINT32 offset, UINT32 size,
                                                     BOOLEAN polling, UINT32* buffer, BOOLEAN strpHdr);
static DEFS_STATUS  ESPI_FLASH_SetRequest_l         (ESPI_FLASH_REQ_ACC_T reqType, UINT32 offset, UINT32 size,
                                                     const UINT32* buffer, UINT8 tag);
static DEFS_STATUS  ESPI_FLASH_ReadReqAuto_l        (UINT32 offset, UINT32 size, BOOLEAN polling,
                                                     UINT32* buffer, BOOLEAN strpHdr, BOOLEAN useDMA);
static DEFS_STATUS  ESPI_FLASH_HandleInputPacket_l  (ESPI_FLASH_REQ_ACC_T req, UINT32* buffer, BOOLEAN strpHdr,
                                                     UINT32 size);
static DEFS_STATUS  ESPI_FLASH_EnqueuePacket_l      (void);
static void         ESPI_FLASH_HandleReqRes_l       (void);
static DEFS_STATUS  ESPI_FLASH_ReceiveDataBuffer_l  (ESPI_FLASH_REQ_ACC_T reqType, UINT32* buffer, BOOLEAN strpHdr,
                                                     UINT32 size);
#ifdef GDMA_MODULE_TYPE
static void         ESPI_FLASH_GdmaIntHandler_l     (UINT8 module, UINT8 channel, DEFS_STATUS status, UINT32 transferLen);
#endif
static void         ESPI_FLASH_AutoModeTransDoneHandler_l (UINT8 module, UINT8 channel, UINT32 status, UINT32 intEnable);
static DEFS_STATUS  ESPI_FLASH_GetCurrReqSize_l     (ESPI_FLASH_REQ_ACC_T reqType, UINT32 flashOffset, UINT32 size,
                                                     UINT32* currSize);
static void         ESPI_FLASH_HandleAutoModeErr_l  (BOOLEAN useDMA);
static void         ESPI_FLASH_ExitAutoReadRequest_l(DEFS_STATUS status, BOOLEAN useDMA);
#ifdef ESPI_CAPABILITY_SAF
static void         ESPI_FLASH_SAF_SendRes_l        (UINT8 comp);
static DEFS_STATUS  ESPI_FLASH_SAF_CheckAddress_l   (void);
static DEFS_STATUS  ESPI_FLASH_SAF_PerformReq_l     (UINT32 addr, UINT32 size);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*                                                   OOB                                                   */
/*---------------------------------------------------------------------------------------------------------*/

static void         ESPI_OOB_Init_l                     (void);
static DEFS_STATUS  ESPI_OOB_BuildCmd_l                 (UINT8 srcAddr, UINT8 destAddr, UINT8 cmdCode,
                                                         UINT8 nWrite, UINT8* writeBuf, UINT8 nRead, UINT8* readBuf);
static DEFS_STATUS  ESPI_OOB_RunCmd_l                   (void);
static DEFS_STATUS  ESPI_OOB_SendCmd_l                  (void);
static DEFS_STATUS  ESPI_OOB_BuildReq_l                 (UINT8 nWrite, UINT8* writeBuf, UINT8 nRead, UINT8* readBuf);
static DEFS_STATUS  ESPI_OOB_SendReq_l                  (void);
static DEFS_STATUS  ESPI_OOB_ReceiveCmd_l               (void);
static DEFS_STATUS  ESPI_OOB_ReceiveReq_l               (void);
static UINT8        ESPI_OOB_PECI_CalcFCS_l             (const UINT8* buff, UINT8 nBytes);
static void         ESPI_OOB_PECI_ReceiveMessage_l      (void);
static void         ESPI_OOB_USB_ReceiveMessage_l       (void);
static void         ESPI_OOB_CRASHLOG_ReceiveMessage_l  (void);
static void         ESPI_OOB_HW_ReceiveMessage_l        (void);
static void         ESPI_OOB_ReceiveMessage_l           (void);
static void         ESPI_OOB_ClearCmdInfo_l             (void);


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                       GENERAL INTERFACE FUNCTIONS                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_Init                                                                              */
/*                                                                                                         */
/* Parameters:      handler - general ESPI interrupts user handler                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs eSPI module initialization                                       */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_Init (ESPI_INT_HANDLER handler)
{
    UINT32 var = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset the eSPI internal state                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_Reset_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set ID                                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_ESPIID, ESPIID_ID, ESPI_MODULE_TYPE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* eSPI Configuration                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    var = REG_READ(ESPI_ESPICFG);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Channel support                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(var ,ESPICFG_PCCHN_SUPP,      TRUE);
    SET_VAR_FIELD(var ,ESPICFG_VWCHN_SUPP,      TRUE);
    SET_VAR_FIELD(var ,ESPICFG_FLASHCHN_SUPP,   FALSE);
    SET_VAR_FIELD(var ,ESPICFG_OOBCHN_SUPP,     FALSE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* IO mode support                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(var ,ESPICFG_IOMODE, ESPI_IO_MODE_SINGLE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Maximum frequency support                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(var ,ESPICFG_MAXFREQ, ESPI_MAX_20_MHz);

#ifdef ESPI_CAPABILITY_RESET_OUTPUT
    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset Output support                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    //lint -e{648} Suppress Overflow in computing constant for operation: 'shift left'
    SET_VAR_FIELD(var ,ESPICFG_RSTO, ESPI_RST_OUT_LOW);
#endif

    REG_WRITE(ESPI_ESPICFG, var);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure Interrupt/Wake-Up                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_userIntHandler_L = handler;

    if (ESPI_userIntHandler_L != NULL)
    {
        INTERRUPT_REGISTER_AND_ENABLE(ESPI_INTERRUPT_PROVIDER, ESPI_INTERRUPT, ESPI_IntHandler,
                                      ESPI_INTERRUPT_POLARITY, ESPI_INTERRUPT_PRIORITY);

#if defined (MIWU_MODULE_TYPE) && defined (ESPI_MIWU_WAKEUP_SUPPORT)
        MIWU_Config(MIWU_ESPI, MIWU_ANY_EDGE, ESPI_WakeUpHandler_l);
#endif
    }
    else
    {
        INTERRUPT_ENABLE(ESPI_INTERRUPT_PROVIDER, ESPI_INTERRUPT, FALSE);

#if defined (MIWU_MODULE_TYPE) && defined (ESPI_MIWU_WAKEUP_SUPPORT)
        MIWU_EnableChannel(MIWU_ESPI, FALSE);
#endif
    }
#if defined (SHM_MODULE_TYPE) && defined (ESPI_CAPABILITY_PC_NON_FATAL_ERR_IND)
    /*-----------------------------------------------------------------------------------------------------*/
    /* Register callback in SHM module for write/read to protected area module                             */
    /*-----------------------------------------------------------------------------------------------------*/
    SHM_EspiRegisterCallback(ESPI_VW_SmNonFatalError_l);
#endif

#ifdef ESPI_CAPABILITY_SAF
    (void)FLASH_DEV_Init(&ESPI_FLASH_SafFlashParams_L);
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_Config                                                                            */
/*                                                                                                         */
/* Parameters:      ioMode          - I/O mode                                                             */
/*                  maxFreq         - ESPI max frequency                                                   */
/*                  resetOutput     - reset output configuration                                           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs eSPI module initialization                                       */
/*lint -e{715}      Suppress 'resetOutput' not referenced                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_Config (
    ESPI_IO_MODE    ioMode,
    ESPI_MAX_FREQ   maxFreq,
    ESPI_RST_OUT    resetOutput
)
{
    UINT32 var = REG_READ(ESPI_ESPICFG);

    SET_VAR_FIELD(var ,ESPICFG_IOMODE,  ioMode);
    SET_VAR_FIELD(var ,ESPICFG_MAXFREQ, maxFreq);
#ifdef ESPI_CAPABILITY_RESET_OUTPUT
    //lint -e{648} Suppress Overflow in computing constant for operation: 'shift left'
    SET_VAR_FIELD(var ,ESPICFG_RSTO,    resetOutput);
#endif
    REG_WRITE(ESPI_ESPICFG, var);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetOpFreq                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         Operating Frequency.                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the eSPI Master Operating Frequency.                            */
/*---------------------------------------------------------------------------------------------------------*/
ESPI_OP_FREQ ESPI_GetOpFreq (void)
{
    return (ESPI_OP_FREQ)READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_OPFREQ);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetIoModeSelect                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         I/O Mode Select.                                                                       */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the eSPI Master I/O Mode Select.                                */
/*---------------------------------------------------------------------------------------------------------*/
ESPI_IO_MODE_SELECT ESPI_GetIoModeSelect (void)
{
    return (ESPI_IO_MODE_SELECT)READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_IOMODESEL);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetAlertMode                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         Alert Mode.                                                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the eSPI Master Alert Mode.                                     */
/*---------------------------------------------------------------------------------------------------------*/
ESPI_ALERT_MODE ESPI_GetAlertMode (void)
{
    return (ESPI_ALERT_MODE)READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_ALERTMODE);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetCrcCheckEnable                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         CRC Checking Enable.                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the eSPI Master CRC Checking Enable.                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN ESPI_GetCrcCheckEnable (void)
{
    return (BOOLEAN)READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_CRC_CHK_EN);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_ChannelSlaveSupport                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  channel - eSPI channel                                                                 */
/*                  enable  - TRUE to enable the channel support by the slave; FALSE otherwise             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables channels support by the slave                            */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_ChannelSlaveSupport (
    ESPI_CHANNEL_TYPE_T channel,
    BOOLEAN             enable
)
{
    switch (channel)
    {
    case ESPI_CHANNEL_PC:
        SET_REG_FIELD(ESPI_ESPICFG ,ESPICFG_PCCHN_SUPP,     enable);
        break;

    case ESPI_CHANNEL_VW:
        SET_REG_FIELD(ESPI_ESPICFG ,ESPICFG_VWCHN_SUPP,     enable);
        break;

    case ESPI_CHANNEL_FLASH:
        SET_REG_FIELD(ESPI_ESPICFG ,ESPICFG_FLASHCHN_SUPP,  enable);
        break;

    case ESPI_CHANNEL_OOB:
        SET_REG_FIELD(ESPI_ESPICFG ,ESPICFG_OOBCHN_SUPP,    enable);
        break;

    default:
        break;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_ChannelSlaveEnable                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  channel - eSPI channel                                                                 */
/*                  enable  - TRUE to enable the channel by the slave; FALSE otherwise                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables channels by the slave                                    */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_ChannelSlaveEnable (
    ESPI_CHANNEL_TYPE_T channel,
    BOOLEAN             enable
)
{
    switch (channel)
    {
    case ESPI_CHANNEL_PC:
        SET_REG_FIELD(ESPI_ESPICFG, ESPICFG_PCHANEN,        enable);
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_PCHANEN, enable);
        break;

    case ESPI_CHANNEL_VW:
        SET_REG_FIELD(ESPI_ESPICFG, ESPICFG_VWCHANEN,       enable);
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_VWCHANEN, enable);
        break;

    case ESPI_CHANNEL_FLASH:
        SET_REG_FIELD(ESPI_ESPICFG, ESPICFG_FLASHCHANEN,    enable);
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_FLASHCHANEN, enable);
        break;

    case ESPI_CHANNEL_OOB:
        SET_REG_FIELD(ESPI_ESPICFG, ESPICFG_OOBCHANEN,      enable);
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_OOBCHANEN, enable);
        break;

    default:
        break;
    }

    SET_VAR_BIT(ESPI_configUpdateEnable, channel);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_ChannelMasterEnable                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  channel - eSPI channel                                                                 */
/*                                                                                                         */
/* Returns:         TRUE if the channel is enabled by the master; FALSE otherwise                          */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks if an eSPI channel is enabled/disabled by the master               */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN ESPI_ChannelMasterEnable (ESPI_CHANNEL_TYPE_T channel)
{
    BOOLEAN val = FALSE;

    switch (channel)
    {
    case ESPI_CHANNEL_PC:
        val = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_HPCHANEN);
        break;

    case ESPI_CHANNEL_VW:
        val = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_HVWCHANEN);
        break;

    case ESPI_CHANNEL_FLASH:
        val = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_HFLASHCHANEN);
        break;

    case ESPI_CHANNEL_OOB:
        val = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_HOOBCHANEN);
        break;

    default:
        break;
    }

    return val;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_IntEnable                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  channel - eSPI channel                                                                 */
/*                  enable  - TRUE to enable the channel interrupt; FALSE otherwise                        */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables eSPI channels interrupts                                 */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_IntEnable (
    ESPI_CHANNEL_TYPE_T channel,
    BOOLEAN             enable
)
{
    UINT32 var = REG_READ(ESPI_ESPIIE);

    switch (channel)
    {
    case ESPI_CHANNEL_PC:
        /*-------------------------------------------------------------------------------------------------*/
        /* Peripheral channel access detected interrupt                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_PERACCIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Peripheral channel transaction deferred interrupt                                               */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_DFRDIE, enable);
        break;

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
    case ESPI_CHANNEL_PC_BM:

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master transfer done interrupt                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_BMTXDONEIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master buffer received interrupt                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_PBMRXIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master slave message received interrupt                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_PMSGRXIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master burst error interrupt                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_BMBURSTERRIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master burst done interrupt                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_BMBURSTDONEIE, enable);
        break;
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

    case ESPI_CHANNEL_VW:
        /*-------------------------------------------------------------------------------------------------*/
        /* Virtual wire updated interrupt                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_VWUPDIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* PLTRST virtual wire was activated interrupt                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_PLTRSTIE, enable);

#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
        /*-------------------------------------------------------------------------------------------------*/
        /* VWx floating virtual wire event                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_VW1IE, enable);
        SET_VAR_FIELD(var, ESPIIE_VW2IE, enable);
        SET_VAR_FIELD(var, ESPIIE_VW3IE, enable);
        SET_VAR_FIELD(var, ESPIIE_VW4IE, enable);
#endif
        break;

    case ESPI_CHANNEL_OOB:
        /*-------------------------------------------------------------------------------------------------*/
        /* OOB data received interrupt                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_OOBRXIE, enable);
        break;

    case ESPI_CHANNEL_FLASH:
        /*-------------------------------------------------------------------------------------------------*/
        /* Flash data received interrupt                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_FLASHRXIE, enable);

        if (READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_FLASHCHANMODE) == ESPI_FLASH_ACCESS_SLAVE_ATTCH)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Slave attach flash access channel read detected interrupt                                   */
            /*---------------------------------------------------------------------------------------------*/
            SET_VAR_FIELD(var, ESPIIE_SFLASHRDIE, enable);
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Automatic mode transfer error interrupt                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_AMERRIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Automatic mode transfer done interrupt                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_AMDONEIE, enable);
        break;

    case ESPI_CHANNEL_GENERIC:
        /*-------------------------------------------------------------------------------------------------*/
        /* In band reset command received interrupt                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_IBRSTIE,  enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* ESPI configuration update interrupt                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_CFGUPDIE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* ESPI bus error interrupt                                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_BERRIE,   enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* ESPI reset activated interrupt                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIIE_ESPIRSTIE, enable);
        break;

    case ESPI_CHANNEL_ALL:
        var = enable ? MASK_FIELD(ESPIIE_ALL) : 0x0;
        break;

    default:
        break;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure interrupt enable                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPIIE, var);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_WakeUpEnable                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  channel - eSPI channel                                                                 */
/*                  enable  - TRUE to enable the channel wake up; FALSE otherwise                          */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables eSPI channels wake up                                    */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_WakeUpEnable (
    ESPI_CHANNEL_TYPE_T channel,
    BOOLEAN             enable
)
{
    UINT32 var = REG_READ(ESPI_ESPIWE);

    switch (channel)
    {
    case ESPI_CHANNEL_PC:
        /*-------------------------------------------------------------------------------------------------*/
        /* Peripheral channel access detected interrupt                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_PERACCWE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Peripheral channel transaction deferred interrupt                                               */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_DFRDWE, enable);
        break;

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
    case ESPI_CHANNEL_PC_BM:
        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master buffer received wake up                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_PBMRXWE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master slave message received wake up                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_PMSGRXWE, enable);
        break;
#endif

    case ESPI_CHANNEL_VW:
        /*-------------------------------------------------------------------------------------------------*/
        /* Virtual wire updated interrupt                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_VWUPDWE, enable);
        break;

    case ESPI_CHANNEL_OOB:
        /*-------------------------------------------------------------------------------------------------*/
        /* OOB data received interrupt                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_OOBRXWE, enable);
        break;

    case ESPI_CHANNEL_FLASH:
        /*-------------------------------------------------------------------------------------------------*/
        /* Flash data received interrupt                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_FLASHRXWE, enable);

        if (READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_FLASHCHANMODE) == ESPI_FLASH_ACCESS_SLAVE_ATTCH)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Slave attach flash access channel read detected interrupt                                   */
            /*---------------------------------------------------------------------------------------------*/
            SET_VAR_FIELD(var, ESPIWE_SFLASHRDWE, enable);
        }
        break;

    case ESPI_CHANNEL_GENERIC:
        /*-------------------------------------------------------------------------------------------------*/
        /* In band reset command received wake up                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_IBRSTWE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* ESPI configuration update wake up                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_CFGUPDWE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* ESPI bus error wake up                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, ESPIWE_BERRWE, enable);

        /*-------------------------------------------------------------------------------------------------*/
        /* ESPI reset activated wake up                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_ESPI_RST_WAKEUP_SUPPORT
        SET_VAR_FIELD(var, ESPIWE_ESPIRSTWE, enable);
#endif
        break;

    case ESPI_CHANNEL_ALL:
        var = enable ? MASK_FIELD(ESPIWE_ALL) : 0x0;
        break;

    default:
        break;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure wake-up enable                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPIWE, var);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_IntHandler                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs eSPI module initialization                                       */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_IntHandler (void)
{
    UINT32 intEnable;
    UINT32 statusMask;
    UINT32 status;

    intEnable   = REG_READ(ESPI_ESPIIE);
#ifdef ESPI_CAPABILITY_VW_WAKEUP
    statusMask  = intEnable | REG_READ(ESPI_ESPIWE) | MASK_FIELD(ESPISTS_VWUPDW);
#else
    statusMask  = intEnable | REG_READ(ESPI_ESPIWE);
#endif
    status      = REG_READ(ESPI_ESPISTS) & statusMask;

    while (status != 0)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Clear handled interrupts                                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(ESPI_ESPISTS, status);

        /*-------------------------------------------------------------------------------------------------*/
        /* An eSPI bus error is detected                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_BERR))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Get error and clean relevant bits                                                           */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_GetError_l(ESPI_OOB_ESPIERR_MASK);

#ifdef ESPI_CAPABILITY_OOB_BERR_RESET_BUFFERS_PTR // TODO: consider remove capability and leave it as a code change because bus error handling was not mentioned in spec before
            /*---------------------------------------------------------------------------------------------*/
            /* Reset the pointers of the OOB Transmit and Receive buffers                                  */
            /*---------------------------------------------------------------------------------------------*/
            SET_REG_FIELD(ESPI_OOBCTL, OOBCTL_RSTBUFHEADS ,0x01);
#endif
            if (READ_VAR_FIELD(intEnable, ESPIIE_BERRIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_BUS_ERR));
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* eSPI Reset is activated                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_ESPIRST))
        {
            ESPI_Reset_l();

            if (READ_VAR_FIELD(intEnable, ESPIIE_ESPIRSTIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_RST_ACTIVE));
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* PLTRST Virtual Wire is asserted                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_PLTRST))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_PLTRSTIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_PLTRST_ACTIVE));
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* The eSPI configuration is updated                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_CFGUPD))
        {
            ESPI_ConfigUpdate_l();

            if (READ_VAR_FIELD(intEnable, ESPIIE_CFGUPDIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_CONFIG_UPDATE));
            }

            ESPI_ResetConnfigUpdate_l();
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* The Master Attached Flash Access Channel data is received                                       */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_FLASHRX))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_FLASHRXIE))
            {
#ifdef ESPI_CAPABILITY_SAF
                if (READ_REG_FIELD(ESPI_ESPICFG ,ESPICFG_FLASHCHANMODE) == ESPI_FLASH_MODE_SAF)
                {
                    ESPI_FLASH_SafPendingIncomingReq_L = TRUE;
                }
                else
#endif
                {
                    ESPI_FLASH_HandleReqRes_l();
                }
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Automatic Mode Transfer Done                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_AMDONE))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_AMDONEIE))
            {
#ifdef GDMA_MODULE_TYPE
                ESPI_FLASH_AutoModeTransDoneHandler_l(ESPI_GDMA_MODULE, ESPI_FLASH_GDMA_CHANNEL, status, intEnable);
#endif
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* OOB Channel message data is received                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_OOBRX))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_OOBRXIE))
            {
                if (ESPI_OOB_ReceiveCmd_l() == DEFS_STATUS_OK)
                {
                    EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_OOB_DATA_RCV));
                }
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Any Master-to-Slave Virtual Wire is updated by the Host                                         */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_VWUPD))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_VWUPDIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_VW_UPDATE));

                if (ESPI_VW_Handler_L != NULL)
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* Check whether the updated VW is event and handle it                                 */
                    /*-------------------------------------------------------------------------------------*/
                    ESPI_VW_HandleInputEvent_l();

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
                    /*-------------------------------------------------------------------------------------*/
                    /* Check whether the updated VW is GPIO and handle it                                  */
                    /*-------------------------------------------------------------------------------------*/
                    ESPI_VW_HandleInputGpio_l();
#endif
                }
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* A Peripheral Channel transaction is received                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_PERACC))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_PERACCIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_PC_ACCESS_DETECTED));
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Peripheral Channel transaction is deferred                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_DFRD))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_DFRDIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_PC_TRANS_DEFFERED));
            }
        }

#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
        /*-------------------------------------------------------------------------------------------------*/
        /* Any of four general purpose Slave-to-Master Virtual Wire floating events occurs (for future     */
        /* use)                                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_VW1))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_VW1IE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_VW1_EVENT));
            }
        }

        if (READ_VAR_FIELD(status, ESPISTS_VW2))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_VW2IE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_VW2_EVENT));
            }
        }

        if (READ_VAR_FIELD(status, ESPISTS_VW3))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_VW3IE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_VW3_EVENT));
            }
        }

        if (READ_VAR_FIELD(status, ESPISTS_VW4))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_VW4IE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_VW4_EVENT));
            }
        }
#endif // ESPI_CAPABILITY_VW_FLOATING_EVENTS

        /*-------------------------------------------------------------------------------------------------*/
        /* A Slave Attached Flash Access Channel read transaction is detected                              */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_SFLASHRD))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_SFLASHRDIE))
            {
#ifdef ESPI_CAPABILITY_SAF
                if ((ESPI_FLASH_SafReqInfo_L.reqType == ESPI_FLASH_SAF_REQ_READ) &&
                    (ESPI_FLASH_SafReqInfo_L.reminder > 0))
                {
                    ESPI_FLASH_SafPendingIncomingRes_L = TRUE;
                }
#endif
                EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_FLASH_READ_ACCESS_DETECTED));
                EXECUTE_FUNC(ESPI_FLASH_userIntHandler_L,   (ESPI_INT_FLASH_READ_ACCESS_DETECTED));
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* An in-band RESET Command is received                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_IBRST))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_IBRSTIE))
            {
                EXECUTE_FUNC(ESPI_userIntHandler_L, (ESPI_INT_IB_RST_CMD_RCV));
            }
        }

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master burst done, This bit is set to 1 when all the 64-byte packets selected by BMBURSTSIZE*/
        /* field in PERCTL register were received and the last buffer contents were read by the Core       */
        /* (useful when using DMA). Default = 0                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_BMBURSTDONE) == 0x01)
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_BMBURSTDONEIE))
            {
                ESPI_PC_BM_AutoModeTransDoneHandler_l(status, intEnable);

            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master Message buffer was received                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_PMSGRX))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_PMSGRXIE))
            {
                ESPI_PC_BM_HandleInMessage_l();
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master Tx buffer data was sent to the Master                                                */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_BMTXDONE))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_BMTXDONEIE))
            {
                ESPI_PC_BM_usrReqInfo_L.buffTransferred++;
                if (ESPI_PC_BM_usrReqInfo_L.reqType == ESPI_PC_MSG_WRITE)
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* Send next request if needed                                                         */
                    /*-------------------------------------------------------------------------------------*/
                    ESPI_PC_BM_HandleReqRes_l();
                }

                if (ESPI_PC_BM_usrReqInfo_L.reqType == ESPI_PC_MSG_LTR)
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* Update status                                                                       */
                    /*-------------------------------------------------------------------------------------*/
                    ESPI_PC_BM_status_L = ESPI_PC_BM_usrReqInfo_L.status;
                }
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Bus Master Completion buffer was received                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(status, ESPISTS_PBMRX))
        {
            if (READ_VAR_FIELD(intEnable, ESPIIE_PBMRXIE))
            {
                ESPI_PC_BM_HandleReqRes_l();
            }
        }
#endif //ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

        /*-------------------------------------------------------------------------------------------------*/
        /* Read status again in case new eSPI events occurred                                              */
        /*-------------------------------------------------------------------------------------------------*/
        status = REG_READ(ESPI_ESPISTS) & statusMask;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetError                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         eSPI error Mask.                                                                       */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the last eSPI error occurred.                                   */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 ESPI_GetError (void)
{
    return ESPI_errorMask;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetConfigUpdate                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         eSPI Config Update Enable Mask.                                                        */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the last eSPI configuration updated change.                     */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 ESPI_GetConfigUpdateEnable (void)
{
    return ESPI_configUpdateEnable;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetConfigUpdate                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         eSPI Config Update Mask.                                                               */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the last eSPI configuration updated.                            */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 ESPI_GetConfigUpdate (void)
{
    return ESPI_configUpdateMask;
}

#ifdef ESPI_CAPABILITY_ESPI_RST_LEVEL_INDICATION
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_InReset                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         TRUE if eSPI is in reset, FALSE otherwise                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves TRUE if eSPI is in reset, FALSE otherwise                       */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN ESPI_InReset (void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* ESPI reset level                                                                                    */
    /* Indicates the current level of the eSPI_RST signal.                                                 */
    /* 0: eSPI_RST# signal is low - asserted (default).                                                    */
    /* 1: eSPI_RST# signal is high - deasserted.                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    return (!READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_ESPIRST_LVL)) ;
}
#endif

#ifdef ESPI_CAPABILITY_AUTO_HANDSHAKE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_ConfigAutoHandshake                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  AutoHsCfg - Automatic Handshake Configuration mask.                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the eSPI auto handshake mode.                                  */
/*                  This allows the Host to control some of the Virtual Wires and the "Channel Ready" bits */
/*                  for all channels, independently of the Core operation (i.e., by hardware).             */
/*                                                                                                         */
/*                  'AutoHsCfg' should be an OR mask of ESPI_AUTO_HANDSHAKE and ESPI_AUTO_HS_DELAY types.  */
/*                  e.g.: To set the VW Channel Ready and HOST_RST_ACK to 512 cycles, 'AutoHsCfg' is:      */
/*                  ESPI_AUTO_VWCRDY | ESPI_AUTO_HS1 | ESPI_AUTO_HS_DELAY(ESPI_HS1, ESPI_HS_DELAY_512)     */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_ConfigAutoHandshake (UINT32 AutoHsCfg)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Set eSPI Host Independence (Configure Automatic Handshake)                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPIHINDP, AutoHsCfg);
}
#endif

#ifdef ESPI_CAPABILITY_HOST_STATUS
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_IsReadyForHostReset                                                               */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         TRUE if module is ready for host platform reset and FALSE otherwise                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns TRUE if module is ready for host platform reset and FALSE         */
/*                  otherwise                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN ESPI_IsReadyForHostReset (void)
{
    return (REG_READ(ESPI_STATUS_IMG) == STATUS_IMG_RST_VAL);
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_PrintRegs (void)
{
    UINT i;

    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*     ESPI     */\n");
    HAL_PRINT("/*--------------*/\n\n");

    HAL_PRINT("ESPIID              = 0x%08X\n", REG_READ(ESPI_ESPIID));
    HAL_PRINT("ESPICFG             = 0x%08X\n", REG_READ(ESPI_ESPICFG));
    HAL_PRINT("ESPISTS             = 0x%08X\n", REG_READ(ESPI_ESPISTS));
    HAL_PRINT("ESPIIE              = 0x%08X\n", REG_READ(ESPI_ESPIIE));
    HAL_PRINT("ESPIWE              = 0x%08X\n", REG_READ(ESPI_ESPIWE));
    HAL_PRINT("VWREGIDX            = 0x%08X\n", REG_READ(ESPI_VWREGIDX));
    HAL_PRINT("VWREGDATA           = 0x%08X\n", REG_READ(ESPI_VWREGDATA));
    HAL_PRINT("OOBRXRDHEAD         = 0x%08X\n", REG_READ(ESPI_OOBRXRDHEAD));
    HAL_PRINT("FLASHRXRDHEAD       = 0x%08X\n", REG_READ(ESPI_FLASHRXRDHEAD));
    HAL_PRINT("FLASHCRC            = 0x%08X\n", REG_READ(ESPI_FLASHCRC));
    HAL_PRINT("FLASHCFG            = 0x%08X\n", REG_READ(ESPI_FLASHCFG));
    HAL_PRINT("FLASHCTL            = 0x%08X\n", REG_READ(ESPI_FLASHCTL));
    HAL_PRINT("ESPIERR             = 0x%08X\n", REG_READ(ESPI_ESPIERR));
#ifdef ESPI_CAPABILITY_AUTO_HANDSHAKE
    HAL_PRINT("ESPIHINDP           = 0x%08X\n", REG_READ(ESPI_ESPIHINDP));
#endif

    for (i = 0; i < ESPI_VWEVSM_NUM; i++)
    {
        HAL_PRINT("VWEeVSM%d%*s           = 0x%08X\n", i, (UINT)(i<10), "", REG_READ(ESPI_VWEVSM(i)));
    }
    for (i = 0; i < ESPI_VWEVMS_NUM; i++)
    {
        HAL_PRINT("VWEVSM%d%*s            = 0x%08X\n", i, (UINT)(i<10), "", REG_READ(ESPI_VWEVMS(i)));
    }
#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
    HAL_PRINT("VWPING              = 0x%08X\n", REG_READ(ESPI_VWPING));
#endif
    HAL_PRINT("VWCTL               = 0x%08X\n", REG_READ(ESPI_VWCTL));

    for (i = 0; i < ESPI_OOBRXBUF_NUM; i++)
    {
        HAL_PRINT("OOBRXBUF%d%*s          = 0x%08X\n",  i, (UINT)(i<10), "", REG_READ(ESPI_OOBRXBUF(i)));
    }
    HAL_PRINT("OOBCTL              = 0x%08X\n", REG_READ(ESPI_OOBCTL));

    for (i = 0; i < ESPI_FLASHRXBUF_NUM; i++)
    {
        HAL_PRINT("FLASHRXBUF%d%*s        = 0x%08X\n",  i, (UINT)(i<10), "", REG_READ(ESPI_FLASHRXBUF(i)));
    }
#ifdef ESPI_CAPABILITY_SAF
    for (i = 0; i < ESPI_FLASH_SAF_PROT_MEM_NUM; i++)
    {
        HAL_PRINT("PRTR_BADDR%d%*s        = 0x%08X\n",  i, (UINT)(i<10), "", REG_READ(ESPI_FLASH_PRTR_BADDRn(i)));
    }

    for (i = 0; i < ESPI_FLASH_SAF_PROT_MEM_NUM; i++)
    {
        HAL_PRINT("PRTR_HADDR%d%*s        = 0x%08X\n",  i, (UINT)(i<10), "", REG_READ(ESPI_FLASH_PRTR_HADDRn(i)));
    }

    for (i = 0; i < ESPI_FLASH_SAF_TAG_RANGE_NUM; i++)
    {
        HAL_PRINT("FLASH_RGN_TAG_OVR%d%*s = 0x%08X\n",  i, (UINT)(i<10), "", REG_READ(ESPI_FLASH_FLASH_RGN_TAG_OVRn(i)));
    }

    for (i = 0; i < ESPI_FLASH_SAF_BUFF_NUM; i++)
    {
        HAL_PRINT("FLASHRDTXBUF%d%*s      = 0x%08X\n",  i, (UINT)(i<10), "", REG_READ(ESPI_FLASHRDTXBUF(i)));
    }
#endif

    HAL_PRINT("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PrintVersion                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_PrintVersion (void)
{
    HAL_PRINT("ESPI        = %X\n", MODULE_VERSION(ESPI_MODULE_TYPE));
}


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                 PERIPHERAL CHANNEL INTERFACE FUNCTIONS                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_Init                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  hostToSlaveMsgHandler - user callback to use when a new message from host is available */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialize PC bus mastering protocol                                      */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_PC_BM_Init (ESPI_PC_BM_MSG_HANDLER hostToSlaveMsgHandler)
{
    ESPI_PC_BM_userMsgHandler_L     = hostToSlaveMsgHandler;
    ESPI_PC_BM_sysRAMis64bAddr_L    = FALSE;
    ESPI_PC_BM_userIntHandler_L     = NULL;
    ESPI_PC_BM_status_L             = DEFS_STATUS_OK;
    ESPI_PC_BM_currReqTag_L         = 0;
    ESPI_PC_BM_RXIE_L               = READ_REG_FIELD(ESPI_ESPIIE, ESPIIE_PBMRXIE);
    ESPI_PC_BM_TXDONE_L             = READ_REG_FIELD(ESPI_ESPIIE, ESPIIE_BMTXDONEIE);
    memset(&ESPI_PC_BM_usrReqInfo_L, 0, sizeof(ESPI_PC_BM_usrReqInfo_L));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify host configurations                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify bus Master transactions are enabled.                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((READ_REG_FIELD(ESPI_PERCFG, PERCFG_BMMEN) == TRUE), DEFS_STATUS_FAIL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify Peripheral Channel Maximum Payload Size field in the Host                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((READ_REG_FIELD(ESPI_PERCFG, PERCFG_PERPLSIZE) == ESPI_PC_BM_MAX_PAYLOAD_SIZE_64B), DEFS_STATUS_FAIL);

#if 0 // No need cause the host can support bigger size but the driver will not request bigger size
    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify Peripheral Channel Maximum Read Request Size                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((READ_REG_FIELD(ESPI_PERCFG, PERCFG_BMREQSIZE) == ESPI_PC_BM_MAX_READ_REQ_SIZE_64B), DEFS_STATUS_FAIL);
#endif

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_Config                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sysRAMis64bAddr  - equals TRUE to configure 64 bit addressing for system RAM and FALSE */
/*                                     otherwise                                                           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures PC bus mastering protocol                                      */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_PC_BM_Config (BOOLEAN sysRAMis64bAddr)
{
    ESPI_PC_BM_sysRAMis64bAddr_L = sysRAMis64bAddr;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_RegisterCallback                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  handler    - user interrupt handler of PC bus mastering protocol                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine register PC bus mastering protocol interrupt handler                      */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_PC_BM_RegisterCallback (ESPI_INT_HANDLER handler)
{
    ESPI_PC_BM_userIntHandler_L = handler;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_SendReq                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType      - request type                                                            */
/*                  offset       - pointer to offset in system RAM (in 32b addressing it will be 32bit     */
/*                                 address and in 64b addressing it will be 64bit address) or message      */
/*                                 header specific(4B)                                                     */
/*                  size         - size of transaction in case of read/write. size of packet data in case  */
/*                                 of message                                                              */
/*                  pollingMode  - equals TRUE if polling is required and FALSE for interrupt mode         */
/*                  buffer       - input/output buffer                                                     */
/*                  strpHdr      - equals TRUE if strip header is required and FALSE otherwise             */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends read/write/message request to host via PC bus mastering protocol    */
/*                                                                                                         */
/*                  limitations under request type ESPI_PC_REQ_READ_AUTO and ESPI_PC_REQ_READ_DMA:         */
/*                      1. Offset must be aligned to 64B                                                   */
/*                      2. Size must be aligned to 64B.                                                    */
/*                      3. Maximum size for transaction is (64B*256)                                       */
/*                  * If using request type ESPI_PC_REQ_READ_DMA the buffer must be at least               */
/*                    4B aligned , 16B aligned will improve performance.                                   */
/*                  * If polling mode == TRUE, PC interrupts should be disabled by the user                */
/*                    If polling mode == FALSE, PC interrupts should be enabled by the user                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_PC_BM_SendReq (
    ESPI_PC_BM_REQ_T    reqType,
    UINT64              offset,
    UINT32              size,
    BOOLEAN             pollingMode,
    UINT32*             buffer,
    BOOLEAN             strpHdr
)
{
    BOOLEAN useDMA  = FALSE;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error check                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    if (reqType < ESPI_PC_BM_REQ_MSG_LTR)
    {
        DEFS_STATUS_COND_CHECK((size > 0), DEFS_STATUS_INVALID_DATA_FIELD);
    }

    switch (reqType)
    {
    case ESPI_PC_BM_REQ_READ_MANUAL:
        /*-------------------------------------------------------------------------------------------------*/
        /* Strip header option must be used                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((strpHdr == TRUE), DEFS_STATUS_INVALID_PARAMETER);

        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_ReqManual_l(ESPI_PC_MSG_READ, offset, size, pollingMode, buffer, strpHdr));
        break;

    case ESPI_PC_BM_REQ_WRITE:
        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_ReqManual_l(ESPI_PC_MSG_WRITE, offset, size, pollingMode, buffer, strpHdr));
        break;

    case ESPI_PC_BM_REQ_MSG_LTR:
        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_ReqManual_l(ESPI_PC_MSG_LTR, offset, size, pollingMode, buffer, strpHdr));
        break;

    case ESPI_PC_BM_REQ_READ_DMA:
        useDMA = TRUE;

        /*-------------------------------------------------------------------------------------------------*/
        /* Strip header option must be used                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((strpHdr == TRUE), DEFS_STATUS_INVALID_PARAMETER);

        /*-------------------------------------------------------------------------------------------------*/
        /* Alignment restrictions                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK(((UINT32)buffer % _4B_ == 0) && ((UINT32)offset % _4B_ == 0), DEFS_STATUS_FAIL);
        /*lint -fallthrough */

    case ESPI_PC_BM_REQ_READ_AUTO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Error check                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK(((size % ESPI_PC_MAX_PAYLOAD_SIZE) == 0), DEFS_STATUS_INVALID_PARAMETER);

        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_ReadReqAuto_l(offset, size, pollingMode, buffer, strpHdr, useDMA));
        break;

    default:
        break;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_GetStatus                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns Peripheral channel bus mastering protocol status                  */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_PC_BM_GetStatus (void)
{
    return ESPI_PC_BM_status_L;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_SetLTRMsgHdr                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  req           - FALSE indicates that eSPI slave has no service                         */
/*                                  requirement. When this bit is TURE, the remaining fields are valid to  */
/*                                  indicate latency tolerance requirement for the eSPI slave.             */
/*                  latencyScale  - This is the multiplier to the Latency Value                            */
/*                                  (LV[9:0]) field to yield an absolute time value for the latency        */
/*                                  tolerance.                                                             */
/*                  latencyValue  - Along with the Latency Scale (LS[2:0]) field,                          */
/*                                  this specifies the service latency that tolerable by the eSPI slave.   */
/*                  msgHdePtr     - Pointer to header buffer                                               */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine fills LTR message header according to input params                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_PC_BM_SetLTRMsgHdr (
    BOOLEAN                         req,
    ESPI_PC_BM_LTR_LATENCY_SCALE_T  latencyScale,
    UINT16                          latencyValue,
    UINT32*                         msgHdrPtr
)
{
    ESPI_PC_BM_LTR_MSG_HDR_T* header = (ESPI_PC_BM_LTR_MSG_HDR_T*)(void*)msgHdrPtr;

    header->rqLsLv8_9   = 0;
    header->reserved[0] = 0;
    header->reserved[1] = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill header fields according to input params                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(header->rqLsLv8_9, ESPI_PC_BM_LTR_RQ, req);
    SET_VAR_FIELD(header->rqLsLv8_9, ESPI_PC_BM_LTR_LS, latencyScale);
    SET_VAR_FIELD(header->rqLsLv8_9, ESPI_PC_BM_LTR_LV, READ_VAR_FIELD(latencyValue, ESPI_PC_BM_LTR_LV8_9));
    header->lv0_7 = READ_VAR_FIELD(latencyValue, ESPI_PC_BM_LTR_LV0_7);

    return DEFS_STATUS_OK;
}

#ifdef ESPI_PC_BM_SELF_TEST
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_SelfTestMsgHandler                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  msgCode  - Message code                                                                */
/*                  msgBuff  - Message buffer                                                              */
/*                  msgLen   - Message length                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles message from host                                                 */
/*lint -e{715}      Suppress 'msgCode/msgBuff/msgLen' not referenced                                       */
/*lint -e{818}      Suppress 'msgBuff' could be declared as pointing to const                              */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_PC_BM_SelfTestMsgHandler (UINT8 msgCode, UINT8* msgBuff, UINT32 msgLen)
{
    // TODO:
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_SelfTestIntHandler                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intType   - interrupt type                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles user interrupt                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_PC_BM_SelfTestIntHandler (ESPI_INT_T intType)
{
    if ((intType == ESPI_INT_PC_BM_BURST_MODE_TRANS_DONE) ||
        (intType == ESPI_INT_PC_BM_MSG_DATA_RCV) ||
        (intType == ESPI_INT_PC_BM_MASTER_DATA_TRANSMITTED))
    {
        ESPI_PC_BM_selfTestStatus_L = DEFS_STATUS_OK;
    }
    if (intType == ESPI_INT_PC_BM_BURST_MODE_TRANS_ERR)
    {
        ESPI_PC_BM_selfTestStatus_L = DEFS_STATUS_FAIL;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_SelfTestInit                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialize the self test                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_PC_BM_SelfTestInit (void)
{
    (void)ESPI_PC_BM_Init(ESPI_PC_BM_SelfTestMsgHandler);
    ESPI_PC_BM_RegisterCallback(ESPI_PC_BM_SelfTestIntHandler);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_SelfTest                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns Peripheral channel bus mastering protocol status                  */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_PC_BM_SelfTest (
    ESPI_PC_BM_REQ_T    req,
    UINT64              offset,
    UINT32              size,
    BOOLEAN             polling,
    UINT32*             readBuffer,
    UINT32*             writeBuffer,
    BOOLEAN             strpHdr,
    BOOLEAN             randomData
)
{
    UINT8*                  writeBufferPtr  = (UINT8*)writeBuffer;
    UINT8*                  readBufferPtr   = (UINT8*)readBuffer;
    volatile DEFS_STATUS    status          = DEFS_STATUS_OK;
    UINT32                  i;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Disable/enable interrupts                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_IntEnable(ESPI_CHANNEL_PC_BM, (BOOLEAN)!polling);

    switch (req)
    {
    case ESPI_PC_BM_REQ_MSG_LTR:
        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_SetLTRMsgHdr(TRUE, ESPI_PC_BM_LTR_LATENCY_SCALE_1NS, 0, writeBuffer));
        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_SendReq(req, (UINT64)writeBuffer[0], 0, polling, NULL, strpHdr));

        /*-------------------------------------------------------------------------------------------------*/
        /* Wait till transaction ends                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        if (!polling)
        {
            do
            {
                status = ESPI_PC_BM_GetStatus();
            }while (status == DEFS_STATUS_SYSTEM_BUSY);
        }
        break;

    case ESPI_PC_BM_REQ_WRITE:
        //Do nothing, there is no point in testing only write
        break;

    case ESPI_PC_BM_REQ_READ_MANUAL:
    case ESPI_PC_BM_REQ_READ_AUTO:
    case ESPI_PC_BM_REQ_READ_DMA:
        /*-------------------------------------------------------------------------------------------------*/
        /* Fill buffer                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        for (i = 0 ; i < size; i++)
        {
            if (randomData)
            {
                writeBufferPtr[i] = (UINT8)(rand() % 0x100); // TODO: add srand
            }
            else
            {
                writeBufferPtr[i] = i % 0x100;
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Write buffer                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_SendReq(ESPI_PC_BM_REQ_WRITE, offset, size, polling, writeBuffer, strpHdr));

        /*-------------------------------------------------------------------------------------------------*/
        /* Wait till transaction ends                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        if (!polling)
        {
            do
            {
                status = ESPI_PC_BM_GetStatus();
            }while (status == DEFS_STATUS_SYSTEM_BUSY);
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Read buffer                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_PC_BM_selfTestStatus_L = DEFS_STATUS_SYSTEM_BUSY;
        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_SendReq(req, offset, size, polling, readBuffer, strpHdr));

        /*-------------------------------------------------------------------------------------------------*/
        /* Wait till transaction ends                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        if (!polling)
        {
            do
            {
                status = ESPI_PC_BM_GetStatus();
            }while (status == DEFS_STATUS_SYSTEM_BUSY);

            DEFS_STATUS_RET_CHECK(ESPI_PC_BM_selfTestStatus_L);
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Compare read and write buffer                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        for (i = 0 ; i < size; i++)
        {
            if (writeBufferPtr[i] != readBufferPtr[i])
            {
                return DEFS_STATUS_FAIL;
            }
        }
        break;

    default:
        break;
    }
    return DEFS_STATUS_OK;
}

#endif //ESPI_PC_BM_SELF_TEST
#endif //ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                    VIRTUAL WIRES INTERFACE FUNCTIONS                                    */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_Init                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  vwHandler    -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ...                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_VW_Init (ESPI_VW_HANDLER_T vwHandler)
{
    ESPI_VW_Handler_L = vwHandler;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure non fatal error wire                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)ESPI_VW_ConfigWire(ESPI_VW_ERROR_NONFATAL, TRUE, ESPI_VW_WIRE_CTRL_SW,
                             ESPI_VW_WIRE_TRIGGER_EDGE, ESPI_VW_WIRE_POL_ACTIVE_HIGH);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure fatal error wire                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)ESPI_VW_ConfigWire(ESPI_VW_ERROR_FATAL, TRUE, ESPI_VW_WIRE_CTRL_SW,
                             ESPI_VW_WIRE_TRIGGER_EDGE, ESPI_VW_WIRE_POL_ACTIVE_HIGH);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_Config                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  intWinSel     -                                                                        */
/*                  gpioIndMap    -                                                                        */
/*                  irqDis        -                                                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ...                                                                       */
/*lint -e{715}      Suppress 'gpioIndMap/irqDis' not referenced                                            */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_VW_Config (
    ESPI_VW_INT_WIN_SEL     intWinSel,
    ESPI_VW_GPIO_IND_MAP    gpioIndMap,
    BOOLEAN                 irqDis
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure interrupt window                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_VWCTL, VWCTL_INTWIN, intWinSel);

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure GPIO mapping                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_VWCTL, VWCTL_GPVWMAP, gpioIndMap);
#endif

#ifdef ESPI_CAPABILITY_VW_MASTER_IRQ_UPD
    /*-----------------------------------------------------------------------------------------------------*/
    /* When set to 1, one of the following is performed:                                                   */
    /*      * If the IRQ is already asserted when the IRQ is enabled (either by enabling the module or by  */
    /*       assigning a non-zero interrupt number), the Master is updated on the current IRQ status.      */
    /*       Master updating is performed, according to the Interrupt Type selected, i.e. an assertion is  */
    /*       sent for "level" type and both an assertion and a deassertion are sent for "edge" type.       */
    /*      * For a "level" type IRQ, if an IRQ assertion has already been sent to the Master when the IRQ */
    /*       is disabled (either by disabling the module or by setting the interrupt number to 0h), the    */
    /*       eSPI_SIF updates the Master on the IRQ being disabled, by sending an IRQ deassertion.         */
    /*       (No deassertion  is sent for an "edge" type IRQ.)                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_VWCTL, VWCTL_IRQ_DIS, irqDis);
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_ConfigIndexOut                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index       - index of the VW                                                          */
/*                  indexEn     - TRUE if index should be enabled and false otherwise                      */
/*                  pltrstEn    - TRUE if index should be reset on platform reset and false otherwise      */
/*                  cdrstEn     - TRUE if index should be reset on Core reset and false otherwise          */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures an outgoing Virtual Wire group                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_ConfigIndexOut (
    UINT8   index,
    BOOLEAN indexEn,
    BOOLEAN pltrstEn,
    BOOLEAN cdrstEn
)
{
    ESPI_VW_TYPE_T  type;
    UINT8           i;
    UINT32          var;

    DEFS_STATUS_RET_CHECK(ESPI_VW_GetType_l(index, &type));

    switch (type)
    {
    case ESPI_VW_TYPE_INT_EV:
        break;

    case ESPI_VW_TYPE_SYS_EV:
    case ESPI_VW_TYPE_PLT:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve Event register number                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetReg_l(index, ESPI_VW_SM, &i));

        var = REG_READ(ESPI_VWEVSM(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure index enable                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_INDEX_EN, indexEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure the Wire 3-0 bits to be reset when PLTRST is asserted                                 */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_ENPLTRST, pltrstEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure the Wire 3-0 bits to be reset when Core Reset is asserted                             */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_ENCDRST, cdrstEn);

        REG_WRITE(ESPI_VWEVSM(i), var);
        break;

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    case ESPI_VW_TYPE_GPIO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve GPIO register number                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetGpio_l(index, ESPI_VW_SM, &i));

        var = REG_READ(ESPI_VWGPSM(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure index enable                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWGPSM_INDEX_EN, indexEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure the Wire 3-0 bits to be reset when PLTRST is asserted                                 */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWGPSM_ENPLTRST, pltrstEn);

        REG_WRITE(ESPI_VWGPSM(i), var);
        break;
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT

    case ESPI_VW_TYPE_NUM:
    default:
        break;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_ConfigIndexIn                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index       - index of the VW                                                          */
/*                  indexEn     - TRUE if index should be enabled and false otherwise                      */
/*                  pltrstEn    - TRUE if index should be reset on platform reset and false otherwise      */
/*                  espirstEn   - TRUE if index should be reset on ESPI reset and false otherwise          */
/*                  intEn       - TRUE if interrupt should be enabled and false otherwise                  */
/*                  wakeupEn    - TRUE if wake up should be enabled and false otherwise                    */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures an incoming Virtual Wire group                                 */
/*lint -e{715}      Suppress 'wakeupEn' not referenced                                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_ConfigIndexIn (
    UINT8   index,
    BOOLEAN indexEn,
    BOOLEAN pltrstEn,
    BOOLEAN espirstEn,
    BOOLEAN intEn,
    BOOLEAN wakeupEn
)
{
    ESPI_VW_TYPE_T  type;
    UINT8           i;
    UINT32          var;

    DEFS_STATUS_RET_CHECK(ESPI_VW_GetType_l(index, &type));

    switch (type)
    {
    case ESPI_VW_TYPE_INT_EV:
        break;

    case ESPI_VW_TYPE_SYS_EV:
    case ESPI_VW_TYPE_PLT:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve Event register number                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetReg_l(index, ESPI_VW_MS, &i));

        var = REG_READ(ESPI_VWEVMS(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure index enable                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVMS_INDEX_EN, indexEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure the Wire 3-0 bits to be reset when PLTRST is asserted                                 */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVMS_ENPLTRST, pltrstEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure the Wire 3-0 bits to be reset when eSPI_RST is asserted                               */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVMS_ENESPIRST, espirstEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure interrupt Enable                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVMS_IE, intEn);

#ifdef ESPI_CAPABILITY_VW_WAKEUP
        /*-------------------------------------------------------------------------------------------------*/
        /* Configure wakeup Enable                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVMS_WE, wakeupEn);
#endif

        REG_WRITE(ESPI_VWEVMS(i), var);
        break;

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    case ESPI_VW_TYPE_GPIO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve GPIO register number                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetGpio_l(index, ESPI_VW_MS, &i));

        var = REG_READ(ESPI_VWGPMS(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure index enable                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWGPMS_INDEX_EN, indexEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure the Wire 3-0 bits to be reset when PLTRST is asserted                                 */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWGPMS_ENPLTRST, pltrstEn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Configure interrupt Enable                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWGPMS_IE, intEn);

        REG_WRITE(ESPI_VWGPMS(i), var);
        break;
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT

    case ESPI_VW_TYPE_NUM:
    default:
        break;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_ConfigWire                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  vw        - Virtual Wire identifier                                                    */
/*                  valid     - TRUE if VW is valid and false otherwise                                    */
/*                  control   - Virtual Wire Control Type                                                  */
/*                  trigger   - Virtual Wire Trigger Type                                                  */
/*                  polarity  - Virtual Wire Polarity Type                                                 */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures a Virtual Wire                                                 */
/*lint -e{715}      Suppress 'control/trigger/polarity' not referenced                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_ConfigWire (
    ESPI_VW                 vw,
    BOOLEAN                 valid,
    ESPI_VW_WIRE_CTRL_T     control,
    ESPI_VW_WIRE_TRIGGER_T  trigger,
    ESPI_VW_WIRE_POL_T      polarity
)
{
    UINT8           i;
    ESPI_VW_TYPE_T  type;
    UINT32          var;
    UINT8           index  = ESPI_VW_GET_INDEX(vw);
    UINT8           wire   = ESPI_VW_GET_WIRE(vw);

    DEFS_STATUS_RET_CHECK(ESPI_VW_GetType_l(index, &type));
    DEFS_STATUS_COND_CHECK(wire < ESPI_VW_WIRE_NUM, ESPI_VW_ERR_WIRE_NOT_FOUND);

    /*lint -save -e502 */
    switch (type)
    {
    case ESPI_VW_TYPE_INT_EV:
        break;

    case ESPI_VW_TYPE_SYS_EV:
    case ESPI_VW_TYPE_PLT:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve Event register number                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetReg_l(index, ESPI_VW_SM, &i));

        var = REG_READ(ESPI_VWEVSM(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Verify index enabled                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((READ_VAR_FIELD(var, VWEVSM_INDEX_EN) == TRUE), ESPI_VW_ERR_INDEX_NOT_ENABLED);

        /*-------------------------------------------------------------------------------------------------*/
        /* Update wire's validity                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_WIRE_VALID(wire), valid);

#ifdef ESPI_CAPABILITY_VW_HW_WIRE
        /*-------------------------------------------------------------------------------------------------*/
        /* Update if wire is HW wire                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_HW_WIRE(wire), control);
#endif

#ifdef ESPI_CAPABILITY_VW_EDGE
        /*-------------------------------------------------------------------------------------------------*/
        /* Update wire's trigger type                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_EDGE(wire), trigger);

        /*-------------------------------------------------------------------------------------------------*/
        /* Update wire's polarity                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_POL(wire), polarity);
#endif

        REG_WRITE(ESPI_VWEVSM(i), var);
        break;

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    case ESPI_VW_TYPE_GPIO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve GPIO register number                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetGpio_l(index, ESPI_VW_SM, &i));

        var = REG_READ(ESPI_VWGPSM(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Verify index enabled                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((READ_VAR_FIELD(var, VWGPSM_INDEX_EN) == TRUE), ESPI_VW_ERR_INDEX_NOT_ENABLED);

        /*-------------------------------------------------------------------------------------------------*/
        /* Update wire's validity                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWGPSM_WIRE_VALID(wire), valid);

        REG_WRITE(ESPI_VWGPSM(i), var);
        break;
#endif

    case ESPI_VW_TYPE_NUM:
    default:
        break;
    }
    /*lint -restore */

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_SetWire                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  vw       - Virtual Wire identifier                                                     */
/*                  value    - State of a Virtual Wire                                                     */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets the state of a Virtual Wire                                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_SetWire (
    ESPI_VW vw,
    UINT    value
)
{
    UINT8           i;
    ESPI_VW_TYPE_T  type;
    UINT32          var;
    UINT8           index  = ESPI_VW_GET_INDEX(vw);
    UINT8           wire   = ESPI_VW_GET_WIRE(vw);

    DEFS_STATUS_RET_CHECK(ESPI_VW_GetType_l(index, &type));
    DEFS_STATUS_COND_CHECK(wire < ESPI_VW_WIRE_NUM, ESPI_VW_ERR_WIRE_NOT_FOUND);
    DEFS_STATUS_COND_CHECK((ESPI_IsChannelSlaveEnable_l(ESPI_CHANNEL_VW) == ENABLE), DEFS_STATUS_HARDWARE_ERROR);

    /*lint -save -e502 */
    switch (type)
    {
    case ESPI_VW_TYPE_INT_EV:
        break;

    case ESPI_VW_TYPE_SYS_EV:
    case ESPI_VW_TYPE_PLT:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve Event register number                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetReg_l(index, ESPI_VW_SM, &i));

        var = REG_READ(ESPI_VWEVSM(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Verify index enabled                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((READ_VAR_FIELD(var, VWEVSM_INDEX_EN) == TRUE), ESPI_VW_ERR_INDEX_NOT_ENABLED);

        /*-------------------------------------------------------------------------------------------------*/
        /* Verify wire's validity                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((READ_VAR_FIELD(var, VWEVSM_WIRE_VALID(wire)) == TRUE), ESPI_VW_ERR_INVALID_WIRE);

#ifdef ESPI_CAPABILITY_VW_HW_WIRE
        /*-------------------------------------------------------------------------------------------------*/
        /* Verify wire is not HW wire                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((READ_VAR_FIELD(var, VWEVSM_HW_WIRE(wire)) == FALSE), ESPI_VW_ERR_HW_CTL_WIRE);
#endif

        /*-------------------------------------------------------------------------------------------------*/
        /* Update wire value                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWEVSM_WIRE(wire), value);

        REG_WRITE(ESPI_VWEVSM(i), var);
        break;

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    case ESPI_VW_TYPE_GPIO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve GPIO register number                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetGpio_l(index, ESPI_VW_SM, &i));

        var = REG_READ(ESPI_VWGPSM(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Verify index enabled                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((READ_VAR_FIELD(var, VWGPSM_INDEX_EN) == TRUE), ESPI_VW_ERR_INDEX_NOT_ENABLED);

        /*-------------------------------------------------------------------------------------------------*/
        /* Verify wire's validity                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((READ_VAR_FIELD(var, VWGPSM_WIRE_VALID(wire)) == TRUE), ESPI_VW_ERR_INVALID_WIRE);

        /*-------------------------------------------------------------------------------------------------*/
        /* Update wire value                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        SET_VAR_FIELD(var, VWGPSM_WIRE(wire), value);

        REG_WRITE(ESPI_VWGPSM(i), var);
        break;
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT

    case ESPI_VW_TYPE_NUM:
    default:
        break;
    }
    /*lint -restore */

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_GetWire                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  vw       - Virtual Wire identifier                                                     */
/*                  value    - State of a Virtual Wire                                                     */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads the state of a Virtual Wire                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_GetWire (
    ESPI_VW vw,
    UINT*   value
)
{
    UINT8           i;
    ESPI_VW_TYPE_T  type;
    UINT32          var;
    UINT8           index  = ESPI_VW_GET_INDEX(vw);
    UINT8           wire   = ESPI_VW_GET_WIRE(vw);
    ESPI_VW_DIR_T   dir    = ESPI_VW_GET_DIR(vw);

    DEFS_STATUS_RET_CHECK(ESPI_VW_GetType_l(index, &type));
    DEFS_STATUS_COND_CHECK(wire < ESPI_VW_WIRE_NUM, ESPI_VW_ERR_WIRE_NOT_FOUND);

    switch (type)
    {
    case ESPI_VW_TYPE_INT_EV:
        break;

    case ESPI_VW_TYPE_SYS_EV:
    case ESPI_VW_TYPE_PLT:
        switch (dir)
        {
        case ESPI_VW_SM:
            /*---------------------------------------------------------------------------------------------*/
            /* Retrieve Event register number                                                              */
            /*---------------------------------------------------------------------------------------------*/
            DEFS_STATUS_RET_CHECK(ESPI_VW_GetReg_l(index, ESPI_VW_SM, &i));

            var = REG_READ(ESPI_VWEVSM(i));

            /*---------------------------------------------------------------------------------------------*/
            /* Read wire value                                                                             */
            /*---------------------------------------------------------------------------------------------*/
            *value = READ_VAR_FIELD(var, VWEVSM_WIRE(wire));
            break;

        case ESPI_VW_MS:
            /*---------------------------------------------------------------------------------------------*/
            /* Retrieve Event register number                                                              */
            /*---------------------------------------------------------------------------------------------*/
            DEFS_STATUS_RET_CHECK(ESPI_VW_GetReg_l(index, ESPI_VW_MS, &i));

            var = REG_READ(ESPI_VWEVMS(i));

            /*---------------------------------------------------------------------------------------------*/
            /* Read wire value                                                                             */
            /*---------------------------------------------------------------------------------------------*/
            *value = READ_VAR_FIELD(var, VWEVMS_WIRE(wire));
            break;
        }
        break;

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    case ESPI_VW_TYPE_GPIO:
        switch (dir)
        {
        case ESPI_VW_SM:
            /*---------------------------------------------------------------------------------------------*/
            /* Retrieve GPIO register number                                                               */
            /*---------------------------------------------------------------------------------------------*/
            DEFS_STATUS_RET_CHECK(ESPI_VW_GetGpio_l(index, ESPI_VW_SM, &i));

            var = REG_READ(ESPI_VWGPSM(i));

            /*---------------------------------------------------------------------------------------------*/
            /* Read wire value                                                                             */
            /*---------------------------------------------------------------------------------------------*/
            *value = READ_VAR_FIELD(var, VWGPSM_WIRE(wire));
            break;

        case ESPI_VW_MS:
            /*---------------------------------------------------------------------------------------------*/
            /* Retrieve GPIO register number                                                               */
            /*---------------------------------------------------------------------------------------------*/
            DEFS_STATUS_RET_CHECK(ESPI_VW_GetGpio_l(index, ESPI_VW_MS, &i));

            var = REG_READ(ESPI_VWGPMS(i));

            /*---------------------------------------------------------------------------------------------*/
            /* Read wire value                                                                             */
            /*---------------------------------------------------------------------------------------------*/
            *value = READ_VAR_FIELD(var, VWGPMS_WIRE(wire));
            break;
        }
        break;
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT

    case ESPI_VW_TYPE_NUM:
    default:
        break;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_WireIsRead                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  vw       - Virtual Wire identifier                                                     */
/*                  value    - State of a Virtual Wire                                                     */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks whether a group was read by the Host after Wire bits have changed  */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_WireIsRead (
    ESPI_VW     vw,
    BOOLEAN*    value
)
{
    UINT8           i;
    UINT32          var;
    ESPI_VW_TYPE_T  type;
    UINT8           index  = ESPI_VW_GET_INDEX(vw);
    UINT8           wire   = ESPI_VW_GET_WIRE(vw);

    DEFS_STATUS_RET_CHECK(ESPI_VW_GetType_l(index, &type));
    DEFS_STATUS_COND_CHECK(wire < ESPI_VW_WIRE_NUM, ESPI_VW_ERR_WIRE_NOT_FOUND);

    switch (type)
    {
    case ESPI_VW_TYPE_INT_EV:
        break;

    case ESPI_VW_TYPE_SYS_EV:
    case ESPI_VW_TYPE_PLT:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve Event register number                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetReg_l(index, ESPI_VW_SM, &i));

        var = REG_READ(ESPI_VWEVSM(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Return TRUE if The group was read by the Host after Wire 3-0 bits changed                       */
        /*-------------------------------------------------------------------------------------------------*/
        *value = !READ_VAR_FIELD(var, VWEVSM_DIRTY);
        break;

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    case ESPI_VW_TYPE_GPIO:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve GPIO register number                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_VW_GetGpio_l(index, ESPI_VW_SM, &i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Return TRUE if The group was read by the Host after Wire 3-0 bits changed                       */
        /*-------------------------------------------------------------------------------------------------*/
        *value = !READ_REG_FIELD(ESPI_VWGPSM(i), VWGPSM_DIRTY);
        break;
#endif

    case ESPI_VW_TYPE_NUM:
    default:
        break;
    }

    return DEFS_STATUS_OK;
}

#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_SetFloating                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  vw       - Virtual Wire identifier                                                     */
/*                  value    - State of a Virtual Wire                                                     */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets the state of a floating Virtual Wire event                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_SetFloating (
    ESPI_VW vw,
    UINT    value
)
{
    ESPI_VW_TYPE_T  type;
    UINT32          var;
    UINT8           index  = ESPI_VW_GET_INDEX(vw);
    UINT8           wire   = ESPI_VW_GET_WIRE(vw);

    DEFS_STATUS_RET_CHECK(ESPI_VW_GetType_l(index, &type));
    DEFS_STATUS_COND_CHECK(wire < ESPI_VW_WIRE_NUM, ESPI_VW_ERR_WIRE_NOT_FOUND);
    DEFS_STATUS_COND_CHECK(type != ESPI_VW_TYPE_INT_EV, ESPI_VW_ERR_INDEX_NOT_FOUND);

    var = REG_READ(ESPI_VWPING);

    /*lint -save -e502 */
    SET_VAR_FIELD(var, VWPING_INDEX,        index);
    SET_VAR_FIELD(var, VWPING_VALID(wire),  TRUE);
    SET_VAR_FIELD(var, VWPING_DATA(wire),   value);
    /*lint -restore */

    REG_WRITE(ESPI_VWPING, var);

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_FloatingIsRead                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  vw       - Virtual Wire identifier                                                     */
/*                  value    - State of a Virtual Wire                                                     */
/*                                                                                                         */
/* Returns:         DEFS_STATUS error code                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks whether a floating Virtual Wire change was read by the Host        */
/*lint -e{715}      Suppress 'vw' not referenced                                                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_FloatingIsRead (
    ESPI_VW     vw,
    BOOLEAN*    value
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* The DIRTY bit of the "floating" events group is set to 1 on any Core write to the VWPING register,  */
    /* as opposed to the other Slave-to-Master virtual wire events, where the DIRTY bit is set to 1 only   */
    /* on a change of one of the valid Wire 3-0 bits.                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    *value = !(READ_REG_FIELD(ESPI_VWPING, VWPING_DIRTY));
    return DEFS_STATUS_OK;
}
#endif // ESPI_CAPABILITY_VW_FLOATING_EVENTS

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_SendBootLoad                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  status - Boot Load Status: TRUE for success; FALSE for failure.                        */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends the SLAVE_BOOT_LOAD_STATUS and SLAVE_BOOT_LOAD_DONE Virtual Wires.  */
/*                  This is an indication provided to the eSPI master for code load completion.            */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_VW_SendBootLoad (BOOLEAN status)
{
    if ((! ESPI_bootLoad) && READ_VAR_BIT(ESPI_configUpdateMask, ESPI_CHANNEL_VW))
    {
        DEFS_STATUS_RET_CHECK(ESPI_VW_SetWire(ESPI_VW_SLAVE_BOOT_LOAD_STATUS, status));
        DEFS_STATUS_RET_CHECK(ESPI_VW_SetWire(ESPI_VW_SLAVE_BOOT_LOAD_DONE,   TRUE));
        ESPI_bootLoad = TRUE;
        return DEFS_STATUS_OK;
    }

    return DEFS_STATUS_SYSTEM_IN_INCORRECT_STATE;
}


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                        FLASH INTERFACE FUNCTIONS                                        */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_Config                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  flashAccess    -  master/slave attached flash                                          */
/*                  writePageSize  -  page size in flash in bytes                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures flash channel                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_FLASH_Config (
    ESPI_FLASH_ACCESS_MODE  flashAccess,
    UINT32                  writePageSize
)
{
    ESPI_FLASH_WritePageSize_L = writePageSize;
    SET_REG_FIELD(ESPI_ESPICFG, ESPICFG_FLASHCHANMODE, flashAccess);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_RegisterCallback                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  handler    -  user callback                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine registers user callback for flash channel notifications                   */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_FLASH_RegisterCallback (ESPI_INT_HANDLER handler)
{
    ESPI_FLASH_userIntHandler_L = handler;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SendReq                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType      - request type                                                            */
/*                  offset       - offset in flash                                                         */
/*                  size         - size of transaction                                                     */
/*                  pollingMode  - equals TRUE if polling is required and FALSE for interrupt mode         */
/*                  buffer       - input/output buffer                                                     */
/*                  strpHdr      - equals TRUE if strip header is required and FALSE otherwise             */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends read/write/erase request to the host                                */
/*                                                                                                         */
/*                  limitations under request type ESPI_FLASH_REQ_READ_DMA and ESPI_FLASH_REQ_READ_AUTO:   */
/*                      1. Flash offset must be aligned to size of transaction                             */
/*                      2. Size must be aligned to 64B.                                                    */
/*                      3. Maximum size for transaction is (64B*256)                                       */
/*                  * If using request type ESPI_FLASH_REQ_READ_DMA the buffer must be at least            */
/*                    4B aligned , 16B aligned will improve performance.                                   */
/*                  * If polling mode == TRUE, FLASH interrupts should be disabled by the user             */
/*                    If polling mode == FALSE, FLASH interrupts should be enabled by the user             */
/*                  * limitation 1 should be fixed in Mrider16 chip                                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_FLASH_SendReq (
    ESPI_FLASH_REQ_T            reqType,
    UINT32                      offset,
    UINT32                      size,
    BOOLEAN                     pollingMode,
    UINT32*                     buffer,
    BOOLEAN                     strpHdr
)
{
    ESPI_FLASH_REQ_ACC_T    req     = ESPI_FLASH_REQ_ACC_READ;
    BOOLEAN                 useDMA  = FALSE;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error check                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((size > 0), DEFS_STATUS_INVALID_DATA_FIELD);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set request                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (reqType)
    {
    case ESPI_FLASH_REQ_READ_DMA:
        /*-------------------------------------------------------------------------------------------------*/
        /* Error check                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((strpHdr == TRUE), DEFS_STATUS_INVALID_PARAMETER);
        useDMA = TRUE;
        /*lint -fallthrough */

    case ESPI_FLASH_REQ_READ_MANUAL:
    case ESPI_FLASH_REQ_READ_AUTO:
        req = ESPI_FLASH_REQ_ACC_READ;
        break;

    case ESPI_FLASH_REQ_WRITE:
        req = ESPI_FLASH_REQ_ACC_WRITE;
        break;

    case ESPI_FLASH_REQ_ERASE:
        req = ESPI_FLASH_REQ_ACC_ERASE;
        break;

    default:
        break;
    }

    switch (reqType)
    {
    case ESPI_FLASH_REQ_READ_MANUAL:
    case ESPI_FLASH_REQ_WRITE:
    case ESPI_FLASH_REQ_ERASE:
#ifdef ESPI_CAPABILITY_FLASH_STRPHDR_IN_MANUAL_MODE
        /*-------------------------------------------------------------------------------------------------*/
        /* Strip header option must be used                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((strpHdr == TRUE), DEFS_STATUS_INVALID_PARAMETER);
#endif
        DEFS_STATUS_RET_CHECK(ESPI_FLASH_ReqManual_l(req, offset, size, pollingMode, buffer, strpHdr));
        break;

    case ESPI_FLASH_REQ_READ_AUTO:
    case ESPI_FLASH_REQ_READ_DMA:
        /*-------------------------------------------------------------------------------------------------*/
        /* Error check                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK(((size % _64B_) == 0), DEFS_STATUS_INVALID_PARAMETER);

        if (useDMA)
        {
            DEFS_STATUS_COND_CHECK(((UINT32)buffer % _4B_ == 0) && (offset % _4B_ == 0), DEFS_STATUS_FAIL);
        }
        DEFS_STATUS_RET_CHECK(ESPI_FLASH_ReadReqAuto_l(offset, size, pollingMode, buffer, strpHdr, useDMA));
        break;

    default:
        break;
    }

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_ResetReqTrans                                                               */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Resets request transaction in automatic mode and disables automatic mode  */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_FLASH_ResetReqTrans (void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset the pointers of the Transmit and Receive buffers                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_RSTBUFHEADS, 0x01);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Ignore further packets                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_currReqInfo_L.size       = 0;
    ESPI_FLASH_currReqInfo_L.buffOffset = 0;
    ESPI_FLASH_currReqInfo_L.currSize   = 0;

    if (ESPI_FLASH_IS_AUTO_MODE_ON)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Turn off automatic mode                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_FLASH_AUTO_MODE(FALSE);
    }

    if (ESPI_FLASH_currReqInfo_L.useDMA)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Disable DMA request                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_DMATHRESH, ESPI_FLASH_DMA_THRESHOLD_DISABLE);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_GetStatus                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, DEFS_STATUS_SYSTEM_BUSY if transaction has not ended yet    */
/*                  and other DEFS_STATUS error on error                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns flash request status                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_FLASH_GetStatus (void)
{
    return ESPI_FLASH_status_L;
}

#ifdef ESPI_CAPABILITY_SAF
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_Config                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  maxReadSizeSupp     - Flash access channel maximum read request size supported         */
/*                  flashSharingCapSupp - Flash sharing capability support                                 */
/*                  eraseBlockSize      - Target Flash erase block size for master’s regions               */
/*                  targetRPMCsupp      - Target Replay Protected Monotonic Counters (RPMC) supported      */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configure the flash SAF registers                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_FLASH_SAF_Config (ESPI_FLASH_SAF_MAX_READ_REQ maxReadSizeSupp,
                                   ESPI_FLASH_SHARING_CAP_SUPP flashSharingCapSupp,
                                   ESPI_FLASH_SAF_ERASE_BLOCK_SIZE eraseBlockSize,
                                   UINT8 targetRPMCsupp)
{
    SET_REG_FIELD(ESPI_FLASHCFG, FLASHCFG_FLASHREQSUP, maxReadSizeSupp);
    SET_REG_FIELD(ESPI_FLASHCFG, FLASHCFG_FLASHCAPA, flashSharingCapSupp);
    SET_REG_FIELD(ESPI_FLASHCFG, FLASHCFG_TRGFLASHEBLKSIZE, eraseBlockSize);
    DEFS_STATUS_COND_CHECK(flashSharingCapSupp < ESPI_FLASH_SAF_MAX_TAR_RPMC_SUPP,DEFS_STATUS_PARAMETER_OUT_OF_RANGE);
    SET_REG_FIELD(ESPI_FLASHCFG, FLASHCFG_TRGRPMCSUPP, targetRPMCsupp);

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_SetRWprotect                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index         - Index of memory range [0..15]                                          */
/*                  baseAddr      - Memory range base addr                                                 */
/*                  highAddr      - Memory range high addr                                                 */
/*                  readProt      - TRUE if memory range is read protect, FALSE otherwise                  */
/*                  writeProt     - TRUE if memory range is write protect, FALSE otherwise                 */
/*                  readTagOvr    - Read tag overrun value, if bit i is set, read request from tag i is    */
/*                                  enabled regardless of read protect bit                                 */
/*                  writeTagOvr   - Write tag overrun value, if bit i is set, write request from tag i is  */
/*                                  enabled regardless of write protect bit                                */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures specific memory range protection options                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_FLASH_SAF_SetRWprotect (UINT8 index, UINT16 baseAddr, UINT16 highAddr, BOOLEAN readProt,
                                         BOOLEAN writeProt, UINT16 readTagOvr, UINT16 writeTagOvr)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify index is valid                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((index < ESPI_FLASH_SAF_PROT_MEM_NUM),DEFS_STATUS_INVALID_PARAMETER);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set base address                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(index),ESPI_FLASH_PRTR_BADDRn_BADDR,baseAddr);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set high address                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASH_PRTR_HADDRn(index),ESPI_FLASH_PRTR_HADDRn_HADDR,highAddr);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set read protect                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(index), ESPI_FLASH_PRTR_BADDRn_FRGN_RPR, readProt);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set write protect                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(index), ESPI_FLASH_PRTR_BADDRn_FRGN_WPR, writeProt);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set read protect tag overrun                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASH_FLASH_RGN_TAG_OVRn(index), ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_RPR_TOVR ,readTagOvr);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set write protect tag overrun                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASH_FLASH_RGN_TAG_OVRn(index), ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_WPR_TOVR, writeTagOvr);

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_ClearRWprotect                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index     - Index of memory range [0..15]                                              */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine clears index i specific memory range protection settings                  */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_FLASH_SAF_ClearRWprotect (UINT8 index)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify index is valid                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((index < ESPI_FLASH_SAF_PROT_MEM_NUM),DEFS_STATUS_INVALID_PARAMETER);

    SET_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(index),ESPI_FLASH_PRTR_BADDRn_BADDR,0);
    SET_REG_FIELD(ESPI_FLASH_PRTR_HADDRn(index),ESPI_FLASH_PRTR_HADDRn_HADDR,0);

    SET_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(index), ESPI_FLASH_PRTR_BADDRn_FRGN_RPR, 0);
    SET_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(index), ESPI_FLASH_PRTR_BADDRn_FRGN_WPR, 0);

    SET_REG_FIELD(ESPI_FLASH_FLASH_RGN_TAG_OVRn(index), ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_RPR_TOVR ,0);
    SET_REG_FIELD(ESPI_FLASH_FLASH_RGN_TAG_OVRn(index), ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_WPR_TOVR, 0);

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_IsPendingReq                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  none                                                                                   */
/*                                                                                                         */
/* Returns:         TRUE if request is pending, FALSE otherwise                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns  TRUE if request is pending, FALSE otherwise                      */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN ESPI_FLASH_SAF_IsPendingReq (void)
{
    return ESPI_FLASH_SafPendingIncomingReq_L;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_HandleReq                                                               */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles incoming request from host                                        */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_FLASH_SAF_HandleReq (void)
{
    UINT32 reqHdr;
    ESPI_FLASH_TRANS_HDR* reqHdrPtr;
    UINT32 roundedSize;
    UINT i;
    UINT8 eraseBlockSize;
    UINT16 tagPlusLength;

    ESPI_FLASH_SafPendingIncomingRes_L = FALSE;
    ESPI_FLASH_SafReqInfo_L.reqType = ESPI_FLASH_SAF_REQ_NUM;
    ESPI_FLASH_SafReqInfo_L.status = DEFS_STATUS_OK;

    ESPI_FLASH_SafPendingIncomingReq_L = FALSE;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parse request header                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    reqHdr = REG_READ(ESPI_FLASHRXRDHEAD);
    reqHdrPtr = (ESPI_FLASH_TRANS_HDR*)(void*)(&reqHdr);
    ESPI_FLASH_SafReqInfo_L.offset   = LE32(REG_READ(ESPI_FLASHRXRDHEAD));
    tagPlusLength = LE16(reqHdrPtr->tagPlusLength);
    ESPI_FLASH_SafReqInfo_L.size = ESPI_FLASH_SafReqInfo_L.reminder = READ_VAR_FIELD(tagPlusLength,HEADER_LENGTH);
    ESPI_FLASH_SafReqInfo_L.reqType = (ESPI_FLASH_SAF_REQ_T)reqHdrPtr->type;
    ESPI_FLASH_SafReqInfo_L.tag = READ_VAR_FIELD(tagPlusLength,HEADER_TAG);

    if (!(ESPI_FLASH_SafReqInfo_L.reqType < ESPI_FLASH_SAF_REQ_NUM))
    {
        return;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Size  and request type check                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    switch(ESPI_FLASH_SafReqInfo_L.reqType)
    {
        case ESPI_FLASH_SAF_REQ_READ:
            if(ESPI_FLASH_SafReqInfo_L.size > ESPI_FLASH_MAX_READ_REQ_SIZE_SUPP)
            {
                ESPI_FLASH_SafReqInfo_L.status = DEFS_STATUS_INVALID_DATA_SIZE;
            }
        break;

        case ESPI_FLASH_SAF_REQ_WRITE:
        case ESPI_FLASH_SAF_REQ_RPMC_OP1:
        case ESPI_FLASH_SAF_REQ_RPMC_OP2:
            if(ESPI_FLASH_SafReqInfo_L.reminder > ESPI_FLASH_MAX_PAYLOAD_REQ_SIZE)
            {
                ESPI_FLASH_SafReqInfo_L.status = DEFS_STATUS_INVALID_DATA_SIZE;
            }
            roundedSize = ROUND_UP(ESPI_FLASH_SafReqInfo_L.reminder,sizeof(UINT32)) / sizeof(UINT32);
            for (i = 0; i < roundedSize; i++)
            {
                ESPI_FLASH_SafReqInfo_L.buffer[i] = REG_READ(ESPI_FLASHRXRDHEAD);
            }
        break;

        case ESPI_FLASH_SAF_REQ_ERASE:
            eraseBlockSize = READ_REG_FIELD(ESPI_FLASHCFG, FLASHCFG_TRGFLASHEBLKSIZE);
            switch (ESPI_FLASH_SafReqInfo_L.size)
            {
                case ESPI_FLASH_ERASE_4K:

                    if (eraseBlockSize == ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_4KB)
                    {
                        ESPI_FLASH_SafReqInfo_L.reminder = _4KB_;
                    }
                    else
                    {
                        ESPI_FLASH_SafReqInfo_L.status = DEFS_STATUS_INVALID_DATA_SIZE;
                    }
                break;

                case ESPI_FLASH_ERASE_16K:
                    if (eraseBlockSize == ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_16KB)
                    {
                        ESPI_FLASH_SafReqInfo_L.reminder = _16KB_;
                    }
                    else
                    {
                        ESPI_FLASH_SafReqInfo_L.status = DEFS_STATUS_INVALID_DATA_SIZE;
                    }
                break;

                case ESPI_FLASH_ERASE_64K:
                    if (eraseBlockSize == ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_64KB)
                    {
                        ESPI_FLASH_SafReqInfo_L.reminder = _64KB_;
                    }
                    else
                    {
                        ESPI_FLASH_SafReqInfo_L.status = DEFS_STATUS_INVALID_DATA_SIZE;
                    }
                break;

                default:
                    ESPI_FLASH_SafReqInfo_L.status = DEFS_STATUS_INVALID_DATA_SIZE;
            }
        break;

        case ESPI_FLASH_SAF_REQ_NUM:
        break;
    }

    ESPI_FLASH_SafPendingIncomingRes_L = TRUE;
    /*-----------------------------------------------------------------------------------------------------*/
    /* If size check failed                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    if (ESPI_FLASH_SafReqInfo_L.status != DEFS_STATUS_OK)
    {
        return;
    }

    ESPI_FLASH_SafReqInfo_L.status = ESPI_FLASH_SAF_CheckAddress_l();
    if (ESPI_FLASH_SafReqInfo_L.status != DEFS_STATUS_OK)
    {
        return;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* If the transaction is not read command                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (ESPI_FLASH_SafReqInfo_L.reqType != ESPI_FLASH_SAF_REQ_READ)
    {

        ESPI_FLASH_SafReqInfo_L.status = ESPI_FLASH_SAF_PerformReq_l(ESPI_FLASH_SafReqInfo_L.offset,
                                                                   ESPI_FLASH_SafReqInfo_L.reminder);
    }

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_GetStatus                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, DEFS_STATUS_SYSTEM_BUSY if transaction has not ended yet    */
/*                  and other DEFS_STATUS error on error                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine return the status of latest request                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_FLASH_SAF_GetStatus (UINT8* status)
{
    DEFS_STATUS ret = DEFS_STATUS_OK;

    switch (ESPI_FLASH_SafReqInfo_L.reqType)
    {
        case ESPI_FLASH_SAF_REQ_READ:
            *status = DEFS_STATUS_OK;
        break;

        case ESPI_FLASH_SAF_REQ_WRITE:
        case ESPI_FLASH_SAF_REQ_ERASE:
        // TODO: ?? NEED FIU STATUS
        case ESPI_FLASH_SAF_REQ_RPMC_OP1:
        case ESPI_FLASH_SAF_REQ_RPMC_OP2:
            ret = FLASH_DEV_PollOnBusyBit(&ESPI_FLASH_SafFlashParams_L, FIU_MODULE_0, FIU_CS_0, 1);
            if (ret == DEFS_STATUS_OK)
            {
                ret = DEFS_STATUS_OK;
            }
            else
            {
                ret = DEFS_STATUS_SYSTEM_BUSY;
            }
        break;

        case ESPI_FLASH_SAF_REQ_NUM:
            ret = DEFS_STATUS_FAIL;
        break;
    }
    return ret;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_IsPendingRes                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  none                                                                                   */
/*                                                                                                         */
/* Returns:         TRUE if response is pending, FALSE otherwise                                           */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns  TRUE if response is pending, FALSE otherwise                     */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN ESPI_FLASH_SAF_IsPendingRes (void)
{
    return ESPI_FLASH_SafPendingIncomingRes_L;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_SendRes                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends response to host                                                    */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_FLASH_SAF_SendRes (void)
{
    UINT8 comp = 0;

    ESPI_FLASH_SafPendingIncomingRes_L = FALSE;
    switch (ESPI_FLASH_SafReqInfo_L.reqType)
    {
        case ESPI_FLASH_SAF_REQ_READ:

            ESPI_FLASH_SafReqInfo_L.currSize = (UINT8)MIN(ESPI_FLASH_SafReqInfo_L.reminder, ESPI_FLASH_MAX_PAYLOAD_REQ_SIZE);

            if (ESPI_FLASH_SafReqInfo_L.currSize > 0)
            {
                ESPI_FLASH_SafReqInfo_L.status = ESPI_FLASH_SAF_PerformReq_l(ESPI_FLASH_SafReqInfo_L.offset + ESPI_FLASH_SafReqInfo_L.bytesRead,
                                                                             ESPI_FLASH_SafReqInfo_L.currSize);

                ESPI_FLASH_SafReqInfo_L.reminder -= ESPI_FLASH_SafReqInfo_L.currSize;
                ESPI_FLASH_SafReqInfo_L.bytesRead += ESPI_FLASH_SafReqInfo_L.currSize;

                if (ESPI_FLASH_SafReqInfo_L.status == DEFS_STATUS_OK)
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* One packet                                                                          */
                    /*-------------------------------------------------------------------------------------*/
                    if (ESPI_FLASH_SafReqInfo_L.size <= ESPI_FLASH_MAX_PAYLOAD_REQ_SIZE)
                    {
                        comp = ESPI_SUCCESSFUL_COMPLETION_WITH_DATA;
                    }
                    else
                    {
                        if (ESPI_FLASH_SafReqInfo_L.bytesRead <= ESPI_FLASH_MAX_PAYLOAD_REQ_SIZE)
                        {
                            comp = (ESPI_SUCCESSFUL_COMPLETION_WITH_DATA & ESPI_FIRST_COMPLETION_MASK);
                        }
                        else if (ESPI_FLASH_SafReqInfo_L.reminder > 0)
                        {
                            comp = (ESPI_SUCCESSFUL_COMPLETION_WITH_DATA & ESPI_MIDDLE_COMPLETION_MASK);
                        }
                        else
                        {
                            comp = (ESPI_SUCCESSFUL_COMPLETION_WITH_DATA & ESPI_LAST_COMPLETION_MASK);
                        }
                    }
                }
                else
                {
                    comp = ESPI_UNSUCCESSFUL_COMPLETION_WO_DATA;
                    /*-------------------------------------------------------------------------------------------------*/
                    /* Indicate to the Host that the recieve buffer is empty                                           */
                    /*-------------------------------------------------------------------------------------------------*/
                    SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_FLASH_ACC_NP_FREE,1);
                }

                if (ESPI_FLASH_SafReqInfo_L.bytesRead == ESPI_FLASH_SafReqInfo_L.size)
                {
                    /*-------------------------------------------------------------------------------------------------*/
                    /* Indicate to the Host that the recieve buffer is empty                                           */
                    /*-------------------------------------------------------------------------------------------------*/
                    SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_FLASH_ACC_NP_FREE,1);
                }

            }
        break;

        case ESPI_FLASH_SAF_REQ_WRITE:
        case ESPI_FLASH_SAF_REQ_ERASE:
        case ESPI_FLASH_SAF_REQ_RPMC_OP1:
        case ESPI_FLASH_SAF_REQ_RPMC_OP2:
            /*---------------------------------------------------------------------------------------------*/
            /* Indicate to the Host that the recieve buffer is empty                                       */
            /*---------------------------------------------------------------------------------------------*/
            SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_FLASH_ACC_NP_FREE,1);

            if (ESPI_FLASH_SafReqInfo_L.status == DEFS_STATUS_OK)
            {
                if (ESPI_FLASH_SafReqInfo_L.outBufferSize > 0)
                {
                    comp = ESPI_SUCCESSFUL_COMPLETION_WITH_DATA;
                }
                else
                {
                    comp = ESPI_SUCCESSFUL_COMPLETION_WO_DATA;
                }
            }
            else
            {
                comp = ESPI_UNSUCCESSFUL_COMPLETION_WO_DATA;
            }
        break;

        case ESPI_FLASH_SAF_REQ_NUM:
            break;
    }
    ESPI_FLASH_SAF_SendRes_l(comp);
    return DEFS_STATUS_OK;
}

#endif // ESPI_CAPABILITY_SAF


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                            OUT OF BAND (Tunnelled SMBus) INTERFACE FUNCTIONS                            */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_ReadPchTemp                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  temp    - Pointer to store temperature value                                           */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends read PCH temperature command                                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_ReadPchTemp (UINT8* temp)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Build OOB Command                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_BuildCmd_l(ESPI_OOB_SMB_SLAVE_SRC_ADDR_EC,
                                              ESPI_OOB_SMB_SLAVE_DEST_ADDR_ESPI_HW,
                                              ESPI_OOB_HW_CMD_GET_PCH_TEMP,
                                              0, NULL, ESPI_OOB_PCH_TEMP_SIZE, temp));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send OOB Command                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_RunCmd_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_ReadPchRtc                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  rtc    -  Pointer to store RTC value                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends read PCH RTC command                                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_ReadPchRtc (UINT8* rtc)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Build OOB Command                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_BuildCmd_l(ESPI_OOB_SMB_SLAVE_SRC_ADDR_EC,
                                              ESPI_OOB_SMB_SLAVE_DEST_ADDR_ESPI_HW,
                                              ESPI_OOB_HW_CMD_GET_PCH_RTC_TIME,
                                              0, NULL, ESPI_OOB_PCH_RTC_SIZE, rtc));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send OOB Command                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_RunCmd_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_ReadCrashLogPch                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  crashLog - Pointer to store CrashLog PMC SSRAM data                                    */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends triggers PMC CrashLog CMD                                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_ReadCrashLog (UINT8* crashLog, ESPI_OOB_CRASHLOG_CMD_T crashLogCmd)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Build OOB Command                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_BuildCmd_l(ESPI_OOB_SMB_SLAVE_SRC_ADDR_EC,
                                              ESPI_OOB_SMB_SLAVE_DEST_ADDR_PMC_FW,
                                              (UINT8)crashLogCmd,
                                              0, NULL, ESPI_OOB_CRASH_LOG_SIZE, crashLog));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send OOB Command                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_RunCmd_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_Init                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  callback  - Application callback to be called from PECI interrupt handler.             */
/*                  peci_freq - NOT in use. compatible with PECI IF                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes the PECI module.                                              */
/*lint -e{715}      Suppress 'peci_freq' not referenced                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_OOB_PECI_Init (
    ESPI_OOB_PECI_CALLBACK_T    callback,
    UINT32                      peci_freq
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize SW driver                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_peciCallback_L             = callback;
    ESPI_OOB_peciCurrentCommand_L       = ESPI_OOB_PECI_COMMAND_NONE;
    ESPI_OOB_peciCc_L                   = ESPI_OOB_PECI_CC_NONE;
    ESPI_OOB_peciRetryCount_L           = ESPI_OOB_PECI_RETRY_MAX_COUNT;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PECI_SetAddress                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  client_address - PECI Client Address.                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets the value of the address frame of the next PECI transaction.         */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_OOB_PECI_SetAddress (UINT8 client_address)
{
    ESPI_OOB_peciClientAddress_L = client_address;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_Trans                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  wr_length - Number of bytes to write.                                                  */
/*                  rd_length - Number of bytes to read.                                                   */
/*                  cmd_code  - Command code.                                                              */
/*                  data_0    - Low 32-bit data to write (when applicable).                                */
/*                  data_1    - High 32-bit data to write (when applicable).                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initiates the parameters of a PECI transaction.                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_Trans (
    UINT8                   wr_length,
    UINT8                   rd_length,
    ESPI_OOB_PECI_COMMAND_T cmd_code,
    UINT32                  data_0,
    UINT32                  data_1
)
{
    UINT8   nWrite  = wr_length + 3; // + PECI Target Address, Write Length, Read Length
    UINT8   nRead   = rd_length + 1; // + PECI Response/Error Status
    UINT    awFcsIndex;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Save transaction parameters                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_peciMsg_L.trgAddr      = ESPI_OOB_peciClientAddress_L;
    ESPI_OOB_peciMsg_L.writeLen     = wr_length;
    ESPI_OOB_peciMsg_L.readLen      = rd_length;
    ESPI_OOB_peciMsg_L.cmd          = (UINT8)cmd_code;
    ESPI_OOB_peciMsg_L.data[0]      = LSB0(data_0);
    ESPI_OOB_peciMsg_L.data[1]      = LSB1(data_0);
    ESPI_OOB_peciMsg_L.data[2]      = LSB2(data_0);
    ESPI_OOB_peciMsg_L.data[3]      = LSB3(data_0);
    ESPI_OOB_peciMsg_L.data[4]      = LSB0(data_1);
    ESPI_OOB_peciMsg_L.data[5]      = LSB1(data_1);
    ESPI_OOB_peciMsg_L.data[6]      = LSB2(data_1);
    ESPI_OOB_peciMsg_L.data[7]      = LSB3(data_1);
    ESPI_OOB_peciCurrentCommand_L   = cmd_code;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize retry counter                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_peciRetryCount_L = ESPI_OOB_PECI_RETRY_MAX_COUNT;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate AW FCS if this is a write transaction and write it to the data out buffer                 */
    /*-----------------------------------------------------------------------------------------------------*/
    if (wr_length > 1 &&
        (cmd_code == ESPI_OOB_PECI_COMMAND_WR_PKG_CFG || cmd_code == ESPI_OOB_PECI_COMMAND_WR_PCI_CFG_LOCAL))
    {
        awFcsIndex = MIN((wr_length - 1), ESPI_OOB_PECI_DATA_SIZE) - 1;

        /*-------------------------------------------------------------------------------------------------*/
        /* Before sending the AW FCS byte, the MSb of FCS result must be inverted.                         */
        /* Otherwise the Write FCS returned by the target in the next byte would be 0x00.                  */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_OOB_peciMsg_L.data[awFcsIndex] =
            ESPI_OOB_PECI_CalcFCS_l((UINT8*)&ESPI_OOB_peciMsg_L, nWrite - 1) ^ 0x80;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Build OOB Command                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_BuildCmd_l(ESPI_OOB_SMB_SLAVE_SRC_ADDR_EC,
                                              ESPI_OOB_SMB_SLAVE_DEST_ADDR_PMC_FW,
                                              ESPI_OOB_PECI_CMD,
                                              nWrite, (UINT8*)&ESPI_OOB_peciMsg_L,
                                              nRead,  ESPI_OOB_peciReadBuff_L));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send OOB Command                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_RunCmd_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_Ping                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a Ping command.                                                     */
/*                  This command is used to enumerate devices or determine if a device has been removed,   */
/*                  been powered-off, etc.                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_Ping (void)
{
    return ESPI_OOB_PECI_Trans(0x00, 0x00, ESPI_OOB_PECI_COMMAND_PING, NO_PECI_DATA, NO_PECI_DATA);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_GetDIB                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a GetDIB command.                                                   */
/*                  This command provides information regarding client revision number and the number of   */
/*                  supported domains.                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_GetDIB (void)
{
    return ESPI_OOB_PECI_Trans(0x01, 0x08, ESPI_OOB_PECI_COMMAND_GET_DIB, NO_PECI_DATA, NO_PECI_DATA);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_GetTemp                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a GetTemp command.                                                  */
/*                  This command is used to retrieve the maximum die temperature from a target PECI        */
/*                  address.                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_GetTemp (void)
{
    return ESPI_OOB_PECI_Trans(0x01, 0x02, ESPI_OOB_PECI_COMMAND_GET_TEMP, NO_PECI_DATA, NO_PECI_DATA);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_RdPkgConfig                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size - The desired data return size (BYTE/WORD/DWORD).                       */
/*                  host_id        - Host ID.                                                              */
/*                  index          - Requested service.                                                    */
/*                  parameter      - Service parameter value.                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdPkgConfig command.                                              */
/*                  This command provides read access to the Package Configuration Space (PCS) within the  */
/*                  processor, including various power and thermal management functions.                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_RdPkgConfig (
    ESPI_OOB_PECI_DATA_SIZE_T   read_data_size,
    UINT8                       host_id,
    UINT8                       index,
    UINT16                      parameter
)
{
    return ESPI_OOB_PECI_Trans(
                            0x05,
                            (UINT8)(read_data_size + 1),
                            ESPI_OOB_PECI_COMMAND_RD_PKG_CFG,
                            ESPI_OOB_PECI_HOST_ID(host_id) | (UINT32)index << 8 | (UINT32)parameter << 16,
                            NO_PECI_DATA);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_WrPkgConfig                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  write_data_size - The desired write granularity (BYTE/WORD/DWORD).                     */
/*                  host_id         - Host ID.                                                             */
/*                  index           - Requested service.                                                   */
/*                  parameter       - Service parameter value.                                             */
/*                  data            - Data to write to the processor Package Configuration Space.          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a WrPkgConfig command.                                              */
/*                  This command provides write access to the Package Configuration Space (PCS) within the */
/*                  processor, including various power and thermal management functions.                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_WrPkgConfig (
    ESPI_OOB_PECI_DATA_SIZE_T   write_data_size,
    UINT8                       host_id,
    UINT8                       index,
    UINT16                      parameter,
    UINT32                      data
)
{
    return ESPI_OOB_PECI_Trans(
                            (UINT8)(write_data_size + 6),
                            0x01,
                            ESPI_OOB_PECI_COMMAND_WR_PKG_CFG,
                            ESPI_OOB_PECI_HOST_ID(host_id) | (UINT32)index << 8 | (UINT32)parameter << 16,
                            data);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_RdIAMSR                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size - The desired data return size (BYTE/WORD/DWORD/QWORD).                 */
/*                  host_id        - Host ID.                                                              */
/*                  processor_id   - Logical processor ID within the CPU.                                  */
/*                  msr_address    - Model Specific Register address.                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdIAMSR command.                                                  */
/*                  This command provides read access to the Model Specific Registers (MSRs) defined in    */
/*                  processor's Intel Architecture (IA).                                                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_RdIAMSR (
    ESPI_OOB_PECI_DATA_SIZE_T   read_data_size,
    UINT8                       host_id,
    UINT8                       processor_id,
    UINT16                      msr_address
)
{
    return ESPI_OOB_PECI_Trans(
                            0x05,
                            (UINT8)(read_data_size + 1),
                            ESPI_OOB_PECI_COMMAND_RD_IAMSR,
                            ESPI_OOB_PECI_HOST_ID(host_id) | (UINT32)processor_id << 8 | (UINT32)msr_address << 16,
                            NO_PECI_DATA);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_RdPCIConfig                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size     - The desired data return size (BYTE/WORD/DWORD).                   */
/*                  host_id            - Host ID.                                                          */
/*                  pci_config_address - PCI configuration address (28-bit).                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdPCIConfig command.                                              */
/*                  This command provides sideband read access to the PCI configuration space maintained   */
/*                  in downstream devices external to the processor.                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_RdPCIConfig (
    ESPI_OOB_PECI_DATA_SIZE_T   read_data_size,
    UINT8                       host_id,
    UINT32                      pci_config_address
)
{
    return ESPI_OOB_PECI_Trans(
                            0x06,
                            (UINT8)(read_data_size + 1),
                            ESPI_OOB_PECI_COMMAND_RD_PCI_CFG,
                            ESPI_OOB_PECI_HOST_ID(host_id) | (UINT32)pci_config_address << 8,
                            (UINT32)LSN(MSB0(pci_config_address)));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_RdPCIConfigLocal                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  read_data_size     - The desired data return size (BYTE/WORD/DWORD).                   */
/*                  host_id            - Host ID.                                                          */
/*                  pci_config_address - PCI configuration address for local accesses (24-bit).            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a RdPCIConfigLocal command.                                         */
/*                  This command provides sideband read access to the PCI configuration space that resides */
/*                  within the processor.                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_RdPCIConfigLocal (
    ESPI_OOB_PECI_DATA_SIZE_T   read_data_size,
    UINT8                       host_id,
    UINT32                      pci_config_address
)
{
    return ESPI_OOB_PECI_Trans(
                            0x05,
                            (UINT8)(read_data_size + 1),
                            ESPI_OOB_PECI_COMMAND_RD_PCI_CFG_LOCAL,
                            ESPI_OOB_PECI_HOST_ID(host_id) | pci_config_address << 8,
                            NO_PECI_DATA);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_WrPCIConfigLocal                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  write_data_size    - The desired write granularity (BYTE/WORD/DWORD).                  */
/*                  host_id            - Host ID.                                                          */
/*                  pci_config_address - PCI configuration address for local accesses (24-bit).            */
/*                  data               - Data to write to the PCI configuration Space within the processor.*/
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a WrPCIConfigLocal command.                                         */
/*                  This command provides sideband write access to the PCI configuration space that        */
/*                  resides within the processor.                                                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_PECI_WrPCIConfigLocal (
    ESPI_OOB_PECI_DATA_SIZE_T   write_data_size,
    UINT8                       host_id,
    UINT32                      pci_config_address,
    UINT32                      data
)
{
    return ESPI_OOB_PECI_Trans(
                            (UINT8)(write_data_size + 6),
                            0x01,
                            ESPI_OOB_PECI_COMMAND_WR_PCI_CFG_LOCAL,
                            ESPI_OOB_PECI_HOST_ID(host_id) | pci_config_address << 8,
                            data);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_GetCompletionCode                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         PECI Completion Code.                                                                  */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the Completion Code of the last PECI transaction.               */
/*---------------------------------------------------------------------------------------------------------*/
ESPI_OOB_PECI_CC_T ESPI_OOB_PECI_GetCompletionCode (void)
{
    return (ESPI_OOB_PECI_CC_T)ESPI_OOB_peciCc_L;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_USB_Init                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  callback  - Application callback to be called from USB interrupt handler.              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes the OOB USB communication.                                    */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_OOB_USB_Init (ESPI_OOB_USB_CALLBACK_T callback)
{
    ESPI_OOB_usbCallback_L = callback;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_USB_Connect                                                                   */
/*                                                                                                         */
/* Parameters:      usb3PN              - USB3 Port Number                                                 */
/*                  usb2PN              - USB2 Port Number                                                 */
/*                  cableOrientation    - orientation of USB cable                                         */
/*                  ufp                 - Whether the PCH should be a UFP (upstream facing port(=1)) or    */
/*                                        DFP (downstream facing port(=0))                                 */
/* Returns:         Status of request                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends USB connect request message to the PCH                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_USB_Connect (
    UINT8   usb3PN,
    UINT8   usb2PN,
    BOOLEAN cableOrientation,
    BOOLEAN ufp
)
{
    ESPI_OOB_usbMsg_L.usage     = ESPI_OOB_USB_USAGE_PORT_CONNECT;
    ESPI_OOB_usbMsg_L.data[0]   = usb3PN;
    ESPI_OOB_usbMsg_L.data[1]   = usb2PN;
    ESPI_OOB_usbMsg_L.data[2]   = (UINT8)(ufp | (cableOrientation << 1));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Build OOB Command                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_BuildCmd_l(ESPI_OOB_SMB_SLAVE_SRC_ADDR_EC,
                                              ESPI_OOB_SMB_SLAVE_DEST_ADDR_CSME_FW,
                                              ESPI_OOB_USB_CMD,
                                              0x04, (UINT8*)&ESPI_OOB_usbMsg_L, 0, NULL));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send OOB Command                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_RunCmd_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_USB_Disconnect                                                                */
/*                                                                                                         */
/* Parameters:      usb3PN              - USB3 Port Number                                                 */
/*                  usb2PN              - USB2 Port Number                                                 */
/* Returns:         Status of request                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends USB disconnect request message to the PCH                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_OOB_USB_Disconnect (
    UINT8 usb3PN,
    UINT8 usb2PN
)
{
    ESPI_OOB_usbMsg_L.usage     = ESPI_OOB_USB_USAGE_PORT_DISCONNECT;
    ESPI_OOB_usbMsg_L.data[0]   = usb3PN;
    ESPI_OOB_usbMsg_L.data[1]   = usb2PN;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Build OOB Command                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_BuildCmd_l(ESPI_OOB_SMB_SLAVE_SRC_ADDR_EC,
                                              ESPI_OOB_SMB_SLAVE_DEST_ADDR_CSME_FW,
                                              ESPI_OOB_USB_CMD,
                                              0x03, (UINT8*)&ESPI_OOB_usbMsg_L, 0, NULL));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send OOB Command                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_RunCmd_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_CRASHLOG_Init                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  callback     - OOB hardware interrupt handler                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine registers OOB hardware interrupt handler                                  */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_OOB_CRASHLOG_Init (ESPI_OOB_CRASHLOG_CALLBACK_T callback)
{
    ESPI_OOB_crashLogCallback_L = callback;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_HW_Init                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  callback     - OOB hardware interrupt handler                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine registers OOB hardware interrupt handler                                  */
/*---------------------------------------------------------------------------------------------------------*/
void ESPI_OOB_HW_Init (ESPI_OOB_HW_CALLBACK_T callback)
{
    ESPI_OOB_hwCallback_L = callback;
}

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                     LOCAL FUNCTIONS IMPLEMENTATION                                      */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                 GENERAL LOCAL FUNCTIONS IMPLEMENTATION                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#if defined (MIWU_MODULE_TYPE) && defined (ESPI_MIWU_WAKEUP_SUPPORT)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_WakeUpHandler_l                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  miwu_src -                                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ...                                                                       */
/*lint -e{715}      Suppress 'miwu_src' not referenced                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_WakeUpHandler_l (MIWU_SRC_T miwu_src)
{
    //Do nothing
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_Reset_l                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine resets the eSPI internal state machine                                    */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_Reset_l (void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize general variables                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_bootLoad           = FALSE;
    ESPI_errorMask          = 0;
    ESPI_configUpdateEnable = 0;
    ESPI_configUpdateMask   = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize the Out-Of-Band (OOB) Channel state machine                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_Init_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize the Flash Access Channel state machine                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_Init_l();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_GetError_l                                                                        */
/*                                                                                                         */
/* Parameters:      mask - mask of relevant error bits to clean                                            */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks the root cause for an eSPI Bus Error.                              */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_GetError_l (UINT32 mask)
{
    UINT32 err = REG_READ(ESPI_ESPIERR);

    ESPI_errorMask = err;
    REG_WRITE(ESPI_ESPIERR, (err & mask));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_ResetConnfigUpdate_l                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine resets the last eSPI configuration updated.                               */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_ResetConnfigUpdate_l (void)
{
    ESPI_configUpdateEnable = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_IsChannelSlaveEnable_l                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  channel - eSPI channel                                                                 */
/*                                                                                                         */
/* Returns:         TRUE if the given channel is enabled; FALSE otherwise                                  */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks if an eSPI channel is enabled                                      */
/*---------------------------------------------------------------------------------------------------------*/
static BOOLEAN ESPI_IsChannelSlaveEnable_l (ESPI_CHANNEL_TYPE_T channel)
{
    BOOLEAN enable = FALSE;

    switch (channel)
    {
    case ESPI_CHANNEL_PC:
        enable = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_PCHANEN);
        break;

    case ESPI_CHANNEL_VW:
        enable = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_VWCHANEN);
        break;

    case ESPI_CHANNEL_FLASH:
        enable = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_FLASHCHANEN);
        break;

    case ESPI_CHANNEL_OOB:
        enable = READ_REG_FIELD(ESPI_ESPICFG, ESPICFG_OOBCHANEN);
        break;

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
    case ESPI_CHANNEL_PC_BM:
        enable = READ_REG_FIELD(ESPI_PERCFG, PERCFG_BMMEN);
        break;
#endif

    default:
        break;
    }
    return enable;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_ConfigUpdate_l                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles eSPI configuration update                                         */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_ConfigUpdate_l (void)
{
    ESPI_CHANNEL_TYPE_T chn;
    ESPI_OP_FREQ        freq;
    ESPI_IO_MODE_SELECT io;
    ESPI_ALERT_MODE     alert;
    BOOLEAN             crc;
    BOOLEAN             chnMasterEnable;

    /*-----------------------------------------------------------------------------------------------------*/
    /* eSPI configuration update eSPI channels update and logging.                                         */
    /* Skip Peripheral Channel, it is handled separately in Platform Reset (PLTRST#)                       */
    /*-----------------------------------------------------------------------------------------------------*/
    for (chn = ESPI_CHANNEL_VW;  (UINT)chn < ESPI_CHANNEL_NUM; chn++)
    {
        chnMasterEnable = ESPI_ChannelMasterEnable(chn);

        /*-------------------------------------------------------------------------------------------------*/
        /* Channel enabled/disabled by Master                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_BIT(ESPI_configUpdateMask, chn) != chnMasterEnable)
        {
            ESPI_ChannelSlaveEnable(chn, chnMasterEnable);
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* eSPI configuration update general capabilities logging                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    freq = ESPI_GetOpFreq();
    if ((ESPI_OP_FREQ)READ_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_OPFREQ) != freq)
    {
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_OPFREQ, freq);
        SET_VAR_BIT(ESPI_configUpdateEnable, ESPI_OPFREQ_BIT);
    }
    io = ESPI_GetIoModeSelect();
    if ((ESPI_IO_MODE_SELECT)READ_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_IOMODESEL) != io)
    {
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_IOMODESEL, io);
        SET_VAR_BIT(ESPI_configUpdateEnable, ESPI_IOMODESEL_BIT);
    }
    alert = ESPI_GetAlertMode();
    if ((ESPI_ALERT_MODE)READ_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_ALERTMODE) != alert)
    {
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_ALERTMODE, alert);
        SET_VAR_BIT(ESPI_configUpdateEnable, ESPI_ALERTMODE_BIT);
    }
    crc = ESPI_GetCrcCheckEnable();
    if ((BOOLEAN)READ_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_CRC_CHK_EN) != crc)
    {
        SET_VAR_FIELD(ESPI_configUpdateMask, ESPICFG_CRC_CHK_EN, crc);
        SET_VAR_BIT(ESPI_configUpdateEnable, ESPI_CRC_CHK_EN_BIT);
    }

#ifndef BOOT_LOAD_FROM_ESPI
    /*-----------------------------------------------------------------------------------------------------*/
    /* In case the firmware is loaded by the booter from SPI:                                              */
    /* The firmware must transmit the states of SLAVE_BOOT_LOAD_STATUS and SLAVE_BOOT_LOAD_DONE Virtual    */
    /* Wires immediately upon the eSPI slave VW channel being enabled by the eSPI-MC.                      */
    /*                                                                                                     */
    /* In case the firmware is loaded by the booter from eSPI:                                             */
    /* The booter must transmit the states of SLAVE_BOOT_LOAD_STATUS and SLAVE_BOOT_LOAD_DONE Virtual      */
    /* Wires after the firmware is loaded via the eSPI Flash-Access channel.                               */
    /*                                                                                                     */
    /* This is an indication provided to the eSPI master for code load completion into the chip internal   */
    /* memory.                                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)ESPI_VW_SendBootLoad(TRUE);
#endif
}


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                    PC LOCAL FUNCTIONS IMPLEMENTATION                                    */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_GetCurrReqSize_l                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType     - request type                                                             */
/*                  ramOffset   - offset in system RAM                                                     */
/*                  size        - size of the whole transaction                                            */
/*                  currSize    - pointer to current size                                                  */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates current request size for manual mode successive requests only  */
/*                  This routine avoids crossing pages in read/write operations                            */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_PC_BM_GetCurrReqSize_l (
    ESPI_PC_MSG_T           reqType,
    UINT64                  ramOffset,
    UINT32                  size,
    UINT32*                 currSize
)
{
    UINT64  sizeUpToEndOfSector;

    switch (reqType)
    {
    case ESPI_PC_MSG_LTR:
    case ESPI_PC_MSG_WRITE:
        /*-------------------------------------------------------------------------------------------------*/
        /* No limitations on LTR message and write request                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        *currSize = MIN(size, ESPI_PC_MAX_PAYLOAD_SIZE);
        break;

    case ESPI_PC_MSG_READ:
        sizeUpToEndOfSector = ROUND_UP(ramOffset, ESPI_PC_READ_SECTOR_SIZE) - ramOffset;
        *currSize = MIN(size, ESPI_PC_MAX_PAYLOAD_SIZE);
        if (sizeUpToEndOfSector > 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Avoid crossing sectors in read request                                                      */
            /*---------------------------------------------------------------------------------------------*/
            *currSize = MIN(*currSize, (UINT32)sizeUpToEndOfSector);
        }
        break;

    default:
        break;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_SetRequest_l                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType    - request type                                                              */
/*                  offset     - offset in flash                                                           */
/*                  size       - size of transaction                                                       */
/*                  buffer     - input buffer (used for write transactions)                                */
/*                  tag        - request tag                                                               */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set request on transmit buffer                                            */
/*                  Notice assumption that buffer size is aligned to sizeof(UINT32)                        */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS ESPI_PC_BM_SetRequest_l (
    ESPI_PC_MSG_T           reqType,
    UINT64                  offset,
    UINT32                  size,
    const UINT32*           buffer,
    UINT8                   tag
)
{
    ESPI_PC_REQ_HDR_T   req = {0};
    UINT                hdrLen = 0;
    UINT                i;
    UINT                loopCount;
    const UINT32*       src;
    UINT64              address;
    UINT32              dataRemainder;
    UINT32              temp;
    UINT8*              tempPtr = (UINT8*)&temp;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Aligned with ESPI_PC_MSG_T, First field is for 32b memory addressing, and the second is for 64b     */
    /* memory addressing                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    const ESPI_PC_BM_CYC_T cycleTypes[] = {{(UINT8)ESPI_PC_BM_CYC_TYPE_MEM_READ_32, (UINT8)ESPI_PC_BM_CYC_TYPE_MEM_READ_64},
                                           {(UINT8)ESPI_PC_BM_CYC_TYPE_MEM_WRITE_32, (UINT8)ESPI_PC_BM_CYC_TYPE_MEM_WRITE_64}};

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set system memory to 64/32 bits addressing                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_PERCTL, PERCTL_MEM64_ACCESS, ESPI_PC_BM_sysRAMis64bAddr_L);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write the whole packet (including header and data payload but excluding the transaction opcode)     */
    /* in the PBM Tx buffer                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/

    SET_VAR_FIELD(req.tagPlusLength, HEADER_LENGTH, size);
    SET_VAR_FIELD(req.tagPlusLength, HEADER_TAG, tag);
    req.tagPlusLength = LE16(req.tagPlusLength);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill header according to message type                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (reqType)
    {
    case ESPI_PC_MSG_READ:
    case ESPI_PC_MSG_WRITE:
        /*-------------------------------------------------------------------------------------------------*/
        /* If system memory has 64 bits addressing                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_PC_BM_sysRAMis64bAddr_L)
        {
            req.cycleType = cycleTypes[reqType]._64bAddrCycType;
            address = LE64(offset);
            req.addr.addr64b = address;
            hdrLen = ESPI_PC_64_BIT_HDR_SIZE;
        }
        else
        {
           req.cycleType = cycleTypes[reqType]._32bAddrCycType;
           req.addr.addr32b = LE32((UINT32)offset);
           hdrLen = ESPI_PC_32_BIT_HDR_SIZE;
        }
        break;

    case ESPI_PC_MSG_LTR:
        req.msgCode = ESPI_PC_BM_MSG_CODE_LTR;
        if (size == 0)
        {
            req.cycleType = ESPI_PC_BM_CYC_TYPE_MSG | ESPI_PC_BM_MSG_ROUTE_LTR;
        }
        else
        {
            req.cycleType = ESPI_PC_BM_CYC_TYPE_MSG_WITH_DATA | ESPI_PC_BM_MSG_ROUTE_LTR;
        }

        req.addr.msgSpec = (UINT32)offset;
        hdrLen = ESPI_PC_MSG_HDR_SIZE;
        break;

    default:
        return DEFS_STATUS_INVALID_PARAMETER;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write the header into the transfer buffer                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    loopCount = hdrLen / sizeof(UINT32);
    src = (UINT32*)(void*)&req;
    for (i = 0; i < loopCount; i++)
    {
        REG_WRITE(ESPI_PBMTXWRHEAD,src[i]);
    }

    switch (reqType)
    {
    case ESPI_PC_MSG_READ:
        /*-------------------------------------------------------------------------------------------------*/
        /* Write the packet size in PERCTL register(including header and data payload but excluding the    */
        /* transaction opcode).                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_PERCTL,PERCTL_BMPKT_LEN,(hdrLen - 1));
        break;

    case ESPI_PC_MSG_LTR:
        SET_REG_FIELD(ESPI_PERCTL,PERCTL_BMPKT_LEN,hdrLen);
        break;

    case ESPI_PC_MSG_WRITE:
        if (size > 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Write the data to be written into the transfer buffer                                       */
            /*---------------------------------------------------------------------------------------------*/
            loopCount = size / sizeof(UINT32);
            dataRemainder = size % sizeof(UINT32);
            src = buffer;
            for (i = 0; i < loopCount; i++)
            {
                REG_WRITE(ESPI_PBMTXWRHEAD,src[i]);
            }
            if (dataRemainder > 0)
            {
                temp = 0;
                for (i = 0; i < dataRemainder; i++)
                {
                    tempPtr[i] = ((UINT8*)buffer)[(loopCount * sizeof(UINT32)) + i];
                }
                REG_WRITE(ESPI_PBMTXWRHEAD, temp);
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Write the packet size in PERCTL register(including header and data payload but excluding the    */
        /* transaction opcode).                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_PERCTL,PERCTL_BMPKT_LEN,(hdrLen + size - 1));
        break;

    default:
        break;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_HandleInputPacket_l                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  req         -  request type                                                            */
/*                  buffer      -  output buffer used for read operation                                   */
/*                  strpHdr     -  equals TRUE if strip header mode is on and FALSE otherwise              */
/*                  size        -  size of transaction (requested size)                                    */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine read and process input packet from receive buffer                         */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_PC_BM_HandleInputPacket_l (
    ESPI_PC_MSG_T           req,
    UINT32*                 buffer,
    BOOLEAN                 strpHdr,
    UINT32                  size
)
{
    UINT                    i;
    UINT32                  loopCount;
    UINT32                  payloadDataLen;
    ESPI_PC_REQ_HDR_T       header;
    UINT32                  dataRemainder;
    UINT32                  temp;
    UINT8*                  tempPtr = (UINT8*)&temp;
    UINT32                  alignedSize;
    UINT16                  tagPlusLength;
    UINT32*                 dest;

    /*-----------------------------------------------------------------------------------------------------*/
    /* No header in receive buffer                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    if (strpHdr)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* If the request was a read request                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (req == ESPI_PC_MSG_READ)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Read data from receive buffer                                                               */
            /*---------------------------------------------------------------------------------------------*/
            loopCount = size / sizeof(UINT32);
            dataRemainder = size - (loopCount * sizeof(UINT32));
            for (i = 0; i < loopCount; i++)
            {
                buffer[i] = REG_READ(ESPI_PBMRXRDHEAD);
            }
            if (dataRemainder > 0)
            {
                alignedSize = (loopCount * sizeof(UINT32));
                temp = REG_READ(ESPI_PBMRXRDHEAD);
                for (i = 0; i < dataRemainder; i++)
                {
                    ((UINT8*)buffer)[i + alignedSize] = tempPtr[i];
                }
            }

        }
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* If the request was a read request                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (req == ESPI_PC_MSG_READ)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Read header from receive buffer                                                             */
            /*---------------------------------------------------------------------------------------------*/
            loopCount = ESPI_PC_READ_REP_HDR_SIZE / sizeof(UINT32);
            dest = ((UINT32*)(void*)&header);
            for (i = 0; i < loopCount; i++)
            {
                dest[i] = REG_READ(ESPI_PBMRXRDHEAD);
            }
            tagPlusLength              = LE16(header.tagPlusLength);

            /*---------------------------------------------------------------------------------------------*/
            /* Check header fields                                                                         */
            /*---------------------------------------------------------------------------------------------*/
            DEFS_STATUS_COND_CHECK((header.cycleType != ESPI_UNSUCCESSFUL_COMPLETION_WO_DATA),
                DEFS_STATUS_RESPONSE_CANT_BE_PROVIDED);

            payloadDataLen = READ_VAR_FIELD(tagPlusLength, HEADER_LENGTH);

            switch (req)
            {
            case ESPI_FLASH_REQ_ACC_READ:
                /*-----------------------------------------------------------------------------------------*/
                /* Check header fields                                                                     */
                /*-----------------------------------------------------------------------------------------*/
                DEFS_STATUS_COND_CHECK((header.cycleType == ESPI_SUCCESSFUL_COMPLETION_WITH_DATA),
                                       DEFS_STATUS_INVALID_DATA_FIELD);
                DEFS_STATUS_COND_CHECK((payloadDataLen == size), DEFS_STATUS_INVALID_DATA_FIELD);

                /*-----------------------------------------------------------------------------------------*/
                /* Copy data from receive buffer                                                           */
                /*-----------------------------------------------------------------------------------------*/
                loopCount = payloadDataLen / sizeof(UINT32);
                dataRemainder = size - (loopCount * sizeof(UINT32));
                for (i = 0; i < loopCount; i++)
                {
                    buffer[i] = REG_READ(ESPI_PBMRXRDHEAD);
                }
                if (dataRemainder > 0)
                {
                    alignedSize = (loopCount * sizeof(UINT32));
                    temp = REG_READ(ESPI_PBMRXRDHEAD);
                    for (i = 0; i < dataRemainder; i++)
                    {
                        ((UINT8*)buffer)[i + alignedSize] = tempPtr[i];
                    }
                }
                break;

            case ESPI_PC_MSG_WRITE:
                //No reply on write transaction
                break;

            default:
                break;
            }
        }
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_ReceiveDataBuffer_l                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType    - request type                                                              */
/*                  buffer     - output buffer for read operation                                          */
/*                  strpHdr    - equals TRUE if strip header mode is on and FALSE otherwise                */
/*                  size       - size of transaction (requested size)                                      */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine receives and handles data buffer under polling mode                       */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_PC_BM_ReceiveDataBuffer_l (
    ESPI_PC_MSG_T           reqType,
    UINT32*                 buffer,
    BOOLEAN                 strpHdr,
    UINT32                  size
)
{
    if (reqType != ESPI_PC_MSG_READ)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Wait till transfer is done                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        BUSY_WAIT_TIMEOUT((READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_BMTXDONE) == FALSE), 0x200000) ;

        /*-------------------------------------------------------------------------------------------------*/
        /* Clear status                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMTXDONE));
    }
    if (reqType == ESPI_PC_MSG_READ)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Wait till receive buffer is full                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        BUSY_WAIT_TIMEOUT((READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_PBMRX) == FALSE), 0x200000) ;

        /*-------------------------------------------------------------------------------------------------*/
        /* Clear status                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_PBMRX));

        /*-------------------------------------------------------------------------------------------------*/
        /* Handle packet                                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(ESPI_PC_BM_HandleInputPacket_l(reqType, buffer, strpHdr, size));
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_ReqManual_l                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  msgType    - request type (read/write/message)                                         */
/*                  offset     - offset in flash or message header specific(4B)                            */
/*                  size       - size of transaction                                                       */
/*                  polling    - equals TRUE if polling is required and FALSE for interrupt mode           */
/*                  buffer     - Input/output buffer, depends on reqType                                   */
/*                  strpHdr    - equals TRUE if strip header is required and FALSE otherwise               */
/*                               * In case it is request message, polling mode is not relevant since no    */
/*                               response is needed                                                        */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends manual request to host                                              */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS  ESPI_PC_BM_ReqManual_l (
    ESPI_PC_MSG_T   msgType,
    UINT64          offset,
    UINT32          size,
    BOOLEAN         pollingMode,
    UINT32*         buffer,
    BOOLEAN         strpHdr
)
{
    UINT          currSize;
    UINT32        buffOffset;
    DEFS_STATUS   status;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get current request size                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_PC_BM_GetCurrReqSize_l(msgType, offset, size, &currSize));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify transmit buffer is empty                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((READ_REG_FIELD(ESPI_PERCTL, PERCTL_BM_NP_AVAIL) == FALSE) &&
                           (READ_REG_FIELD(ESPI_PERCTL, PERCTL_BM_MSG_AVAIL) == FALSE) &&
                           (READ_REG_FIELD(ESPI_PERCTL, PERCTL_BM_PC_AVAIL) == FALSE),
                           DEFS_STATUS_SYSTEM_BUSY);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set manual mode                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_PC_AUTO_MODE(FALSE);

    if (!pollingMode)
    {
        ESPI_PC_BM_usrReqInfo_L.buffer          = buffer;
        ESPI_PC_BM_usrReqInfo_L.buffOffset      = 0;
        ESPI_PC_BM_usrReqInfo_L.currSize        = currSize;
        ESPI_PC_BM_usrReqInfo_L.RamOffset       = offset;
        ESPI_PC_BM_usrReqInfo_L.reqType         = msgType;
        ESPI_PC_BM_usrReqInfo_L.size            = size;
        ESPI_PC_BM_usrReqInfo_L.status          = DEFS_STATUS_OK;
        ESPI_PC_BM_usrReqInfo_L.strpHdr         = strpHdr;
        ESPI_PC_BM_usrReqInfo_L.useDMA          = FALSE;
        ESPI_PC_BM_usrReqInfo_L.buffTransferred = 0;
        ESPI_PC_BM_usrReqInfo_L.manualMode      = TRUE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set header/non header in receive buffer                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMSTRPHDR, strpHdr);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Update request tag                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_PC_BM_currReqTag_L = ESPI_PC_BM_GET_NEXT_TAG(ESPI_PC_BM_currReqTag_L);

    DEFS_STATUS_RET_CHECK(ESPI_PC_BM_SetRequest_l(msgType, offset, currSize, buffer, ESPI_PC_BM_currReqTag_L));

    ESPI_PC_BM_status_L = DEFS_STATUS_SYSTEM_BUSY;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enqueue the transaction                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    status = ESPI_PC_BM_EnqueuePacket_l(msgType);
    if (status != DEFS_STATUS_OK)
    {
        ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
        return status;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Polling mode                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    if (pollingMode)
    {
        buffOffset = 0;
        status = ESPI_PC_BM_ReceiveDataBuffer_l(msgType, buffer, strpHdr, currSize);
        if (status != DEFS_STATUS_OK)
        {
            ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
            return status;
        }
        buffOffset += currSize;

        while (buffOffset < size)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Get current request size                                                                    */
            /*---------------------------------------------------------------------------------------------*/
            status = (ESPI_PC_BM_GetCurrReqSize_l(msgType, (offset + buffOffset), (size - buffOffset), &currSize));

            if (status != DEFS_STATUS_OK)
            {
                ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
                return status;
            }

            /*---------------------------------------------------------------------------------------------*/
            /* Update request tag                                                                          */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_PC_BM_currReqTag_L = ESPI_PC_BM_GET_NEXT_TAG(ESPI_PC_BM_currReqTag_L);

            /*---------------------------------------------------------------------------------------------*/
            /* Set next request and send                                                                   */
            /*---------------------------------------------------------------------------------------------*/
            status = (ESPI_PC_BM_SetRequest_l(msgType,
                                              (offset + buffOffset),
                                              currSize,
                                              (UINT32*)(void*)&(((UINT8*)buffer)[buffOffset]),
                                              ESPI_PC_BM_currReqTag_L));
            if (status != DEFS_STATUS_OK)
            {
                ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
                return status;
            }

            /*---------------------------------------------------------------------------------------------*/
            /* Enqueue the packet for transmission                                                         */
            /*---------------------------------------------------------------------------------------------*/
            status = ESPI_PC_BM_EnqueuePacket_l(msgType);
            if (status != DEFS_STATUS_OK)
            {
                ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
                return status;
            }

            status = ESPI_PC_BM_ReceiveDataBuffer_l(msgType,
                                                    (UINT32*)(void*)&(((UINT8*)buffer)[buffOffset]),
                                                    strpHdr,
                                                    currSize);
            if (status != DEFS_STATUS_OK)
            {
                ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
                return status;
            }
            buffOffset += currSize;
        }

        if (msgType == ESPI_PC_MSG_READ)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Clear transfer done status                                                                  */
            /*---------------------------------------------------------------------------------------------*/
            REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMTXDONE));
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* If no error occurred                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_PC_BM_status_L == DEFS_STATUS_SYSTEM_BUSY)
        {
            ESPI_PC_BM_status_L = DEFS_STATUS_OK;
        }
        return ESPI_PC_BM_status_L;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_ReadReqAuto_l                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  offset     - offset in flash                                                           */
/*                  size       - size of transaction                                                       */
/*                  polling    - equals TRUE if polling is required and FALSE for interrupt mode           */
/*                  buffer     - Input/output buffer, depends on reqType                                   */
/*                  strpHdr    - equals TRUE if strip header is required and FALSE otherwise               */
/*                  useDMA     - equals TRUE if DMA is required and FALSE otherwise                        */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a read automatic request to host.                                   */
/*                  RAM offset must be aligned to size of transaction                                      */
/*                  Size must be aligned to 64B.                                                           */
/*                  Maximum size for transaction is (64B*256)                                              */
/*                  If using DMA mode the buffer must be at least 4B aligned , 16B aligned will improve    */
/*                  performance.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
//lint -efunc(527, ESPI_PC_BM_ReadReqAuto_l) Suppress Unreachable code at token ';'
static DEFS_STATUS  ESPI_PC_BM_ReadReqAuto_l (
    UINT64  offset,
    UINT32  size,
    BOOLEAN polling,
    UINT32* buffer,
    BOOLEAN strpHdr,
    BOOLEAN useDMA
)
{
    UINT16      numOfTrans  = (UINT16)(size / ESPI_PC_MAX_PAYLOAD_SIZE);
    DEFS_STATUS status      = DEFS_STATUS_OK;
    UINT        j;
    UINT32      intEnableReg;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error check                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
#ifndef GDMA_MODULE_TYPE
    DEFS_STATUS_COND_CHECK((useDMA == FALSE), DEFS_STATUS_INVALID_PARAMETER);
#endif
    DEFS_STATUS_COND_CHECK(numOfTrans <= ESPI_PC_PAUTO_MAX_TRANS_COUNT, DEFS_STATUS_INVALID_DATA_SIZE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify transmit buffer is empty                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((READ_REG_FIELD(ESPI_PERCTL, PERCTL_BM_NP_AVAIL)  == FALSE) &&
                           (READ_REG_FIELD(ESPI_PERCTL, PERCTL_BM_MSG_AVAIL) == FALSE) &&
                           (READ_REG_FIELD(ESPI_PERCTL, PERCTL_BM_PC_AVAIL)  == FALSE),
                           DEFS_STATUS_SYSTEM_BUSY);

    if (!polling)
    {
        ESPI_PC_BM_usrReqInfo_L.buffer          = buffer;
        ESPI_PC_BM_usrReqInfo_L.buffOffset      = 0;
        ESPI_PC_BM_usrReqInfo_L.currSize        = ESPI_PC_MAX_PAYLOAD_SIZE;
        ESPI_PC_BM_usrReqInfo_L.RamOffset       = offset;
        ESPI_PC_BM_usrReqInfo_L.reqType         = ESPI_PC_MSG_READ;
        ESPI_PC_BM_usrReqInfo_L.size            = size;
        ESPI_PC_BM_usrReqInfo_L.status          = DEFS_STATUS_OK;
        ESPI_PC_BM_usrReqInfo_L.strpHdr         = useDMA ? TRUE : strpHdr;
        ESPI_PC_BM_usrReqInfo_L.useDMA          = useDMA;
        ESPI_PC_BM_usrReqInfo_L.buffTransferred = 0;
        ESPI_PC_BM_usrReqInfo_L.manualMode      = FALSE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set automatic mode                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_PC_AUTO_MODE(TRUE);

    if (useDMA)
    {
#ifdef GDMA_MODULE_TYPE
        /*-------------------------------------------------------------------------------------------------*/
        /* Strip header                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMSTRPHDR, 0x01);

        /*-------------------------------------------------------------------------------------------------*/
        /* Initialize GDMA module                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(GDMA_InitEspi(ESPI_GDMA_MODULE, ESPI_PC_BM_GDMA_CHANNEL));

        /*-------------------------------------------------------------------------------------------------*/
        /* Find relevant transfer width according to input buffer                                          */
        /*-------------------------------------------------------------------------------------------------*/
        if ((UINT32)buffer % _16B_ == 0)
        {
            DEFS_STATUS_RET_CHECK(GDMA_Config(ESPI_GDMA_MODULE, ESPI_PC_BM_GDMA_CHANNEL, FALSE,FALSE,GDMA_TRANSFER_WIDTH_16B));
            /*---------------------------------------------------------------------------------------------*/
            /* Define threshold value of the flash receive buffer above which a DMA request is asserted    */
            /*---------------------------------------------------------------------------------------------*/
            SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMDMATHRESH, ESPI_BM_DMA_THRESHOLD_16B);
        }
        else if ((UINT32)buffer % _4B_ == 0)
        {
            DEFS_STATUS_RET_CHECK(GDMA_Config(ESPI_GDMA_MODULE, ESPI_PC_BM_GDMA_CHANNEL, FALSE,FALSE,GDMA_TRANSFER_WIDTH_4B));
            /*---------------------------------------------------------------------------------------------*/
            /* Define threshold value of the flash receive buffer above which a DMA request is asserted    */
            /*---------------------------------------------------------------------------------------------*/
            SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMDMATHRESH, ESPI_BM_DMA_THRESHOLD_4B);
        }
        else
        {
            return DEFS_STATUS_FAIL;
        }
#endif  /* GDMA_MODULE_TYPE */
    }
    else // No DMA
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* DMA request is disabled                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMDMATHRESH, ESPI_BM_DMA_THRESHOLD_DISABLE);

        /*-------------------------------------------------------------------------------------------------*/
        /* Set header/non header in receive buffer                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMSTRPHDR, strpHdr);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set number of transactions required                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMBURSTSIZE, (numOfTrans- 1));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set tag for next request                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_PC_BM_currReqTag_L = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set request in transfer buffer                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_PC_BM_SetRequest_l(ESPI_PC_MSG_READ, offset, ESPI_PC_MAX_PAYLOAD_SIZE,
                                                  (UINT32*)(void*)buffer, ESPI_PC_BM_currReqTag_L));
    if (useDMA)
    {
#ifdef GDMA_MODULE_TYPE
        ESPI_PC_BM_RX_INTERRUPT_SAVE_DISABLE(ESPI_PC_BM_RXIE_L);
        ESPI_PC_BM_TXDONE_INTERRUPT_SAVE_DISABLE(ESPI_PC_BM_TXDONE_L);

        status = GDMA_Transfer(ESPI_GDMA_MODULE, ESPI_PC_BM_GDMA_CHANNEL, REG_ADDR(ESPI_PBMRXRDHEAD), (UINT8*)buffer, size, ESPI_PC_BM_GdmaIntHandler_l);
        if (status != DEFS_STATUS_OK)
        {
            ESPI_PC_BM_ExitAutoReadRequest_l(status, useDMA);
            return ESPI_PC_BM_status_L;
        }
#endif  /* GDMA_MODULE_TYPE */
    }

    ESPI_PC_BM_status_L = DEFS_STATUS_SYSTEM_BUSY;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Enqueue the first transaction (the other transactions will be generated automatically)              */
    /*-----------------------------------------------------------------------------------------------------*/
    status = ESPI_PC_BM_EnqueuePacket_l(ESPI_PC_MSG_READ);
    if (status != DEFS_STATUS_OK)
    {
        ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
        return status;
    }

    if (polling)
    {
        if (!useDMA)
        {
            j = 0;
            /*---------------------------------------------------------------------------------------------*/
            /* While burst mode done status is not set                                                     */
            /*---------------------------------------------------------------------------------------------*/
            while((READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_BMBURSTDONE) == FALSE))
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Wait till receive buffer is full                                                        */
                /*-----------------------------------------------------------------------------------------*/
                status= ESPI_PC_BM_ReceiveDataBuffer_l(ESPI_PC_MSG_READ,
                                                       (UINT32*)(void*)&(((UINT8*)buffer)[j * ESPI_PC_MAX_PAYLOAD_SIZE]),
                                                       strpHdr,
                                                       ESPI_PC_MAX_PAYLOAD_SIZE);
                j++;
                if (status != DEFS_STATUS_OK)
                {
                    break;
                }

                /*-----------------------------------------------------------------------------------------*/
                /* While receive buffer is full                                                            */
                /*-----------------------------------------------------------------------------------------*/
                while ((j < numOfTrans) && (READ_REG_FIELD(ESPI_PERCTL,PERCTL_BMBURST_BFULL) == 0x01))
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* Clear status                                                                        */
                    /*-------------------------------------------------------------------------------------*/
                    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_PBMRX));

                    /*-------------------------------------------------------------------------------------*/
                    /* Handle packet                                                                       */
                    /*-------------------------------------------------------------------------------------*/
                    status = (ESPI_PC_BM_HandleInputPacket_l(ESPI_PC_MSG_READ,
                                                            (UINT32*)(void*)&(((UINT8*)buffer)[j * ESPI_PC_MAX_PAYLOAD_SIZE]),
                                                            strpHdr,
                                                            ESPI_PC_MAX_PAYLOAD_SIZE));
                    j++;
                    if (status != DEFS_STATUS_OK)
                    {
                        break;
                    }
                }
            }
        }

        if (useDMA)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Wait till request transfer is done                                                          */
            /*---------------------------------------------------------------------------------------------*/
            j = ESPI_FLASH_AUTO_AMDONE_LOOP_COUNT;
            do
            {
                if (j-- == 0)
                {
                    status = DEFS_STATUS_RESPONSE_TIMEOUT;
                    break;
                }
            } while (READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_BMBURSTDONE) == FALSE);
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Check that no error occurred. Error can be: Unsuccessful Completion or protocol error           */
        /* Incorrect TAG or Length < 64 bytes                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_BMBURSTERR) == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Disable interrupts such that ESPI_errorMask will not be changed                             */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_INTERRUPT_SAVE_DISABLE(intEnableReg);

            /*---------------------------------------------------------------------------------------------*/
            /* Get error and clean relevant bits                                                           */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_GetError_l(ESPI_PC_BM_ESPIERR_MASK);

            /*---------------------------------------------------------------------------------------------*/
            /* Handle burst error                                                                          */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_PC_BM_HandleBurstErr_l(useDMA);

            /*---------------------------------------------------------------------------------------------*/
            /* Restore interrupts                                                                          */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_INTERRUPT_RESTORE(intEnableReg);
        }
        else
        {
            ESPI_PC_BM_ExitAutoReadRequest_l(DEFS_STATUS_OK, useDMA);
        }

        return ESPI_PC_BM_status_L;
    }
    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_HandleReqRes_l                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles request response under interrupt mode                             */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_PC_BM_HandleReqRes_l (void)
{
    DEFS_STATUS status = DEFS_STATUS_OK;

    /*-----------------------------------------------------------------------------------------------------*/
    /* If its DMA mode, ignore                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    if (ESPI_PC_BM_usrReqInfo_L.useDMA == TRUE)
    {
        return;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* If the user expects data                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    if (ESPI_PC_BM_usrReqInfo_L.buffOffset < ESPI_PC_BM_usrReqInfo_L.size)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* If packet handling failed                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_PC_BM_HandleInputPacket_l(ESPI_PC_BM_usrReqInfo_L.reqType,
                                           (UINT32*)(void*)&((UINT8*)ESPI_PC_BM_usrReqInfo_L.buffer)[ESPI_PC_BM_usrReqInfo_L.buffOffset],
                                           ESPI_PC_BM_usrReqInfo_L.strpHdr,
                                           ESPI_PC_BM_usrReqInfo_L.currSize)
            != DEFS_STATUS_OK)
        {
            ESPI_PC_BM_usrReqInfo_L.status= DEFS_STATUS_FAIL;
        }
        ESPI_PC_BM_usrReqInfo_L.buffOffset += ESPI_PC_BM_usrReqInfo_L.currSize;


        /*-------------------------------------------------------------------------------------------------*/
        /* If it's automatic mode (read operation only)                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if ((ESPI_PC_BM_usrReqInfo_L.manualMode == FALSE) &&
            (ESPI_PC_BM_usrReqInfo_L.reqType == ESPI_PC_MSG_READ))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* While there is more data in buffers                                                         */
            /*---------------------------------------------------------------------------------------------*/
            while ((ESPI_PC_BM_usrReqInfo_L.buffOffset < ESPI_PC_BM_usrReqInfo_L.size) &&
                   (READ_REG_FIELD(ESPI_PERCTL, PERCTL_BMBURST_BFULL) == TRUE))
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Clear receive buffer status                                                             */
                /*-----------------------------------------------------------------------------------------*/
                REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_PBMRX));

                /*-----------------------------------------------------------------------------------------*/
                /* Handle packet                                                                           */
                /*-----------------------------------------------------------------------------------------*/
                status = ESPI_PC_BM_HandleInputPacket_l(ESPI_PC_BM_usrReqInfo_L.reqType,
                                                        (UINT32*)(void*)&((UINT8*)ESPI_PC_BM_usrReqInfo_L.buffer)[ESPI_PC_BM_usrReqInfo_L.buffOffset],
                                                        ESPI_PC_BM_usrReqInfo_L.strpHdr,
                                                        ESPI_PC_BM_usrReqInfo_L.currSize);
                /*-----------------------------------------------------------------------------------------*/
                /* If packet handling failed                                                               */
                /*-----------------------------------------------------------------------------------------*/
                if (status != DEFS_STATUS_OK)
                {
                    ESPI_PC_BM_usrReqInfo_L.status = DEFS_STATUS_FAIL;
                }
                ESPI_PC_BM_usrReqInfo_L.buffOffset += ESPI_PC_BM_usrReqInfo_L.currSize;
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* If this is manual mode                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_PC_BM_usrReqInfo_L.manualMode == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* If operation is not done                                                                    */
            /*---------------------------------------------------------------------------------------------*/
            if (ESPI_PC_BM_usrReqInfo_L.buffOffset < ESPI_PC_BM_usrReqInfo_L.size)
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Get current request size                                                                */
                /*-----------------------------------------------------------------------------------------*/
                if (ESPI_PC_BM_GetCurrReqSize_l(ESPI_PC_BM_usrReqInfo_L.reqType,
                                                (ESPI_PC_BM_usrReqInfo_L.RamOffset + ESPI_PC_BM_usrReqInfo_L.buffOffset),
                                                (ESPI_PC_BM_usrReqInfo_L.size - ESPI_PC_BM_usrReqInfo_L.buffOffset),
                                                &ESPI_PC_BM_usrReqInfo_L.currSize) != DEFS_STATUS_OK)
                {
                    ESPI_PC_BM_usrReqInfo_L.status = DEFS_STATUS_FAIL;
                }

                /*-----------------------------------------------------------------------------------------*/
                /* Set tag for next request                                                                */
                /*-----------------------------------------------------------------------------------------*/
                ESPI_PC_BM_currReqTag_L = ESPI_PC_BM_GET_NEXT_TAG(ESPI_PC_BM_currReqTag_L);

                /*-----------------------------------------------------------------------------------------*/
                /* Set next request                                                                        */
                /*-----------------------------------------------------------------------------------------*/
                (void)(ESPI_PC_BM_SetRequest_l(ESPI_PC_BM_usrReqInfo_L.reqType,
                                               (ESPI_PC_BM_usrReqInfo_L.RamOffset + ESPI_PC_BM_usrReqInfo_L.buffOffset),
                                               ESPI_PC_BM_usrReqInfo_L.currSize,
                                               (UINT32*)(void*)&((UINT8*)ESPI_PC_BM_usrReqInfo_L.buffer)[ESPI_PC_BM_usrReqInfo_L.buffOffset],
                                               ESPI_PC_BM_currReqTag_L));

                ESPI_PC_BM_usrReqInfo_L.buffTransferred = 0;

                /*-----------------------------------------------------------------------------------------*/
                /* Clear transfer done indication of last transaction                                      */
                /*-----------------------------------------------------------------------------------------*/
                if (READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_BMTXDONE) == 0x01)
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* Clear status                                                                        */
                    /*-------------------------------------------------------------------------------------*/
                    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMTXDONE));
                }
                /*-----------------------------------------------------------------------------------------*/
                /* Enqueue the packet for transmission                                                     */
                /*-----------------------------------------------------------------------------------------*/
                status = ESPI_PC_BM_EnqueuePacket_l(ESPI_PC_BM_usrReqInfo_L.reqType);
                if (status != DEFS_STATUS_OK)
                {
                    ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
                }
            }
            else
            {
                ESPI_PC_BM_status_L = ESPI_PC_BM_usrReqInfo_L.status;
                if (ESPI_PC_BM_usrReqInfo_L.reqType == ESPI_PC_MSG_READ)
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* operation is done, notify user                                                      */
                    /*-------------------------------------------------------------------------------------*/
                    EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_PC_BM_MASTER_DATA_RCV));
                    EXECUTE_FUNC(ESPI_PC_BM_userIntHandler_L,   (ESPI_INT_PC_BM_MASTER_DATA_RCV));
                }
                else
                {
                    EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_PC_BM_MASTER_DATA_TRANSMITTED));
                    EXECUTE_FUNC(ESPI_PC_BM_userIntHandler_L,   (ESPI_INT_PC_BM_MASTER_DATA_TRANSMITTED));
                }
            }
        }
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* User does not expects data                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Only for single mode                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_PC_AUTO_MODE_IS_ON == FALSE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Send host NON FATAL ERROR virtual wire indication                                           */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_VW_SmNonFatalError_l();
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_HandleInMessage_l                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles incoming message from host, called from interrupt only            */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_PC_BM_HandleInMessage_l (void)
{
    UINT                    i;
    UINT32                  loopCount;
    UINT32                  payloadDataLen;
    ESPI_PC_REQ_HDR_T       header;
    UINT16                  tagPlusLength;
    UINT32*                 dest;
    UINT32                  buff[ESPI_PC_MAX_PAYLOAD_SIZE];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read header from receive buffer                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    header.tagPlusLength = 0x00;
    loopCount = ESPI_PC_MSG_HDR_SIZE / sizeof(UINT32);
    dest = ((UINT32*)(void*)&header);
    for (i = 0; i < loopCount; i++)
    {
        dest[i] = REG_READ(ESPI_PBMRXRDHEAD);
    }
    tagPlusLength = LE16(header.tagPlusLength);

    /*-----------------------------------------------------------------------------------------------------*/
    /* If the cycle type is not a message                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    if (((header.cycleType & ESPI_PC_BM_CYC_TYPE_MSG_MASK) != ESPI_PC_BM_CYC_TYPE_MSG) &&
        ((header.cycleType & ESPI_PC_BM_CYC_TYPE_MSG_MASK) != ESPI_PC_BM_CYC_TYPE_MSG_WITH_DATA))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Drop packet                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        return;
    }

    payloadDataLen = READ_VAR_FIELD(tagPlusLength, HEADER_LENGTH);

    /*-----------------------------------------------------------------------------------------------------*/
    /* If payload field in header is non zero and the cycle type is not a message with data                */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((payloadDataLen > 0) && ((header.cycleType & ESPI_PC_BM_CYC_TYPE_MSG_MASK) != ESPI_PC_BM_CYC_TYPE_MSG_WITH_DATA))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Drop packet                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        return;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Place message specific header in buffer                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    *buff = header.addr.msgSpec;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Place message data if there is any                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    if (payloadDataLen > 0)
    {
        loopCount = payloadDataLen / sizeof(UINT32);

        for (i = 0; i < loopCount; i++)
        {
            buff[i + 1] = REG_READ(ESPI_PBMRXRDHEAD);
        }
    }

    switch (header.msgCode)
    {
    case ESPI_PC_BM_MSG_CODE_LTR:
        /*-------------------------------------------------------------------------------------------------*/
        /* Pass message to the user including message specific header                                      */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_PC_BM_userMsgHandler_L(header.msgCode, (UINT8*)buff, payloadDataLen + sizeof(header.addr.msgSpec));
        break;

    default:
        /*-------------------------------------------------------------------------------------------------*/
        /* Send host NON FATAL ERROR virtual wire indication                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_VW_SmNonFatalError_l();
        break;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_AutoModeTransDoneHandler_l                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  status      - ESPI status register value                                               */
/*                  intEnable   - ESPI interrupt enable register value                                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles automatic mode transfer done interrupt                            */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_PC_BM_AutoModeTransDoneHandler_l (
    UINT32 status,
    UINT32 intEnable
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear request information                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_PC_BM_usrReqInfo_L.size       = 0;
    ESPI_PC_BM_usrReqInfo_L.buffOffset = 0;
    ESPI_PC_BM_usrReqInfo_L.currSize   = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Automatic Mode Transfer Error                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------------------------------------*/
    /* BMBURSTERR: This bit is set to 1 when, either an unsuccessful completion or a bad Length            */
    /* value are received from the Master. When this bit is set to 1, the burst transaction                */
    /* (split or automatic) is aborted, the BMBURSTDONE bit is set to 1 (i.e. done) and both               */
    /* BMSPLITEN and BMAMTEN bits are set to 0.                                                            */
    /* Default = 0                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(status, ESPISTS_BMBURSTERR))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Get error and clean relevant bits                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_GetError_l(ESPI_PC_BM_ESPIERR_MASK);

        /*-------------------------------------------------------------------------------------------------*/
        /* Handle burst error                                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_PC_BM_HandleBurstErr_l(ESPI_PC_BM_usrReqInfo_L.useDMA);

        ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;

        if (READ_VAR_FIELD(intEnable, ESPIIE_BMBURSTERRIE))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Notify user                                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_PC_BM_BURST_MODE_TRANS_ERR));
            EXECUTE_FUNC(ESPI_PC_BM_userIntHandler_L,   (ESPI_INT_PC_BM_BURST_MODE_TRANS_ERR));
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* No error                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        ESPI_PC_BM_ExitAutoReadRequest_l(DEFS_STATUS_OK, ESPI_PC_BM_usrReqInfo_L.useDMA);

        ESPI_PC_BM_status_L = ESPI_PC_BM_usrReqInfo_L.status;

        if (READ_VAR_FIELD(intEnable, ESPIIE_BMBURSTDONEIE))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Notify user                                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_PC_BM_BURST_MODE_TRANS_DONE));
            EXECUTE_FUNC(ESPI_PC_BM_userIntHandler_L,   (ESPI_INT_PC_BM_BURST_MODE_TRANS_DONE));
        }
    }
}

#ifdef GDMA_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_GdmaIntHandler_l                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  module      - GDMA module number                                                       */
/*                  channel     - GDMA channel number                                                      */
/*                  status      - GDMA operation status                                                    */
/*                  transferLen - number of transferred bytes                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles GDMA interrupt handler.                                           */
/*lint -e{715}      Suppress 'status/transferLen' not referenced                                           */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_PC_BM_GdmaIntHandler_l (
    UINT8       module,
    UINT8       channel,
    DEFS_STATUS status,
    UINT32      transferLen
)
{
    //Do nothing, waiting on burst done interrupt
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_ExitAutoReadRequest_l                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Exit read request; set manual mode and return status                      */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_PC_BM_ExitAutoReadRequest_l (
    DEFS_STATUS status,
    BOOLEAN     useDMA
)
{
    if (READ_REG_FIELD(ESPI_ESPISTS,ESPISTS_BMTXDONE) == TRUE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Clear bus mastering transfer done status                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMTXDONE));
    }

    if (READ_REG_FIELD(ESPI_ESPISTS,ESPISTS_BMBURSTDONE) == TRUE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Clear automatic mode done status                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMBURSTDONE));
    }

    if (useDMA)
    {
        if (READ_REG_FIELD(ESPI_ESPISTS,ESPISTS_PBMRX) == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Clear bus mastering receive status                                                          */
            /*---------------------------------------------------------------------------------------------*/
            REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_PBMRX));
        }
        ESPI_PC_BM_RX_INTERRUPT_RESTORE(ESPI_PC_BM_RXIE_L);
        ESPI_PC_BM_TXDONE_INTERRUPT_RESTORE(ESPI_PC_BM_TXDONE_L);
    }

    ESPI_PC_AUTO_MODE(FALSE);

    ESPI_PC_BM_status_L = status;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_HandleBurstErr_l                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles automatic mode burst error                                        */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_PC_BM_HandleBurstErr_l (BOOLEAN useDMA)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear error bit                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMBURSTERR));

    /*-----------------------------------------------------------------------------------------------------*/
    /* If it's not protocol error/ unsuccessful completion error                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!READ_VAR_FIELD(ESPI_GetError(), ESPIERR_UNPBM) && !READ_VAR_FIELD(ESPI_GetError(), ESPIERR_PROTERR))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Send host NON FATAL ERROR virtual wire indication                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_VW_SmNonFatalError_l();
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset the pointers of the Transmit and Receive buffers                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_PERCTL, PERCTL_RSTPBUFHEADS, 0x01);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear bus mastering transfer done status                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMTXDONE));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear bus mastering receive status                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_PBMRX));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear bus mastering burst done status                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_BMBURSTDONE));

    if (useDMA)
    {
#ifdef GDMA_MODULE_TYPE
        /*-------------------------------------------------------------------------------------------------*/
        /* Disable DMA request                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_PERCTL, PERCTL_BMDMATHRESH, ESPI_BM_DMA_THRESHOLD_DISABLE);

        /*-------------------------------------------------------------------------------------------------*/
        /* Restore interrupts handling                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_PC_BM_RX_INTERRUPT_RESTORE(ESPI_PC_BM_RXIE_L);
        ESPI_PC_BM_TXDONE_INTERRUPT_RESTORE(ESPI_PC_BM_TXDONE_L);
#endif  /* GDMA_MODULE_TYPE */
    }

    ESPI_PC_BM_status_L = DEFS_STATUS_FAIL;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_PC_BM_EnqueuePacket_l                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index    -                                                                             */
/*                  type     -                                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ...                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_PC_BM_EnqueuePacket_l (ESPI_PC_MSG_T msgType)
{
    DEFS_STATUS_COND_CHECK((ESPI_IsChannelSlaveEnable_l(ESPI_CHANNEL_PC_BM) == ENABLE), DEFS_STATUS_HARDWARE_ERROR);

    switch (msgType)
    {
    case ESPI_PC_MSG_READ:
        SET_REG_FIELD(ESPI_PERCTL, PERCTL_BM_NP_AVAIL,  0x01);
        break;

    case ESPI_PC_MSG_WRITE:
        SET_REG_FIELD(ESPI_PERCTL, PERCTL_BM_PC_AVAIL,  0x01);
        break;

    case ESPI_PC_MSG_LTR:
        SET_REG_FIELD(ESPI_PERCTL, PERCTL_BM_MSG_AVAIL, 0x01);
        break;

    default:
        return DEFS_STATUS_FAIL;
    }

    return DEFS_STATUS_OK;
}

#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                    VW LOCAL FUNCTIONS IMPLEMENTATION                                    */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_GetType_l                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index    - VW index                                                                    */
/*                  type     - VW type                                                                     */
/*                                                                                                         */
/* Returns:         VW type                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the Virtual Wire type.                                          */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_VW_GetType_l (
    UINT8           index,
    ESPI_VW_TYPE_T* type
)
{
    UINT i;

    for (i = 0; i < ESPI_VW_TYPE_NUM; i++)
    {
        if ((index >= ESPI_VW_indexRange[i].start) && (index <= ESPI_VW_indexRange[i].end))
        {
            *type = (ESPI_VW_TYPE_T)i;
            return DEFS_STATUS_OK;
        }
    }

    return ESPI_VW_ERR_INDEX_NOT_FOUND;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_GetReg_l                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index    - VW index                                                                    */
/*                  dir      - VW direction (Slave-to-Master/Master-to-Slave)                              */
/*                  reg      - VW event register number                                                    */
/*                                                                                                         */
/* Returns:         VW event register number                                                               */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the Virtual Wire event register number.                         */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_VW_GetReg_l (
    UINT8           index,
    ESPI_VW_DIR_T   dir,
    UINT8*          reg
)
{
    UINT8   i;

    switch (dir)
    {
    case ESPI_VW_SM:
        for (i = 0; i < ESPI_VWEVSM_NUM; i++)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Find the event register index                                                               */
            /*---------------------------------------------------------------------------------------------*/
            if (READ_REG_FIELD(ESPI_VWEVSM(i), VWEVSM_INDEX) == index)
            {
                *reg = i;
                return DEFS_STATUS_OK;
            }
        }
        break;

    case ESPI_VW_MS:
        for (i = 0; i < ESPI_VWEVMS_NUM; i++)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Find the event register index                                                               */
            /*---------------------------------------------------------------------------------------------*/
            if (READ_REG_FIELD(ESPI_VWEVMS(i), VWEVMS_INDEX) == index)
            {
                *reg = i;
                return DEFS_STATUS_OK;
            }
        }
        break;

    default:
        break;
    }

    return ESPI_VW_ERR_INDEX_NOT_FOUND;
}

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_GetGpio_l                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  index    - GPIO index                                                                  */
/*                  dir      - GPIO direction (Slave-to-Master/Master-to-Slave)                            */
/*                  gpio     - GPIO register number                                                        */
/*                                                                                                         */
/* Returns:         GPIO register number                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine retrieves the GPIO register number.                                       */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_VW_GetGpio_l (
    UINT8           index,
    ESPI_VW_DIR_T   dir,
    UINT8*          gpio
)
{
    UINT8 gpioMap       = READ_REG_FIELD(ESPI_VWCTL, VWCTL_GPVWMAP);
    UINT8 gpioWinStart  = (UINT8)(ESPI_VW_indexRange[ESPI_VW_TYPE_GPIO].start  +
                                  (ESPI_VWGPSM_NUM * 2 * (UINT)gpioMap)        +
                                  (ESPI_VWGPSM_NUM     * (UINT)dir));
    UINT8 gpioWinEnd    = gpioWinStart + ESPI_VWGPSM_NUM - 1;

    DEFS_STATUS_COND_CHECK(((index >= gpioWinStart) && (index <= gpioWinEnd)), ESPI_VW_ERR_INDEX_NOT_FOUND);

    *gpio = index - gpioWinStart;

    return DEFS_STATUS_OK;
}
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_HandleInputEvent_l                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles input VW event                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_VW_HandleInputEvent_l (void)
{
    UINT32      var;
    UINT        i;
    UINT8       value;
    ESPI_VW     vw;
    UINT8       index;
    UINT8       wire;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for modification in Virtual Wire Event Master-to-Slave Register n                             */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < ESPI_VWEVMS_NUM; i++)
    {
        var = REG_READ(ESPI_VWEVMS(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Find modified groups                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(var, VWEVMS_MODIFIED) == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Clear modified bit                                                                          */
            /*---------------------------------------------------------------------------------------------*/
            SET_VAR_FIELD(var, VWEVMS_MODIFIED, 0x01);

            if (READ_VAR_FIELD(var, VWEVMS_IE) == TRUE)
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Retrieve the group index                                                                */
                /*-----------------------------------------------------------------------------------------*/
                index = READ_VAR_FIELD(var, VWEVMS_INDEX);

                /*-----------------------------------------------------------------------------------------*/
                /* Go over all wires                                                                       */
                /*-----------------------------------------------------------------------------------------*/
                for (wire = 0; wire < ESPI_VW_WIRE_NUM; wire++)
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* Verify wire's validity                                                              */
                    /*-------------------------------------------------------------------------------------*/
                    if (READ_VAR_FIELD(var, VWEVMS_WIRE_VALID(wire)) == TRUE)
                    {
                        vw = ESPI_VW_DEF(index, wire, ESPI_VW_MS);

                        /*---------------------------------------------------------------------------------*/
                        /* Read wire value                                                                 */
                        /*---------------------------------------------------------------------------------*/
                        value = (READ_VAR_FIELD(var, VWEVMS_WIRE(wire)));

                        /*---------------------------------------------------------------------------------*/
                        /* Invoke user callback                                                            */
                        /*---------------------------------------------------------------------------------*/
                        EXECUTE_FUNC(ESPI_VW_Handler_L, (vw, value));
                    }
                }
            }

            REG_WRITE(ESPI_VWEVMS(i), var);
        }
    }
}

#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_HandleInputGpio_l                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles input VW GPIO                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_VW_HandleInputGpio_l (void)
{
    UINT32      var;
    UINT        i;
    UINT8       value;
    ESPI_VW     vw;
    UINT8       index;
    UINT8       wire;
    UINT8       gpioMap = READ_REG_FIELD(ESPI_VWCTL, VWCTL_GPVWMAP);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for modification in Virtual Wire GPIO Master-to-Slave Register                                */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < ESPI_VWGPMS_NUM; i++)
    {
        var = REG_READ(ESPI_VWGPMS(i));

        /*-------------------------------------------------------------------------------------------------*/
        /* Find modified groups                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_VAR_FIELD(var, VWGPMS_MODIFIED) == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Clear modified bit                                                                          */
            /*---------------------------------------------------------------------------------------------*/
            SET_VAR_FIELD(var, VWGPMS_MODIFIED, 0x01);

            if (READ_VAR_FIELD(var, VWGPMS_IE) == TRUE)
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Retrieve the group index                                                                */
                /*-----------------------------------------------------------------------------------------*/
                index = (UINT8)(ESPI_VW_indexRange[ESPI_VW_TYPE_GPIO].start +
                                (ESPI_VWGPSM_NUM * 2 * (UINT)gpioMap)       +
                                (ESPI_VWGPSM_NUM     * (UINT)ESPI_VW_MS)    +
                                i);

                /*-----------------------------------------------------------------------------------------*/
                /* Go over all wires                                                                       */
                /*-----------------------------------------------------------------------------------------*/
                for (wire = 0; wire < ESPI_VW_WIRE_NUM; wire++)
                {
                    vw = ESPI_VW_DEF(index, wire, ESPI_VW_MS);

                    /*-------------------------------------------------------------------------------------*/
                    /* Read wire value                                                                     */
                    /*-------------------------------------------------------------------------------------*/
                    value = (READ_VAR_FIELD(var, VWGPMS_WIRE(wire)));

                    /*-------------------------------------------------------------------------------------*/
                    /* Invoke user callback                                                                */
                    /*-------------------------------------------------------------------------------------*/
                    EXECUTE_FUNC(ESPI_VW_Handler_L, (vw, value));
                }
            }

            REG_WRITE(ESPI_VWGPMS(i), var);
        }
    }
}
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT

#if defined (ESPI_CAPABILITY_FLASH_NON_FATAL_ERR_IND) || defined (ESPI_CAPABILITY_PC_NON_FATAL_ERR_IND) || defined (ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_VW_SmNonFatalError_l                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends the host non fatal error indication                                 */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_VW_SmNonFatalError_l (void)
{
#ifndef ESPI_CAPABILITY_VW_EDGE
    BOOLEAN wireIsRead = FALSE;
    volatile UINT32  counter    = ESPI_VW_NON_FATAL_ERR_TIMEOUT;
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set non fatal error indication                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)ESPI_VW_SetWire(ESPI_VW_ERROR_NONFATAL, 0x01);

#ifndef ESPI_CAPABILITY_VW_EDGE
    /*-----------------------------------------------------------------------------------------------------*/
    /* Wait for the host to read the indication                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    while ((wireIsRead == FALSE) && (counter > 0))
    {
        (void)ESPI_VW_WireIsRead(ESPI_VW_ERROR_NONFATAL, &wireIsRead);
        counter--;
    }
#endif
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear non fatal error indication                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)ESPI_VW_SetWire(ESPI_VW_ERROR_NONFATAL, 0x00);

}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                  FLASH LOCAL FUNCTIONS IMPLEMENTATION                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_Init_l                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialize flash channel global variables                                 */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_FLASH_Init_l (void)
{
    ESPI_FLASH_currReqInfo_L.buffOffset = 0;
    ESPI_FLASH_currReqInfo_L.size       = 0;
    ESPI_FLASH_currReqInfo_L.currSize   = 0;
    ESPI_FLASH_currReqTag_L             = 0;
    ESPI_FLASH_status_L                 = DEFS_STATUS_OK;
    ESPI_FLASH_WritePageSize_L          = ESPI_FLASH_PROGRAM_PAGE_DEFAULT_SIZE;
#ifdef ESPI_CAPABILITY_SAF
    ESPI_FLASH_SafPendingIncomingReq_L  = FALSE;
    ESPI_FLASH_SafPendingIncomingRes_L  = FALSE;
    ESPI_FLASH_SafReqInfo_L.reqType     = ESPI_FLASH_SAF_REQ_NUM;
    ESPI_FLASH_SafReqInfo_L.status      = DEFS_STATUS_OK;
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_ReqManual_l                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType    - request type (read/write/erase)                                           */
/*                  offset     - offset in flash                                                           */
/*                  size       - size of transaction                                                       */
/*                  polling    - equals TRUE if polling is required and FALSE for interrupt mode           */
/*                  buffer     - Input/output buffer, depends on reqType                                   */
/*                  strpHdr    - equals TRUE if strip header is required and FALSE otherwise               */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends manual request to flash                                             */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS  ESPI_FLASH_ReqManual_l (
    ESPI_FLASH_REQ_ACC_T    reqType,
    UINT32                  offset,
    UINT32                  size,
    BOOLEAN                 polling,
    UINT32*                 buffer,
    BOOLEAN                 strpHdr
)
{
    UINT32                  currSize    = 0;
    UINT32                  buffOffset  = 0;
    DEFS_STATUS             status;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get current request size                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_FLASH_GetCurrReqSize_l(reqType, (offset + buffOffset), (size - buffOffset), &currSize));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set manual mode                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_AUTO_MODE(FALSE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Verify transmit buffer is empty                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK(READ_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_FLASH_ACC_TX_AVAIL) == FALSE,
                           DEFS_STATUS_SYSTEM_BUSY);

    if (!polling)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Save request info                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_FLASH_currReqInfo_L.buffer         = buffer;
        ESPI_FLASH_currReqInfo_L.currSize       = currSize;
        ESPI_FLASH_currReqInfo_L.reqType        = reqType;
        ESPI_FLASH_currReqInfo_L.size           = size;
        ESPI_FLASH_currReqInfo_L.strpHdr        = strpHdr;
        ESPI_FLASH_currReqInfo_L.buffOffset     = 0;
        ESPI_FLASH_currReqInfo_L.manualMode     = TRUE;
        ESPI_FLASH_currReqInfo_L.flashOffset    = offset;
        ESPI_FLASH_currReqInfo_L.useDMA         = FALSE;
        ESPI_FLASH_currReqInfo_L.status         = DEFS_STATUS_OK;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set header/non header in receive buffer                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_STRPHDR, strpHdr);

#ifdef ESPI_CAPABILITY_FLASH_AUTO_MODE_TAG_CHANGE
    /*-----------------------------------------------------------------------------------------------------*/
    /* Update tag                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_currReqTag_L = ESPI_FLASH_GET_NEXT_TAG(ESPI_FLASH_currReqTag_L);
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set request                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_FLASH_SetRequest_l(reqType, offset, currSize, buffer, ESPI_FLASH_currReqTag_L));

    ESPI_FLASH_status_L = DEFS_STATUS_SYSTEM_BUSY;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enqueue the packet for transmission                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    status = ESPI_FLASH_EnqueuePacket_l();
    if (status != DEFS_STATUS_OK)
    {
        ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
        return status;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Polling mode                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    if (polling)
    {
        status = ESPI_FLASH_ReceiveDataBuffer_l(reqType, buffer, strpHdr, currSize);
        if (status != DEFS_STATUS_OK)
        {
            ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
            return status;
        }
        buffOffset += currSize;

        while (buffOffset < size)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Get current request size                                                                    */
            /*---------------------------------------------------------------------------------------------*/
            status = (ESPI_FLASH_GetCurrReqSize_l(reqType, (offset + buffOffset),
                                                              (size - buffOffset), &currSize));
            if (status != DEFS_STATUS_OK)
            {
                ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
                return status;
            }

#ifdef ESPI_CAPABILITY_FLASH_AUTO_MODE_TAG_CHANGE
            /*---------------------------------------------------------------------------------------------*/
            /* Update tag                                                                                  */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_FLASH_currReqTag_L = ESPI_FLASH_GET_NEXT_TAG(ESPI_FLASH_currReqTag_L);

#endif
            /*---------------------------------------------------------------------------------------------*/
            /* Set next request and send                                                                   */
            /*---------------------------------------------------------------------------------------------*/
            status = (ESPI_FLASH_SetRequest_l(reqType, (offset + buffOffset),
                                              currSize,
                                              (UINT32*)(void*)&(((UINT8*)buffer)[buffOffset]),
                                              ESPI_FLASH_currReqTag_L));
            if (status != DEFS_STATUS_OK)
            {
                ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
                return status;
            }

            /*---------------------------------------------------------------------------------------------*/
            /* Enqueue the packet for transmission                                                         */
            /*---------------------------------------------------------------------------------------------*/
            status = ESPI_FLASH_EnqueuePacket_l();
            if (status != DEFS_STATUS_OK)
            {
                ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
                return status;
            }

            status = ESPI_FLASH_ReceiveDataBuffer_l(reqType,
                                                    (UINT32*)(void*)&(((UINT8*)buffer)[buffOffset]),
                                                    strpHdr,
                                                    currSize);
            if (status != DEFS_STATUS_OK)
            {
                ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
                return status;
            }
            buffOffset += currSize;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* If no error occurred                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_FLASH_status_L == DEFS_STATUS_SYSTEM_BUSY)
        {
            ESPI_FLASH_status_L = DEFS_STATUS_OK;
        }
        return ESPI_FLASH_status_L;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SetRequest_l                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType    - request type                                                              */
/*                  offset     - offset in flash                                                           */
/*                  size       - size of transaction                                                       */
/*                  buffer     - input buffer (used for write transactions)                                */
/*                  tag        - request tag                                                               */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set request on transmit buffer                                            */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_SetRequest_l (
    ESPI_FLASH_REQ_ACC_T    reqType,
    UINT32                  offset,
    UINT32                  size,
    const UINT32*           buffer,
    UINT8                   tag
)
{
    ESPI_FLASH_TRANS_HDR    trasHdr = {0};
    UINT32                  loopCount;
    UINT32                  address;
    UINT32                  i;
    UINT32                  dataRemainder;
    UINT32                  temp;
    UINT8*                  tempPtr = (UINT8*)&temp;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set header                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    trasHdr.pktLen = sizeof(trasHdr) + sizeof(address) - sizeof(trasHdr.pktLen);
    if (reqType == ESPI_FLASH_REQ_ACC_WRITE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Error check                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((size <= ESPI_FLASH_TRANS_DATA_PAYLOAD_MAX_SIZE), DEFS_STATUS_INVALID_PARAMETER);

        trasHdr.pktLen += (UINT8)size;
    }
    trasHdr.type = (UINT8)reqType;
    address = LE32(offset);
    SET_VAR_FIELD(trasHdr.tagPlusLength, HEADER_LENGTH, size);
    SET_VAR_FIELD(trasHdr.tagPlusLength, HEADER_TAG, tag);
    trasHdr.tagPlusLength = LE16(trasHdr.tagPlusLength);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write header to transfer buffer                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_FLASHTXWRHEAD, (*((UINT32*)(void*)(&trasHdr))));
    REG_WRITE(ESPI_FLASHTXWRHEAD, address);

    if (reqType == ESPI_FLASH_REQ_ACC_WRITE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Write payload data to transfer buffer                                                           */
        /*-------------------------------------------------------------------------------------------------*/
        loopCount = size / sizeof(UINT32);
        dataRemainder = size % sizeof(UINT32);
        for (i = 0; i < loopCount; i++)
        {
            REG_WRITE(ESPI_FLASHTXWRHEAD, buffer[i]);
        }
        if (dataRemainder > 0)
        {
            temp = 0;
            for (i = 0; i < dataRemainder; i++)
            {
                tempPtr[i] = ((UINT8*)buffer)[(loopCount * sizeof(UINT32)) + i];
            }
            REG_WRITE(ESPI_FLASHTXWRHEAD, temp);
        }
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_ReadReqAuto_l                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  offset     - offset in flash                                                           */
/*                  size       - size of transaction                                                       */
/*                  polling    - equals TRUE if polling is required and FALSE for interrupt mode           */
/*                  buffer     - Input/output buffer, depends on reqType                                   */
/*                  strpHdr    - equals TRUE if strip header is required and FALSE otherwise               */
/*                  useDMA     - equals TRUE if DMA is required and FALSE otherwise                        */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends a read automatic request to host.                                   */
/*                  Flash offset must be aligned to size of transaction                                    */
/*                  Size must be aligned to 64B.                                                           */
/*                  Maximum size for transaction is (64B*256)                                              */
/*                  If using DMA mode the buffer must be at least 4B aligned , 16B aligned will improve    */
/*                  performance.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_ReadReqAuto_l (
    UINT32  offset,
    UINT32  size,
    BOOLEAN polling,
    UINT32* buffer,
    BOOLEAN strpHdr,
    BOOLEAN useDMA
)
{
    UINT32      j;
    UINT16      numOfReadTrans;
    DEFS_STATUS status;
    UINT32      intEnableReg;
#ifndef ESPI_CAPABILITY_FLASH_AUTO_READ_OFFSET_ALIGN
    UINT32      sizeCeilPow2;
#endif
#ifndef GDMA_MODULE_TYPE
    DEFS_STATUS_COND_CHECK((useDMA == FALSE), DEFS_STATUS_INVALID_PARAMETER);
#endif
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error check                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_FLASH_AUTO_READ_OFFSET_ALIGN
    DEFS_STATUS_COND_CHECK(((offset % ESPI_FLASH_MAX_READ_REQ_SIZE) == 0), DEFS_STATUS_INVALID_PARAMETER);
#else
    /*-----------------------------------------------------------------------------------------------------*/
    /* Address should be aligned to the length rounded up to the closest power of 2                        */
    /*-----------------------------------------------------------------------------------------------------*/
    ROUND_UP_POWER2(size, sizeCeilPow2);
    DEFS_STATUS_COND_CHECK(((offset % sizeCeilPow2) == 0), DEFS_STATUS_INVALID_PARAMETER);
#endif
    DEFS_STATUS_COND_CHECK(((size % ESPI_FLASH_MAX_READ_REQ_SIZE) == 0), DEFS_STATUS_INVALID_DATA_SIZE);
    numOfReadTrans = (UINT16)(size / ESPI_FLASH_MAX_READ_REQ_SIZE);
    DEFS_STATUS_COND_CHECK((numOfReadTrans <= ESPI_FLASH_AUTO_MODE_MAX_TRANS_SIZE), DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

#ifndef ESPI_CAPABILITY_FLASH_MODE_CHANGE
    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset the pointers of the Transmit and Receive buffers                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_RSTBUFHEADS, 0x01);
#endif

    if (!polling)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Save request info                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_FLASH_currReqInfo_L.buffer         = buffer;
        ESPI_FLASH_currReqInfo_L.reqType        = ESPI_FLASH_REQ_ACC_READ;
        ESPI_FLASH_currReqInfo_L.size           = size;
        ESPI_FLASH_currReqInfo_L.useDMA         = useDMA;
        ESPI_FLASH_currReqInfo_L.strpHdr        = useDMA ? TRUE : strpHdr;
        ESPI_FLASH_currReqInfo_L.currSize       = ESPI_FLASH_MAX_READ_REQ_SIZE;
        ESPI_FLASH_currReqInfo_L.buffOffset     = 0;
        ESPI_FLASH_currReqInfo_L.flashOffset    = offset;
        ESPI_FLASH_currReqInfo_L.manualMode     = FALSE;
        ESPI_FLASH_currReqInfo_L.status         = DEFS_STATUS_OK;
    }

    if (useDMA)
    {
#ifdef GDMA_MODULE_TYPE
        /*-------------------------------------------------------------------------------------------------*/
        /* Strip header                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_STRPHDR, 0x01);

        /*-------------------------------------------------------------------------------------------------*/
        /* Initialize GDMA module                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(GDMA_InitEspi(ESPI_GDMA_MODULE, ESPI_FLASH_GDMA_CHANNEL));

        /*-------------------------------------------------------------------------------------------------*/
        /* Find relevant transfer width according to input buffer                                          */
        /*-------------------------------------------------------------------------------------------------*/
        if ((UINT32)buffer % _16B_ == 0)
        {
            DEFS_STATUS_RET_CHECK(GDMA_Config(ESPI_GDMA_MODULE, ESPI_FLASH_GDMA_CHANNEL, FALSE, FALSE, GDMA_TRANSFER_WIDTH_16B));
            /*---------------------------------------------------------------------------------------------*/
            /* Define threshold value of the flash receive buffer above which a DMA request is asserted    */
            /*---------------------------------------------------------------------------------------------*/
            SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_DMATHRESH, ESPI_FLASH_DMA_THRESHOLD_16B);
        }
        else if ((UINT32)buffer % _4B_ == 0)
        {
            DEFS_STATUS_RET_CHECK(GDMA_Config(ESPI_GDMA_MODULE, ESPI_FLASH_GDMA_CHANNEL, FALSE, FALSE, GDMA_TRANSFER_WIDTH_4B));
            /*---------------------------------------------------------------------------------------------*/
            /* Define threshold value of the flash receive buffer above which a DMA request is asserted    */
            /*---------------------------------------------------------------------------------------------*/
            SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_DMATHRESH, ESPI_FLASH_DMA_THRESHOLD_4B);
        }
        else
        {
            return DEFS_STATUS_FAIL;
        }
#endif  /* GDMA_MODULE_TYPE */
    }
    else //No DMA
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* DMA request is disabled                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_DMATHRESH, ESPI_FLASH_DMA_THRESHOLD_DISABLE);

        /*-------------------------------------------------------------------------------------------------*/
        /* Set header/non header in receive buffer                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_STRPHDR, strpHdr);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set auto mode on                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_AUTO_MODE(TRUE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set transfer size                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_AMTSIZE, (numOfReadTrans - 1));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check transfer buffer is empty                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((READ_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_FLASH_ACC_TX_AVAIL) == FALSE), DEFS_STATUS_SYSTEM_BUSY);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set header                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_currReqTag_L = 0;
    DEFS_STATUS_RET_CHECK(ESPI_FLASH_SetRequest_l(ESPI_FLASH_REQ_ACC_READ, offset, ESPI_FLASH_MAX_READ_REQ_SIZE,
                                                  buffer, ESPI_FLASH_currReqTag_L));

    if (useDMA)
    {
#ifdef GDMA_MODULE_TYPE
        ESPI_FLASH_RX_INTERRUPT_SAVE_DISABLE(ESPI_FLASH_RXIE_L);
        status = (GDMA_Transfer(ESPI_GDMA_MODULE, ESPI_FLASH_GDMA_CHANNEL, REG_ADDR(ESPI_FLASHRXRDHEAD), (UINT8*)buffer, size, ESPI_FLASH_GdmaIntHandler_l));
        if (status != DEFS_STATUS_OK)
        {
            ESPI_FLASH_ExitAutoReadRequest_l(status, useDMA);
            return ESPI_FLASH_status_L;
        }
#endif  /* GDMA_MODULE_TYPE */
    }

    ESPI_FLASH_status_L = DEFS_STATUS_SYSTEM_BUSY;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enqueue the packet for transmission                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    status = ESPI_FLASH_EnqueuePacket_l();
    if (status != DEFS_STATUS_OK)
    {
        ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
        return status;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Polling mode                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    if (polling)
    {
        if (!useDMA)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* while not all packets arrived                                                               */
            /*---------------------------------------------------------------------------------------------*/
            j = 0;
            while (READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_AMDONE) == FALSE)
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Wait for receive buffer indication and handle packet                                    */
                /*-----------------------------------------------------------------------------------------*/
                status = ESPI_FLASH_ReceiveDataBuffer_l(ESPI_FLASH_REQ_ACC_READ,
                                                        &buffer[(j * ESPI_FLASH_MAX_READ_REQ_SIZE) / sizeof(UINT32)],
                                                        strpHdr, ESPI_FLASH_MAX_READ_REQ_SIZE);
                j++;
                if (status != DEFS_STATUS_OK)
                {
                    break;
                }

                /*-----------------------------------------------------------------------------------------*/
                /* While more packets awaits in buffer                                                     */
                /*-----------------------------------------------------------------------------------------*/
                while ((j < numOfReadTrans) && (READ_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_AMTBFULL) == TRUE))
                {
                    /*-------------------------------------------------------------------------------------*/
                    /* Clear receive buffer status                                                         */
                    /*-------------------------------------------------------------------------------------*/
                    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_FLASHRX));

                    /*-------------------------------------------------------------------------------------*/
                    /* Handle packet                                                                       */
                    /*-------------------------------------------------------------------------------------*/
                    status = (ESPI_FLASH_HandleInputPacket_l(ESPI_FLASH_REQ_ACC_READ,
                                                             &buffer[(j * ESPI_FLASH_MAX_READ_REQ_SIZE) / sizeof(UINT32)],
                                                             strpHdr, ESPI_FLASH_MAX_READ_REQ_SIZE));
                    if (status != DEFS_STATUS_OK)
                    {
                        break;
                    }
                    j++;
                }

            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Wait till request transfer is done                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        j = ESPI_FLASH_AUTO_AMDONE_LOOP_COUNT;
        status = DEFS_STATUS_OK;
        do
        {
            if (j-- == 0)
            {
                status = DEFS_STATUS_RESPONSE_TIMEOUT;
                break;
            }
        } while (READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_AMDONE) == FALSE);

        /*-------------------------------------------------------------------------------------------------*/
        /* If ended with error                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        if (READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_AMERR) == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Disable interrupts such that ESPI_errorMask will not be changed                             */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_INTERRUPT_SAVE_DISABLE(intEnableReg);

            /*---------------------------------------------------------------------------------------------*/
            /* Get error and clean relevant bits                                                           */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_GetError_l(ESPI_FLASH_ESPIERR_MASK);

            /*---------------------------------------------------------------------------------------------*/
            /* Handle automatic mode error                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_FLASH_HandleAutoModeErr_l(useDMA);

            /*---------------------------------------------------------------------------------------------*/
            /* Restore interrupts                                                                          */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_INTERRUPT_RESTORE(intEnableReg);
        }
        else
        {
            ESPI_FLASH_ExitAutoReadRequest_l(DEFS_STATUS_OK, useDMA);
        }

        return ESPI_FLASH_status_L;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_EnqueuePacket_l                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enqueues a Master-Attached flash request packet for transmission          */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_EnqueuePacket_l (void)
{
    DEFS_STATUS_COND_CHECK((ESPI_IsChannelSlaveEnable_l(ESPI_CHANNEL_FLASH) == ENABLE), DEFS_STATUS_HARDWARE_ERROR);

    SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_FLASH_ACC_TX_AVAIL, 0x01);

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_HandleInputPacket_l                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  req         -  request type                                                            */
/*                  buffer      -  output buffer used for read operation                                   */
/*                  strpHdr     -  equals TRUE if strip header mode is on and FALSE otherwise              */
/*                  size        -  size of transaction (requested size)                                    */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine read and process input packet from receive buffer                         */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_HandleInputPacket_l (
    ESPI_FLASH_REQ_ACC_T    req,
    UINT32*                 buffer,
    BOOLEAN                 strpHdr,
    UINT32                  size
)
{
    UINT                    i;
    UINT32                  loopCount;
    UINT32                  payloadDataLen;
    ESPI_FLASH_TRANS_HDR    header;
    UINT32                  dataRemainder;
    UINT32                  temp;
    UINT8*                  tempPtr = (UINT8*)&temp;
    UINT32                  alignedSize;
    UINT16                  tagPlusLength;

    /*-----------------------------------------------------------------------------------------------------*/
    /* No header in receive buffer                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    if (strpHdr)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* If the request was a read request                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (req == ESPI_FLASH_REQ_ACC_READ)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Read data from receive buffer                                                               */
            /*---------------------------------------------------------------------------------------------*/
            loopCount = size / sizeof(UINT32);
            dataRemainder = size - (loopCount * sizeof(UINT32));
            for (i = 0; i < loopCount; i++)
            {
                buffer[i] = REG_READ(ESPI_FLASHRXRDHEAD);
            }
            if (dataRemainder > 0)
            {
                alignedSize = (loopCount * sizeof(UINT32));
                temp = REG_READ(ESPI_FLASHRXRDHEAD);
                for (i = 0; i < dataRemainder; i++)
                {
                    ((UINT8*)buffer)[i + alignedSize] = tempPtr[i];
                }
            }

        }
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Read header from receive buffer                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        *((UINT32*)(void*)&header) = REG_READ(ESPI_FLASHRXRDHEAD);
        tagPlusLength              = LE16(header.tagPlusLength);

        /*-------------------------------------------------------------------------------------------------*/
        /* Check header fields                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((header.type != ESPI_UNSUCCESSFUL_COMPLETION_WO_DATA), DEFS_STATUS_RESPONSE_CANT_BE_PROVIDED);

        /*-------------------------------------------------------------------------------------------------*/
        /* No need to check the TAG, it is checked by the hardware and AMERR bit is set to 1, if the TAG   */
        /* is incorrect.                                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
#ifndef ESPI_CAPABILITY_FLASH_AUTO_MODE_TAG_CHANGE
        DEFS_STATUS_COND_CHECK(READ_VAR_FIELD(tagPlusLength, HEADER_TAG) == ESPI_FLASH_currReqTag_L, DEFS_STATUS_INVALID_DATA_FIELD);
        ESPI_FLASH_currReqTag_L = ESPI_FLASH_GET_NEXT_TAG(ESPI_FLASH_currReqTag_L);
#endif
        payloadDataLen = READ_VAR_FIELD(tagPlusLength, HEADER_LENGTH);

        switch (req)
        {
        case ESPI_FLASH_REQ_ACC_READ:
            DEFS_STATUS_COND_CHECK((header.type == ESPI_SUCCESSFUL_COMPLETION_WITH_DATA), DEFS_STATUS_INVALID_DATA_FIELD);
            DEFS_STATUS_COND_CHECK((payloadDataLen == size), DEFS_STATUS_INVALID_DATA_FIELD);

            /*---------------------------------------------------------------------------------------------*/
            /* Copy data from receive buffer                                                               */
            /*---------------------------------------------------------------------------------------------*/
            loopCount = payloadDataLen / sizeof(UINT32);
            dataRemainder = size - (loopCount * sizeof(UINT32));
            for (i = 0; i < loopCount; i++)
            {
                buffer[i] = REG_READ(ESPI_FLASHRXRDHEAD);
            }
            if (dataRemainder > 0)
            {
                alignedSize = (loopCount * sizeof(UINT32));
                temp = REG_READ(ESPI_FLASHRXRDHEAD);
                for (i = 0; i < dataRemainder; i++)
                {
                    ((UINT8*)buffer)[i + alignedSize] = tempPtr[i];
                }
            }
            break;

        case ESPI_FLASH_REQ_ACC_ERASE:
        case ESPI_FLASH_REQ_ACC_WRITE:
            DEFS_STATUS_COND_CHECK((header.type == ESPI_SUCCESSFUL_COMPLETION_WO_DATA), DEFS_STATUS_INVALID_DATA_FIELD);
            DEFS_STATUS_COND_CHECK((payloadDataLen == 0), DEFS_STATUS_INVALID_DATA_FIELD);
            break;

        default:
            break;
        }
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_HandleReqRes_l                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles request response under interrupt mode                             */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_FLASH_HandleReqRes_l (void)
{
    DEFS_STATUS status = DEFS_STATUS_OK;

    /*-----------------------------------------------------------------------------------------------------*/
    /* If its DMA mode, ignore                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    if (ESPI_FLASH_currReqInfo_L.useDMA == TRUE)
    {
        return;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* If the user expects data                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    if (ESPI_FLASH_currReqInfo_L.buffOffset < ESPI_FLASH_currReqInfo_L.size)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* If packet handling failed                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_FLASH_HandleInputPacket_l(ESPI_FLASH_currReqInfo_L.reqType,
                                           (UINT32*)(void*)&((UINT8*)ESPI_FLASH_currReqInfo_L.buffer)[ESPI_FLASH_currReqInfo_L.buffOffset],
                                           ESPI_FLASH_currReqInfo_L.strpHdr,
                                           ESPI_FLASH_currReqInfo_L.currSize)
            != DEFS_STATUS_OK)
        {
            ESPI_FLASH_currReqInfo_L.status = DEFS_STATUS_FAIL;
        }
        ESPI_FLASH_currReqInfo_L.buffOffset += ESPI_FLASH_currReqInfo_L.currSize;

        /*-------------------------------------------------------------------------------------------------*/
        /* If it's automatic mode (read operation only)                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if ((ESPI_FLASH_currReqInfo_L.manualMode == FALSE) &&
            (ESPI_FLASH_currReqInfo_L.reqType == ESPI_FLASH_REQ_ACC_READ))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* While there is more data in buffers                                                         */
            /*---------------------------------------------------------------------------------------------*/
            while ((ESPI_FLASH_currReqInfo_L.buffOffset < ESPI_FLASH_currReqInfo_L.size) &&
                   (READ_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_AMTBFULL) == TRUE))
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Clear receive buffer status                                                             */
                /*-----------------------------------------------------------------------------------------*/
                REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_FLASHRX));

                /*-----------------------------------------------------------------------------------------*/
                /* Handle packet                                                                           */
                /*-----------------------------------------------------------------------------------------*/
                status = ESPI_FLASH_HandleInputPacket_l(ESPI_FLASH_currReqInfo_L.reqType,
                                                        (UINT32*)(void*)&((UINT8*)ESPI_FLASH_currReqInfo_L.buffer)[ESPI_FLASH_currReqInfo_L.buffOffset],
                                                        ESPI_FLASH_currReqInfo_L.strpHdr,
                                                        ESPI_FLASH_currReqInfo_L.currSize);
                /*-----------------------------------------------------------------------------------------*/
                /* If packet handling failed                                                               */
                /*-----------------------------------------------------------------------------------------*/
                if (status != DEFS_STATUS_OK)
                {
                    ESPI_FLASH_currReqInfo_L.status = DEFS_STATUS_FAIL;
                }
                ESPI_FLASH_currReqInfo_L.buffOffset += ESPI_FLASH_currReqInfo_L.currSize;
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* If this is manual mode                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_FLASH_currReqInfo_L.manualMode == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* If operation is not done                                                                    */
            /*---------------------------------------------------------------------------------------------*/
            if (ESPI_FLASH_currReqInfo_L.buffOffset < ESPI_FLASH_currReqInfo_L.size)
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Get current request size                                                                */
                /*-----------------------------------------------------------------------------------------*/
                if (ESPI_FLASH_GetCurrReqSize_l(ESPI_FLASH_currReqInfo_L.reqType,
                                                (ESPI_FLASH_currReqInfo_L.flashOffset + ESPI_FLASH_currReqInfo_L.buffOffset),
                                                (ESPI_FLASH_currReqInfo_L.size - ESPI_FLASH_currReqInfo_L.buffOffset),
                                                &ESPI_FLASH_currReqInfo_L.currSize) != DEFS_STATUS_OK)
                {
                    ESPI_FLASH_currReqInfo_L.status = DEFS_STATUS_FAIL;
                }

#ifdef ESPI_CAPABILITY_FLASH_AUTO_MODE_TAG_CHANGE
                /*-----------------------------------------------------------------------------------------*/
                /* Update tag                                                                              */
                /*-----------------------------------------------------------------------------------------*/
                ESPI_FLASH_currReqTag_L = ESPI_FLASH_GET_NEXT_TAG(ESPI_FLASH_currReqTag_L);
#endif

                /*-----------------------------------------------------------------------------------------*/
                /* Set next request                                                                        */
                /*-----------------------------------------------------------------------------------------*/
                (void)(ESPI_FLASH_SetRequest_l(ESPI_FLASH_currReqInfo_L.reqType,
                                               (ESPI_FLASH_currReqInfo_L.flashOffset + ESPI_FLASH_currReqInfo_L.buffOffset),
                                               ESPI_FLASH_currReqInfo_L.currSize,
                                               (UINT32*)(void*)&((UINT8*)ESPI_FLASH_currReqInfo_L.buffer)[ESPI_FLASH_currReqInfo_L.buffOffset],
                                               ESPI_FLASH_currReqTag_L));

                /*-----------------------------------------------------------------------------------------*/
                /* Enqueue the packet for transmission                                                     */
                /*-----------------------------------------------------------------------------------------*/
                if (ESPI_FLASH_EnqueuePacket_l() != DEFS_STATUS_OK)
                {
                    ESPI_FLASH_currReqInfo_L.status = DEFS_STATUS_FAIL;
                }
            }
            else
            {
                ESPI_FLASH_status_L = ESPI_FLASH_currReqInfo_L.status;
                /*-----------------------------------------------------------------------------------------*/
                /* operation is done, notify user                                                          */
                /*-----------------------------------------------------------------------------------------*/
                EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_FLASH_DATA_RCV));
                EXECUTE_FUNC(ESPI_FLASH_userIntHandler_L,   (ESPI_INT_FLASH_DATA_RCV));
            }
        }
    }
#ifdef ESPI_CAPABILITY_FLASH_NON_FATAL_ERR_IND
    /*-----------------------------------------------------------------------------------------------------*/
    /* User does not expects data                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Only in manual mode                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        if (ESPI_FLASH_IS_AUTO_MODE_ON == FALSE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Send host NON FATAL ERROR virtual wire indication                                           */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_VW_SmNonFatalError_l();
        }
    }
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_ReceiveDataBuffer_l                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType    - request type                                                              */
/*                  buffer     - output buffer for read operation                                          */
/*                  strpHdr    - equals TRUE if strip header mode is on and FALSE otherwise                */
/*                  size       - size of transaction (requested size)                                      */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine receives and handles data buffer under polling mode                       */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_ReceiveDataBuffer_l (
    ESPI_FLASH_REQ_ACC_T    reqType,
    UINT32*                 buffer,
    BOOLEAN                 strpHdr,
    UINT32                  size
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Wait till receive buffer is full                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    BUSY_WAIT_TIMEOUT((READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_FLASHRX) == FALSE), ESPI_FLASH_RX_TIMEOUT) ;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear status                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_FLASHRX));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Handle packet                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_FLASH_HandleInputPacket_l(reqType, buffer, strpHdr, size));

    return DEFS_STATUS_OK;
}

#ifdef GDMA_MODULE_TYPE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_GdmaIntHandler_l                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  module      - GDMA module number                                                       */
/*                  channel     - GDMA channel number                                                      */
/*                  status      - GDMA operation status                                                    */
/*                  transferLen - number of transferred bytes                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles GDMA interrupt handler.                                           */
/*lint -e{715}      Suppress 'module/channel/status/transferLen' not referenced                            */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_FLASH_GdmaIntHandler_l (
    UINT8       module,
    UINT8       channel,
    DEFS_STATUS status,
    UINT32      transferLen
)
{
    //Do nothing, waiting on AMDONE
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_AutoModeTransDoneHandler_l                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  module      - GDMA module number                                                       */
/*                  channel     - GDMA channel number                                                      */
/*                  status      - ESPI status register value                                               */
/*                  intEnable   - ESPI interrupt enable register value                                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles automatic mode transfer done interrupt                            */
/*lint -e{715}      Suppress 'module/channel' not referenced                                               */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_FLASH_AutoModeTransDoneHandler_l (
    UINT8  module,
    UINT8  channel,
    UINT32 status,
    UINT32 intEnable
)
{
#ifdef GDMA_MODULE_TYPE
    /*-----------------------------------------------------------------------------------------------------*/
    /* If its DMA mode                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if (ESPI_FLASH_currReqInfo_L.useDMA == TRUE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Clear flash receive status                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_FLASHRX));

        /*-------------------------------------------------------------------------------------------------*/
        /* Restore RX interrupt enable status                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_FLASH_RX_INTERRUPT_RESTORE(ESPI_FLASH_RXIE_L);

#ifndef ESPI_CAPABILITY_FLASH_MODE_CHANGE
        /*-------------------------------------------------------------------------------------------------*/
        /* Disable DMA                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_DMATHRESH, ESPI_FLASH_DMA_THRESHOLD_DISABLE);
#endif
    }
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear request information                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_currReqInfo_L.size       = 0;
    ESPI_FLASH_currReqInfo_L.buffOffset = 0;
    ESPI_FLASH_currReqInfo_L.currSize   = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Automatic Mode Transfer Error                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(status, ESPISTS_AMERR))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Get error and clean relevant bits                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_GetError_l(ESPI_FLASH_ESPIERR_MASK);

        ESPI_FLASH_HandleAutoModeErr_l(ESPI_FLASH_currReqInfo_L.useDMA);

        ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
        if (READ_VAR_FIELD(intEnable, ESPIIE_AMERRIE))
        {
            EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_FLASH_AUTO_MODE_TRANS_ERR));
            EXECUTE_FUNC(ESPI_FLASH_userIntHandler_L,   (ESPI_INT_FLASH_AUTO_MODE_TRANS_ERR));
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* No error                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Turn off automatic mode                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_FLASH_AUTO_MODE(FALSE);

        ESPI_FLASH_status_L = ESPI_FLASH_currReqInfo_L.status;

        if (READ_VAR_FIELD(intEnable, ESPIIE_AMDONEIE))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Notify user                                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            EXECUTE_FUNC(ESPI_userIntHandler_L,         (ESPI_INT_FLASH_AUTO_MODE_TRANS_DONE));
            EXECUTE_FUNC(ESPI_FLASH_userIntHandler_L,   (ESPI_INT_FLASH_AUTO_MODE_TRANS_DONE));
        }
    }
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_GetCurrReqSize_l                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  reqType     - request type                                                             */
/*                  flashOffset - offset in flash                                                          */
/*                  size        - size of the whole transaction                                            */
/*                  currSize    - pointer to current size                                                  */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates current request size for manual mode successive requests only  */
/*                  This routine avoids crossing pages in read/write operations                            */
/*                  This routine verifies legal size for erase operation                                   */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_GetCurrReqSize_l (
    ESPI_FLASH_REQ_ACC_T    reqType,
    UINT32                  flashOffset,
    UINT32                  size,
    UINT32*                 currSize
)
{
    UINT32  sizeUpToEndOfPage;
    UINT32  sizeUpToEndOfSector;

    switch (reqType)
    {
    case ESPI_FLASH_REQ_ACC_WRITE:
        sizeUpToEndOfPage = ROUND_UP(flashOffset, ESPI_FLASH_WritePageSize_L) - flashOffset;
        *currSize = MIN(size, ESPI_FLASH_MaxTransSize[reqType]);
        if (sizeUpToEndOfPage > 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Avoid crossing pages in write request                                                       */
            /*---------------------------------------------------------------------------------------------*/
            *currSize = MIN(*currSize, sizeUpToEndOfPage);
        }
        break;

    case ESPI_FLASH_REQ_ACC_READ:
        sizeUpToEndOfSector = ROUND_UP(flashOffset, ESPI_FLASH_READ_SECTOR_SIZE) - flashOffset;
        *currSize = MIN(size, ESPI_FLASH_MaxTransSize[reqType]);
        if (sizeUpToEndOfSector > 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Avoid crossing sectors in read request                                                      */
            /*---------------------------------------------------------------------------------------------*/
            *currSize = MIN(*currSize, sizeUpToEndOfSector);
        }
        break;

    case ESPI_FLASH_REQ_ACC_ERASE:
        /*-------------------------------------------------------------------------------------------------*/
        /* Error check                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_COND_CHECK((size > 0) && (size < ESPI_FLASH_BLOCK_ERASE_NUM), DEFS_STATUS_PARAMETER_OUT_OF_RANGE);
        *currSize = size;
        break;

    default:
        break;
    }

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_HandleAutoModeErr_l                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles automatic mode error                                              */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_FLASH_HandleAutoModeErr_l (BOOLEAN useDMA)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear status                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_AMERR));

#ifdef ESPI_CAPABILITY_FLASH_NON_FATAL_ERR_IND
    /*-----------------------------------------------------------------------------------------------------*/
    /* If it's not unsuccessful completion or protocol error                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!READ_VAR_FIELD(ESPI_GetError(), ESPIERR_UNFLASH) && !READ_VAR_FIELD(ESPI_GetError(), ESPIERR_PROTERR))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Send host NON FATAL ERROR virtual wire indication                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_VW_SmNonFatalError_l();
    }
#endif

#ifdef ESPI_CAPABILITY_FLASH_CLEAR_STATUS
    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear receive buffer status                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_FLASHRX));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear automatic mode done status                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_AMDONE));
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset the pointers of the Transmit and Receive buffers                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_RSTBUFHEADS, 0x01);

    if (useDMA)
    {
#ifdef GDMA_MODULE_TYPE
        /*-------------------------------------------------------------------------------------------------*/
        /* Disable DMA request                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_DMATHRESH, ESPI_FLASH_DMA_THRESHOLD_DISABLE);

        /*-------------------------------------------------------------------------------------------------*/
        /* Restore interrupts handling                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_FLASH_RX_INTERRUPT_RESTORE(ESPI_FLASH_RXIE_L);
#endif
    }

    ESPI_FLASH_status_L = DEFS_STATUS_FAIL;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_ExitAutoReadRequest_l                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Exit read request; set manual mode and return status                      */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_FLASH_ExitAutoReadRequest_l (
    DEFS_STATUS status,
    BOOLEAN useDMA
)
{
    if (READ_REG_FIELD(ESPI_ESPISTS,ESPISTS_AMDONE) == TRUE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Clear automatic mode done status                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_AMDONE));
    }
    if (useDMA)
    {
#ifdef GDMA_MODULE_TYPE
        if (READ_REG_FIELD(ESPI_ESPISTS,ESPISTS_FLASHRX) == TRUE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Clear bus mastering receive status                                                          */
            /*---------------------------------------------------------------------------------------------*/
            REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_FLASHRX));
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Restore receive buffer interrupt                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_FLASH_RX_INTERRUPT_RESTORE(ESPI_FLASH_RXIE_L);

#ifndef ESPI_CAPABILITY_FLASH_MODE_CHANGE
        /*-------------------------------------------------------------------------------------------------*/
        /* Disable DMA                                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(ESPI_FLASHCTL, FLASHCTL_DMATHRESH, ESPI_FLASH_DMA_THRESHOLD_DISABLE);
#endif
#endif
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set auto mode off                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_FLASH_AUTO_MODE(FALSE);

    ESPI_FLASH_status_L = status;
}

#ifdef ESPI_CAPABILITY_SAF
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_SendRes_l                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  comp        - Completion status                                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine send response                                                             */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_FLASH_SAF_SendRes_l(UINT8 comp)
{
    ESPI_FLASH_TRANS_HDR resHdr = {0};
    UINT roundedSize;
    UINT i;

    switch(ESPI_FLASH_SafReqInfo_L.reqType)
    {
        case ESPI_FLASH_SAF_REQ_READ:
        case ESPI_FLASH_SAF_REQ_RPMC_OP2:
            SET_VAR_FIELD(resHdr.tagPlusLength, HEADER_LENGTH, ESPI_FLASH_SafReqInfo_L.outBufferSize);
        break;

        case ESPI_FLASH_SAF_REQ_WRITE:
        case ESPI_FLASH_SAF_REQ_ERASE:
        case ESPI_FLASH_SAF_REQ_RPMC_OP1:
            SET_VAR_FIELD(resHdr.tagPlusLength, HEADER_LENGTH, 0);
        break;

        case ESPI_FLASH_SAF_REQ_NUM:
        default:
        break;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set response header                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    resHdr.type = comp;
    SET_VAR_FIELD(resHdr.tagPlusLength, HEADER_TAG, ESPI_FLASH_SafReqInfo_L.tag);
    resHdr.pktLen = (UINT8)(READ_VAR_FIELD(resHdr.tagPlusLength, HEADER_LENGTH) + 3);


    /*-----------------------------------------------------------------------------------------------------*/
    /* Wait till transmit buffer is available                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    while (READ_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_FLASH_ACC_TX_AVAIL) == 1) ;
    REG_WRITE(ESPI_FLASHTXWRHEAD,(*((UINT32*)(void*)(&resHdr))));
    if (ESPI_FLASH_SafReqInfo_L.reqType == ESPI_FLASH_SAF_REQ_READ)
    {
        roundedSize = ROUND_UP(ESPI_FLASH_SafReqInfo_L.outBufferSize,sizeof(UINT32)) / sizeof(UINT32);
        for(i = 0; i < roundedSize; i++)
        {
            REG_WRITE(ESPI_FLASHTXWRHEAD,ESPI_FLASH_SafReqInfo_L.buffer[i]);
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Notify host transfer buffer is full                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(ESPI_FLASHCTL,FLASHCTL_FLASH_ACC_TX_AVAIL,0x01);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_CheckAddress_l                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, DEFS_STATUS_SYSTEM_BUSY if transaction has not ended yet    */
/*                  and other DEFS_STATUS error on error                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine verify request type is valid for the address range requested              */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_SAF_CheckAddress_l(void)
{
    UINT i;
    UINT16 highAddr;
    UINT16 baseAddr;
    BOOLEAN readProt;
    BOOLEAN writeProt;
    UINT16 readTagOvr;
    UINT16 writeTagOvr;

    /*-----------------------------------------------------------------------------------------------------*/
    /* For each memory protection range                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < ESPI_FLASH_SAF_PROT_MEM_NUM; i++)
    {
        highAddr = READ_REG_FIELD(ESPI_FLASH_PRTR_HADDRn(i),ESPI_FLASH_PRTR_HADDRn_HADDR);
        if (highAddr != 0)
        {
            baseAddr = READ_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(i),ESPI_FLASH_PRTR_BADDRn_BADDR);
            /*---------------------------------------------------------------------------------------------*/
            /* If request memory is within the memory range of index i                                     */
            /*---------------------------------------------------------------------------------------------*/
            if (((ESPI_FLASH_SafReqInfo_L.offset >= baseAddr) && (ESPI_FLASH_SafReqInfo_L.offset <= highAddr))
                ||
                (((ESPI_FLASH_SafReqInfo_L.offset + ESPI_FLASH_SafReqInfo_L.size) >= baseAddr)
                  && ((ESPI_FLASH_SafReqInfo_L.offset + ESPI_FLASH_SafReqInfo_L.size) <= highAddr)))
            {
                switch(ESPI_FLASH_SafReqInfo_L.reqType)
                {
                    case ESPI_FLASH_SAF_REQ_READ:
                        readProt = READ_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(i), ESPI_FLASH_PRTR_BADDRn_FRGN_RPR);
                        if (readProt)
                        {
                            readTagOvr = READ_REG_FIELD(ESPI_FLASH_FLASH_RGN_TAG_OVRn(i), ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_RPR_TOVR);
                            /*-----------------------------------------------------------------------------*/
                            /* If range is read protected and relevant tag overrun is not set              */
                            /*-----------------------------------------------------------------------------*/
                            if (READ_VAR_BIT(readTagOvr,ESPI_FLASH_SafReqInfo_L.tag) == FALSE)
                            {
                                return DEFS_STATUS_FAIL;
                            }
                        }
                        break;

                    case ESPI_FLASH_SAF_REQ_ERASE:
                    case ESPI_FLASH_SAF_REQ_WRITE:
                        writeProt = READ_REG_FIELD(ESPI_FLASH_PRTR_BADDRn(i), ESPI_FLASH_PRTR_BADDRn_FRGN_WPR);
                        if (writeProt)
                        {
                            writeTagOvr = READ_REG_FIELD(ESPI_FLASH_FLASH_RGN_TAG_OVRn(i), ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_WPR_TOVR);
                            /*-----------------------------------------------------------------------------*/
                            /* If range is write protected and relevant tag overrun is not set             */
                            /*-----------------------------------------------------------------------------*/
                            if (READ_VAR_BIT(writeTagOvr,ESPI_FLASH_SafReqInfo_L.tag) == FALSE)
                            {
                                return DEFS_STATUS_FAIL;
                            }
                        }
                        break;

                    case ESPI_FLASH_SAF_REQ_RPMC_OP1:
                    case ESPI_FLASH_SAF_REQ_RPMC_OP2:
                    case ESPI_FLASH_SAF_REQ_NUM:
                    default:
                        break;
                }
            }
        }
    }
    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_FLASH_SAF_PerformReq_l                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  addr        - Start address                                                            */
/*                  size        - Size of request                                                          */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, DEFS_STATUS_SYSTEM_BUSY if transaction has not ended yet    */
/*                  and other DEFS_STATUS error on error                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs the request on the flash                                         */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_FLASH_SAF_PerformReq_l(UINT32 addr, UINT32 size)
{

    switch(ESPI_FLASH_SafReqInfo_L.reqType)
    {
        case ESPI_FLASH_SAF_REQ_READ:
            DEFS_STATUS_RET_CHECK(FLASH_DEV_Read(&ESPI_FLASH_SafFlashParams_L, FIU_MODULE_0, FIU_CS_0, addr, (UINT8*)(void*)(ESPI_FLASH_SafReqInfo_L.buffer), size));
            ESPI_FLASH_SafReqInfo_L.outBufferSize = size;
        break;

        case ESPI_FLASH_SAF_REQ_WRITE:
            DEFS_STATUS_RET_CHECK(FLASH_DEV_WritePage(&ESPI_FLASH_SafFlashParams_L, FIU_MODULE_0, FIU_CS_0, addr, (UINT8*)(void*)(ESPI_FLASH_SafReqInfo_L.buffer), size, FALSE));
            ESPI_FLASH_SafReqInfo_L.outBufferSize = 0;
        break;

        case ESPI_FLASH_SAF_REQ_ERASE:
            DEFS_STATUS_RET_CHECK(FLASH_DEV_EraseType(&ESPI_FLASH_SafFlashParams_L, FIU_MODULE_0,FIU_CS_0, FLASH_DEV_ERASE_BLOCK, addr, FALSE));
            ESPI_FLASH_SafReqInfo_L.outBufferSize = 0;
        break;

        case ESPI_FLASH_SAF_REQ_RPMC_OP1:
            ESPI_FLASH_SafReqInfo_L.outBufferSize = 0;
            // TODO: DEFS_STATUS_RET_CHECK(FLASH_RPMC_OP1(FIU_MODULE_0, size, inBuff));
        break;

        case ESPI_FLASH_SAF_REQ_RPMC_OP2:
            ESPI_FLASH_SafReqInfo_L.outBufferSize = 0; // TODO ???
            // TODO: DEFS_STATUS_RET_CHECK(FLASH_RPMC_OP2(FIU_MODULE_0, size, inBuff, outbuff, outBufferSize));
        break;

        case ESPI_FLASH_SAF_REQ_NUM:
        default:
            break;
    }
    return DEFS_STATUS_OK;
}
#endif // ESPI_CAPABILITY_SAF


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                   OOB LOCAL FUNCTIONS IMPLEMENTATION                                    */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_Init_l                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes the Out-Of-Band channel.                                      */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_OOB_Init_l (void)
{
    UINT i;

    for (i = 0; i < ESPI_OOB_CMD_LAST; i++)
    {
        ESPI_OOB_cmdInfo[i].srcAddr     = 0;
        ESPI_OOB_cmdInfo[i].destAddr    = 0;
        ESPI_OOB_cmdInfo[i].cmdCode     = 0;
        ESPI_OOB_cmdInfo[i].nWrite      = 0;
        ESPI_OOB_cmdInfo[i].nRead       = 0;
        ESPI_OOB_cmdInfo[i].writeBuf    = NULL;
        ESPI_OOB_cmdInfo[i].readBuf     = NULL;
    }

    ESPI_OOB_currOutCmdInfo_L           = NULL;
    ESPI_OOB_currInCmdInfo_L            = NULL;
    ESPI_OOB_currReqTag_L               = 0;
    ESPI_OOB_Status_L                   = DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_BuildCmd_l                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  srcAddr     - Source address                                                           */
/*                  destAddr    - Destination address                                                      */
/*                  cmdCode     - Command code                                                             */
/*                  nWrite      - Number of bytes to write                                                 */
/*                  writeBuf    - Write buffer pointer                                                     */
/*                  nRead       - Number of bytes to read                                                  */
/*                  readBuf     - Read buffer pointer                                                      */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine builds OOB command                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_OOB_BuildCmd_l (
    UINT8   srcAddr,
    UINT8   destAddr,
    UINT8   cmdCode,
    UINT8   nWrite,
    UINT8*  writeBuf,
    UINT8   nRead,
    UINT8*  readBuf
)
{
    switch (destAddr)
    {
    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_ESPI_HW:
        ESPI_OOB_currOutCmdInfo_L = &ESPI_OOB_cmdInfo[ESPI_OOB_CMD_HW];
        break;

    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_CSME_FW:
        ESPI_OOB_currOutCmdInfo_L = &ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CSME];
        break;

    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_PMC_FW:
        ESPI_OOB_currOutCmdInfo_L = &ESPI_OOB_cmdInfo[ESPI_OOB_CMD_PMC];
        break;

    default:
        ESPI_OOB_currOutCmdInfo_L = NULL;
        return DEFS_STATUS_FAIL;
    }

    ESPI_OOB_currOutCmdInfo_L->srcAddr  = srcAddr;
    ESPI_OOB_currOutCmdInfo_L->destAddr = destAddr;
    ESPI_OOB_currOutCmdInfo_L->cmdCode  = cmdCode;
    ESPI_OOB_currOutCmdInfo_L->nWrite   = nWrite;
    ESPI_OOB_currOutCmdInfo_L->writeBuf = writeBuf;
    ESPI_OOB_currOutCmdInfo_L->nRead    = nRead;
    ESPI_OOB_currOutCmdInfo_L->readBuf  = readBuf;

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_RunCmd_l                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine runs command in polling/interrupt mode                                    */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_OOB_RunCmd_l (void)
{
    BOOLEAN oobIntEn = READ_REG_FIELD(ESPI_ESPIIE, ESPIIE_OOBRXIE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send OOB Command Request                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_SendCmd_l());

    /*-----------------------------------------------------------------------------------------------------*/
    /* Interrupt mode - response is handled via interrupt routine                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (oobIntEn)
    {
        return DEFS_STATUS_OK;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Polling mode - wait till receive buffer is full                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    BUSY_WAIT_TIMEOUT((READ_REG_FIELD(ESPI_ESPISTS, ESPISTS_OOBRX) == FALSE), ESPI_OOB_RX_TIMEOUT) ;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear status                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(ESPI_ESPISTS, MASK_FIELD(ESPISTS_OOBRX));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Receive OOB Command response                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_ReceiveCmd_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_SendCmd_l                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends OOB command                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_OOB_SendCmd_l (void)
{
    ESPI_OOB_MCTP_PKT   mctpPct = {0};
    UINT8               addr;
    UINT                i, msgOffset = sizeof(ESPI_OOB_MCTP_PKT);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Destination Slave Address - LSBit is 0                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    addr = ESPI_OOB_currOutCmdInfo_L->destAddr;
    CLEAR_VAR_BIT(addr, 0);
    mctpPct.destAddr = addr;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Source Slave Address - LSBit is 1                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    addr = ESPI_OOB_currOutCmdInfo_L->srcAddr;
    SET_VAR_BIT(addr, 0);
    mctpPct.srcAddr = addr;

    /*-----------------------------------------------------------------------------------------------------*/
    /* OOB Channel SMBus Command Code                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    mctpPct.cmdCode = ESPI_OOB_currOutCmdInfo_L->cmdCode;

    /*-----------------------------------------------------------------------------------------------------*/
    /* OOB Byte Count includes the OOB Channel SMBus Slave Source Address, so we add 1 byte                */
    /*-----------------------------------------------------------------------------------------------------*/
    mctpPct.byteCount = ESPI_OOB_currOutCmdInfo_L->nWrite + 1;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Build the MCTP Packet                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    *((UINT32*)ESPI_OOB_OutBuf_L) = *((UINT32*)(void*)&mctpPct);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate OOB Channel Request length; fill Request data into buffer                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < ESPI_OOB_currOutCmdInfo_L->nWrite; i++)
    {
        ESPI_OOB_OutBuf_L[msgOffset + i] = ESPI_OOB_currOutCmdInfo_L->writeBuf[i];
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Build the OOB Channel SMBus request                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_BuildReq_l((ESPI_OOB_currOutCmdInfo_L->nWrite + (UINT8)msgOffset), ESPI_OOB_OutBuf_L,
                                              (ESPI_OOB_currOutCmdInfo_L->nRead  + (UINT8)msgOffset), ESPI_OOB_InBuf_L));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send the OOB Channel SMBus request                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(ESPI_OOB_SendReq_l());

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_BuildReq_l                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  nWrite      - Number of bytes to write                                                 */
/*                  writeBuf    - Write buffer pointer                                                     */
/*                  nRead       - Number of bytes to read                                                  */
/*                  readBuf     - Read buffer pointer                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine builds OOB request                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_OOB_BuildReq_l (
    UINT8   nWrite,
    UINT8*  writeBuf,
    UINT8   nRead,
    UINT8*  readBuf
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check number of bytes to write/read does not exceed the maximum payload                             */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((nWrite <= ESPI_OOB_MAX_PAYLOAD), DEFS_STATUS_PARAMETER_OUT_OF_RANGE);
    DEFS_STATUS_COND_CHECK((nRead  <= ESPI_OOB_MAX_PAYLOAD), DEFS_STATUS_PARAMETER_OUT_OF_RANGE);

    ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].nWrite   = nWrite;
    ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].writeBuf = writeBuf;
    ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].nRead    = nRead;
    ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].readBuf  = readBuf;

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_SendReq_l                                                                     */
/*                                                                                                         */
/* Parameters:     none                                                                                    */
/*                                                                                                         */
/* Returns:        DEFS_STATUS_OK on success and other DEFS_STATUS error on error                          */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends OOB request                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_OOB_SendReq_l (void)
{
    ESPI_FLASH_TRANS_HDR    trasHdr         = {0};
    UINT16                  tagPlusLength   = 0;
    UINT8                   nWrite          = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].nWrite;
    UINT8*                  writeBuf        = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].writeBuf;
    UINT                    i               = 0;
    OOB_DATA                data;
#ifndef ESPI_CAPABILITY_OOBHEAD
    UINT                    buf             = 0;
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check OOB is available, to verify that the Tx buffer is empty                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((READ_REG_FIELD(ESPI_OOBCTL, OOBCTL_OOB_AVAIL) == 0), DEFS_STATUS_HARDWARE_ERROR);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write the OOB transaction header                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(tagPlusLength, HEADER_LENGTH, nWrite);
    SET_VAR_FIELD(tagPlusLength, HEADER_TAG,    ESPI_OOB_currReqTag_L);

    trasHdr.pktLen          = (sizeof(trasHdr) - sizeof(trasHdr.pktLen)) + nWrite;
    trasHdr.type            = ESPI_OOB_MESSAGE;
    trasHdr.tagPlusLength   = LE16(tagPlusLength);

#ifdef ESPI_CAPABILITY_OOBHEAD
    REG_WRITE(ESPI_OOBTXWRHEAD,     (*((UINT32*)(void*)(&trasHdr))));
#else
    REG_WRITE(ESPI_OOBTXBUF(buf++), (*((UINT32*)(void*)(&trasHdr))));
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write the OOB Data Payload                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    while (nWrite)
    {
        data.value = 0;

                    { data.bytes.lsb0 = writeBuf[i++]; nWrite--; }
        if (nWrite) { data.bytes.lsb1 = writeBuf[i++]; nWrite--; }
        if (nWrite) { data.bytes.lsb2 = writeBuf[i++]; nWrite--; }
        if (nWrite) { data.bytes.lsb3 = writeBuf[i++]; nWrite--; }

#ifdef ESPI_CAPABILITY_OOBHEAD
        REG_WRITE(ESPI_OOBTXWRHEAD,     (UINT32)data.value);
#else
        REG_WRITE(ESPI_OOBTXBUF(buf++), (UINT32)data.value);
#endif
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set OOB available bit, to enqueue the packet for transmission                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK((ESPI_IsChannelSlaveEnable_l(ESPI_CHANNEL_OOB) == ENABLE), DEFS_STATUS_HARDWARE_ERROR);
    REG_WRITE(ESPI_OOBCTL, MASK_FIELD(OOBCTL_OOB_AVAIL));

    return DEFS_STATUS_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_ReceiveCmd_l                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles incoming OOB command                                              */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_OOB_ReceiveCmd_l (void)
{
    ESPI_OOB_MCTP_PKT   mctpPct = {0};
    DEFS_STATUS         status  = DEFS_STATUS_OK;
    UINT8               srcAddr;
    UINT8               destAddr;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read the received request buffer                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_Status_L = ESPI_OOB_ReceiveReq_l();

    if (ESPI_OOB_Status_L == DEFS_STATUS_INVALID_DATA_FIELD)
    {
        srcAddr = ESPI_OOB_currOutCmdInfo_L->destAddr;
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Read the MCTP Response Packet                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        *((UINT32*)(void*)&mctpPct) = *((UINT32*)ESPI_OOB_InBuf_L);

        /*-------------------------------------------------------------------------------------------------*/
        /* Check Source Slave Address (PCH OOB HW Handler / CSME FW) - LSBit 1, therefore clear it         */
        /*-------------------------------------------------------------------------------------------------*/
        srcAddr = mctpPct.srcAddr;
        CLEAR_VAR_BIT(srcAddr, 0);
    }

    switch (srcAddr)
    {
    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_ESPI_HW:
        ESPI_OOB_currInCmdInfo_L = &ESPI_OOB_cmdInfo[ESPI_OOB_CMD_HW];
        break;

    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_CSME_FW:
        ESPI_OOB_currInCmdInfo_L = &ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CSME];
        break;

    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_PMC_FW:
        ESPI_OOB_currInCmdInfo_L = &ESPI_OOB_cmdInfo[ESPI_OOB_CMD_PMC];
        break;

    default:
        ESPI_OOB_currInCmdInfo_L = NULL;
        return DEFS_STATUS_FAIL;
    }

    if (ESPI_OOB_Status_L != DEFS_STATUS_INVALID_DATA_FIELD)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Check:                                                                                          */
        /* - Destination Slave Address (eSPI Slave 0 [EC]) - LSBit 0                                       */
        /* - Source Slave Address (PCH HW / CSME FW / PMC FW) - LSBit 1                                    */
        /* - Command Code                                                                                  */
        /* - Byte Count (which includes the Source Slave Address)                                          */
        /*-------------------------------------------------------------------------------------------------*/
        destAddr = ESPI_OOB_currInCmdInfo_L->srcAddr;
        CLEAR_VAR_BIT(destAddr, 0);

        srcAddr = ESPI_OOB_currInCmdInfo_L->destAddr;
        SET_VAR_BIT(srcAddr, 0);

        if (mctpPct.destAddr   != destAddr ||
            mctpPct.srcAddr    != srcAddr  ||
            mctpPct.cmdCode    != ESPI_OOB_currInCmdInfo_L->cmdCode ||
            (ESPI_OOB_Status_L != DEFS_STATUS_INVALID_DATA_SIZE &&
             mctpPct.byteCount != (ESPI_OOB_currInCmdInfo_L->nRead + 1)))
        {
            status = DEFS_STATUS_INVALID_DATA_FIELD;
        }
    }

    switch (ESPI_OOB_currInCmdInfo_L->destAddr)
    {
    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_ESPI_HW:
        switch (ESPI_OOB_currInCmdInfo_L->cmdCode)
        {
        case ESPI_OOB_HW_CMD_GET_PCH_TEMP:
        case ESPI_OOB_HW_CMD_GET_PCH_RTC_TIME:
            ESPI_OOB_HW_ReceiveMessage_l();
            break;

        default:
            status = DEFS_STATUS_INVALID_DATA_FIELD;
        }
        break;

    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_CSME_FW:
        switch (ESPI_OOB_currInCmdInfo_L->cmdCode)
        {
        case ESPI_OOB_USB_CMD:
            ESPI_OOB_USB_ReceiveMessage_l();
            break;

        default:
            status = DEFS_STATUS_INVALID_DATA_FIELD;
        }
        break;

    case ESPI_OOB_SMB_SLAVE_DEST_ADDR_PMC_FW:
        switch (ESPI_OOB_currInCmdInfo_L->cmdCode)
        {
        case ESPI_OOB_PECI_CMD:
            ESPI_OOB_PECI_ReceiveMessage_l();
            break;

        case ESPI_OOB_CRASHLOG_CMD_GET_PCH:
        case ESPI_OOB_CRASHLOG_CMD_GET_CPU:
        case ESPI_OOB_CRASHLOG_CMD_GET_PCH_CPU:
        case ESPI_OOB_CRASHLOG_CMD_GET_1K_CORE:
        case ESPI_OOB_CRASHLOG_CMD_GET_1K_CRITICAL:
            ESPI_OOB_CRASHLOG_ReceiveMessage_l();
            break;

        default:
            status = DEFS_STATUS_INVALID_DATA_FIELD;
        }
        break;

    default:
        status = DEFS_STATUS_INVALID_DATA_FIELD;
    }

    return status;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_ReceiveReq_l                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success and other DEFS_STATUS error on error                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles incoming OOB request                                              */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS ESPI_OOB_ReceiveReq_l (void)
{
    ESPI_FLASH_TRANS_HDR    trasHdr         = {0};
    UINT16                  tagPlusLength   = 0;
    UINT8                   nRead           = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].nRead;
    UINT8*                  readBuf         = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CUR].readBuf;
    UINT                    i               = 0;
    DEFS_STATUS             status          = DEFS_STATUS_OK;
    OOB_DATA                data;
#ifndef ESPI_CAPABILITY_OOBHEAD
    UINT                    buf             = 0;
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read the OOB transaction header                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_OOBHEAD
    *((UINT32*)(void*)&trasHdr) = REG_READ(ESPI_OOBRXRDHEAD);
#else
    *((UINT32*)(void*)&trasHdr) = REG_READ(ESPI_OOBRXBUF(buf++));
#endif
    tagPlusLength               = LE16(trasHdr.tagPlusLength);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check header fields                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((trasHdr.type != ESPI_OOB_MESSAGE) || (READ_VAR_FIELD(tagPlusLength, HEADER_TAG) != ESPI_OOB_currReqTag_L))
    {
        status = DEFS_STATUS_INVALID_DATA_FIELD;
    }
    else if (READ_VAR_FIELD(tagPlusLength, HEADER_LENGTH) != nRead)
    {
        status = DEFS_STATUS_INVALID_DATA_SIZE;
    }

#if 0
    /*-----------------------------------------------------------------------------------------------------*/
    /* Update tag for next transaction:                                                                    */
    /* It seems that the PCH always returns TAG=0 on PUT_OOB, even when the respective GET_OOB contains a  */
    /* non-zero tag. therefore, we avoid incrementing the tag between requests                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_currReqTag_L = ESPI_FLASH_GET_NEXT_TAG(ESPI_OOB_currReqTag_L);
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read the OOB Data Payload                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    while (nRead)
    {
#ifdef ESPI_CAPABILITY_OOBHEAD
        data.value = REG_READ(ESPI_OOBRXRDHEAD);
#else
        data.value = REG_READ(ESPI_OOBRXBUF(buf++));
#endif

                    { readBuf[i++] = data.bytes.lsb0; nRead--; }
        if (nRead)  { readBuf[i++] = data.bytes.lsb1; nRead--; }
        if (nRead)  { readBuf[i++] = data.bytes.lsb2; nRead--; }
        if (nRead)  { readBuf[i++] = data.bytes.lsb3; nRead--; }
    }

    REG_WRITE(ESPI_OOBCTL, MASK_FIELD(OOBCTL_OOB_FREE));

    return status;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_CalcFCS_l                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  buff    - Buffer to calculate FCS on                                                   */
/*                  nBytes  - Number of bytes to calculate                                                 */
/*                                                                                                         */
/* Returns:         The calculated FCS                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates Assured / Read/Write FCS, which are required in write / read   */
/*                  transactions.                                                                          */
/*                  FCS provides the processor client / originator a high degree of confidence that the    */
/*                  data it received from the host / client is correct.                                    */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8 ESPI_OOB_PECI_CalcFCS_l (const UINT8* buff, UINT8 nBytes)
{
    UINT    i;
    UINT8   fcs = 0;

    for (i = 0; i < nBytes; i++)
    {
        fcs ^= buff[i];
        fcs  = ESPI_OOB_PECI_crc8_table[fcs];
    }

    return fcs;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_PECI_ReceiveMessage_l                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  PECI handler function - this routine performs the following:                           */
/*                  - Retry to send the PECI transaction in cases of errors.                               */
/*                  - Inform upper-layer on error type in case all retries fail.                           */
/*                  - Fill the PECI data buffer for the user's application use.                            */
/*                  - Invoke the application callback function and clear the interrupt.                    */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_OOB_PECI_ReceiveMessage_l (void)
{
    UINT                        i, offset       = 0;
    UINT8                       nBytesResponse  = 1;
    UINT8                       nBytesCc        = 0;
    UINT8                       dataSize;
    UINT32                      dataBuff[2]     = {0, 0};
    BOOLEAN                     isResponceErr   = FALSE;
    UINT8                       cc              = (UINT8)ESPI_OOB_PECI_CC_NONE;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read response data from buffer                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_ReceiveMessage_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retrieve PECI Response/Error Status and check it                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    isResponceErr = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_PMC].readBuf[offset++] != ESPI_OOB_PECI_CC_COMMAND_PASSED_DATA_VALID;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retrieve the command completion code (applicable for specific commands only)                        */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (ESPI_OOB_peciCurrentCommand_L)
    {
    case ESPI_OOB_PECI_COMMAND_PING:
    case ESPI_OOB_PECI_COMMAND_GET_DIB:
    case ESPI_OOB_PECI_COMMAND_GET_TEMP:
        break;

    case ESPI_OOB_PECI_COMMAND_RD_PKG_CFG:
    case ESPI_OOB_PECI_COMMAND_WR_PKG_CFG:
    case ESPI_OOB_PECI_COMMAND_RD_IAMSR:
    case ESPI_OOB_PECI_COMMAND_WR_IAMSR:
    case ESPI_OOB_PECI_COMMAND_RD_PCI_CFG:
    case ESPI_OOB_PECI_COMMAND_WR_PCI_CFG:
    case ESPI_OOB_PECI_COMMAND_RD_PCI_CFG_LOCAL:
    case ESPI_OOB_PECI_COMMAND_WR_PCI_CFG_LOCAL:
    case ESPI_OOB_PECI_COMMAND_NONE:
    default:
        /*-------------------------------------------------------------------------------------------------*/
        /* Retrieve Completion Code only when there is no HW error (otherwise CC is not updated)           */
        /*-------------------------------------------------------------------------------------------------*/
        if (!isResponceErr)
        {
            cc = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_PMC].readBuf[offset];
        }
        offset++;
        nBytesCc = 1;
        break;
    }

    ESPI_OOB_peciCc_L = cc;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retrieve the amount of data to read (excluding Completion Code, if applicable, and Response bytes)  */
    /*-----------------------------------------------------------------------------------------------------*/
    dataSize = (ESPI_OOB_cmdInfo[ESPI_OOB_CMD_PMC].nRead - nBytesResponse) - nBytesCc;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retry to send the PECI transaction in the following cases:                                          */
    /* - Bad FCS                                                                                           */
    /* - Abort FCS                                                                                         */
    /* - CC: 0x8x                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (isResponceErr || (cc & 0x80) == 0x80)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Retry to send the PECI transaction                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_OOB_peciRetryCount_L--;

        if (ESPI_OOB_peciRetryCount_L > 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Set the 'retry' bit to indicate this is a retry transaction                                 */
            /*---------------------------------------------------------------------------------------------*/
            SET_VAR_FIELD(ESPI_OOB_peciMsg_L.data[0], ESPI_OOB_PECI_RETRY, 1);

            /*---------------------------------------------------------------------------------------------*/
            /* Re-start the failed transaction                                                             */
            /*---------------------------------------------------------------------------------------------*/
            (void)ESPI_OOB_RunCmd_l();
        }
        else
        {
            /*---------------------------------------------------------------------------------------------*/
            /* PECI transaction failed for [PECI_RETRY_MAX_COUNT] times - Initialize retry counter         */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_OOB_peciRetryCount_L = ESPI_OOB_PECI_RETRY_MAX_COUNT;

            /*---------------------------------------------------------------------------------------------*/
            /* Clear current OOB Command Information                                                       */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_OOB_ClearCmdInfo_l();

            /*---------------------------------------------------------------------------------------------*/
            /* Abort the PECI transaction and inform on error                                              */
            /*---------------------------------------------------------------------------------------------*/
            EXECUTE_FUNC(ESPI_OOB_peciCallback_L, (ESPI_OOB_peciCurrentCommand_L, ESPI_OOB_PECI_DATA_SIZE_NONE,
                                                   ESPI_OOB_PECI_TRANS_DONE_CC_ERROR, NO_PECI_DATA, NO_PECI_DATA));
        }
    }
    else if ((cc & 0x90) == 0x90)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Clear current OOB Command Information                                                           */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_OOB_ClearCmdInfo_l();

        /*-------------------------------------------------------------------------------------------------*/
        /* Abort the PECI transaction and inform on error in case CC = 0x9x                                */
        /*-------------------------------------------------------------------------------------------------*/
        EXECUTE_FUNC(ESPI_OOB_peciCallback_L, (ESPI_OOB_peciCurrentCommand_L, ESPI_OOB_PECI_DATA_SIZE_NONE,
                                               ESPI_OOB_PECI_TRANS_DONE_CC_ERROR, NO_PECI_DATA, NO_PECI_DATA));
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Read response data from buffer                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        for (i = 0; i < dataSize; i++)
        {
            ((UINT8*)dataBuff)[i] = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_PMC].readBuf[offset + i];
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Clear current OOB Command Information                                                           */
        /*-------------------------------------------------------------------------------------------------*/
        ESPI_OOB_ClearCmdInfo_l();

        /*-------------------------------------------------------------------------------------------------*/
        /* Transaction is completed successfully                                                           */
        /*-------------------------------------------------------------------------------------------------*/
        EXECUTE_FUNC(ESPI_OOB_peciCallback_L, (ESPI_OOB_peciCurrentCommand_L, (ESPI_OOB_PECI_DATA_SIZE_T)dataSize,
                                               ESPI_OOB_PECI_TRANS_DONE_OK, dataBuff[0], dataBuff[1]));
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_USB_ReceiveMessage_l                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  OOB USB handler function - this routine verifies response matches request and return   */
/*                  status of request.                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_OOB_USB_ReceiveMessage_l (void)
{
    DEFS_STATUS             status;
    ESPI_OOB_USB_USAGE_T    request     = (ESPI_OOB_USB_USAGE_T)ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CSME].writeBuf[0];
    UINT8                   response    = ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CSME].readBuf[0];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read response data from buffer                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_ReceiveMessage_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retrieve the USB usage                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (request)
    {
    case ESPI_OOB_USB_USAGE_PORT_CONNECT:
    case ESPI_OOB_USB_USAGE_PORT_DISCONNECT:
        /*-------------------------------------------------------------------------------------------------*/
        /* Verify response matches request                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        if (response == (ESPI_OOB_USB_USAGE_RES_MASK(request)))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* A status of 00h indicates successful completion.                                            */
            /* A status of 01h indicates an error occurred                                                 */
            /*---------------------------------------------------------------------------------------------*/
            status = (ESPI_OOB_cmdInfo[ESPI_OOB_CMD_CSME].readBuf[3] == 0) ? DEFS_STATUS_OK : DEFS_STATUS_FAIL;

            /*---------------------------------------------------------------------------------------------*/
            /* Clear current OOB Command Information                                                       */
            /*---------------------------------------------------------------------------------------------*/
            ESPI_OOB_ClearCmdInfo_l();

            /*---------------------------------------------------------------------------------------------*/
            /* Notify user                                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            EXECUTE_FUNC(ESPI_OOB_usbCallback_L, (request, status));
        }
        break;

    default:
        break;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_CRASHLOG_ReceiveMessage_l                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  OOB CRASHLOG handler function - this routine saves response data                       */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_OOB_CRASHLOG_ReceiveMessage_l (void)
{
    ESPI_OOB_CRASHLOG_CMD_T cmdCode = (ESPI_OOB_CRASHLOG_CMD_T)ESPI_OOB_cmdInfo[ESPI_OOB_CMD_PMC].cmdCode;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read response data from buffer                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_ReceiveMessage_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear current OOB Command Information                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_ClearCmdInfo_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Call user callback                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    EXECUTE_FUNC(ESPI_OOB_crashLogCallback_L, (cmdCode, ESPI_OOB_Status_L));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_HW_ReceiveMessage_l                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  OOB HW handler function - this routine saves response data                             */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_OOB_HW_ReceiveMessage_l (void)
{
    ESPI_OOB_HW_CMD_T cmdCode = (ESPI_OOB_HW_CMD_T)ESPI_OOB_cmdInfo[ESPI_OOB_CMD_HW].cmdCode;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read response data from buffer                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_ReceiveMessage_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear current OOB Command Information                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    ESPI_OOB_ClearCmdInfo_l();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Call user callback                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    EXECUTE_FUNC(ESPI_OOB_hwCallback_L, (cmdCode, ESPI_OOB_Status_L));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_ReceiveMessage_l                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads the OOB response data                                               */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_OOB_ReceiveMessage_l (void)
{
    UINT i, msgOffset = sizeof(ESPI_OOB_MCTP_PKT);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read response data from buffer                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < ESPI_OOB_currInCmdInfo_L->nRead; i++)
    {
        ESPI_OOB_currInCmdInfo_L->readBuf[i] = ESPI_OOB_InBuf_L[msgOffset + i];
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESPI_OOB_ClearCmdInfo_l                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine clears the OOB command information structure                              */
/*---------------------------------------------------------------------------------------------------------*/
static void ESPI_OOB_ClearCmdInfo_l  (void)
{
    ESPI_OOB_currInCmdInfo_L->srcAddr   = 0;
    ESPI_OOB_currInCmdInfo_L->destAddr  = 0;
    ESPI_OOB_currInCmdInfo_L->cmdCode   = 0;
    ESPI_OOB_currInCmdInfo_L->nWrite    = 0;
    ESPI_OOB_currInCmdInfo_L->nRead     = 0;
    ESPI_OOB_currInCmdInfo_L->writeBuf  = NULL;
    ESPI_OOB_currInCmdInfo_L->readBuf   = NULL;
    *((UINT32*)ESPI_OOB_InBuf_L)        = 0;
}

