/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   MC_Priority_Settings.h                                                   */
/*            This file contains priority settings for Memory Controller      */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/

/*

MC Arbitration and Priority Settings

AXIy_WRLEN                          Number of AXI beats for a write transfer
AXIy_RDLEN                          Number of AXI beats for a read transfer
AXIy_W_PRIORITY                     0-  Highest WR priority
AXIy_R_PRIORITY                     0-  Highest RD priority
                                    Set the priority group of RD/WR transactions for each AXI bus
AXIy_FIXED_PORT_PRIORITY_ENABLE     Per port or per command priority enable using AXIy_W_PRIORITY and AXIy_R_PRIORITY
                                    Should be set to "1"
AXIy_FIFO_TYPE_REG                  Interface Synch or not to FIFO
                                    Dependent on MMC and interface clock ratio / synchronization
AXI_ALIGNED_STROBE_DISABLE          Read modify write support for un-aligned address
                                    Should be set to "0" supported
AXIy_priorityZ_relative_priority    Values 1-Fh, 1 highest priority.
                                    If few AXI interfaces transactions has the same priority this priority provides additional (relative) priority level
AXIy_PRIORITY_RELAX                 Counter value to trigger priority relax on port.
                                    Counter which enables some bandwidth to ports in low priority groups if the MMC interface bandwidth if fully occupied.
AXIy_PORT_ORDERING                  Reassigned port order for port.
                                    Set the order of commands with the same priority and relative priority - No need to use

*/


#define AXI0_W_PRIORITY                     0x00
#define AXI0_R_PRIORITY                     0x00
#define AXI0_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI0_FIFO_TYPE_REG                  0x00
#define AXI0_PRIORITY0_RELATIVE_PRIORITY    0x06
#define AXI0_PRIORITY1_RELATIVE_PRIORITY    0x01
#define AXI0_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI0_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI0_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI0_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI0_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI0_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI0_PRIORITY_RELAX                 0x0064
#define AXI0_PORT_ORDERING                  0x00

#define AXI1_W_PRIORITY                     0x01
#define AXI1_R_PRIORITY                     0x01
#define AXI1_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI1_FIFO_TYPE_REG                  0x00
#define AXI1_PRIORITY0_RELATIVE_PRIORITY    0x01
#define AXI1_PRIORITY1_RELATIVE_PRIORITY    0x06
#define AXI1_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI1_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI1_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI1_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI1_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI1_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI1_PRIORITY_RELAX                 0x0064
#define AXI1_PORT_ORDERING                  0x00

#define AXI2_W_PRIORITY                     0x01
#define AXI2_R_PRIORITY                     0x01
#define AXI2_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI2_FIFO_TYPE_REG                  0x00
#define AXI2_PRIORITY0_RELATIVE_PRIORITY    0x01
#define AXI2_PRIORITY1_RELATIVE_PRIORITY    0x02
#define AXI2_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI2_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI2_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI2_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI2_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI2_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI2_PRIORITY_RELAX                 0x0064
#define AXI2_PORT_ORDERING                  0x00

#define AXI3_W_PRIORITY                     0x01
#define AXI3_R_PRIORITY                     0x01
#define AXI3_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI3_FIFO_TYPE_REG                  0x00
#define AXI3_PRIORITY0_RELATIVE_PRIORITY    0x01
#define AXI3_PRIORITY1_RELATIVE_PRIORITY    0x02
#define AXI3_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI3_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI3_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI3_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI3_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI3_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI3_PRIORITY_RELAX                 0x0064
#define AXI3_PORT_ORDERING                  0x00

#define AHB4_WRLEN                          0x0f
#define AHB4_RDLEN                          0x0f
#define AXI4_W_PRIORITY                     0x01
#define AXI4_R_PRIORITY                     0x01
#define AXI4_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI4_FIFO_TYPE_REG                  0x00
#define AXI4_PRIORITY0_RELATIVE_PRIORITY    0x01
#define AXI4_PRIORITY1_RELATIVE_PRIORITY    0x02
#define AXI4_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI4_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI4_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI4_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI4_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI4_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI4_PRIORITY_RELAX                 0x0064
#define AXI4_PORT_ORDERING                  0x00

#define AHB5_WRLEN                          0x0f
#define AHB5_RDLEN                          0x0f
#define AXI5_W_PRIORITY                     0x01
#define AXI5_R_PRIORITY                     0x01
#define AXI5_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI5_FIFO_TYPE_REG                  0x00
#define AXI5_PRIORITY0_RELATIVE_PRIORITY    0x01
#define AXI5_PRIORITY1_RELATIVE_PRIORITY    0x02
#define AXI5_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI5_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI5_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI5_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI5_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI5_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI5_PRIORITY_RELAX                 0x0064
#define AXI5_PORT_ORDERING                  0x00

#define AHB6_WRLEN                          0x0f
#define AHB6_RDLEN                          0x0f
#define AXI6_W_PRIORITY                     0x01
#define AXI6_R_PRIORITY                     0x01
#define AXI6_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI6_FIFO_TYPE_REG                  0x00
#define AXI6_PRIORITY0_RELATIVE_PRIORITY    0x01
#define AXI6_PRIORITY1_RELATIVE_PRIORITY    0x02
#define AXI6_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI6_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI6_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI6_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI6_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI6_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI6_PRIORITY_RELAX                 0x0064
#define AXI6_PORT_ORDERING                  0x00

#define AHB7_WRLEN                          0x0f
#define AHB7_RDLEN                          0x0f
#define AXI7_W_PRIORITY                     0x00
#define AXI7_R_PRIORITY                     0x00
#define AXI7_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI7_FIFO_TYPE_REG                  0x00
#define AXI7_PRIORITY0_RELATIVE_PRIORITY    0x02
#define AXI7_PRIORITY1_RELATIVE_PRIORITY    0x01
#define AXI7_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI7_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI7_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI7_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI7_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI7_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI7_PRIORITY_RELAX                 0x0064
#define AXI7_PORT_ORDERING                  0x00

#define AHB8_WRLEN                          0x0f
#define AHB8_RDLEN                          0x0f
#define AXI8_W_PRIORITY                     0x00
#define AXI8_R_PRIORITY                     0x00
#define AXI8_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI8_FIFO_TYPE_REG                  0x00
#define AXI8_PRIORITY0_RELATIVE_PRIORITY    0x02
#define AXI8_PRIORITY1_RELATIVE_PRIORITY    0x01
#define AXI8_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI8_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI8_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI8_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI8_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI8_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI8_PRIORITY_RELAX                 0x0064
#define AXI8_PORT_ORDERING                  0x00

#define AHB9_WRLEN                          0x0f
#define AHB9_RDLEN                          0x0f
#define AXI9_W_PRIORITY                     0x00
#define AXI9_R_PRIORITY                     0x00
#define AXI9_FIXED_PORT_PRIORITY_ENABLE     0x01
#define AXI9_FIFO_TYPE_REG                  0x00
#define AXI9_PRIORITY0_RELATIVE_PRIORITY    0x04
#define AXI9_PRIORITY1_RELATIVE_PRIORITY    0x01
#define AXI9_PRIORITY2_RELATIVE_PRIORITY    0x01
#define AXI9_PRIORITY3_RELATIVE_PRIORITY    0x01
#define AXI9_PRIORITY4_RELATIVE_PRIORITY    0x01
#define AXI9_PRIORITY5_RELATIVE_PRIORITY    0x01
#define AXI9_PRIORITY6_RELATIVE_PRIORITY    0x01
#define AXI9_PRIORITY7_RELATIVE_PRIORITY    0x01
#define AXI9_PRIORITY_RELAX                 0x0064
#define AXI9_PORT_ORDERING                  0x00

#define AHB10_WRLEN                         0x0f
#define AHB10_RDLEN                         0x0f
#define AXI10_W_PRIORITY                    0x01
#define AXI10_R_PRIORITY                    0x01
#define AXI10_FIXED_PORT_PRIORITY_ENABLE    0x01
#define AXI10_FIFO_TYPE_REG                 0x00
#define AXI10_PRIORITY0_RELATIVE_PRIORITY   0x01
#define AXI10_PRIORITY1_RELATIVE_PRIORITY   0x02
#define AXI10_PRIORITY2_RELATIVE_PRIORITY   0x01
#define AXI10_PRIORITY3_RELATIVE_PRIORITY   0x01
#define AXI10_PRIORITY4_RELATIVE_PRIORITY   0x01
#define AXI10_PRIORITY5_RELATIVE_PRIORITY   0x01
#define AXI10_PRIORITY6_RELATIVE_PRIORITY   0x01
#define AXI10_PRIORITY7_RELATIVE_PRIORITY   0x01
#define AXI10_PRIORITY_RELAX                0x0064
#define AXI10_PORT_ORDERING                 0x00

#define AHB11_WRLEN                         0x0f
#define AHB11_RDLEN                         0x0f
#define AXI11_W_PRIORITY                    0x01
#define AXI11_R_PRIORITY                    0x01
#define AXI11_FIXED_PORT_PRIORITY_ENABLE    0x01
#define AXI11_FIFO_TYPE_REG                 0x00
#define AXI11_PRIORITY0_RELATIVE_PRIORITY   0x01
#define AXI11_PRIORITY1_RELATIVE_PRIORITY   0x02
#define AXI11_PRIORITY2_RELATIVE_PRIORITY   0x01
#define AXI11_PRIORITY3_RELATIVE_PRIORITY   0x01
#define AXI11_PRIORITY4_RELATIVE_PRIORITY   0x01
#define AXI11_PRIORITY5_RELATIVE_PRIORITY   0x01
#define AXI11_PRIORITY6_RELATIVE_PRIORITY   0x01
#define AXI11_PRIORITY7_RELATIVE_PRIORITY   0x01
#define AXI11_PRIORITY_RELAX                0x0064
#define AXI11_PORT_ORDERING                 0x00

#define AHB12_WRLEN                         0x0f
#define AHB12_RDLEN                         0x0f
#define AXI12_W_PRIORITY                    0x01
#define AXI12_R_PRIORITY                    0x01
#define AXI12_FIXED_PORT_PRIORITY_ENABLE    0x01
#define AXI12_FIFO_TYPE_REG                 0x00
#define AXI12_PRIORITY0_RELATIVE_PRIORITY   0x01
#define AXI12_PRIORITY1_RELATIVE_PRIORITY   0x02
#define AXI12_PRIORITY2_RELATIVE_PRIORITY   0x01
#define AXI12_PRIORITY3_RELATIVE_PRIORITY   0x01
#define AXI12_PRIORITY4_RELATIVE_PRIORITY   0x01
#define AXI12_PRIORITY5_RELATIVE_PRIORITY   0x01
#define AXI12_PRIORITY6_RELATIVE_PRIORITY   0x01
#define AXI12_PRIORITY7_RELATIVE_PRIORITY   0x01
#define AXI12_PRIORITY_RELAX                0x0064
#define AXI12_PORT_ORDERING                 0x00



