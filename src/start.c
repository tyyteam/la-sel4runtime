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
#include <sel4/sel4.h>
#include "start.h"

void __sel4runtime_start_main(
	int (*main)(),
	unsigned long argc,
	char **argv,
	char **envp,
	auxv_t auxv[]
) {
	// Simple start to begin with.
	main(argc, argv, envp);
	while (1) {
		seL4_Yield();
	}
}
