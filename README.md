----------------------------------------------------------------------
 SPDX-License-Identifier: GPL-2.0                                     
 Copyright (c) 2010-2019 by Nuvoton Technology Corporation   
 All rights reserved       

 NPCM7XX (Poleg) BootBlock Release Letter 
----------------------------------------------------------------------

# NPCM7XX bootBlock Package
Release Letter
Version: 10.10.02
Date:    May 2019


## PACKAGE CONTENTS
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


## DESCRIPTION
-----------
This package contains the Nuvoton Poleg bootBlock for the NPCM7XX.
This package is released in beta quality.

Package does not contain security related code. To use Poleg security features,
as well as NIST800-193 support, contact Nuvoton. 


## REQUIREMENTS
------------
Hardware:
- A Nuvoton NPCM7XX A1 Silicon BMC device.


## INSTALLATION PROCEDURE
----------------------
After compilation (see HowToBuild.txt) program   deliverables\bootblock\Images\Poleg_bootblock_basic.bin
to flash via the IGPS (https://github.com/Nuvoton-Israel/igps) or other means.


## ENHANCEMENTS
------------
- First release.


## FIXED ISSUES
------------
- None.


## KNOWN ISSUES
------------
- None.


## HISTORY
-------
- First release.



-----------------------------------------------------------
For contact information see "Contact Us" at www.nuvoton.com
-----------------------------------------------------------

