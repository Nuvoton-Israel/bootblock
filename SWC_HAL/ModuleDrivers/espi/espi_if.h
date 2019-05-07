/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   espi_if.h                                                                */
/* This file contains Enhanced Serial Peripheral Interface (eSPI) Module driver */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _ESPI_IF_H
#define _ESPI_IF_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_IF()

#if defined ESPI_MODULE_TYPE
#include __MODULE_HEADER(espi, ESPI_MODULE_TYPE)
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
/* ESPI Channels                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_CHANNEL_NUM                    4

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI General Capabilities                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OPFREQ_BIT                     4
#define ESPI_IOMODESEL_BIT                  5
#define ESPI_ALERTMODE_BIT                  6
#define ESPI_CRC_CHK_EN_BIT                 7

/*---------------------------------------------------------------------------------------------------------*/
/* eSPI Channel type                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_CHANNEL_PC     = 0,    // Channel 0: Peripheral (Host Interface) Channel
    ESPI_CHANNEL_VW,            // Channel 1: Virtual Wire Channel
    ESPI_CHANNEL_OOB,           // Channel 2: Out-Of-Band Channel
    ESPI_CHANNEL_FLASH,         // Channel 3: Flash Access Channel
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
    ESPI_CHANNEL_PC_BM,
#endif
    ESPI_CHANNEL_GENERIC,       // General (no Channel-Specific)
    ESPI_CHANNEL_ALL,           // All Channels
} ESPI_CHANNEL_TYPE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI interrupt types                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_INT_IB_RST_CMD_RCV                 = 0,
    ESPI_INT_CONFIG_UPDATE                  = 1,
    ESPI_INT_BUS_ERR                        = 2,
    ESPI_INT_OOB_DATA_RCV                   = 3,
    ESPI_INT_FLASH_DATA_RCV                 = 4,
    ESPI_INT_FLASH_READ_ACCESS_DETECTED     = 5,
    ESPI_INT_PC_ACCESS_DETECTED             = 6,
    ESPI_INT_PC_TRANS_DEFFERED              = 7,
    ESPI_INT_VW_UPDATE                      = 8,
    ESPI_INT_RST_ACTIVE                     = 9,
    ESPI_INT_PLTRST_ACTIVE                  = 10,
#ifndef ESPI_CAPABILITY_VW_NO_FLOATING_EVENTS
    ESPI_INT_VW1_EVENT                      = 11,
    ESPI_INT_VW2_EVENT                      = 12,
    ESPI_INT_VW3_EVENT                      = 13,
    ESPI_INT_VW4_EVENT                      = 14,
#endif // ESPI_CAPABILITY_VW_NO_FLOATING_EVENTS
    ESPI_INT_FLASH_AUTO_MODE_TRANS_ERR      = 15,
    ESPI_INT_FLASH_AUTO_MODE_TRANS_DONE     = 16,
    ESPI_INT_PC_BM_MASTER_DATA_TRANSMITTED  = 19,
    ESPI_INT_PC_BM_MASTER_DATA_RCV          = 20,
    ESPI_INT_PC_BM_MSG_DATA_RCV             = 21,
    ESPI_INT_PC_BM_BURST_MODE_TRANS_ERR     = 22,
    ESPI_INT_PC_BM_BURST_MODE_TRANS_DONE    = 23,
}ESPI_INT_T;

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI Error types                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_ERR_INVCMD,
    ESPI_ERR_INVCYC,
    ESPI_ERR_CRCERR,
    ESPI_ERR_ABCOMP,
    ESPI_ERR_PROTERR,
    ESPI_ERR_BADSIZE,
    ESPI_ERR_NPBADALN,
    ESPI_ERR_PCBADALN,
    ESPI_ERR_FLBADALN,
    ESPI_ERR_UNCMD,
    ESPI_ERR_EXTRACYC,
    ESPI_ERR_VWERR
} ESPI_ERR_T;

/*---------------------------------------------------------------------------------------------------------*/
/* I/O Mode Supported                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_IO_MODE_SINGLE         = 0,    /* (default) do not change - code relies on enum value */
    ESPI_IO_MODE_SINGLE_DUAL,
    ESPI_IO_MODE_SINGLE_QUAD,
    ESPI_IO_MODE_SINGLE_DUAL_QUAD,
} ESPI_IO_MODE;

/*---------------------------------------------------------------------------------------------------------*/
/* I/O Mode Select                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_IO_MODE_SELECT_SINGLE  = 0,    /* (default) do not change - code relies on enum value */
    ESPI_IO_MODE_SELECT_DUAL,
    ESPI_IO_MODE_SELECT_QUAD
} ESPI_IO_MODE_SELECT;

/*---------------------------------------------------------------------------------------------------------*/
/* Maximum Frequency Supported                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_MAX_20_MHz             = 0,    /* (default) do not change - code relies on enum value */
    ESPI_MAX_25_MHz,
    ESPI_MAX_33_MHz,
    ESPI_MAX_50_MHz,
    ESPI_MAX_66_MHz,
} ESPI_MAX_FREQ;

/*---------------------------------------------------------------------------------------------------------*/
/* Operating Frequency                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OP_20_MHz              = 0,    /* (default) do not change - code relies on enum value */
    ESPI_OP_25_MHz,
    ESPI_OP_33_MHz,
    ESPI_OP_50_MHz,
    ESPI_OP_66_MHz,
} ESPI_OP_FREQ;

/*---------------------------------------------------------------------------------------------------------*/
/* Reset Output                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_RST_OUT_LOW            = 0,    /* (default) do not change - code relies on enum value */
    ESPI_RST_OUT_HIGH,
} ESPI_RST_OUT;

/*---------------------------------------------------------------------------------------------------------*/
/* Alert mode                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_ALERT_MODE_IO1         = 0,    /* (default) do not change - code relies on enum value */
    ESPI_ALERT_MODE_PIN,
} ESPI_ALERT_MODE;

#ifdef ESPI_CAPABILITY_AUTO_HANDSHAKE
/*---------------------------------------------------------------------------------------------------------*/
/* Selectable Delay for Automatic Handshake, the delay is in cycles of the High-frequency Clock (FMCLK)    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_HS1                    = 0,    /* (default) do not change - code relies on enum value */
    ESPI_HS2                    = 1,
    ESPI_HS3                    = 2,
} ESPI_HS_NUM;

typedef enum
{
    ESPI_HS_DELAY_512           = 0,    /* (default) do not change - code relies on enum value */
    ESPI_HS_DELAY_1024          = 1,
    ESPI_HS_DELAY_2048          = 2,
    ESPI_HS_DELAY_8192          = 3
} ESPI_HS_DELAY;

/*---------------------------------------------------------------------------------------------------------*/
/* Set Handshake Delay:                                                                                    */
/* 'num' should be of type ESPI_HS_NUM; 'delay' should be of type ESPI_HS_DELAY                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_AUTO_HS_DELAY(num, delay)  (delay << (9 + num * 4))

typedef enum
{
    ESPI_AUTO_PCRDY     = (1 << 0),     /* (default) do not change - code relies on enum value */
    ESPI_AUTO_VWCRDY    = (1 << 1),
    ESPI_AUTO_OOBCRDY   = (1 << 2),
    ESPI_AUTO_FCARDY    = (1 << 3),
    ESPI_AUTO_SBLD      = (1 << 4),
    ESPI_AUTO_HS1       = (1 << 8),
    ESPI_AUTO_HS2       = (1 << 12),
    ESPI_AUTO_HS3       = (1 << 16),
} ESPI_AUTO_HANDSHAKE;
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* General interrupt handler                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_INT_HANDLER)(ESPI_INT_T intType);


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   PC                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Host to slave message int handler                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_PC_BM_MSG_HANDLER)(UINT8 msgCode, UINT8* msgBuff, UINT32 msgLen);

/*---------------------------------------------------------------------------------------------------------*/
/* Slave to host request types                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_PC_BM_REQ_READ_MANUAL,
    ESPI_PC_BM_REQ_READ_AUTO,
    ESPI_PC_BM_REQ_READ_DMA,
    ESPI_PC_BM_REQ_WRITE,
    ESPI_PC_BM_REQ_MSG_LTR, //Do not change order, define extra message types after this definition
} ESPI_PC_BM_REQ_T;

/*---------------------------------------------------------------------------------------------------------*/
/* LTR message header latency scale field values                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_PC_BM_LTR_LATENCY_SCALE_1NS,
    ESPI_PC_BM_LTR_LATENCY_SCALE_32NS,
    ESPI_PC_BM_LTR_LATENCY_SCALE_1024NS,
    ESPI_PC_BM_LTR_LATENCY_SCALE_32768NS,
    ESPI_PC_BM_LTR_LATENCY_SCALE_1048576NS,
    ESPI_PC_BM_LTR_LATENCY_SCALE_33554432NS,
} ESPI_PC_BM_LTR_LATENCY_SCALE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* DMA Request Threshold                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_BM_DMA_THRESHOLD_DISABLE = 0,  //(default)
    ESPI_BM_DMA_THRESHOLD_4B      = 2,
    ESPI_BM_DMA_THRESHOLD_16B     = 3,
} ESPI_BM_DMA_THRESHOLD_SIZE;


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   VW                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* eSPI VW Direction                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_VW_SM,         // Slave-to-Master
    ESPI_VW_MS          // Master-to-Slave
} ESPI_VW_DIR_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Virtual Wire type and fields                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef UINT16          ESPI_VW;

#define ESPI_VW_WIRE    0, 2
#define ESPI_VW_DIR     7, 1
#define ESPI_VW_INDEX   8, 8

/*---------------------------------------------------------------------------------------------------------*/
/* Virtual Wires Definition macros (System events, Platform-Specific events, GPIOs)                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_VW_GET_INDEX(vw)                   (UINT8)READ_VAR_FIELD(vw, ESPI_VW_INDEX)
#define ESPI_VW_GET_WIRE(vw)                    (UINT8)READ_VAR_FIELD(vw, ESPI_VW_WIRE)
#define ESPI_VW_GET_DIR(vw)                     (ESPI_VW_DIR_T)READ_VAR_FIELD(vw, ESPI_VW_DIR)

#define ESPI_VW_DEF(idx, wire, dir)             (ESPI_VW)(BUILD_FIELD_VAL(ESPI_VW_INDEX, idx)   |   \
                                                          BUILD_FIELD_VAL(ESPI_VW_WIRE,  wire)  |   \
                                                          BUILD_FIELD_VAL(ESPI_VW_DIR,   dir))

/*---------------------------------------------------------------------------------------------------------*/
/* System events Virtual Wires                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_VW_SLP_S3                  ESPI_VW_DEF(0x02, 0, ESPI_VW_MS)    // S3 Sleep Control (SLP_S3#)
#define ESPI_VW_SLP_S4                  ESPI_VW_DEF(0x02, 1, ESPI_VW_MS)    // S4 Sleep Control (SLP_S4#)
#define ESPI_VW_SLP_S5                  ESPI_VW_DEF(0x02, 2, ESPI_VW_MS)    // S5 Sleep Control (SLP_S5#)
#define ESPI_VW_SUS_STAT                ESPI_VW_DEF(0x03, 0, ESPI_VW_MS)    // Suspend Status (SUS_STAT#)
#define ESPI_VW_PLTRST                  ESPI_VW_DEF(0x03, 1, ESPI_VW_MS)    // Platform Reset (PLTRST#)
#define ESPI_VW_OOB_RST_WARN            ESPI_VW_DEF(0x03, 2, ESPI_VW_MS)    // OOB Reset Warn (OOB_RST_WARN)
#define ESPI_VW_OOB_RST_ACK             ESPI_VW_DEF(0x04, 0, ESPI_VW_SM)    // OOB Reset Acknowledge (OOB_RST_ACK)
#define ESPI_VW_WAKE                    ESPI_VW_DEF(0x04, 2, ESPI_VW_SM)    // Wake the Host from Sx (WAKE#)
#define ESPI_VW_PME                     ESPI_VW_DEF(0x04, 3, ESPI_VW_SM)    // PCI Power Management Event (PME#)
#define ESPI_VW_SLAVE_BOOT_LOAD_DONE    ESPI_VW_DEF(0x05, 0, ESPI_VW_SM)    // Slave Boot Load Done
#define ESPI_VW_ERROR_FATAL             ESPI_VW_DEF(0x05, 1, ESPI_VW_SM)    // Error Fatal
#define ESPI_VW_ERROR_NONFATAL          ESPI_VW_DEF(0x05, 2, ESPI_VW_SM)    // Error Non-Fatal
#define ESPI_VW_SLAVE_BOOT_LOAD_STATUS  ESPI_VW_DEF(0x05, 3, ESPI_VW_SM)    // Slave Boot Load Status
#define ESPI_VW_SCI                     ESPI_VW_DEF(0x06, 0, ESPI_VW_SM)    // System Controller Interrupt (SCI#)
#define ESPI_VW_SMI                     ESPI_VW_DEF(0x06, 1, ESPI_VW_SM)    // System Management Interrupt (SMI#)
#define ESPI_VW_RCIN                    ESPI_VW_DEF(0x06, 2, ESPI_VW_SM)    // Reset CPU INIT (RCIN#)
#define ESPI_VW_HOST_RST_ACK            ESPI_VW_DEF(0x06, 3, ESPI_VW_SM)    // Host Reset Acknowledge (HOST_RST_ACK)
#define ESPI_VW_HOST_RST_WARN           ESPI_VW_DEF(0x07, 0, ESPI_VW_MS)    // Host Reset Warn (HOST_RST_WARN)
#define ESPI_VW_SMIOUT                  ESPI_VW_DEF(0x07, 1, ESPI_VW_MS)    // SMI Output (SMIOUT#)
#define ESPI_VW_NMIOUT                  ESPI_VW_DEF(0x07, 2, ESPI_VW_MS)    // NMI Output (NMIOUT#)

/*---------------------------------------------------------------------------------------------------------*/
/* Platform-Specific events Virtual Wires                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_VW_SUS_ACK                 ESPI_VW_DEF(0x40, 0, ESPI_VW_SM)    // Suspend Acknowledge (SUS_ACK#)
#define ESPI_VW_SUS_WARN                ESPI_VW_DEF(0x41, 0, ESPI_VW_MS)    // Suspend Warn (SUS_WARN#)
#define ESPI_VW_SUS_PWRDN_ACK           ESPI_VW_DEF(0x41, 1, ESPI_VW_MS)    // Suspend Power-Down Ack. (SUS_PWRDN_ACK)
#define ESPI_VW_SLP_A                   ESPI_VW_DEF(0x41, 3, ESPI_VW_MS)    // Sleep A# (SLP_A#)
#define ESPI_VW_SLP_LAN                 ESPI_VW_DEF(0x42, 0, ESPI_VW_MS)    // LAN Sub-System Sleep Control (SLP_LAN#)
#define ESPI_VW_SLP_WLAN                ESPI_VW_DEF(0x42, 1, ESPI_VW_MS)    // Wireless LAN Sub-System Sleep Control (SLP_WLAN#)
#define ESPI_VW_HOST_C10                ESPI_VW_DEF(0x47, 0, ESPI_VW_MS)    // Suspend Power-Down Ack (SUS_PWRDN_ACK)
#ifdef ESPI_CAPABILITY_AMD
#define ESPI_VW_FL_ACK                  ESPI_VW_DEF(0x43, 0, ESPI_VW_MS)    // AMD FLASH ACK
#define ESPI_VW_FL_REQ                  ESPI_VW_DEF(0x45, 0, ESPI_VW_SM)    // AMD FLASH REQUEST
#define ESPI_VW_FL_REQ_ATOMIC           ESPI_VW_DEF(0x45, 1, ESPI_VW_SM)    // AMD FLASH REQUEST ATOMIC
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* VW Number                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_WIRE_0 = (1 << 0),
    ESPI_WIRE_1 = (1 << 1),
    ESPI_WIRE_2 = (1 << 2),
    ESPI_WIRE_3 = (1 << 3),
} ESPI_VW_NUM_T;

#define ESPI_VW_WIRE_NUM        4

/*---------------------------------------------------------------------------------------------------------*/
/* VW type                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_VW_TYPE_INT_EV,            //Interrupt event
    ESPI_VW_TYPE_SYS_EV,            //System Event
    ESPI_VW_TYPE_PLT,               //Platform specific
#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
    ESPI_VW_TYPE_GPIO,              //General Purpose I/O Expander
#endif // ESPI_CAPABILITY_VW_NO_GPIO_SUPPORT
    ESPI_VW_TYPE_NUM
} ESPI_VW_TYPE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* VW Interrupt Window Select                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_VW_INT_WIN_00_15 = 0,          //(default)
    ESPI_VW_INT_WIN_16_31,
    ESPI_VW_INT_WIN_32_47,
    ESPI_VW_INT_WIN_48_63,
} ESPI_VW_INT_WIN_SEL;

/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Virtual Wire Indices Mapping                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_VW_GPIO_WIN_128_159 = 0,       //(default)
    ESPI_VW_GPIO_WIN_160_191,
    ESPI_VW_GPIO_WIN_192_223,
    ESPI_VW_GPIO_WIN_224_255,
    ESPI_VW_GPIO_WIN_NUM,
} ESPI_VW_GPIO_IND_MAP;

/*---------------------------------------------------------------------------------------------------------*/
/* Virtual Wires Errors                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_VW_ERR_INDEX_NOT_FOUND     DEFS_STATUS_CUSTOM_ERROR_00
#define ESPI_VW_ERR_WIRE_NOT_FOUND      DEFS_STATUS_CUSTOM_ERROR_01
#define ESPI_VW_ERR_INDEX_NOT_ENABLED   DEFS_STATUS_CUSTOM_ERROR_02
#define ESPI_VW_ERR_INVALID_WIRE        DEFS_STATUS_CUSTOM_ERROR_03
#ifdef ESPI_CAPABILITY_VW_HW_WIRE
#define ESPI_VW_ERR_HW_CTL_WIRE         DEFS_STATUS_CUSTOM_ERROR_04
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Virtual Wire user handler                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_VW_HANDLER_T)(ESPI_VW vw, UINT8 value);

/*---------------------------------------------------------------------------------------------------------*/
/* Hardware-/Software-Controlled Wire                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_VW_WIRE_CTRL_SW = 0,
    ESPI_VW_WIRE_CTRL_HW
} ESPI_VW_WIRE_CTRL_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Polarity of Wire (asserted signal level)                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_VW_WIRE_POL_ACTIVE_LOW = 0,
    ESPI_VW_WIRE_POL_ACTIVE_HIGH
} ESPI_VW_WIRE_POL_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Trigger Type of Wire                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_VW_WIRE_TRIGGER_LEVEL = 0,
    ESPI_VW_WIRE_TRIGGER_EDGE
} ESPI_VW_WIRE_TRIGGER_T;

/*---------------------------------------------------------------------------------------------------------*/
/*                                                  FLASH                                                  */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Flash user events                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_REQ_READ_MANUAL,
    ESPI_FLASH_REQ_READ_AUTO,
    ESPI_FLASH_REQ_READ_DMA,
    ESPI_FLASH_REQ_WRITE,
    ESPI_FLASH_REQ_ERASE,
} ESPI_FLASH_REQ_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash user events                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_REQ_ACC_READ,
    ESPI_FLASH_REQ_ACC_WRITE,
    ESPI_FLASH_REQ_ACC_ERASE,
} ESPI_FLASH_REQ_ACC_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash Access Channel Mode                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_ACCESS_MASTER_ATTCH,     //(default)
    ESPI_FLASH_ACCESS_SLAVE_ATTCH,
} ESPI_FLASH_ACCESS_MODE;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash read mode                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_READ_MODE_MANUAL,        //(default)
    ESPI_FLASH_READ_MODE_AUTO,
} ESPI_FLASH_READ_MODE;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash Access Channel Block Erase Size                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_BLOCK_ERASE_4KB = 1,     //(default)
    ESPI_FLASH_BLOCK_ERASE_64KB,
    ESPI_FLASH_BLOCK_ERASE_4KB_AND_64KB,
    ESPI_FLASH_BLOCK_ERASE_128KB,
    ESPI_FLASH_BLOCK_ERASE_256KB,
    ESPI_FLASH_BLOCK_ERASE_NUM,
} ESPI_FLASH_BLOCK_ERASE_SIZE;

#if 0
/*---------------------------------------------------------------------------------------------------------*/
/* Flash user events                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_EVENT_READ_DONE_SUCCESS,
    ESPI_FLASH_EVENT_READ_ERROR,
} ESPI_FLASH_EVENT_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash Access Channel Maximum Payload Size                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_MAX_PAYLOAD_64B,         //(default)
    ESPI_FLASH_MAX_PAYLOAD_128B,
    ESPI_FLASH_MAX_PAYLOAD_256B,
} ESPI_FLASH_MAX_PAYLOAD_SIZE;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash Access Channel Maximum Read Request Size                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_MAX_READ_64B = 1,        //(default)
    ESPI_FLASH_MAX_READ_128B,
    ESPI_FLASH_MAX_READ_256B,
    ESPI_FLASH_MAX_READ_512B,
    ESPI_FLASH_MAX_READ_1024B,
    ESPI_FLASH_MAX_READ_2048B,
    ESPI_FLASH_MAX_READ_4096B,
} ESPI_FLASH_MAX_READ_SIZE;
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DMA Request Threshold                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_DMA_THRESHOLD_DISABLE = 0,//(default)
    ESPI_FLASH_DMA_THRESHOLD_4B      = 2,
    ESPI_FLASH_DMA_THRESHOLD_16B     = 3,
} ESPI_FLASH_DMA_THRESHOLD_SIZE;

#ifdef ESPI_CAPABILITY_SAF
/*---------------------------------------------------------------------------------------------------------*/
/* Flash Access Channel Maximum Read Request Size Supported                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_SAF_MAX_READ_REQ_64B            = 0,
    ESPI_FLASH_SAF_MAX_READ_REQ_64B_DEF        = 1, //(default)
    ESPI_FLASH_SAF_MAX_READ_REQ_128B           = 2,
    ESPI_FLASH_SAF_MAX_READ_REQ_256B           = 3,
    ESPI_FLASH_SAF_MAX_READ_REQ_512B           = 4,
    ESPI_FLASH_SAF_MAX_READ_REQ_1024B          = 5,
    ESPI_FLASH_SAF_MAX_READ_REQ_2048B          = 6,
    ESPI_FLASH_SAF_MAX_READ_REQ_4096B          = 7,
} ESPI_FLASH_SAF_MAX_READ_REQ;

/*---------------------------------------------------------------------------------------------------------*/
/* Target Flash Erase Block Size For Master’s Regions                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_1KB        = 0,
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_2KB        = 1,
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_4KB        = 2, //(default)
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_8KB        = 3,
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_16KB       = 4,
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_32KB       = 5,
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_64KB       = 6,
    ESPI_FLASH_SAF_ERASE_BLOCK_SIZE_128KB      = 7,
} ESPI_FLASH_SAF_ERASE_BLOCK_SIZE;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash Sharing Capability Support                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_FLASH_SHARING_CAP_SUPP_MAF_DEF        = 0, //(default)
    ESPI_FLASH_SHARING_CAP_SUPP_MAF            = 1,
    ESPI_FLASH_SHARING_CAP_SUPP_SAF            = 2,
    ESPI_FLASH_SHARING_CAP_SUPP_SAF_AND_MAF    = 3,
} ESPI_FLASH_SHARING_CAP_SUPP;

/*---------------------------------------------------------------------------------------------------------*/
/* MAX Target Replay Protected Monotonic Counters(RPMC) Supported                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_SAF_MAX_TAR_RPMC_SUPP        64

#endif // ESPI_CAPABILITY_SAF

/*---------------------------------------------------------------------------------------------------------*/
/* FLASH user handler                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_FLASH_HANDLER)(void);//(ESPI_FLASH_EVENT_T event);


/*---------------------------------------------------------------------------------------------------------*/
/*                                                   OOB                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Channel Maximum Payload Size                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_MAX_PAYLOAD_64B,           //(default)
    ESPI_OOB_MAX_PAYLOAD_128B,
    ESPI_OOB_MAX_PAYLOAD_256B,
} ESPI_OOB_MAX_PAYLOAD_SIZE;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Hardware Completion Code                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_PECI_CC_NONE                                                        = 0x00,
    ESPI_OOB_PECI_CC_COMMAND_PASSED_DATA_VALID                                   = 0x40,
    ESPI_OOB_PECI_CC_RESPONSE_TIMEOUT_PROCESSOR_NOT_ABLE_TO_GENERATE_RESPONSE    = 0x80,
    ESPI_OOB_PECI_CC_RESPONSE_TIMEOUT_PROCESSOR_NOT_ABLE_TO_ALLOCATE_RESOURCES   = 0x81,
    ESPI_OOB_PECI_CC_RESURCES_IN_LOW_POWER_MODE                                  = 0x82,
    ESPI_OOB_PECI_CC_UNKNOWN_INVALID_ILLEGAL_REQUEST                             = 0x90,
    ESPI_OOB_PECI_CC_HW_FW_ASSOC_LOGIC_ERROR                                     = 0x91
} ESPI_OOB_PECI_CC_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Data Size                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_PECI_DATA_SIZE_NONE     = 0x00,
    ESPI_OOB_PECI_DATA_SIZE_BYTE     = 0x01,
    ESPI_OOB_PECI_DATA_SIZE_WORD     = 0x02,
    ESPI_OOB_PECI_DATA_SIZE_DWORD    = 0x04,
    ESPI_OOB_PECI_DATA_SIZE_QWORD    = 0x08
} ESPI_OOB_PECI_DATA_SIZE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Transaction Completion Code                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_PECI_TRANS_DONE_OK          = 0x02,     /* DO NOT CHANGE ENUM NUMBERS */
    ESPI_OOB_PECI_TRANS_DONE_AVL_ERR     = 0x04,     /* DO NOT CHANGE ENUM NUMBERS */
    ESPI_OOB_PECI_TRANS_DONE_CRC_ERR     = 0x08,     /* DO NOT CHANGE ENUM NUMBERS */
    ESPI_OOB_PECI_TRANS_DONE_ABORT_ERR   = 0x10,     /* DO NOT CHANGE ENUM NUMBERS */
    ESPI_OOB_PECI_TRANS_DONE_ABORT_CTN   = 0x20,     /* DO NOT CHANGE ENUM NUMBERS */
    ESPI_OOB_PECI_TRANS_DONE_CC_ERROR    = 0x40      /* DO NOT CHANGE ENUM NUMBERS */
} ESPI_OOB_PECI_TRANS_DONE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB PECI Command Code                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_PECI_COMMAND_PING               = 0x00,
    ESPI_OOB_PECI_COMMAND_GET_DIB            = 0xF7,
    ESPI_OOB_PECI_COMMAND_GET_TEMP           = 0x01,
    ESPI_OOB_PECI_COMMAND_RD_PKG_CFG         = 0xA1,
    ESPI_OOB_PECI_COMMAND_WR_PKG_CFG         = 0xA5,
    ESPI_OOB_PECI_COMMAND_RD_IAMSR           = 0xB1,
    ESPI_OOB_PECI_COMMAND_WR_IAMSR           = 0xB5,
    ESPI_OOB_PECI_COMMAND_RD_PCI_CFG         = 0x61,
    ESPI_OOB_PECI_COMMAND_WR_PCI_CFG         = 0x65,
    ESPI_OOB_PECI_COMMAND_RD_PCI_CFG_LOCAL   = 0xE1,
    ESPI_OOB_PECI_COMMAND_WR_PCI_CFG_LOCAL   = 0xE5,
    ESPI_OOB_PECI_COMMAND_NONE               = 0xFF
} ESPI_OOB_PECI_COMMAND_T;

/*---------------------------------------------------------------------------------------------------------*/
/* OOB USB Command Code                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_USB_USAGE_PORT_CONNECT               = 0x00,
    ESPI_OOB_USB_USAGE_PORT_DISCONNECT            = 0x01,
} ESPI_OOB_USB_USAGE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PECI Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_OOB_PECI_CALLBACK_T)(ESPI_OOB_PECI_COMMAND_T command, ESPI_OOB_PECI_DATA_SIZE_T data_size,
              ESPI_OOB_PECI_TRANS_DONE_T sts, UINT32 data_low, UINT32 data_high);

/*---------------------------------------------------------------------------------------------------------*/
/* USB Callback function                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_OOB_USB_CALLBACK_T)(ESPI_OOB_USB_USAGE_T usage, DEFS_STATUS status);

/*---------------------------------------------------------------------------------------------------------*/
/* OOB CRASHLOG Commands                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_CRASHLOG_CMD_GET_PCH           = 0x20,
    ESPI_OOB_CRASHLOG_CMD_GET_CPU           = 0x21,
    ESPI_OOB_CRASHLOG_CMD_GET_PCH_CPU       = 0x22,
    ESPI_OOB_CRASHLOG_CMD_GET_1K_CORE       = 0x23,
    ESPI_OOB_CRASHLOG_CMD_GET_1K_CRITICAL   = 0x24
} ESPI_OOB_CRASHLOG_CMD_T;

/*---------------------------------------------------------------------------------------------------------*/
/* CRASHLOG Callback function                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_OOB_CRASHLOG_CALLBACK_T)(ESPI_OOB_CRASHLOG_CMD_T cmd, DEFS_STATUS status);

/*---------------------------------------------------------------------------------------------------------*/
/* OOB HW Commands                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    ESPI_OOB_HW_CMD_GET_PCH_TEMP               = 0x01,
    ESPI_OOB_HW_CMD_GET_PCH_RTC_TIME           = 0x02,
} ESPI_OOB_HW_CMD_T;

/*---------------------------------------------------------------------------------------------------------*/
/* HW Callback function                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*ESPI_OOB_HW_CALLBACK_T)(ESPI_OOB_HW_CMD_T cmd, DEFS_STATUS status);

/*---------------------------------------------------------------------------------------------------------*/
/* OOB Channel SMBus Command Read Size                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOB_PCH_TEMP_SIZE                  0x01
#define ESPI_OOB_PCH_RTC_SIZE                   0x08
#define ESPI_OOB_CRASH_LOG_SIZE                 0x40

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*                                               GENERAL API                                               */
/*---------------------------------------------------------------------------------------------------------*/

void                ESPI_Init                       (ESPI_INT_HANDLER handler);
void                ESPI_Config                     (ESPI_IO_MODE ioMode, ESPI_MAX_FREQ maxFreq, ESPI_RST_OUT resetOutput);

ESPI_OP_FREQ        ESPI_GetOpFreq                  (void);
ESPI_IO_MODE_SELECT ESPI_GetIoModeSelect            (void);
ESPI_ALERT_MODE     ESPI_GetAlertMode               (void);
BOOLEAN             ESPI_GetCrcCheckEnable          (void);

void                ESPI_ChannelSlaveSupport        (ESPI_CHANNEL_TYPE_T channel, BOOLEAN enable);
void                ESPI_ChannelSlaveEnable         (ESPI_CHANNEL_TYPE_T channel, BOOLEAN enable);
BOOLEAN             ESPI_ChannelMasterEnable        (ESPI_CHANNEL_TYPE_T channel);

void                ESPI_IntEnable                  (ESPI_CHANNEL_TYPE_T channel, BOOLEAN enable);
void                ESPI_WakeUpEnable               (ESPI_CHANNEL_TYPE_T channel, BOOLEAN enable);
void                ESPI_IntHandler                 (void);

UINT32              ESPI_GetError                   (void);
UINT8               ESPI_GetConfigUpdateEnable      (void);
UINT32              ESPI_GetConfigUpdate            (void);

#ifdef ESPI_CAPABILITY_ESPI_RST_LEVEL_INDICATION
BOOLEAN             ESPI_InReset                    (void);
#endif

#ifdef ESPI_CAPABILITY_AUTO_HANDSHAKE
void                ESPI_ConfigAutoHandshake        (UINT32 AutoHsCfg);
#endif

#ifdef ESPI_CAPABILITY_HOST_STATUS
BOOLEAN             ESPI_IsReadyForHostReset        (void);
#endif

void                ESPI_PrintRegs                  (void);
void                ESPI_PrintVersion               (void);


/*---------------------------------------------------------------------------------------------------------*/
/*                                             PC CHANNEL API                                              */
/*---------------------------------------------------------------------------------------------------------*/

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
DEFS_STATUS         ESPI_PC_BM_Init                 (ESPI_PC_BM_MSG_HANDLER hostToSlaveMsgHandler);
void                ESPI_PC_BM_Config               (BOOLEAN sysRAMis64bAddr);
void                ESPI_PC_BM_RegisterCallback     (ESPI_INT_HANDLER handler);
DEFS_STATUS         ESPI_PC_BM_SendReq              (ESPI_PC_BM_REQ_T reqType, UINT64 offset, UINT32 size,
                                                     BOOLEAN pollingMode, UINT32* buffer, BOOLEAN strpHdr);
DEFS_STATUS         ESPI_PC_BM_GetStatus            (void);
DEFS_STATUS         ESPI_PC_BM_SetLTRMsgHdr         (BOOLEAN req, ESPI_PC_BM_LTR_LATENCY_SCALE_T latencyScale,
                                                     UINT16 latencyValue, UINT32* msgHdrPtr);
#ifdef ESPI_PC_BM_SELF_TEST
void                ESPI_PC_BM_SelfTestMsgHandler   (UINT8 msgCode, UINT8* msgBuff, UINT32 msgLen);
void                ESPI_PC_BM_SelfTestIntHandler   (ESPI_INT_T intType);
void                ESPI_PC_BM_SelfTestInit         (void);
DEFS_STATUS         ESPI_PC_BM_SelfTest             (ESPI_PC_BM_REQ_T req, UINT64 offset, UINT32 size, BOOLEAN polling,
                                                     UINT32* readBuffer, UINT32* writeBuffer, BOOLEAN strpHdr, BOOLEAN randomData);
#endif
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

/*---------------------------------------------------------------------------------------------------------*/
/*                                             VW CHANNEL API                                              */
/*---------------------------------------------------------------------------------------------------------*/
void                ESPI_VW_Init                    (ESPI_VW_HANDLER_T vwHandler);
void                ESPI_VW_Config                  (ESPI_VW_INT_WIN_SEL intWinSel, ESPI_VW_GPIO_IND_MAP gpioIndMap, BOOLEAN irqDis);

DEFS_STATUS         ESPI_VW_ConfigIndexOut          (UINT8 index, BOOLEAN indexEn, BOOLEAN pltrstEn, BOOLEAN cdrstEn);
DEFS_STATUS         ESPI_VW_ConfigIndexIn           (UINT8 index, BOOLEAN indexEn, BOOLEAN pltrstEn, BOOLEAN espirstEn,
                                                     BOOLEAN intEn, BOOLEAN wakeupEn);
DEFS_STATUS         ESPI_VW_ConfigWire              (ESPI_VW vw, BOOLEAN valid, ESPI_VW_WIRE_CTRL_T control,
                                                     ESPI_VW_WIRE_TRIGGER_T trigger, ESPI_VW_WIRE_POL_T polarity);
DEFS_STATUS         ESPI_VW_SetWire                 (ESPI_VW vw, UINT value);
DEFS_STATUS         ESPI_VW_GetWire                 (ESPI_VW vw, UINT *value);
DEFS_STATUS         ESPI_VW_WireIsRead              (ESPI_VW vw, BOOLEAN *value);

#ifndef ESPI_CAPABILITY_VW_FLOATING_EVENTS
DEFS_STATUS         ESPI_VW_SetFloating             (ESPI_VW vw, UINT value);
DEFS_STATUS         ESPI_VW_FloatingIsRead          (ESPI_VW vw, BOOLEAN *value);
#endif
DEFS_STATUS         ESPI_VW_SendBootLoad            (BOOLEAN status);


/*---------------------------------------------------------------------------------------------------------*/
/*                                            FLASH CHANNEL API                                            */
/*---------------------------------------------------------------------------------------------------------*/

void                ESPI_FLASH_Config               (ESPI_FLASH_ACCESS_MODE flashAccess, UINT32 writePageSize);
void                ESPI_FLASH_RegisterCallback     (ESPI_INT_HANDLER handler);

DEFS_STATUS         ESPI_FLASH_SendReq              (ESPI_FLASH_REQ_T reqType, UINT32 offset, UINT32 size,
                                                     BOOLEAN pollingMode, UINT32* buffer, BOOLEAN strpHdr);

void                ESPI_FLASH_ResetReqTrans        (void);
DEFS_STATUS         ESPI_FLASH_GetStatus            (void);

#ifdef ESPI_CAPABILITY_SAF
DEFS_STATUS         ESPI_FLASH_SAF_Config           (ESPI_FLASH_SAF_MAX_READ_REQ maxReadSizeSupp,
                                                     ESPI_FLASH_SHARING_CAP_SUPP flashSharingCapSupp,
                                                     ESPI_FLASH_SAF_ERASE_BLOCK_SIZE eraseBlockSize,
                                                     UINT8 targetRPMCsupp);
DEFS_STATUS         ESPI_FLASH_SAF_SetRWprotect     (UINT8 index, UINT16 baseAddr, UINT16 highAddr, BOOLEAN readProt,
                                                     BOOLEAN writeProt, UINT16 readTagOvr, UINT16 writeTagOvr);
DEFS_STATUS         ESPI_FLASH_SAF_ClearRWprotect   (UINT8 index);
BOOLEAN             ESPI_FLASH_SAF_IsPendingReq     (void);
void                ESPI_FLASH_SAF_HandleReq        (void);
DEFS_STATUS         ESPI_FLASH_SAF_GetStatus        (UINT8* status);
BOOLEAN             ESPI_FLASH_SAF_IsPendingRes     (void);
DEFS_STATUS         ESPI_FLASH_SAF_SendRes          (void);
#endif // ESPI_CAPABILITY_SAF

/*---------------------------------------------------------------------------------------------------------*/
/*                                             OOB CHANNEL API                                             */
/*---------------------------------------------------------------------------------------------------------*/

DEFS_STATUS         ESPI_OOB_ReadPchTemp            (UINT8* temp);
DEFS_STATUS         ESPI_OOB_ReadPchRtc             (UINT8* rtc);

DEFS_STATUS         ESPI_OOB_ReadCrashLog           (UINT8* crashLog, ESPI_OOB_CRASHLOG_CMD_T crashLogCmd);

void                ESPI_OOB_PECI_Init              (ESPI_OOB_PECI_CALLBACK_T callback, UINT32 peci_freq);
void                ESPI_OOB_PECI_SetAddress        (UINT8 client_address);
DEFS_STATUS         ESPI_OOB_PECI_Trans             (UINT8 wr_length, UINT8 rd_length, ESPI_OOB_PECI_COMMAND_T cmd_code, UINT32 data_0, UINT32 data_1);
DEFS_STATUS         ESPI_OOB_PECI_Ping              (void);
DEFS_STATUS         ESPI_OOB_PECI_GetDIB            (void);
DEFS_STATUS         ESPI_OOB_PECI_GetTemp           (void);
DEFS_STATUS         ESPI_OOB_PECI_RdPkgConfig       (ESPI_OOB_PECI_DATA_SIZE_T read_data_size, UINT8 host_id, UINT8 index, UINT16 parameter);
DEFS_STATUS         ESPI_OOB_PECI_WrPkgConfig       (ESPI_OOB_PECI_DATA_SIZE_T write_data_size, UINT8 host_id, UINT8 index, UINT16 parameter, UINT32 data);
DEFS_STATUS         ESPI_OOB_PECI_RdIAMSR           (ESPI_OOB_PECI_DATA_SIZE_T read_data_size, UINT8 host_id, UINT8 processor_id, UINT16 msr_address);
DEFS_STATUS         ESPI_OOB_PECI_RdPCIConfig       (ESPI_OOB_PECI_DATA_SIZE_T read_data_size, UINT8 host_id, UINT32 pci_config_address);
DEFS_STATUS         ESPI_OOB_PECI_RdPCIConfigLocal  (ESPI_OOB_PECI_DATA_SIZE_T read_data_size, UINT8 host_id, UINT32 pci_config_address);
DEFS_STATUS         ESPI_OOB_PECI_WrPCIConfigLocal  (ESPI_OOB_PECI_DATA_SIZE_T read_data_size, UINT8 host_id, UINT32 pci_config_address, UINT32 data);
ESPI_OOB_PECI_CC_T  ESPI_OOB_PECI_GetCompletionCode (void);

void                ESPI_OOB_USB_Init               (ESPI_OOB_USB_CALLBACK_T callback);
DEFS_STATUS         ESPI_OOB_USB_Connect            (UINT8 usb3PN, UINT8 usb2PN, BOOLEAN cableOrientation, BOOLEAN ufp);
DEFS_STATUS         ESPI_OOB_USB_Disconnect         (UINT8 usb3PN, UINT8 usb2PN);

void                ESPI_OOB_CRASHLOG_Init          (ESPI_OOB_CRASHLOG_CALLBACK_T callback);

void                ESPI_OOB_HW_Init                (ESPI_OOB_HW_CALLBACK_T callback);


#endif  /* _ESPI_IF_H */

