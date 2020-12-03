/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   bootblock_version.c                                                      */
/*            This file contains bootblock version constants                  */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/

#define BOOTBLOCK_VERSION_C

#include "boot.h"

/*
BootblockVersion = N7N6N5N4N3N2N1N0

Where:

·         N7N6 – BOOTBLOCK code type:
			o        0x00 – Chip

·         N5N4 – BOOTBLOCK code Major Version.

·         N3N2 – BOOTBLOCK code Minor Version.

·         N1N0 – BOOTBLOCK code Revision.
*/

//Dont forget the --keep=Bootblock_Version.o(*) in linker command line when building
//the project if "Remove Unused Sections" is used, otherwise the linker removes
//this object from the BOOTBLOCK images since the constants here are not referenced.


// Z1: 00A9_2750h
// Z2: 04A9_2750h
// A1: 10A9_2750h

const BOOTBLOCK_Version_T bb_version = {
	.VersionDescription =  "\n"
					"BB Basic\n"
					__DATE__
					"\n"
					__TIME__
                                       "\n\0",

	.BootBlockTag    = 'B' | 'O'<<8 | 'O'<<16 | 'T'<<24,

    #define BOOT_DEBUG_VAL 0x00000000

	.BootblockVersion = (BOOT_DEBUG_VAL | 0x0101016)     //ver 10.10.16
};

#undef BOOTBLOCK_VERSION_C

