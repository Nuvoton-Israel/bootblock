----------------------------------------------------------------------
 SPDX-License-Identifier: GPL-2.0                                     
 Copyright (c) 2010-2024 by Nuvoton Technology Corporation   
 All rights reserved       

 NPCM7XX (Poleg) BootBlock Release Letter 
----------------------------------------------------------------------

# NPCM7XX BootBlock Package
Release Letter
Version: 10.10.19
Date:    Mar 2024


# PACKAGE CONTENTS
----------------
This package contains the following files/folders:
- BootBlock_Sources_RelLetter.txt                     - This release letter
- Sources\Src\                                        - Main source files
- Sources\SWC_DEFS\                                   - definition source files
- Sources\SWC_HAL\                                    - HW drivers files
- Sources\Build.bat, clean.bat                        - Building batch files for Windows
- Sources\makefile                                    - makefile for Windows and Linux
- Sources\HowToBuild.txt                              - How to build description

Output Files:
- Images\Poleg_bootblock_basic.elf                    - Output image elf
- Images\Poleg_bootblock_basic.dat                    - Output image dat
- Images\Poleg_bootblock_basic.bin                    - Output image bin
- Images\Poleg_bootblock_basic.map                    - Output image map


# DESCRIPTION
-----------
This package contains the Nuvoton Poleg bootBlock for the NPCM7XX.
This package is released in beta quality.

Package does not contain security related code. To use Poleg security features,
as well as NIST800-193 support, contact Nuvoton. 


# REQUIREMENTS
------------
Hardware:
- A Nuvoton NPCM7XX A1 Silicon BMC device.


# INSTALLATION PROCEDURE
----------------------
After compilation (see HowToBuild.txt) program   deliverables\bootblock\Images\Poleg_bootblock_basic.bin
to flash via the IGPS (https://github.com/Nuvoton-Israel/igps) or other means.


# ENHANCEMENTS
------------
Bootblock 10.10.19     Date: 27.03.24
==================     ===============
- uboot can only be loaded to DRAM from the range 0x100 till end of DRAM.
- Avoid TOCTOU: read destination address and size only once.
- Remove XIP.

# FIXED ISSUES
------------
- None.


# KNOWN ISSUES
------------
- None.


# HISTORY
----------------
Bootblock 10.10.18     Date: 24.07.24
==================     ===============
- Add support for baud rate setting
- supported baud rates 115200, 460800

Bootblock 10.10.16     Date: 11.11.20
==================     ===============
- MC: start enhanced training in 0x14.
- MC: when find a step in enhanced training, take +4 (was +2)

Bootblock 10.10.15     Date: 22.10.20
==================     ===============
- MC: add option to change enhanced training range.
- MC: Change default enhanced training range to [0x16:0x28].
- MC: minimum DQS eye is 30.
- MC: in enhanced training sweep only in DQS, not out DQS.

Bootblock 10.10.13     Date: 19.10.20
==================     ===============
- MC: add option to override DQS in and out init values from header.

Bootblock 10.10.11     Date: 26.11.19 
==================     ===============
- Fix an issue in INTCR3 settings (FIU_FIX field).

Bootblock 10.10.12     Date: 21.07.20
==================     ===============
- SPI0 frequency is now only limited to be up to 50MHz (remove 40MHz lower limit).
  This check is only performed if PLLs are changes (RUN_BMC or new header frequency values).

Bootblock 10.10.10     Date: 17.11.19 
==================     ===============
- MC: in enhanced training: change the sweep range accrording to the location of the step. (run time optimization)

Bootblock 10.10.09     Date: 27.10.19 
==================     ===============
- CLK: fix an issue when setting MC and CPU clocks to different values (using the header).
- eSPI: init only on PORST. 


Bootblock 10.10.08
==================
- MC: longer memory test in DQS sweep.
- MC: fix enhanced training (DQS optimized eye selection)
      Read leveling only inside enhanced training (less time to run).
      
      
Bootblock 10.10.07
==================
- Set INTCR3.FIX_FIX.
- WP open only during update or logging.
- Allow secured rollback.
- Add self test to WP.

Bootblock 10.10.06 
==================
- NIST: LED blink while update\recovery is now 4Hz.

Bootblock 10.10.05 
==================
- NIST: bug fix: when setting status register of flash need to wait for BUSY bit to clear.
- Reset BB mailbox status before use (before boot starts it still show the status from last reset (not related to PORST). 

Bootblock 10.10.04 
==================
- NIST: limit uboot OTP version to 24 bytes (192 versions).
- NIST: lock the OTP before jump to uboot (FCFG1,2)
- NIST: Fix issue in WP~ locking. Use status register instead. 


Bootblock 10.10.03 
==================
- NIST: Bug fix WP~ while using Flash#1.
- NIST: GPIO blink pattern.


Bootblock 10.10.01 
==================
- Write 0xFF on the public key to be invalidated. (Key will show CRC error).
- Add WD num field and WD stay on to header.
- GPIO toggle while copy flash. GPIO value selected on header. 
- Bug fix: Add key number to the log. Print more info (update, recovery and halt).
- Print key status in text, not value. For readability.


Bootblock 10.10.00 
==================
- NIST: bug fix: write tag after writing version number to OTP.
- NIST: bug fix: PK_INVLID - write only these bit to FUSTRAP (previous version set oSecBoot as well).
- Re-arrange heaader fields. See BootBlock user guide for details.

Bootblock 10.09.09
==================
- Bug fix: program from end of last hole to end of flash was not performed.


Bootblock 10.09.08
==================
- NIST will not run if oAltImageLoc is not fused.
- OTP : skip three smirred bytes in OTP uboot version. Also print it all.
- Bug fix: if security log is not defined skip logging.
- NIST: Add key revoking.
- If ROM code runs in basic mode so does the BB, regardless of the header.
- Stop WD when halt.

Bootblock 10.09.07
==================
- Support security compliant to NIST800-193.
	* all features from spec included, except for key revoke.

	
Bootblock 10.09.06
==================
- Support security compliant to NIST800-193.
	* all features from spec included, except for security log and key revoke.

Bootblock 10.09.05
==================
- Add a field in the header to select eSPI or LPC mode. Offset is 301 from start.
  	0xFF: Do nothing as current BB version
	0x00: configure pins to LPC + GPIO168 to TRIS + enable LPC  + release HOSTWAIT.
	0x01: configure pins to eSPI + GPIO162 to TRIS + enable eSPI
	0x02: configure pins to GPIOs TRIS.

Bootblock 10.09.04
==================
- Support RUN_BMC.
- Add an option to set PLL0 when CPU frequency == MC Frequency (and assigned to
  PLL1). For Run BMC PLL0 (TOCK signal) is set to 125MHz.


Bootblock 10.09.03
==================
- KCS: IBFIE disables in CORST.

Bootblock 10.09.02
==================
- Disable KBCI irq in warm boot. This is needed to eliminate race condition.


Bootblock 10.09.01
==================
- Disable KCS and post code irq in warm boot. This is needed to eliminate race
condition.

Bootblock 10.09.00
==================
- ECC training: add enhanced sweep option for ECC lane as well. This will
	increase jitter tolerance.

Bootblock 10.08.09
==================
- Bug fix ECC training


Bootblock 10.08.08
==================
- MC: fix issue in enhanced training. Reduce the runtime.

Bootblock 10.08.07
==================
- MC: Add support for ECC disable and enhanced training disable on header.
  default: do enhanced training, use ECC if present.
- MC:  double refresh rate in PHY to support 1GB DRAM devices. (350ns)

Bootblock 10.08.05
==================
- Print to UART0 on Nuvoton board.
- Note:   Need to update UBOOT and LINUX as well. To see prints on SI2
  need to set STRAP11 On.

Bootblock 10.08.00
==================
- Support NPCM730.


-----------------------------------------------------------
For contact information see "Contact Us" at www.nuvoton.com
-----------------------------------------------------------
