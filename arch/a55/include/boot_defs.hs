//------------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from ARM Limited or its affiliates.
//
//            (C) COPYRIGHT 2013-2018 ARM Limited or its affiliates.
//                ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from ARM Limited or its affiliates.
//
//            Release Information : CORINTH-MP090-r4p0-00rel0
//
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose:
//   This file contains definitions that are used by multiple files.  It is
//   loaded by files that require these symbols using the ".include" directive.
//-----------------------------------------------------------------------------

// NOTICE: These must all match the values in boot_defs.h

        // Constants
        .equ     MB,             1048576                          // One megabyte
        .equ     KB,             1024                             // One killobyte
//        .equ     STACKHEAP,      0x18100000                       // Base of stack / heap area
//        .equ     REGION,         (256 * MB)                       // How much memory for thread + heap + stack
//        .equ     MAX_THREADS,    32                               // Max 32 threads
//        .equ     THREAD_SIZE,    256                              // bytes of thread local data
        //.equ     CPU_STACK_SIZE, (1 * MB)                         // 1MB per CPU thread
        .equ     CPU_STACK_SIZE, (8 * KB)                         // 1MB per CPU thread
//        .equ     THREAD_AREA,    (MAX_THREADS * THREAD_SIZE)      // Bytes for all possible threads
//        .equ     STACK_SIZE,     (CPU_STACK_SIZE * MAX_THREADS)   // 1MB x 32 = Total 32MB of stack
//        .equ     HEAP_SIZE,      (REGION - STACK_SIZE - THREAD_AREA) // Leftovers
//        .equ     TRICKBOX,       0x13000000                       // Base address of Trickbox

