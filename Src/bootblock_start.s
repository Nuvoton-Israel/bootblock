//-----------------------------------------------------------
// SPDX-License-Identifier: GPL-2.0
//
// Copyright ARM Ltd 2009. All rights reserved.
// Copyright (c) 2010-2019 by Nuvoton Technology Corporation
// All rights reserved
//
//-----------------------------------------------------------


// -----------------------------------------------------------------------------
// --- Start Code
//-----------------------------------------------------------------------------
		.global bootblock_main
		.global disable_caches
		.global invalidate_caches
		.global enable_instruction_caches
// #####################################################################################################

	// ------------------------------------------------------------
	// Reset Handler - Generic initialization, run by all CPUs
	// ------------------------------------------------------------

	.global Check_ROMCode_Status
	.global BootBlockStart


BootBlockStart:

		PUSH    {r0-r12}            // Push all user registers into the stack
		PUSH    {r8}                // second parameter of Check_ROMCode_Status if to do signature check, in Z1 r8 holds this information will be poped into r1
		PUSH    {LR}                // first parameter of Check_ROMCode_Status is LR of ROM code, will be poped into r0

		BL      disable_caches
		BL      invalidate_caches
		BL      enable_instruction_caches

		POP     {r0}                // first parameter of Check_ROMCode_Status is LR of ROM code, was pushed from LR
		POP     {r1}                // second parameter of Check_ROMCode_Status if to do signature check, in Z1 r8 holds this information, was pushed from r8
		BL      Check_ROMCode_Status
		PUSH    {r0}                // Check_ROMCode_Status returns the LR address that might be modified


continue_boot:

		//
		// Branch to C lib code
		// ----------------------
		BL       bootblock_main

		POP     {LR}                // restore LR from stack
		POP     {r0-r12}            // Restore stacked registers
		MOV     PC, LR              // return to ROM code


	// ------------------------------------------------------------
	// End of code
	// ------------------------------------------------------------

  .end

// ------------------------------------------------------------
// End of startup.s
// ------------------------------------------------------------

