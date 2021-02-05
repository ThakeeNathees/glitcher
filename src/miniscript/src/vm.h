/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef VM_H
#define VM_H

#include "common.h"
#include "miniscript.h"
#include "utils/utf8.h"
#include "utils/var.h"

struct VM {
	size_t bytes_allocated;
};

// A realloc wrapper which handles memory allocations of the VM.
// - To allocate new memory pass NULL to parameter [memory] and 0 to
//   parameter [old_size] on failure it'll return NULL.
// - To free an already allocated memory pass 0 to parameter [old_size]
//   and it'll returns NULL.
// - The [old_size] parameter is required to keep track of the VM's
//    allocations to trigger the garbage collections.
void* ms_realloc(VM* vm, void* memory, size_t old_size, size_t new_size);

#endif // VM_H