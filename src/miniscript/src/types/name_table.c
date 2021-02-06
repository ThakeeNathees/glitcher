/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "name_table.h"
#include "../var.h"
#include "../vm.h"

void nameTableInit(NameTable* self) {
	stringBufferInit(self);
}

void nameTableClear(NameTable* self, VM* vm) {
	stringBufferClear(self, vm);
}

int nameTableAdd(NameTable* self, VM* vm, const char* name, size_t length) {
	String* string = varNewString(vm, name, (uint32_t)length);

	vmPushTempRef(vm, &string->_super);
	stringBufferWrite(self, vm, string);
	vmPopTempRef(vm);

	return (int)(self->count - 1);
}
