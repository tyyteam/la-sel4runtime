/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
#include "elf_helper.h"

// seL4-specific auxiliary vector values.
#define AT_SEL4_BOOT_INFO         64
#define AT_SEL4_CSPACE_DESCRIPTOR 65
#define AT_SEL4_VSYSCALL          66
#define AT_SEL4_IPC_BUFFER_PTR    67
#define AT_SEL4_IPC_BUFFER        68
#define AT_SEL4_TCB               69
#define AT_SEL4_CNODE             70
#define AT_SEL4_VSPACE            71
#define AT_SEL4_ASID_POOL         72