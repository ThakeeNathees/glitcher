/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "vm.h"

void* ms_realloc(VM* vm, void* memory, size_t old_size, size_t new_size) {

	// Track the total allocated memory of the VM to trigger the GC.
	vm->bytes_allocated += new_size - old_size;

	// TODO: If vm->bytes_allocated > some_value -> GC();

	if (new_size == 0) {
		free(memory);
		return NULL;
	}

	return realloc(memory, new_size);
}
