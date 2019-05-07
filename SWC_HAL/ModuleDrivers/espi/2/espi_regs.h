/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   espi_regs.h                                                              */
/* This file contains Enhanced Serial Peripheral Interface (eSPI) module registers */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _ESPI_REGS_H
#define _ESPI_REGS_H

#include __CHIP_H_FROM_DRV()

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              eSPI Registers                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*                                             Common Registers                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_ESPIID             (ESPI_BASE_ADDR + 0x000), ESPI_ACCESS, 32
#define ESPI_ESPICFG            (ESPI_BASE_ADDR + 0x004), ESPI_ACCESS, 32
#define ESPI_ESPISTS            (ESPI_BASE_ADDR + 0x008), ESPI_ACCESS, 32
#define ESPI_ESPIIE             (ESPI_BASE_ADDR + 0x00C), ESPI_ACCESS, 32
#define ESPI_ESPIWE             (ESPI_BASE_ADDR + 0x010), ESPI_ACCESS, 32
#define ESPI_VWREGIDX           (ESPI_BASE_ADDR + 0x014), ESPI_ACCESS, 32
#define ESPI_VWREGDATA          (ESPI_BASE_ADDR + 0x018), ESPI_ACCESS, 32
#define ESPI_OOBRXRDHEAD        (ESPI_BASE_ADDR + 0x01C), ESPI_ACCESS, 32
#define ESPI_OOBTXWRHEAD        (ESPI_BASE_ADDR + 0x020), ESPI_ACCESS, 32
#define ESPI_FLASHRXRDHEAD      (ESPI_BASE_ADDR + 0x028), ESPI_ACCESS, 32
#define ESPI_FLASHTXWRHEAD      (ESPI_BASE_ADDR + 0x02C), ESPI_ACCESS, 32
#define ESPI_FLASHCRC           (ESPI_BASE_ADDR + 0x030), ESPI_ACCESS, 32
#define ESPI_FLASHCFG           (ESPI_BASE_ADDR + 0x034), ESPI_ACCESS, 32
#define ESPI_FLASHCTL           (ESPI_BASE_ADDR + 0x038), ESPI_ACCESS, 32
#define ESPI_ESPIERR            (ESPI_BASE_ADDR + 0x03C), ESPI_ACCESS, 32
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#define ESPI_PBMRXRDHEAD        (ESPI_BASE_ADDR + 0x040), ESPI_ACCESS, 32
#define ESPI_PBMTXWRHEAD        (ESPI_BASE_ADDR + 0x044), ESPI_ACCESS, 32
#define ESPI_PERCFG             (ESPI_BASE_ADDR + 0x048), ESPI_ACCESS, 32
#define ESPI_PERCTL             (ESPI_BASE_ADDR + 0x04C), ESPI_ACCESS, 32
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#ifdef ESPI_CAPABILITY_HOST_STATUS
#define ESPI_STATUS_IMG         (ESPI_BASE_ADDR + 0x050), ESPI_ACCESS, 16
#define ESPI_ESPI_TEN           (ESPI_BASE_ADDR + 0x0F0), ESPI_ACCESS, 8
#define ESPI_ESPI_ENG           (ESPI_BASE_ADDR + 0x0F1), ESPI_ACCESS, 8
#endif //ESPI_CAPABILITY_HOST_STATUS
#ifdef ESPI_CAPABILITY_AUTO_HANDSHAKE
#define ESPI_ESPIHINDP          (ESPI_BASE_ADDR + 0x080), ESPI_ACCESS, 32
#endif // ESPI_CAPABILITY_AUTO_HANDSHAKE

/*---------------------------------------------------------------------------------------------------------*/
/*                                      Virtual Wire Channel Registers                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_VWEVSM(n)          (ESPI_BASE_ADDR + 0x100 + (4*(n))), ESPI_ACCESS, 32
#define ESPI_VWEVMS(n)          (ESPI_BASE_ADDR + 0x140 + (4*(n))), ESPI_ACCESS, 32
#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
#define ESPI_VWGPSM(n)          (ESPI_BASE_ADDR + 0x180 + (4*(n))), ESPI_ACCESS, 32
#define ESPI_VWGPMS(n)          (ESPI_BASE_ADDR + 0x1C0 + (4*(n))), ESPI_ACCESS, 32
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT
#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
#define ESPI_VWPING             (ESPI_BASE_ADDR + 0x2F8),           ESPI_ACCESS, 32
#endif //ESPI_CAPABILITY_VW_FLOATING_EVENTS
#define ESPI_VWCTL              (ESPI_BASE_ADDR + 0x2FC),           ESPI_ACCESS, 32

/*---------------------------------------------------------------------------------------------------------*/
/*                                          OOB Channel Registers                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOBRXBUF(n)        (ESPI_BASE_ADDR + 0x300 + (4*(n))), ESPI_ACCESS, 32
#define ESPI_OOBTXBUF(n)        (ESPI_BASE_ADDR + 0x380 + (4*(n))), ESPI_ACCESS, 32
#define ESPI_OOBCTL             (ESPI_BASE_ADDR + 0x3FC),           ESPI_ACCESS, 32

/*---------------------------------------------------------------------------------------------------------*/
/*                                      Flash Access Channel Registers                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASHRXBUF(n)      (ESPI_BASE_ADDR + 0x400 + (4*(n))), ESPI_ACCESS, 32
#define ESPI_FLASHTXBUF(n)      (ESPI_BASE_ADDR + 0x480 + (4*(n))), ESPI_ACCESS, 32
#define ESPI_FLASHBASE          (ESPI_BASE_ADDR + 0x4F0),           ESPI_ACCESS, 32

/*---------------------------------------------------------------------------------------------------------*/
/*                                        Peripheral Channel buffers                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#define ESPI_PBMRXBUF(n)        (ESPI_BASE_ADDR + 0x500 + (4*(n))), ESPI_ACCESS, 32 // n = 0 - 17
#define ESPI_PBMTXBUF(n)        (ESPI_BASE_ADDR + 0x580 + (4*(n))), ESPI_ACCESS, 32 // n = 0 - 18
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

/*---------------------------------------------------------------------------------------------------------*/
/*                                  Slave Attached Flash Access Registers                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_SAF
#define ESPI_FLASH_SAF_PROT_MEM_NUM     16
#define ESPI_FLASH_SAF_TAG_RANGE_NUM    8
#define ESPI_FLASH_SAF_BUFF_NUM         8
#define ESPI_FLASH_PRTR_BADDRn(n)       (ESPI_BASE_ADDR + 0x600 + (4*(n))), ESPI_ACCESS, 32 // n = 0 - 15
#define ESPI_FLASH_PRTR_HADDRn(n)       (ESPI_BASE_ADDR + 0x640 + (4*(n))), ESPI_ACCESS, 32 // n = 0 - 15
#define ESPI_FLASH_FLASH_RGN_TAG_OVRn(n)(ESPI_BASE_ADDR + 0x680 + (4*(n))), ESPI_ACCESS, 32 // n = 0 - 7
#define ESPI_FLASHRDTXBUF(buf)(n)       (ESPI_BASE_ADDR + 0x700 + ((buf) * 0x40) + (4*(n))), ESPI_ACCESS, 32 // n = 0 - 15
#endif // ESPI_CAPABILITY_SAF

/*---------------------------------------------------------------------------------------------------------*/
/*                                                 Fields                                                  */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ESPIID fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIID_ID               0,  8
#define ESPIID_VERSION          8,  8

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ESPICFG fields                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPICFG_PCHANEN         0,  1
#define ESPICFG_VWCHANEN        1,  1
#define ESPICFG_OOBCHANEN       2,  1
#define ESPICFG_FLASHCHANEN     3,  1
#define ESPICFG_HPCHANEN        4,  1
#define ESPICFG_HVWCHANEN       5,  1
#define ESPICFG_HOOBCHANEN      6,  1
#define ESPICFG_HFLASHCHANEN    7,  1
#define ESPICFG_IOMODE          8,  2
#define ESPICFG_MAXFREQ         10, 3
#define ESPICFG_FLASHCHANMODE   16, 1
#define ESPICFG_OPFREQ          17, 3
#define ESPICFG_IOMODESEL       20, 2
#define ESPICFG_ALERTMODE       22, 1
#define ESPICFG_CRC_CHK_EN      23, 1
#define ESPICFG_PCCHN_SUPP      24, 1
#define ESPICFG_VWCHN_SUPP      25, 1
#define ESPICFG_OOBCHN_SUPP     26, 1
#define ESPICFG_FLASHCHN_SUPP   27, 1
#ifdef ESPI_CAPABILITY_RESET_OUTPUT
#define ESPICFG_RSTO            31, 1
#endif // ESPI_CAPABILITY_RESET_OUTPUT

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ESPISTS fields                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPISTS_IBRST           0,  1
#define ESPISTS_CFGUPD          1,  1
#define ESPISTS_BERR            2,  1
#define ESPISTS_OOBRX           3,  1
#define ESPISTS_FLASHRX         4,  1
#define ESPISTS_SFLASHRD        5,  1
#define ESPISTS_PERACC          6,  1
#define ESPISTS_DFRD            7,  1
#define ESPISTS_VWUPD           8,  1
#define ESPISTS_ESPIRST         9,  1
#define ESPISTS_PLTRST          10, 1
#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
#define ESPISTS_VW1             11, 1
#define ESPISTS_VW2             12, 1
#define ESPISTS_VW3             13, 1
#define ESPISTS_VW4             14, 1
#endif // ESPI_CAPABILITY_VW_FLOATING_EVENTS
#define ESPISTS_AMERR           15, 1
#define ESPISTS_AMDONE          16, 1
#ifdef ESPI_CAPABILITY_VW_WAKEUP
#define ESPISTS_VWUPDW          17, 1
#endif
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#define ESPISTS_BMTXDONE        19, 1
#define ESPISTS_PBMRX           20, 1
#define ESPISTS_PMSGRX          21, 1
#define ESPISTS_BMBURSTERR      22, 1
#define ESPISTS_BMBURSTDONE     23, 1
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#ifdef ESPI_CAPABILITY_ESPI_RST_LEVEL_INDICATION
#define ESPISTS_ESPIRST_LVL     24, 1
#endif // ESPI_CAPABILITY_ESPI_RST_LEVEL_INDICATION
#ifdef ESPI_CAPABILITY_SAF
#define ESPISTS_FLASHPRTERR     25, 1
#endif // ESPI_CAPABILITY_SAF


/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ESPIIE fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIIE_IBRSTIE          0,  1
#define ESPIIE_CFGUPDIE         1,  1
#define ESPIIE_BERRIE           2,  1
#define ESPIIE_OOBRXIE          3,  1
#define ESPIIE_FLASHRXIE        4,  1
#define ESPIIE_SFLASHRDIE       5,  1
#define ESPIIE_PERACCIE         6,  1
#define ESPIIE_DFRDIE           7,  1
#define ESPIIE_VWUPDIE          8,  1
#define ESPIIE_ESPIRSTIE        9,  1
#define ESPIIE_PLTRSTIE         10, 1
#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
#define ESPIIE_VW1IE            11, 1
#define ESPIIE_VW2IE            12, 1
#define ESPIIE_VW3IE            13, 1
#define ESPIIE_VW4IE            14, 1
#endif // ESPI_CAPABILITY_VW_FLOATING_EVENTS
#define ESPIIE_AMERRIE          15, 1
#define ESPIIE_AMDONEIE         16, 1
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#define ESPIIE_BMTXDONEIE       19, 1
#define ESPIIE_PBMRXIE          20, 1
#define ESPIIE_PMSGRXIE         21, 1
#define ESPIIE_BMBURSTERRIE     22, 1
#define ESPIIE_BMBURSTDONEIE    23, 1
#ifdef ESPI_CAPABILITY_SAF
#define ESPIIE_FLASHPRTERRIE    25, 1
#endif // ESPI_CAPABILITY_SAF
#define ESPIIE_ALL              0, 25
#else
#define ESPIIE_ALL              0, 17
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ESPIWE fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIWE_IBRSTWE              0,  1
#define ESPIWE_CFGUPDWE             1,  1
#define ESPIWE_BERRWE               2,  1
#define ESPIWE_OOBRXWE              3,  1
#define ESPIWE_FLASHRXWE            4,  1
#define ESPIWE_SFLASHRDWE           5,  1
#define ESPIWE_PERACCWE             6,  1
#define ESPIWE_DFRDWE               7,  1
#define ESPIWE_VWUPDWE              8,  1

#ifdef ESPI_CAPABILITY_ESPI_RST_WAKEUP_SUPPORT
#define ESPIWE_ESPIRSTWE            9,  1
#endif // ESPI_CAPABILITY_ESPI_RST_WAKEUP_SUPPORT

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#define ESPIWE_PBMRXWE              20,  1
#define ESPIWE_PMSGRXWE             21,  1
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#define ESPIWE_ALL                  0,   22
#elif defined (ESPI_CAPABILITY_ESPI_RST_WAKEUP_SUPPORT)
#define ESPIWE_ALL                  0, 10
#else
#define ESPIWE_ALL                  0,  9
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_VWREGIDX fields                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define VWREGIDX_VWREGIDX           2, 10

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_OOBRXBUF fields                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_OOBRXBUF_NUM           20

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_OOBCTL fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define OOBCTL_OOB_FREE             0,  1
#define OOBCTL_OOB_AVAIL            1,  1
#define OOBCTL_RSTBUFHEADS          2,  1
#define OOBCTL_OOBPLSIZE            10, 3

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_FLASHCFG fields                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef ESPI_CAPABILITY_SAF
#define FLASHCFG_FLASHREQSUP        0,  3
#endif // ESPI_CAPABILITY_SAF
#define FLASHCFG_FLASHBLERSSIZE     7,  3
#define FLASHCFG_FLASHPLSIZE        10, 3
#define FLASHCFG_FLASHREQSIZE       13, 3
#ifdef ESPI_CAPABILITY_SAF
#define FLASHCFG_FLASHCAPA          16, 2
#define FLASHCFG_TRGFLASHEBLKSIZE   18, 8
#define FLASHCFG_TRGRPMCSUPP        26, 6
#endif // ESPI_CAPABILITY_SAF

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_FLASHCTL fields                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define FLASHCTL_FLASH_ACC_NP_FREE  0,  1
#define FLASHCTL_FLASH_ACC_TX_AVAIL 1,  1
#define FLASHCTL_STRPHDR            2,  1
#define FLASHCTL_DMATHRESH          3,  2
#define FLASHCTL_AMTSIZE            5,  9
#define FLASHCTL_RSTBUFHEADS        13, 1
#define FLASHCTL_CRCEN              14, 1
#define FLASHCTL_CHKSUMSEL          15, 1
#define FLASHCTL_AMTEN              16, 1
#define FLASHCTL_AMTBFULL           17, 1
#ifdef ESPI_CAPABILITY_SAF
#define FLASHCTL_SAF_AUTO_READ      18, 1
#define FLASHCTL_SAF_PROT_LOCK      19, 1
#endif // ESPI_CAPABILITY_SAF


/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ESPIERR fields                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIERR_INVCMD              0,  1
#define ESPIERR_INVCYC              1,  1
#define ESPIERR_CRCERR              2,  1
#define ESPIERR_ABCOMP              3,  1
#define ESPIERR_PROTERR             4,  1
#define ESPIERR_BADSIZE             5,  1
#define ESPIERR_NPBADALN            6,  1
#define ESPIERR_PCBADALN            7,  1
#define ESPIERR_FLBADALN            8,  1
#define ESPIERR_UNCMD               9,  1
#define ESPIERR_EXTRACYC            10, 1
#define ESPIERR_VWERR               11, 1
#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#define ESPIERR_UNPBM               14, 1
#endif //ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
#ifdef ESPI_CAPABILITY_FLASH_UC_ERR
#define ESPIERR_UNFLASH             15, 1
#endif //ESPI_CAPABILITY_FLASH_UC_ERR

/*---------------------------------------------------------------------------------------------------------*/
/*                                      Peripheral Channel Registers                                       */
/*---------------------------------------------------------------------------------------------------------*/

#ifdef ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT
/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_PERCFG fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define PERCFG_BMMEN                2,  1
#define PERCFG_PERPLSIZE            10, 3
#define PERCFG_BMREQSIZE            13, 3

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_PERCTL fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define PERCTL_PER_PC_FREE          0,  1
#define PERCTL_BMSTRPHDR            2,  1
#define PERCTL_BMDMATHRESH          3,  2
#define PERCTL_BMBURSTSIZE          5,  8
#define PERCTL_RSTPBUFHEADS         13, 1
#define PERCTL_MEM64_ACCESS         14, 1
#define PERCTL_BMBRSTEN             16, 1
#define PERCTL_BMBURST_BFULL        17, 1
#define PERCTL_BM_NP_AVAIL          19, 1
#define PERCTL_BM_PC_AVAIL          20, 1
#define PERCTL_BM_MSG_AVAIL         21, 1
#define PERCTL_BMPKT_LEN            24, 8
#endif // ESPI_CAPABILITY_ESPI_PC_BM_SUPPORT

#ifdef ESPI_CAPABILITY_HOST_STATUS
/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_STATUS_IMG fields                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define STATUS_IMG_PC_FREE          0,  1
#define STATUS_IMG_NP_FREE          1,  1
#define STATUS_IMG_VWIRE_FREE       2,  1
#define STATUS_IMG_OOB_FREE         3,  1
#define STATUS_IMG_PC_AVAIL         4,  1
#define STATUS_IMG_NP_AVAIL         5,  1
#define STATUS_IMG_VWIRE_AVAIL      6,  1
#define STATUS_IMG_OOB_AVAIL        7,  1
#define STATUS_IMG_FLASH_C_FREE     8,  1
#ifdef ESPI_CAPABILITY_SAF
#define STATUS_IMG_FLASH_NP_FREE    9,  1
#define STATUS_IMG_FLASH_C_AVAIL    12, 1
#endif // ESPI_CAPABILITY_SAF
#define STATUS_IMG_FLASH_NP_AVAIL   13, 1

#define STATUS_IMG_RST_VAL          0x30F

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ENG fields                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_ENG_CHAN_FREE_EN       7,  1
#endif // ESPI_CAPABILITY_HOST_STATUS

#ifdef ESPI_CAPABILITY_AUTO_HANDSHAKE
/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_ESPIHINDP fields                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPIHINDP_AUTO_PCRDY        0,  1
#define ESPIHINDP_AUTO_VWCRDY       1,  1
#define ESPIHINDP_AUTO_OOBCRDY      2,  1
#define ESPIHINDP_AUTO_FCARDY       3,  1
#define ESPIHINDP_AUTO_SBLD         4,  1
#define ESPIHINDP_AUTO_HS1          8,  1
#define ESPIHINDP_HS1_DELAY         9,  2
#define ESPIHINDP_AUTO_HS2          12, 1
#define ESPIHINDP_HS2_DELAY         13, 2
#define ESPIHINDP_AUTO_HS3          16, 1
#define ESPIHINDP_HS3_DELAY         17, 2
#endif //  ESPI_CAPABILITY_AUTO_HANDSHAKE

/*---------------------------------------------------------------------------------------------------------*/
/*                                      Virtual Wire Channel Registers                                     */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_VWEVSM fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
//wires 0-3
#define VWEVSM_WIRE(n)              (n),1
#define VWEVSM_WIREn                0,  4
#define VWEVSM_WIRE_VALID(n)      (4+n),1
#define VWEVSM_WIRE_VALIDn          4,  4
#define VWEVSM_INDEX                8,  7
#define VWEVSM_INDEX_EN             15, 1
#define VWEVSM_DIRTY                16, 1
#define VWEVSM_ENPLTRST             17, 1
#define VWEVSM_ENCDRST              19, 1
#ifdef ESPI_CAPABILITY_VW_EDGE
//wires 0-3
#define VWEVSM_POL(n)        ((n) + 20),1
#define VWEVSM_POLn                 20, 4
#endif
#ifdef ESPI_CAPABILITY_VW_HW_WIRE
//wires 0-3
#define VWEVSM_HW_WIRE(n)    ((n) + 24),1
#define VWEVSM_HW_WIREn             24, 4
#endif // ESPI_CAPABILITY_VW_HW_WIRE
#ifdef ESPI_CAPABILITY_VW_EDGE
//wires 0-3
#define VWEVSM_EDGE(n)       ((n) + 28),1
#define VWEVSM_EDGEn                28, 4
#endif

#define ESPI_VWEVSM_NUM             10

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_VWEVMS fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
//wires 0-3
#define VWEVMS_WIRE(n)              (n),1
#define VWEVMS_WIRE_VALID(n)      (4+n),1
#define VWEVMS_INDEX                8,  7
#define VWEVMS_INDEX_EN             15, 1
#define VWEVMS_MODIFIED             16, 1
#define VWEVMS_ENPLTRST             17, 1
#define VWEVMS_IE                   18, 1
#define VWEVMS_ENESPIRST            19, 1
#ifdef ESPI_CAPABILITY_VW_WAKEUP
#define VWEVMS_WE                   20, 1
#endif
#define ESPI_VWEVMS_NUM             12

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_VWGPSM fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define VWGPSM_WIRE(n)             (n), 1
#define VWGPSM_WIRE_VALID(n)   ((n)+4), 1
#define VWGPSM_WIRE_VALIDn          4,  4
#define VWGPSM_INDEX_EN             15, 1
#define VWGPSM_DIRTY                16, 1
#define VWGPSM_ENPLTRST             17, 1

#define ESPI_VWGPSM_NUM             16

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_VWGPMS fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define VWGPMS_WIRE(n)             (n), 1
#define VWGPMS_INDEX_EN             15, 1
#define VWGPMS_MODIFIED             16, 1
#define VWGPMS_ENPLTRST             17, 1
#define VWGPMS_IE                   18, 1

#define ESPI_VWGPMS_NUM             16

#ifdef ESPI_CAPABILITY_VW_FLOATING_EVENTS
/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_VWPING fields                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define VWPING_DATA(n)            (n), 1
#define VWPING_VALID(n)       (4+(n)), 1
#define VWPING_INDEX                8, 8
#define VWPING_DIRTY               16, 1
#endif //ESPI_CAPABILITY_VW_FLOATING_EVENTS

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_VWCTL fields                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define VWCTL_INTWIN                0,  2
#ifdef ESPI_CAPABILITY_VW_GPIO_SUPPORT
#define VWCTL_GPVWMAP               2,  3
#endif // ESPI_CAPABILITY_VW_GPIO_SUPPORT
#ifdef ESPI_CAPABILITY_VW_MASTER_IRQ_UPD
#define VWCTL_IRQ_DIS               20, 1
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*                                      Flash Access Channel Registers                                     */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_FLASHRXBUF fields                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASHRXBUF_NUM                17

#ifdef ESPI_CAPABILITY_SAF
/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_FLASHBASE fields                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define FLASHBASE_FLBASE_LOW               0, 16
#define FLASHBASE_FLBASE_HIGH              16,11

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_FLASH_PRTR_BADDRn(n) fields                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_PRTR_BADDRn_BADDR       12,15
#define ESPI_FLASH_PRTR_BADDRn_FRGN_RPR    30, 1
#define ESPI_FLASH_PRTR_BADDRn_FRGN_WPR    31, 1

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_FLASH_PRTR_HADDRn(n) fields                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_PRTR_HADDRn_HADDR       12,15

/*---------------------------------------------------------------------------------------------------------*/
/* ESPI_FLASH_FLASH_RGN_TAG_OVRn(n) fields                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_WPR_TOVR       0 ,16
#define ESPI_FLASH_FLASH_RGN_TAG_OVRn_FRNG_RPR_TOVR       16,16

#endif // ESPI_CAPABILITY_SAF

#endif /* _ESPI_REGS_H */

