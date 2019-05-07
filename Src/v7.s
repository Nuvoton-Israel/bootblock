//-----------------------------------------------------------
// SPDX-License-Identifier: GPL-2.0
//
// Copyright ARM Ltd 2009. All rights reserved.
// Copyright (c) 2010-2019 by Nuvoton Technology Corporation
// All rights reserved
//
//-----------------------------------------------------------

// ------------------------------------------------------------
// v7-A Cache and Branch Prediction Maintenance Operations
// ------------------------------------------------------------

  .section v7CacheOpp, "a"


// ------------------------------------------------------------
// Cache Maintenance
// ------------------------------------------------------------

  .global enable_instruction_caches
  // void enable_instruction_caches(void);
enable_instruction_caches:
  MRC     p15, 0, r0, c1, c0, 0   // Read System Control Register configuration data
  ORR     r0, r0, #(1 << 12)      // Set I bit
  MCR     p15, 0, r0, c1, c0, 0   // Write System Control Register configuration data
  BX      lr


  .global disable_instruction_caches
  // void disable_instruction_caches(void)
disable_instruction_caches:
  MRC     p15, 0, r0, c1, c0, 0   // Read System Control Register configuration data
  BIC     r0, r0, #(1 << 12)      // Clear I bit
  MCR     p15, 0, r0, c1, c0, 0   // Write System Control Register configuration data
  BX      lr


    .global invalidate_instruction_caches
	// void invalidate_instruction_caches(void);
invalidate_instruction_caches:
  MOV     r0, #0
  MCR     p15, 0, r0, c7, c5, 0     // ICIALLU - Invalidate entire I Cache, and flushes branch target cache
  BX      lr


  .global enable_caches
  // void enable_caches(void);
enable_caches:
  MRC     p15, 0, r0, c1, c0, 0   // Read System Control Register configuration data
  ORR     r0, r0, #(1 << 2)       // Set C bit
  ORR     r0, r0, #(1 << 12)      // Set I bit
  MCR     p15, 0, r0, c1, c0, 0   // Write System Control Register configuration data
  BX      lr


  .global disable_caches
  // void disable_caches(void)
disable_caches:
  MRC     p15, 0, r0, c1, c0, 0   // Read System Control Register configuration data
  BIC     r0, r0, #(1 << 2)       // Clear C bit
  BIC     r0, r0, #(1 << 12)      // Clear I bit
  MCR     p15, 0, r0, c1, c0, 0   // Write System Control Register configuration data
  BX      lr


  .global clean_dcache
  // void clean_dcache(void);
clean_dcache:
  PUSH    {r4-r12}

  //
  // Based on code example given in section 11.2.4 of ARM DDI 0406B
  //

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     clean_dcache_finished
  MOV     r10, #0

clean_dcache_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     clean_dcache_skip         // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7               // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

clean_dcache_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

clean_dcache_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c10, 2   // DCCSW - clean by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     clean_dcache_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     clean_dcache_loop2

clean_dcache_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     clean_dcache_loop1

clean_dcache_finished:
  POP     {r4-r12}

  BX      lr

  .global clean_invalidate_dcache
  // void clean_invalidate_dcache(void);
clean_invalidate_dcache:
  PUSH    {r4-r12}

  //
  // Based on code example given in section 11.2.4 of ARM DDI 0406B
  //

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     clean_invalidate_dcache_finished
  MOV     r10, #0

clean_invalidate_dcache_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     clean_invalidate_dcache_skip // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7                // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

clean_invalidate_dcache_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

clean_invalidate_dcache_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c14, 2   // DCCISW - clean and invalidate by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     clean_invalidate_dcache_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     clean_invalidate_dcache_loop2

clean_invalidate_dcache_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     clean_invalidate_dcache_loop1

clean_invalidate_dcache_finished:
  POP     {r4-r12}

  BX      lr


  .global invalidate_caches
  // void invalidate_caches(void);
invalidate_caches:
  PUSH    {r4-r12}

  //
  // Based on code example given in section B2.2.4/11.2.4 of ARM DDI 0406B
  //

  MOV     r0, #0
  MCR     p15, 0, r0, c7, c5, 0     // ICIALLU - Invalidate entire I Cache, and flushes branch target cache

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     invalidate_caches_finished
  MOV     r10, #0

invalidate_caches_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     invalidate_caches_skip    // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7                // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

invalidate_caches_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

invalidate_caches_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c6, 2    // DCISW - invalidate by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     invalidate_caches_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     invalidate_caches_loop2

invalidate_caches_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     invalidate_caches_loop1

invalidate_caches_finished:
  POP     {r4-r12}
  BX      lr


  .global invalidate_caches_is
  // void invalidate_caches_is(void);
invalidate_caches_is:
  PUSH    {r4-r12}

  MOV     r0, #0
  MCR     p15, 0, r0, c7, c1, 0     // ICIALLUIS - Invalidate entire I Cache inner shareable

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     invalidate_caches_is_finished
  MOV     r10, #0

invalidate_caches_is_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     invalidate_caches_is_skip // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7                // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

invalidate_caches_is_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

invalidate_caches_is_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c6, 2    // DCISW - clean by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     invalidate_caches_is_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     invalidate_caches_is_loop2

invalidate_caches_is_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     invalidate_caches_is_loop1

invalidate_caches_is_finished:
  POP     {r4-r12}
  BX      lr

// ------------------------------------------------------------
// TLB
// ------------------------------------------------------------

  .global invalidate_unified_tlb
  // void invalidate_unified_tlb(void);
invalidate_unified_tlb:
  MOV     r0, #1
  MCR     p15, 0, r0, c8, c7, 0                 // TLBIALL - Invalidate entire unified TLB
  BX      lr

  .global invalidate_unified_tlb_is
  // void invalidate_unified_tlb_is(void);
invalidate_unified_tlb_is:
  MOV     r0, #1
  MCR     p15, 0, r0, c8, c3, 0                 // TLBIALLIS - Invalidate entire unified TLB Inner Shareable
  BX      lr

// ------------------------------------------------------------
// Branch Prediction
// ------------------------------------------------------------

  .global enable_branch_prediction
  // void enable_branch_prediction(void);
enable_branch_prediction:
  MRC     p15, 0, r0, c1, c0, 0                 // Read SCTLR
  ORR     r0, r0, #(1 << 11)                    // Set the Z bit (bit 11)
  MCR     p15, 0,r0, c1, c0, 0                  // Write SCTLR
  BX      lr

  .global disable_branch_prediction
  // void disable_branch_prediction(void)
disable_branch_prediction:
  MRC     p15, 0, r0, c1, c0, 0                 // Read SCTLR
  BIC     r0, r0, #(1 << 11)                    // Clear the Z bit (bit 11)
  MCR     p15, 0,r0, c1, c0, 0                  // Write SCTLR
  BX      lr

  .global invalidate_branch_target_cache
  // void invalidate_branch_target_cache(void)
invalidate_branch_target_cache:
  MOV     r0, #0
  MCR     p15, 0, r0, c7, c5, 6                 // BPIALL - Invalidate entire branch predictor array
  BX      lr

  .global invalidate_branch_target_cache_is
  // void invalidate_branch_target_cache_is(void)
invalidate_branch_target_cache_is:
  MOV     r0, #0
  MCR     p15, 0, r0, c7, c1, 6                 // BPIALLIS - Invalidate entire branch predictor array Inner Shareable
  BX      lr

// ------------------------------------------------------------
// High Vecs
// ------------------------------------------------------------

  .global enable_highvecs
  // void enable_highvecs(void);
enable_highvecs:
  MRC     p15, 0, r0, c1, c0, 0 // Read Control Register
  ORR     r0, r0, #(1 << 13)    // Set the V bit (bit 13)
  MCR     p15, 0, r0, c1, c0, 0 // Write Control Register
  BX      lr

  .global disable_highvecs
  // void disable_highvecs(void);
disable_highvecs:
  MRC     p15, 0, r0, c1, c0, 0 // Read Control Register
  BIC     r0, r0, #(1 << 13)    // Clear the V bit (bit 13)
  MCR     p15, 0, r0, c1, c0, 0 // Write Control Register
  BX      lr

// ------------------------------------------------------------
// Context ID
// ------------------------------------------------------------

  .global get_context_id
  // uint32_t get_context_id(void);
get_context_id:
  MRC     p15, 0, r0, c13, c0, 1 // Read Context ID Register
  BX      lr

  .global set_context_id
  // void set_context_id(uint32_t);
set_context_id:
  MCR     p15, 0, r0, c13, c0, 1 // Write Context ID Register
  BX      lr

// ------------------------------------------------------------
// ID registers
// ------------------------------------------------------------

  .global get_MIDR
  // uint32_t get_MIDR(void);
get_MIDR:
  MRC     p15, 0, r0, c0, c0, 0 // Read Main ID Register (MIDR)
  BX      lr

  .global get_MPIDR
  // uint32_t get_MPIDR(void);
get_MPIDR:
  MRC     p15, 0, r0, c0 ,c0, 5// Read Multiprocessor ID register (MPIDR)
  BX      lr


//
//   jump to 0x80000200 (at the flash. Used for tester.
//
    .global asm_jump_to_address
  // void asm_jump_to_address(uint32 address);
asm_jump_to_address:
  PUSH    {LR}                // first parameter of Check_ROMCode_Status is LR of ROM code, will be poped into r0
  ISB                               // ISB to sync
  BLX     r0
  POP     {LR}                // restore LR from stack
  BX      lr



      .global asm_halt
  // void asm_halt(void);
asm_halt:
  ISB                              // ISB to sync
  CPSID i                          // Disable interrupts
halt4ever:
  WFI
  B      halt4ever


// ------------------------------------------------------------
// End of code
// ------------------------------------------------------------

  .end

// ------------------------------------------------------------
// End of v7.s
// ------------------------------------------------------------

