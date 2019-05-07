/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   mc_regs.h                                                                */
/*            This file contains registers definition of Memory Controler     */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _MC_REGS_H
#define _MC_REGS_H

#include __CHIP_H_FROM_DRV()


/**************************************************************************************************************************/
/*   Register SCL_START                                                                                                   */
/**************************************************************************************************************************/
#define  SCL_START                      (MCPHY_BASE_ADDR + 0x100) , MCPHY_ACCESS, 32        /* Offset: 100h */
#define  SCL_START_SAVE_RESTORE_DATA     31 , 1              /* 31 SAVE_RESTORE_DATA Used in the SCL routines and should not be set by the user  When set, the user did not allocate space in the DRAM for SCL or bit-leveling training data  Therefore during DSCL and/or dynamic bit-leveling, the user’s data will be saved and overwritten with training data to complete the process and then restored for normal operation */
#define  SCL_START_wrlvl                 30 , 1              /* 30 wrlvl  Used in the SCL routines and should not be set by the user                                                  */
#define  SCL_START_CONTINUOUS_RDS        29 , 1              /* 29 CONTINUOUS_RDS Used in SCL routines and should not be set by the user                                              */
#define  SCL_START_SET_DDR_SCL_GO_DONE   28 , 1              /* 28 SET_DDR_SCL_GO_DONE Used in SCL routines and should not be set by the user                                         */
#define  SCL_START_INCR_SCL              26 , 1              /* 26 INCR_SCL Set to enable the Dynamic SCL                                                                             */
#define  SCL_START_RD_ONLY               25 , 1              /* 25 RD_ONLY Used in SCL routines and should not be set by the user  When set, SCL will only perform read back  Bit is not used currently */
#define  SCL_START_wr_only               24 , 1              /* 24 wr_only  Used in SCL routines and should not be set by the user  When set, SCL will only write the SCL data to the SCL address location in DRAM */
#define  SCL_START_bit_lvl_mpr           23 , 1              /* 23 bit_lvl_mpr  Used in SCL routines and should not be set by the user  Set to run bit-leveling using the multi-purpose register */
#define  SCL_START_bit_lvl_norm          22 , 1              /* 22 bit_lvl_norm  Used in SCL routines and should not be set by the user  Set to run bit-leveling using the DRAM array */
#define  SCL_START_bit_lvl_from_saved    21 , 1              /* 21 bit_lvl_from_saved  Used in SCL routines and should not be set by the user  If set, dynamic bit-leveling will use previously saved bit-leveled values  Otherwise, each time dynamic bit-leveling is run, it will start bit-leveling from scratch (i e , will set all initial delay values for all bits to 0) */
#define  SCL_START_write_side_bit_lvl    20 , 1              /* 20 write_side_bit_lvl  Set this high in order to run write side bit-leveling at initialization from the DRAM  Set in conjunction with bit_lvl_norm  If this is not set, bit-leveling will not automatically apply any trim on the write side and any calibration there will have to be programmed */
#define  SCL_START_cycle_cnt             20 , 8             /* 27-20 cycle_cnt. Read-only value that indicates the number of delay elements required to delay the clock signal to align with the read DQS falling edge (after it has been delayed ¼ cycle by the analog or digital DLL).*/
#define  SCL_START_cuml_scl_rslt         0 , 3              /* 2-0 cuml_scl_rslt. Read-only value that gets set after SCL is run. It shows whether SCL passed or failed on each byte lane, 1 bit per byte lane.*/

/**************************************************************************************************************************/
/*   Register SCL_DATA_0                                                                                                  */
/**************************************************************************************************************************/
#define  SCL_DATA_0                     (MCPHY_BASE_ADDR + 0x104) , MCPHY_ACCESS, 32        /* Offset: 104h */
#define  SCL_DATA_0_scl_data             0 , 32             /* 31-0 scl_data. Used in the SCL routines and should not be set by the user.                                            */

/**************************************************************************************************************************/
/*   Register SCL_DATA_1                                                                                                  */
/**************************************************************************************************************************/
#define  SCL_DATA_1                     (MCPHY_BASE_ADDR + 0x108) , MCPHY_ACCESS, 32        /* Offset: 108h */
#define  SCL_DATA_1_scl_data             0 , 32             /* 31-0 scl_data. Used in the SCL routines and should not be set by the user.                                            */

/**************************************************************************************************************************/
/*   Register SCL_LATENCY                                                                                                 */
/**************************************************************************************************************************/
#define  SCL_LATENCY                    (MCPHY_BASE_ADDR + 0x10C) , MCPHY_ACCESS, 32        /* Offset: 10Ch */
#define  SCL_LATENCY_scl_calib_wait_latency  12 , 3             /* 14-12 scl_calib_wait_latency. Number of cycles to wait in the SCL state machine after updating the                    */
#define  SCL_LATENCY_dqs_gate_disabled   9 , 1               /* 9 dqs_gate_disabled  Set to disable DQS gating logic  Recommended value is 0                                          */
#define  SCL_LATENCY_ext_loopback        8 , 1               /* 8 ext_loopback  Set to put the PHY in external loopback mode                                                          */
#define  SCL_LATENCY_main_clk_dly        4 , 4              /* 7-4 main_clk_dly. Automatically programmed by SCL based on the delay for generating dfi_rddata_valid                  */
#define  SCL_LATENCY_capture_clk_dly     0 , 4              /* 3-0 capture_clk_dly. Used to specify the number of delay line steps that the SCL state machine should                 */

/**************************************************************************************************************************/
/*   Register SCL_RD_ADDR                                                                                                 */
/**************************************************************************************************************************/
#define  SCL_RD_ADDR                    (MCPHY_BASE_ADDR + 0x110) , MCPHY_ACCESS, 32        /* Offset: 110h */
#define  SCL_RD_ADDR_data_phy_sel        8 , 3              /* 10-8 data_phy_sel. SCL related.                                                                                       */
#define  SCL_RD_ADDR_rotate_ctr_strt_val  4 , 4              /* 7-4 rotate_ctr_strt_val. SCL related. Not user programmable.                                                          */
#define  SCL_RD_ADDR_scl_rd_addr         0 , 2              /* 1-0 scl_rd_addr. SCL related.                                                                                         */

/**************************************************************************************************************************/
/*   Register SCL_RD_DATA                                                                                                 */
/**************************************************************************************************************************/
#define  SCL_RD_DATA                    (MCPHY_BASE_ADDR + 0x114) , MCPHY_ACCESS, 32        /* Offset: 114h */
#define  SCL_RD_DATA_scl_rd_data         0 , 32             /* 31-0 scl_rd_data. SCL related.                                                                                        */

/**************************************************************************************************************************/
/*   Register SCL_CONFIG_1                                                                                                */
/**************************************************************************************************************************/
#define  SCL_CONFIG_1                   (MCPHY_BASE_ADDR + 0x118) , MCPHY_ACCESS, 32        /* Offset: 118h */
#define  SCL_CONFIG_1_ddr_odt_ctrl_wr    24 , 1              /* 24 ddr_odt_ctrl_wr  ODT signals that should be driven to the DDR2/3 SDRAM when running SCL  1 bit                     */
#define  SCL_CONFIG_1_ddr_odt_ctrl_rd    16 , 1              /* 16 ddr_odt_ctrl_rd  Not used                                                                                          */
#define  SCL_CONFIG_1_local_odt_ctrl     12 , 4             /* 15-12 local_odt_ctrl. When bit 0 is set, dynamic termination for PHYDDR2/3 pads is disabled. When bit 1 is            */
#define  SCL_CONFIG_1_additive_latency   8 , 4              /* 11-8 additive_latency. DRAM Additive Latency.                                                                         */
#define  SCL_CONFIG_1_rd_cas_latency     4 , 4              /* 7-4 rd_cas_latency. DRAM Read CAS Latency                                                                             */
#define  SCL_CONFIG_1_dly_dfi_phyupd_ack  3 , 1               /* 3 dly_dfi_phyupd_ack  Delay the dfi phyupdack from the controller if it does not support issuing                      */
#define  SCL_CONFIG_1_burst8             0 , 1               /* 0 burst8  Used by the PHY while running the SCL test to understand if the DDR is in burst of 4 mode (0)               */

/**************************************************************************************************************************/
/*   Register SCL_CONFIG_2                                                                                                */
/**************************************************************************************************************************/
#define  SCL_CONFIG_2                   (MCPHY_BASE_ADDR + 0x11C) , MCPHY_ACCESS, 32        /* Offset: 11Ch */
#define  SCL_CONFIG_2_swap_phase         31 , 1              /* 31 swap_phase  When the PHY sends DRAM commands during PHY training sequences, this bit decides                       */
#define  SCL_CONFIG_2_analog_dll_for_scl  28 , 1              /* 28 analog_dll_for_scl  Set this bit to 1 to use the analog delay line for delaying data_capture_clk                   */
#define  SCL_CONFIG_2_scl_step_size      25 , 3             /* 27-25 scl_step_size. Number of steps by which the delay should be incremented for the data_capture_clk                */
#define  SCL_CONFIG_2_dly_dfi_wrdata     24 , 1              /* 24 dly_dfi_wrdata  Set when PHY is operating in half_rate mode and write latency is either even and write             */
#define  SCL_CONFIG_2_scl_test_cs        0 , 2              /* 1-0 scl_test_cs. Used to control which chip select SCL will run on. 1 bit per chip select. Set to 1 to run SCL        */
#define  SCL_CONFIG_2_double_ref_dl      12 , 1              /* 12 double_ref_dl. Needed for DRAM of 1Gb      */



/**************************************************************************************************************************/
/*   Register SCL_CONFIG_3                                                                                                */
/**************************************************************************************************************************/
#define  SCL_CONFIG_3                   (MCPHY_BASE_ADDR + 0x16C) , MCPHY_ACCESS, 32        /* Offset: 16C */

/**************************************************************************************************************************/
/*   Register PHY_PAD_CTRL                                                                                                */
/**************************************************************************************************************************/
#define  PHY_PAD_CTRL                   (MCPHY_BASE_ADDR + 0x120) , MCPHY_ACCESS, 32        /* Offset: 120h */
#define  PHY_PAD_CTRL_reten_en_state     31 , 1              /* 31 reten_en_state  When the PHY includes a retention pad, this bit indicates the state of the retention               */
#define  PHY_PAD_CTRL_preamble_dly       29 , 2             /* 30-29 preamble_dly. Controls the length of the preamble for write.                                                    */
#define  PHY_PAD_CTRL_receiver_en        28 , 1              /* 28 receiver_en  Set to 1 to allow pad receivers on bidirectional I/Os to be turned on                                 */
#define  PHY_PAD_CTRL_clk_drive          20 , 1              /* 20 clk_drive  I/O pad drive strength control for clock pads                                                           */
#define  PHY_PAD_CTRL_adrctrl_drive      16 , 1              /* 16 adrctrl_drive  I/O pad drive strength control for address and control pads                                         */
#define  PHY_PAD_CTRL_half_rate          14 , 1              /* 14 half_rate  When programmable half-rate option is present in the PHY, this bit should be set to select              */
#define  PHY_PAD_CTRL_reduce_cmd_latency  11 , 1              /* 11 reduce_cmd_latency  Not used                                                                                       */
#define  PHY_PAD_CTRL_no_external_dll    9 , 1               /* 9 no_external_dll  If 1, it indicates that the PHY should use its digital DLL to center the dqs signal on the         */
#define  PHY_PAD_CTRL_extra_oen_clk      8 , 1               /* 8 extra_oen_clk  Set to drive pad output enables for an extra cycle after a write burst if necessary                  */
#define  PHY_PAD_CTRL_dq_dqs_drive       4 , 1               /* 4 dq_dqs_drive  Pad drive strength control for dq (MDx) and dqs (nDQSx and DQSx) pads when pad                        */
#define  PHY_PAD_CTRL_odt_sel            0 , 3              /* 2-0 odt_sel. Controls dynamic termination value.                                                                      */

/**************************************************************************************************************************/
/*   Register PHY_DLL_RECALIB                                                                                             */
/**************************************************************************************************************************/
#define  PHY_DLL_RECALIB                (MCPHY_BASE_ADDR + 0x124) , MCPHY_ACCESS, 32        /* Offset: 124h */
#define  PHY_DLL_RECALIB_ctr_start_val   28 , 4             /* 31-28 ctr_start_val. Controls the start value of the digital DLL master delay line.                                   */
#define  PHY_DLL_RECALIB_incr_dly_adrctrl_ma  27 , 1              /* 27 incr_dly_adrctrl_ma  Used only if override_adrctrl is 0  Set to increment the delay on the slave delay             */
#define  PHY_DLL_RECALIB_disable_recalib  26 , 1              /* 26 disable_recalib  Prevents the digital DLL from recalibrating after the first time                                  */
#define  PHY_DLL_RECALIB_recalib_cnt     8 , 18             /* 25-8 recalib_cnt. Determines how often the PHY issues the dfi_phyupd_req type 1 to controller in PHY clock            */
#define  PHY_DLL_RECALIB_dlls_trim_adrctrl_ma  0 , 6              /* 5-0 dlls_trim_adrctrl_ma. Trim settings for adjusting the output timing of row/column address with respect            */

/**************************************************************************************************************************/
/*   Register PHY_DLL_ADRCTRL                                                                                             */
/**************************************************************************************************************************/
#define  PHY_DLL_ADRCTRL                (MCPHY_BASE_ADDR + 0x128) , MCPHY_ACCESS, 32        /* Offset: 128h */
#define  PHY_DLL_ADRCTRL_dll_mas_dly     24 , 8             /* 31-24 dll_mas_dly. Digital DLL master delay line setting (corresponds to 1 clock delay).                              */
#define  PHY_DLL_ADRCTRL_dll_slv_dly_wire  16 , 6             /* 21-16 dll_slv_dly_wire. Slave delay line setting found by the digital DLL master (corresponds to ¼ clock              */
#define  PHY_DLL_ADRCTRL_incr_dly_adrctrl  9 , 1               /* 9 incr_dly_adrctrl  Used only if override_adrctrl is 0  Set to increment the delay on the slave delay line            */
#define  PHY_DLL_ADRCTRL_override_adrctrl  8 , 1               /* 8 override_adrctrl  Used to override the digital DLL master delay line and set the slave delay line output            */
#define  PHY_DLL_ADRCTRL_dlls_trim_adrctrl  0 , 6              /* 5-0 dlls_trim_adrctrl. Trim settings for adjusting the output timing of bank address and control signals with         */

/**************************************************************************************************************************/
/*   Register PHY_LANE_SEL                                                                                                */
/**************************************************************************************************************************/
#define  PHY_LANE_SEL                   (MCPHY_BASE_ADDR + 0x12C) , MCPHY_ACCESS, 32        /* Offset: 12Ch */
#define  PHY_LANE_SEL_bit_sel            8 , 4              /* 11-8 bit_sel. Must be programmed to appropriate value before programming                                              */
#define  PHY_LANE_SEL_phy_lane_sel       0 , 8              /* 7-0 phy_lane_sel. Used to select one of the byte lanes during programming. The value to be programmed                 */

/**************************************************************************************************************************/
/*   Register PHY_DLL_TRIM_1                                                                                              */
/**************************************************************************************************************************/
#define  PHY_DLL_TRIM_1                 (MCPHY_BASE_ADDR + 0x130) , MCPHY_ACCESS, 32        /* Offset: 130h */
#define  PHY_DLL_TRIM_1_override_1       8 , 3              /* 10-8 override_1. If using the digital DLL on the WRITE side (by programming no_analog_dll_wrclk to 1),                */
#define  PHY_DLL_TRIM_1_dlls_trim_1      0 , 6              /* 5-0 dlls_trim_1. Can be used to adjust the trim settings on the internal digital or analog DLL for controlling        */

/**************************************************************************************************************************/
/*   Register PHY_DLL_TRIM_2                                                                                              */
/**************************************************************************************************************************/
#define  PHY_DLL_TRIM_2                 (MCPHY_BASE_ADDR + 0x134) , MCPHY_ACCESS, 32        /* Offset: 134h */
#define  PHY_DLL_TRIM_2_inc_or_dec_all_trims    17, 1              /* 16 clear_all_trims  See description for inc_dec_all_trims      */
#define  PHY_DLL_TRIM_2_clear_all_trims    16, 1              /* 16 clear_all_trims  When this field is set to 1 when writing this register, all dlls_trim_2 values for all byte       */
#define  PHY_DLL_TRIM_2_inc_dec_all_trims  8, 6              /* 13-8 inc_dec_all_trims. When this field is non-zero when writing this register, all dlls_trim_2 values for all        */
#define  PHY_DLL_TRIM_2_dlls_trim_2        0, 6              /* 5-0 dlls_trim_2. Can be used to adjust output dq/dqs/dm timing with respect to DRAM clock.                            */

/**************************************************************************************************************************/
/*   Register PHY_DLL_TRIM_3                                                                                              */
/**************************************************************************************************************************/
#define  PHY_DLL_TRIM_3                 (MCPHY_BASE_ADDR + 0x138) , MCPHY_ACCESS, 32        /* Offset: 138h */
#define  PHY_DLL_TRIM_3_override_3       8 , 3              /* 10-8 override_3. If using the digital DLL on the READ side (by programming no_external_dll to 0), then each           */
#define  PHY_DLL_TRIM_3_dlls_trim_3      0 , 6              /* 5-0 dlls_trim_3. Can be used to adjust input dqs timing with respect to input dq by adjusting the trim                */

/**************************************************************************************************************************/
/*   Register PHY_DLL_INCR_TRIM_1                                                                                         */
/**************************************************************************************************************************/
#define  PHY_DLL_INCR_TRIM_1            (MCPHY_BASE_ADDR + 0x164) , MCPHY_ACCESS, 32        /* Offset: 164h */
#define  PHY_DLL_INCR_TRIM_1_incr_dly_1  0 , 3              /* 2-0 incr_dly_1. If set, dlls_trim_1 is to be used to increment the associated delay. Otherwise, dlls_trim_1           */

/**************************************************************************************************************************/
/*   Register PHY_DLL_INCR_TRIM_3                                                                                         */
/**************************************************************************************************************************/
#define  PHY_DLL_INCR_TRIM_3            (MCPHY_BASE_ADDR + 0x168) , MCPHY_ACCESS, 32        /* Offset: 168h */
#define  PHY_DLL_INCR_TRIM_3_incr_dly_3  0 , 3              /* 2-0 incr_dly_3. If set, dlls_trim_3 is to be used to increment the associated delay. Otherwise, dlls_trim_3           */

/**************************************************************************************************************************/
/*   Register SCL_DCAPCLK_DLY                                                                                             */
/**************************************************************************************************************************/
#define  SCL_DCAPCLK_DLY                (MCPHY_BASE_ADDR + 0x13C) , MCPHY_ACCESS, 32        /* Offset: 13Ch */
#define  SCL_DCAPCLK_DLY_dcap_byte_dly   0 , 8              /* 7-0 dcap_byte_dly. SCL data_capture_clk setting programmed per byte lane. Automatically programmed by                 */

/**************************************************************************************************************************/
/*  SCL_MAIN_CLK_DELTA                                                                          */
/**************************************************************************************************************************/
#define  SCL_MAIN_CLK_DELTA           (MCPHY_BASE_ADDR + 0x140) , MCPHY_ACCESS, 32      /* Offset: 140h */
#define  SCL_MAIN_CLK_DELTA_scl_rslt  4 , 3              /* 6-4 scl_rslt. Read-only value that gets set after SCL is run. It shows whether SCL passed or failed on each           */
#define  SCL_MAIN_CLK_DELTA_main_clk_delta_dly  0 , 3              /* 2-0 main_clk_delta_dly. SCL latency setting programmed per byte lane. Automatically programmed by                     */

/**************************************************************************************************************************/
/*   PHY_ERR_INJECT                  */
/**************************************************************************************************************************/
#define  PHY_ERR_INJECT                (MCPHY_BASE_ADDR + 0x144) , MCPHY_ACCESS, 32         /* Offset: 140h */
#define  PHY_ERR_INJECT_err_inject_vec  0 , 32             /* 31-0 err_inject_vec. If deliberate injection of error is desired for test purposes, then setting any of the bits in   */

/**************************************************************************************************************************/
/*   Register WRLVL_CTRL                                                                                                  */
/**************************************************************************************************************************/
#define  WRLVL_CTRL                     (MCPHY_BASE_ADDR + 0x148) , MCPHY_ACCESS, 32        /* Offset: 148h */
#define  WRLVL_CTRL_turn_on_dq_op        31 , 1              /* 31 turn_on_dq_op  SCL related  Used only for debug                                                                    */
#define  WRLVL_CTRL_turn_on_dqs_op       30 , 1              /* 30 turn_on_dqs_op  SCL related  Used only for debug                                                                   */
#define  WRLVL_CTRL_turn_on_ddr_odt      29 , 1              /* 29 turn_on_ddr_odt  SCL related  Used only for debug                                                                  */
#define  WRLVL_CTRL_turn_on_dq_odt       28 , 1              /* 28 turn_on_dq_odt  SCL related  Used only for debug                                                                   */
#define  WRLVL_CTRL_ip_dq_sample         20 , 8             /* 27-20 ip_dq_sample. SCL related. Used only for debug.                                                                 */
#define  WRLVL_CTRL_pulse_dqs            20 , 1              /* 20 pulse_dqs  SCL related  Used only for debug                                                                        */
#define  WRLVL_CTRL_wrlvl_rslt           0 , 3              /* 2-0 wrlvl_rslt. SCL related. Used to check if write-leveling passed.                                                  */

/**************************************************************************************************************************/
/*   Register WRLVL_AUTOINC_TRIM                                                                                          */
/**************************************************************************************************************************/
#define  WRLVL_AUTOINC_TRIM             (MCPHY_BASE_ADDR + 0x14C) , MCPHY_ACCESS, 32        /* Offset: 14Ch */
#define  WRLVL_AUTOINC_TRIM_incr_scl     25 , 1              /* 25 incr_scl  Read back value for bit of the same name in SCL_START                                                    */
#define  WRLVL_AUTOINC_TRIM_rd_only      24 , 1              /* 24 rd_only  Read back value for bit of the same name in SCL_START                                                     */
#define  WRLVL_AUTOINC_TRIM_wr_only      23 , 1              /* 23 wr_only  Read back value for bit of the same name in SCL_START                                                     */
#define  WRLVL_AUTOINC_TRIM_bit_lvl_mpr  22 , 1              /* 22 bit_lvl_mpr  Read back value for bit of the same name in SCL_START                                                 */
#define  WRLVL_AUTOINC_TRIM_bit_lvl_norm  21 , 1              /* 21 bit_lvl_norm  Read back value for bit of the same name in SCL_START                                                */
#define  WRLVL_AUTOINC_TRIM_bit_lvl_from_saved  20 , 1              /* 20 bit_lvl_from_saved  Read back value for bit of the same name in SCL_START                                          */
#define  WRLVL_AUTOINC_TRIM_start_wr_lvl  0 , 3              /* 2-0 start_wr_lvl. Used in SCL routines and should not be set by the user.                                             */

/**************************************************************************************************************************/
/*   Register WRLVL_DYN_ODT                                                                                               */
/**************************************************************************************************************************/
#define  WRLVL_DYN_ODT                  (MCPHY_BASE_ADDR + 0x150) , MCPHY_ACCESS, 32        /* Offset: 150h */
#define  WRLVL_DYN_ODT_dynamic_odt_on    16 , 16            /* 31-16 dynamic_odt_on. Only for DDR3-4. Value that should be programmed in DDR4 SDRAM’s Mode                           */
#define  WRLVL_DYN_ODT_dynamic_odt_off   0 , 16             /* 15-0 dynamic_odt_off. Only for DDR3-4. Value that should be programmed in DDR4 SDRAM’s Mode                           */

/**************************************************************************************************************************/
/*  WRLVL_ON_OFF              */
/**************************************************************************************************************************/
#define  WRLVL_ON_OFF                           (MCPHY_BASE_ADDR + 0x154) , MCPHY_ACCESS, 32        /* Offset: 154h */
#define  WRLVL_ON_OFF_wr_lvl_on                  16 , 16            /* 31-16 wr_lvl_on. Only for DDR3-4. Value that should be programmed in DDR4 SDRAM’s Mode Register 1 to                  */
#define  WRLVL_ON_OFF_wr_lvl_off                 0 , 16             /* 15-0 wr_lvl_off. Only for DDR3-4. Value that should be programmed in DDR4 SDRAM’s Mode Register 1 to                  */

/**************************************************************************************************************************/
/*   Register UNQ_ANALOG_DLL_1                                                                                            */
/**************************************************************************************************************************/
#define  UNQ_ANALOG_DLL_1               (MCPHY_BASE_ADDR + 0x15C) , MCPHY_ACCESS, 32        /* Offset: 15Ch */
#define  UNQ_ANALOG_DLL_1_no_analog_dll_wrclk  24 , 1              /* 24 no_analog_dll_wrclk  If set, the analog DLL is bypassed and the PHYs digital DLL is used instead                   */
#define  UNQ_ANALOG_DLL_1_analog_dll_reset  0 , 3              /* 2-0 analog_dll_reset. If analog DLL is used, reset control for the same on a per-byte-lane basis.                     */

/**************************************************************************************************************************/
/*   Register UNQ_ANALOG_DLL_2                                                                                            */
/**************************************************************************************************************************/
#define  UNQ_ANALOG_DLL_2               (MCPHY_BASE_ADDR + 0x160) , MCPHY_ACCESS, 32        /* Offset: 160h */
#define  UNQ_ANALOG_DLL_2_mas_dly        24 , 7             /* 30-24 mas_dly. Indication from Analog DLL about its master delay line setting.                                        */
#define  UNQ_ANALOG_DLL_2_analog_dll_lock  0 , 3              /* 2-0 analog_dll_lock. Indication from Analog DLL about its lock state.                                                 */

/**************************************************************************************************************************/
/*   Register UNQ_ANALOG_DLL_3                                                                                            */
/**************************************************************************************************************************/
#define  UNQ_ANALOG_DLL_3               (MCPHY_BASE_ADDR + 0x190) , MCPHY_ACCESS, 32        /* Offset: 190h */
#define  UNQ_ANALOG_DLL_3_rise_cycle_cnt_dphy  16 , 8             /* 23-16 rise_cycle_cnt_dphy. Indication from Analog DLL of half clock cycle delay value.                                */
#define  UNQ_ANALOG_DLL_3_phase2         8 , 5              /* 12-8 phase2. Phase by which DQS should be delayed on the read side. Bit 4 is used to delay by 1/4 cycle,              */
#define  UNQ_ANALOG_DLL_3_phase1         0 , 5              /* 4-0 phase1. Phase by which DQS should be delayed on the write side. Bit 4 is used to delay by 1/4 cycle,              */

/**************************************************************************************************************************/
/*   Register UNIQUIFY_IO_1                                                                                               */
/**************************************************************************************************************************/
#define  UNIQUIFY_IO_1                  (MCPHY_BASE_ADDR + 0x170) , MCPHY_ACCESS, 32        /* Offset: 170h */
#define  UNIQUIFY_IO_1_override_cal_n    23 , 1              /* 23 override_cal_n  See override_nfet_cal_code                                                                         */
#define  UNIQUIFY_IO_1_incr_cal_n        22 , 1              /* 22 incr_cal_n  Can be used to increment (if 1) or decrement (if 0) the nfet cal code from cal_pad by                  */
#define  UNIQUIFY_IO_1_override_nfet_cal_code  16 , 4             /* 19-16 override_nfet_cal_code. NFET Calibration code can be set manually with an override value when                   */
#define  UNIQUIFY_IO_1_override_cal_p    15 , 1              /* 15 override_cal_p  See override_pfet_cal_code                                                                         */
#define  UNIQUIFY_IO_1_incr_cal_p        14 , 1              /* 14 incr_cal_p  Can be used to increment (if 1) or decrement (if 0) the pfet cal code from cal_pad by                  */
#define  UNIQUIFY_IO_1_override_pfet_cal_code  8 , 4              /* 11-8 override_pfet_cal_code. PFET Calibration code can be set manually with an override value when                    */
#define  UNIQUIFY_IO_1_cal_long          6 , 1               /* 6 cal_long (LC)  The user can enable explicit long calibration by setting and clearing this bit  Not required         */
#define  UNIQUIFY_IO_1_cal_short         5 , 1               /* 5 cal_short (SC)  The user can enable explicit short calibration by setting and clearing this bit  Not                */
#define  UNIQUIFY_IO_1_cal_end           4 , 1               /* 4 cal_end (CE)                                                                                                        */
#define  UNIQUIFY_IO_1_cal_end_seen      1 , 1               /* 1 R/W0C cal_end_seen (ENDC)  End Calibration  Goes high the first time ENDC is asserted  This is a sticky bit         */
#define  UNIQUIFY_IO_1_start_io_calib    0 , 1               /* 0 start_io_calib  (CCAL)  Continuous calibration                                                                      */

/**************************************************************************************************************************/
/*   Register UNIQUIFY_IO_2                                                                                               */
/**************************************************************************************************************************/
#define  UNIQUIFY_IO_2                  (MCPHY_BASE_ADDR + 0x174) , MCPHY_ACCESS, 32        /* Offset: 174h */
#define  UNIQUIFY_IO_2_updated_nfet_cal_code  24 , 4             /* 27-24 updated_nfet_cal_code. updated_nfet_cal_code = {LCN4, LCN3, LCN2, LCN1} Fed back to the                         */
#define  UNIQUIFY_IO_2_updated_pfet_cal_code  16 , 4             /* 19-16 updated_pfet_cal_code. Updated_pfet_cal_code = {LCP4, LCP3, LCP2, LCP1} Fed back to                             */
#define  UNIQUIFY_IO_2_nfet_cal_code     8 , 4              /* 11-8 nfet_cal_code. nfet_cal_code = {LCN4, LCN3, LCN2, LCN1} NFET Calibration code from cal_pad.                      */
#define  UNIQUIFY_IO_2_pfet_cal_code     0 , 4              /* 3-0 pfet_cal_code. pfet_cal_code = {LCP4, LCP3, LCP2, LCP1} PFET Calibration code from cal_pad.                       */

/**************************************************************************************************************************/
/*   Register UNIQUIFY_ADDR_CTRL_LOOPBACK_1                                                                               */
/**************************************************************************************************************************/
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1  (MCPHY_BASE_ADDR + 0x178) , MCPHY_ACCESS, 32        /* Offset: 178h */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_rst_n_result  10 , 1              /* 10 rst_n_result  Compare result for DDR3 SDRAM rst_n output                                                           */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_odt_result  9 , 1               /* 9 odt_result  Compare result for DRAM ODT output                                                                      */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_ba_result  6 , 3              /* 8-6 ba_result. Compare result for DRAM BA (bank address) output.                                                      */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_cke_result  5 , 1               /* 5 cke_result  Compare result for DRAM CKE output                                                                      */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_we_n_result  4 , 1               /* 4 we_n_result  Compare result for DRAM WEN output                                                                     */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_cas_n_result  3 , 1               /* 3 cas_n_result  Compare result for DRAM CASN output                                                                   */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_ras_n_result  2 , 1               /* 2 ras_n_result  Compare result for DRAM RASN output                                                                   */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_1_Addr_ctrl_loopback  0 , 2              /* 1-0 Addr_ctrl_loopback. Bit 0 is used to enable loopback test on the address control pins.                            */

/**************************************************************************************************************************/
/*   to enable this to happen (addr_ctrl_loopback_rcvr_en).                                                               */
/**************************************************************************************************************************/
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_2    (MCPHY_BASE_ADDR + 0x17C) , MCPHY_ACCESS, 32      /* Offset: 17Ch */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_2_addr_ctrl_loopback_rcvr_en_cs_n_result  16 , 1              /* 16 cs_n_result  Compare result for DRAM CSN output                                                                    */
#define  UNIQUIFY_ADDR_CTRL_LOOPBACK_2_addr_ctrl_loopback_rcvr_en_ma_result  0 , 16             /* 15-0 ma_result. Compare result for DRAM MA (row / column address) output.                                             */

/**************************************************************************************************************************/
/*   Register UNIQUIFY_DLL_LPBK_1                                                                                         */
/**************************************************************************************************************************/
#define  UNIQUIFY_DLL_LPBK_1            (MCPHY_BASE_ADDR + 0x180) , MCPHY_ACCESS, 32        /* Offset: 180h */
#define  UNIQUIFY_DLL_LPBK_1_dll_lpbk    0 , 2              /* 1-0 dll_lpbk. Loopback output of the internal analog DLL back on the input through an inverter to create a            */

/**************************************************************************************************************************/
/*   Register PHY_SCL_START_ADDR                                                                                          */
/**************************************************************************************************************************/
#define  PHY_SCL_START_ADDR             (MCPHY_BASE_ADDR + 0x188) , MCPHY_ACCESS, 32        /* Offset: 188h */
#define  PHY_SCL_START_ADDR_scl_start_bank_addr  29 , 3             /* 31-29 scl_start_bank_addr. Bank address to use when running SCL.                                                      */
#define  PHY_SCL_START_ADDR_scl_start_col_addr  16 , 13            /* 28-16 scl_start_col_addr. Column address to use when running SCL.                                                     */
#define  PHY_SCL_START_ADDR_scl_start_row_addr  0 , 16             /* 15-0 scl_start_row_addr. Row address to use when running SCL.                                                         */

/**************************************************************************************************************************/
/*   Register PHY_DLL_RISE_FALL                                                                                           */
/**************************************************************************************************************************/
#define  PHY_DLL_RISE_FALL              (MCPHY_BASE_ADDR + 0x18C) , MCPHY_ACCESS, 32        /* Offset: 18Ch */
#define  PHY_DLL_RISE_FALL_rise_cycle_cnt  4 , 8              /* 11-4 rise_cycle_cnt. Half-cycle period detected by the DLL.                                                           */
#define  PHY_DLL_RISE_FALL_min_rise_fall_dly  0 , 4              /* 3-0 min_rise_fall_dly. Used to prevent DLL from locking on the half-cycle because of clock jitter.                    */

/**************************************************************************************************************************/
/*   Register DSCL_CNT                                                                                                    */
/**************************************************************************************************************************/
#define  DSCL_CNT                       (MCPHY_BASE_ADDR + 0x19C) , MCPHY_ACCESS, 32        /* Offset: 19Ch */
#define  DSCL_CNT_dscl_save_restore_needed  25 , 1              /* 25 dscl_save_restore_needed  If set to 1 whenever DSCL is run, the data in the DRAM is saved and                      */
#define  DSCL_CNT_dscl_en                24 , 1              /* 24 dscl_en  Set to 1 to enable DSCL to run continuously without software intervention                                 */
#define  DSCL_CNT_dscl_exp_cnt           0 , 24             /* 23-0 dscl_exp_cnt. When dscl_en is set, this controls the frequency at which the PHY issues dfi_phyupd                */

/**************************************************************************************************************************/
/*   Register IP_DQ_DQS_BITWISE_TRIM                                                                                      */
/**************************************************************************************************************************/
#define  IP_DQ_DQS_BITWISE_TRIM         (MCPHY_BASE_ADDR + 0x194) , MCPHY_ACCESS, 32        /* Offset: 194h */
#define  IP_DQ_DQS_BITWISE_TRIM_bit_lvl_done_status  8 , 3              /* 10-8 bit_lvl_done_status. Read-only indication of whether bit-leveling has finished on each byte lane, 1 bit          */
#define  IP_DQ_DQS_BITWISE_TRIM_ip_dq_trim_override  7 , 1               /* 7 ip_dq_trim_override  Used to override the trim delay of all input DQs found from bit-leveling and to set            */
#define  IP_DQ_DQS_BITWISE_TRIM_ip_dq_dqs_bitwise_trim_reg  0 , 7              /* 6-0 ip_dq_dqs_bitwise_trim_reg. Trim delay and Incr/Decr setting for each input DQ bit. The MSB is the                */

/**************************************************************************************************************************/
/*   Register OP_DQ_DM_DQS_BITWISE_TRIM                                                                                   */
/**************************************************************************************************************************/
#define  OP_DQ_DM_DQS_BITWISE_TRIM      (MCPHY_BASE_ADDR + 0x1A0) , MCPHY_ACCESS, 32        /* Offset: 1A0h */
#define  OP_DQ_DM_DQS_BITWISE_TRIM_op_dq_trim_override  7 , 1               /* 7 op_dq_trim_override  Used to override the trim delay of all output DQs from bit-leveling and to set the             */
#define  OP_DQ_DM_DQS_BITWISE_TRIM_op_dq_dm_dqs_bitwise_trim_reg  0 , 7              /* 6-0 op_dq_dm_dqs_bitwise_trim_reg. Trim delay and Incr/Decr setting for each output DQ/DM/DQS. The                    */


/**************************************************************************************************************************/
/*   PHY_DLL_TRIM_CLK                                                                                   */
/**************************************************************************************************************************/
#define  PHY_DLL_TRIM_CLK      (MCPHY_BASE_ADDR + 0x1A4) , MCPHY_ACCESS, 32         /* Offset: 1A4h */
#define  PHY_DLL_TRIM_CLK_dlls_trim_clk              0 , 7             /* 6-0 R/W dlls_trim_clk. Used to control the output delay of clock with respect to data signals (dq/dqs/dm). Prior        */


/**************************************************************************************************************************/
/*   Register DYNAMIC_BIT_LVL                                                                                             */
/**************************************************************************************************************************/
#define  DYNAMIC_BIT_LVL                (MCPHY_BASE_ADDR + 0x1AC) , MCPHY_ACCESS, 32        /* Offset: 1ACh */
#define  DYNAMIC_BIT_LVL_bit_lvl_failure_status  14 , 3             /* 16-14 bit_lvl_failure_status. Read-only indication of whether bit-leveling has failed on each byte lane, 1 bit        */
#define  DYNAMIC_BIT_LVL_bit_lvl_read_latency  8 , 6              /* 13-8 bit_lvl_read_latency. Used by bit-leveling logic after the dynamic bit-leveling process is started to delay      */
#define  DYNAMIC_BIT_LVL_bit_lvl_trim_latency  4 , 4              /* 7-4 bit_lvl_trim_latency. Used by bit-leveling logic to add delay in-between samplings of input DQ such               */
#define  DYNAMIC_BIT_LVL_dynamic_bit_leveling  0 , 1               /* 0 dynamic_bit_leveling  Set to run bit-leveling dynamically                                                           */

/**************************************************************************************************************************/
/*   Register PHY_REV_CNTRL_REG                                                                                           */
/**************************************************************************************************************************/
#define  PHY_REV_CNTRL_REG              (MCPHY_BASE_ADDR + 0x1B0) , MCPHY_ACCESS, 32        /* Offset: 1B0 */
#define  PHY_REV_CNTRL_REG_PHY_REVISION_NUMBER  0 , 32             /* 31-0 PHY_REVISION_NUMBER. Tells the revision number of this DDR PHY.                                                  */

/**************************************************************************************************************************/
/*   Register SCL_WINDOW_TRIM                                                                                             */
/**************************************************************************************************************************/
#define  SCL_WINDOW_TRIM                (MCPHY_BASE_ADDR + 0x1B4) , MCPHY_ACCESS, 32        /* Offset: 1B4h */
#define  SCL_WINDOW_TRIM_incr_winB       23 , 1              /* 23 incr_winB  Increment if 1 or decrement if 0 control for incr_winB_val                                              */
#define  SCL_WINDOW_TRIM_incr_winB_val   20 , 3             /* 22-20 incr_winB_val. Value by which window B center point needs to be incremented or decremented if the               */
#define  SCL_WINDOW_TRIM_incr_winA       19 , 1              /* 19 incr_winA  Increment if 1 or decrement if 0 control for incr_winA_val                                              */
#define  SCL_WINDOW_TRIM_incr_winA_val   16 , 3             /* 18-16 incr_winA_val. Value by which window A center point needs to be incremented or decremented if the               */
#define  SCL_WINDOW_TRIM_winB_extra_margin  8 , 8              /* 15-8 winB_extra_margin. Can be used to add additional weightage to SCL window B in case window B has                  */
#define  SCL_WINDOW_TRIM_winA_extra_margin  0 , 8              /* 7-0 winA_extra_margin. Can be used to add additional weightage to SCL window A when comparing to                      */

/**************************************************************************************************************************/
/*   Register DISABLE_GATING_FOR SCL                                                                                      */
/**************************************************************************************************************************/
#define  DISABLE_GATING_FOR_SCL          (MCPHY_BASE_ADDR + 0x1B8) , MCPHY_ACCESS, 32       /* Offset: 1B8h */
#define  DISABLE_GATING_FOR_SCL_disable_x_prop_fix  1 , 1               /* 1 disable_x_prop_fix  Only used for simulation, this register will not be synthesized  Should be set to 1             */
#define  DISABLE_GATING_FOR_SCL_disable_scl_gating  0 , 1               /* 0 disable_scl_gating  Setting this to 1 will disable the traditional SCL gating logic  This gating should be          */

/**************************************************************************************************************************/
/*   Register SCL_CONFIG_4                                                                                                */
/**************************************************************************************************************************/
#define  SCL_CONFIG_4                   (MCPHY_BASE_ADDR + 0x1BC) , MCPHY_ACCESS, 32        /* Offset: 1BCh */
#define  SCL_CONFIG_4_dly_dfi_wrdata_en_start  0 , 1               /* 0 dly_dfi_wrdata_en_start  Used to delay the wrdata_en_start in the PHY by one cycle when swap                        */

/**************************************************************************************************************************/
/*   Register DYNAMIC_WRITE_BIT_LVL                                                                                       */
/**************************************************************************************************************************/
#define  DYNAMIC_WRITE_BIT_LVL          (MCPHY_BASE_ADDR + 0x1C0) , MCPHY_ACCESS, 32        /* Offset: 1C0h */
#define  DYNAMIC_WRITE_BIT_LVL_bit_lvl_wr_failure_status  20 , 3             /* 22-20 bit_lvl_wr_failure_status. Failure status of write side bit-leveling, per lane. 1 signifies that a lane has     */
#define  DYNAMIC_WRITE_BIT_LVL_bit_lvl_wr_side_read_lat  12 , 7             /* 18-12 bit_lvl_wr_side_read_lat. Latency value used to know when to expect the first read issued by the addr           */
#define  DYNAMIC_WRITE_BIT_LVL_bit_lvl_wr_side_trim_lat  4 , 6              /* 9-4 bit_lvl_wr_side_trim_lat. Latency value used to know when to evaluate the next sample and update                  */
#define  DYNAMIC_WRITE_BIT_LVL_dynamic_write_bit_leveling  0 , 1               /* 0 dynamic_write_bit_leveling  Set to run bit-leveling dynamically with independent write side training                */

/**************************************************************************************************************************/
/*   Register DDR4_CONFIG_1                                                                                               */
/**************************************************************************************************************************/
#define  DDR4_CONFIG_1                  (MCPHY_BASE_ADDR + 0x1C4) , MCPHY_ACCESS, 32        /* Offset: 1C4h */
#define  DDR4_CONFIG_1_scl_bit_lvl_in_ddr4  7 , 1               /* 7 scl_bit_lvl_in_ddr4  This bit must be programmed high before starting SCL or bit-leveling with DDR4                 */
#define  DDR4_CONFIG_1_scl_start_bg_addr  4 , 2              /* 5-4 scl_start_bg_addr. Starting bank group for scl and bit-leveling routines.                                         */
#define  DDR4_CONFIG_1_scl_start_row_addr  1 , 2              /* 2-1 scl_start_row_addr. Upper bits, 17:16, of row addr for the start location of SCL data, only applicable in         */
#define  DDR4_CONFIG_1_ddr4              0 , 1               /* 0 ddr4  Must be set to 1 for DDR4                                                                                     */

/**************************************************************************************************************************/
/*   Register VREF_TRAINING                                                                                               */
/**************************************************************************************************************************/
#define  VREF_TRAINING                  (MCPHY_BASE_ADDR + 0x1C8) , MCPHY_ACCESS, 32        /* Offset: 1C8h */
#define  VREF_TRAINING_ddr_vref_value    12 , 6             /* 17-12 ddr_vref_value. Set to the desired vref value that will be programmed in the DRAM. This should be set           */
#define  VREF_TRAINING_ddr_vref_range    11 , 1              /* 11 ddr_vref_range  Set to 0 or 1 depending on the desired range of vref values in the DRAM, based on                  */
#define  VREF_TRAINING_vref_value        4 , 6              /* 9-4 vref_value. Vref value for the PHY per lane. Program phy_lane_sel to <byte number>*7 before                       */
#define  VREF_TRAINING_vref_output_enable  2 , 1               /* 2 vref_output_enable  Set high to enable internally generated VREF reference voltage from UVREF pad                   */
#define  VREF_TRAINING_update_ddr_vref   1 , 1               /* 1 update_ddr_vref  Set high to issue a mode register command from the PHY to update the vref training                 */
#define  VREF_TRAINING_vref_training     0 , 1               /* 0 vref_training  Set high to signal the PHY to operate in vref training mode                                          */

/**************************************************************************************************************************/
/*   Register VREF_TRAINING_WINDOWS                                                                                       */
/**************************************************************************************************************************/
#define  VREF_TRAINING_WINDOWS          (MCPHY_BASE_ADDR + 0x1D4) , MCPHY_ACCESS, 32        /* Offset: 1D4h */
#define  VREF_TRAINING_WINDOWS_ddr_window_1  24 , 6             /* 29-24 ddr_window_1. Measured size, in delay steps, of the 1 portion of the write data eye.                            */
#define  VREF_TRAINING_WINDOWS_ddr_window_0  16 , 6             /* 21-16 ddr_window_0. Measured size, in delay steps, of the 0 portion of the write data eye.                            */
#define  VREF_TRAINING_WINDOWS_window_1  8 , 8              /* 15-8 window_1. Measured size, in delay steps, of the 1 portion of the read data eye.                                  */
#define  VREF_TRAINING_WINDOWS_window_0  0 , 6              /* 5-0 window_0. Measured size, in delay steps, of the 0 portion of the read data eye.                                   */

/**************************************************************************************************************************/
/*   Register DC_PARAM_OUT                                                                                                */
/**************************************************************************************************************************/
#define  DC_PARAM_OUT                   (MCPHY_BASE_ADDR + 0x1D8) , MCPHY_ACCESS, 32        /* Offset: 1D8h */
#define  DC_PARAM_OUT_dc_param_wdata     0 , 32             /* 31-0 dc_param_wdata. Data to be sent on DDR I/Os for test purposes.                                                   */

/**************************************************************************************************************************/
/*   Register DC_PARAM_SEL                                                                                                */
/**************************************************************************************************************************/
#define  DC_PARAM_SEL                   (MCPHY_BASE_ADDR + 0x1DC) , MCPHY_ACCESS, 32        /* Offset: 1DCh */
#define  DC_PARAM_SEL_dc_param_rdata     12 , 9             /* 20-12 dc_param_rdata. Holds the input from the DDR as {jtag_ip_dqs, jtag_ip_dq} for the byte lane selected            */
#define  DC_PARAM_SEL_dc_param_sel       4 , 5              /* 8-4 dc_param_sel. Set to target the desired PHY byte lane for the test input or output. Program hex value             */
#define  DC_PARAM_SEL_dc_param_mode      1 , 2              /* 2-1 dc_param_mode. Program to switch between DC parametric modes as follows;                                          */
#define  DC_PARAM_SEL_dc_param_enable    0 , 1               /* 0 dc_param_enable  Enables the test mode output value and test mode output enable for DDR I/Os                        */




typedef struct _MC_REGISTERS_
{
    UINT32   reg_SCL_START  ;                       //100h Varies per bit 0000_0000h
    UINT32   reg_SCL_DATA_0  ;                       //104h R/W 789B_3DEh
    UINT32   reg_SCL_DATA_1  ;                       //108h R/W F10E_4A56h
    UINT32   reg_SCL_LATENCY  ;                       //10Ch R/W 0000_5262h
    UINT32   reg_SCL_RD_ADDR  ;                       //110h R/W 0000_0040h
    UINT32   reg_SCL_RD_DATA  ;                       //114h R Undefined
    UINT32   reg_SCL_CONFIG_1  ;                       //118h R/W 0100_40B5h
    UINT32   reg_SCL_CONFIG_2  ;                       //11Ch R/W 1300_0801h
    UINT32   reg_PHY_PAD_CTRL  ;                       //120h R/W 5044_404Bh
    UINT32   reg_PHY_DLL_RECALIB  ;                       //124h R/W 3400_0000h
    UINT32   reg_PHY_DLL_ADRCTRL  ;                       //128h Variesper bit0000_0000h
    UINT32   reg_PHY_LANE_SEL  ;                       //12Ch R/W 0000_0000h
    UINT32   reg_PHY_DLL_TRIM_1  ;                       //130h R/W 0000_0000h
    UINT32   reg_PHY_DLL_TRIM_2  ;                       //134h Varies per bit 0000_0000h
    UINT32   reg_PHY_DLL_TRIM_3  ;                       //138h R/W 0000_0000h
    UINT32   reg_SCL_DCAPCLK_DLY  ;                       //13Ch R/W 0000_0010h
    UINT32   reg_SCL_MAIN_CLK_DELTA  ;                       //140h Varies per bit 0000_0000h
    UINT32   reg_PHY_ERR_INJECT  ;                       //144h R/W 0000_0000h
    UINT32   reg_WRLVL_CTRL  ;                       //148h Varies per bit 0000_0000h
    UINT32   reg_WRLVL_AUTOINC_TRIM  ;                       //14Ch Varies per bit 0000_0000h
    UINT32   reg_WRLVL_DYN_ODT  ;                       //150h R/W 0218_0018h
    UINT32   reg_WRLVL_ON_OFF  ;                       //154h R/W 00C6_0046h
    UINT32   reg_UNQ_ANALOG_DLL_1  ;                       //15Ch R/W 0000_0007h
    UINT32   reg_UNQ_ANALOG_DLL_2  ;                       //160h R 0000_0000h
    UINT32   reg_PHY_DLL_INCR_TRIM_1  ;                       //164h R/W 0000_0000h
    UINT32   reg_PHY_DLL_INCR_TRIM_3  ;                       //168h R/W 0000_0000h
    UINT32   reg_UNIQUIFY_IO_1  ;                       //170h Varies per bit 0000_0000h
    UINT32   reg_UNIQUIFY_IO_2  ;                       //174h RO Undefined
    UINT32   reg_UNIQUIFY_ADDR_CTRL_LOOPBACK_1  ;                       //178h Varies  er bit 0000_0000h
    UINT32   reg_UNIQUIFY_ADDR_CTRL_LOOPBACK_2  ;                       //17Ch R 0000_0000h
    UINT32   reg_UNIQUIFY_DLL_LPBK_1  ;                       //180h R/W 0000_0000h
    UINT32   reg_PHY_SCL_START_ADDR  ;                       //188h R/W 0000_0000h
    UINT32   reg_PHY_DLL_RISE_FALL  ;                       //18Ch Varies per bit 0000_0006h
    UINT32   reg_UNQ_ANALOG_DLL_3  ;                       //190h Varies per bit 0000_1010h
    UINT32   reg_IP_DQ_DQS_BITWISE_TRIM  ;                       //194h Varies per bit 0000_0000h
    UINT32   reg_DSCL_CNT  ;                       //19Ch R/W 0000_0000h
    UINT32   reg_OP_DQ_DM_DQS_BITWISE_TRIM  ;                       //1A0h R/W 0000_0000h
    UINT32   reg_PHY_DLL_TRIM_CLK  ;                       //1A4h R/W 0000_0000h
    UINT32   reg_DYNAMIC_BIT_LVL  ;                       //1ACh Varies per bit 0000_2770h
    UINT32   reg_PHY_REV_CNTRL_REG  ;                       //1B0h R 0001_0812h
    UINT32   reg_SCL_WINDOW_TRIM  ;                       //1B4h R/W 0000_0000h
    UINT32   reg_DISABLE_GATING_FOR_SCL  ;                       //1B8h R/W 0000_0000h
    UINT32   reg_SCL_CONFIG_4  ;                       //1BCh R/W 0000_0000h
    UINT32   reg_DYNAMIC_WRITE_BIT_LVL  ;                       //1C0h Varies per bit 0000_0000h
    UINT32   reg_DDR4_CONFIG_1  ;                       //1C4h R/W 0000_0000h
    UINT32   reg_VREF_TRAINING  ;                       //1C8h R/W 0000_0004h
    UINT32   reg_VREF_TRAINING_WINDOWS  ;                       //1D4h R Undefined
    UINT32   reg_DC_PARAM_OUT  ;                       //1D8h R/W 0000_0000h
    UINT32   reg_DC_PARAM_SEL  ;                       //1DCh Varies per bit 0000_0000h
} MC_REGS_T;



// lior
#define MC_DDR_DLL_ENABLE   ((UINT32)1<<0)
#define MC_DDR_DRV_48       ((UINT32)1<<1)
#define MC_DDR_DRV_34       ((UINT32)0<<1)
#define MC_DDR_AL_DISABLE   ((UINT32)0<<3) // 00:Disable, 10:CL-2, 01:CL-1
#define MC_DDR_WL_ENABLE    ((UINT32)1<<7)
#define MC_DDR_ODT_NO       ((UINT32)0<<8)
#define MC_DDR_ODT_60       ((UINT32)1<<8)
#define MC_DDR_ODT_120      ((UINT32)2<<8)
#define MC_DDR_ODT_40       ((UINT32)3<<8)
#define MC_DDR_ODT_240      ((UINT32)4<<8)
#define MC_DDR_ODT_48       ((UINT32)5<<8)
#define MC_DDR_ODT_80       ((UINT32)6<<8)
#define MC_DDR_ODT_34       ((UINT32)7<<8)


/*---------------------------------------------------------------------------------------------------------*/
/* In DENALI_CTL_80_DATA register and WRLVL_ON_OFF register change to MC_DDR_DRV_48, MC_DDR_ODT_120. - check*/
/* PHY spec for the correct bits.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define MC_MR1              ((MC_DDR_DLL_ENABLE|MC_DDR_DRV_48|MC_DDR_ODT_120)&0xFFFF)
#define DENALI_CTL_80_DATA  (MC_MR1) // MR1_DATA_F0_0 // DLL, DRV and ODT


// PHY output driver for DQ/DQS/DM signals
#define PHY_DRV_DQ_34R  (UINT32)0<<4
#define PHY_DRV_DQ_48R  (UINT32)1<<4

// PHY output driver for Address/Control signals
#define PHY_DRV_CTL_34R (UINT32)0<<16
#define PHY_DRV_CTL_48R (UINT32)1<<16

// PHY output driver for clock signals
#define PHY_DRV_CLK_34R (UINT32)0<<20
#define PHY_DRV_CLK_48R (UINT32)1<<20

// PHY input ODT for DQ/DQS signals
#define PHY_ODT_DQ_48R  (UINT32)0x05
#define PHY_ODT_DQ_60R  (UINT32)0x04
#define PHY_ODT_DQ_80R  (UINT32)0x03
#define PHY_ODT_DQ_120R (UINT32)0x02
#define PHY_ODT_DQ_240R (UINT32)0x01
#define PHY_ODT_DQ_NO   (UINT32)0x00



#endif // _MC_REGS_H

