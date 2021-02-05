/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef VM_H
#define VM_H

#include "common.h"
#include "miniscript.h"
#include "var.h"

// The maximum number of temporary object reference to protect them from being
// garbage collected.
#define MAX_TEMP_REFERENCE 8

struct VM {

	// Native class instances.
	Class* class_null;
	Class* class_bool;
	Class* class_num;

	Class* class_string;
	Class* class_array;
	Class* class_map;
	Class* class_range;

	Class* class_object;
	Class* class_class;

	// The first object in the link list of all heap allocated objects.
	Object* first;

	size_t bytes_allocated;

	// A stack of temporary object references to ensure that the object
	// doesn't garbage collected.
	Object* temp_reference[MAX_TEMP_REFERENCE];
	int temp_reference_count;
};

// A realloc wrapper which handles memory allocations of the VM.
// - To allocate new memory pass NULL to parameter [memory] and 0 to
//   parameter [old_size] on failure it'll return NULL.
// - To free an already allocated memory pass 0 to parameter [old_size]
//   and it'll returns NULL.
// - The [old_size] parameter is required to keep track of the VM's
//    allocations to trigger the garbage collections.
void* vmRealloc(VM* self, void* memory, size_t old_size, size_t new_size);

// Push the object to temporary references stack.
void vmPushTempRef(VM* self, Object* obj);

// Pop the top most object from temporary reference stack.
void vmPopTempRef(VM* self);

#endif // VM_H