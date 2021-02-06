/*
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "var.h"
#include "vm.h"

void varInitObject(Object* self, VM* vm, ObjectType type) {
	self->type = type;
	self->next = vm->first;
	vm->first = self;
	// TODO: set isGray = false;
}

#if VAR_NAN_TAGGING
// A union to reinterpret a double as raw bits and back.
typedef union {
	uint64_t bits64;
	uint32_t bits32[2];
	double num;
} _DoubleBitsConv;
#endif

Var varDoubleToVar(double value) {
#if VAR_NAN_TAGGING
	_DoubleBitsConv bits;
	bits.num = value;
	return bits.bits64;
#else
	// TODO:
#endif // VAR_NAN_TAGGING
}

static inline double varVarToDouble(Var value) {
#if VAR_NAN_TAGGING
	_DoubleBitsConv bits;
	bits.bits64 = value;
	return bits.num;
#else
	// TODO:
#endif // VAR_NAN_TAGGING
}

String* varNewString(VM* vm, const char* text, uint32_t length) {

	ASSERT(length == 0 || text != NULL, "Unexpected NULL string.");

	String* string = ALLOCATE_DYNAMIC(vm, String, length + 1, char);
	varInitObject(&string->_super, vm, OBJ_STRING);
	string->length = length;

	if (length != 0) memcpy(string->data, text, length);
	string->data[length] = '\0';
	return string;
}
