/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

 /** A template header to emulate C++ template and every occurence of
  * $name$ will be replaced by the name of the buffer and $type$ will be
  * replaced by the element type of the buffer (by a pre compile script) */

/** Replace the following line with "$name$_buffer.h" */
#include "buffer.template.h"
#include "../utils/utils.h"

void ms_$name$BufferInit($name$Buffer* self) {
	self->data = NULL;
	self->count = 0;
	self->capacity = 0;
}

void ms_$name$BufferClear($name$Buffer* self, VM* vm) {
	ms_realloc(vm, self->data, self->capacity * sizeof($type$), 0);
	self->data = NULL;
	self->count = 0;
	self->capacity = 0;
}

void ms_$name$BufferFill($name$Buffer* self, VM* vm, $type$ data, int count) {
	
	if (self->capacity < self->count + count) {
		int capacity = ms_powerOf2Ceil(self->count + count);
		self->data = ms_realloc(vm, self->data,
			self->capacity * sizeof($type$), capacity * sizeof($type$));
		self->capacity = capacity;
	}

	for (int i = 0; i < count; i++) {
		self->data[self->count++] = data;
	}
}

void ms_$name$BufferWrite($name$Buffer* self, VM* vm, $type$ data) {
	ms_$name$BufferFill(self, vm, data, 1);
}
