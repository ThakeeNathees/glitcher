/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "parser.h"


void parserInit(Parser* self) {
	self->vm = NULL;
	self->source = NULL;
	self->token_start = NULL;
	self->current_char = NULL;
	self->current_line = 1;
	self->has_errors = false;

	self->next.type = TK_ERROR;
	self->next.start = NULL;
	self->next.length = 0;
	self->next.line = 1;
	self->next.value = VAR_UNDEFINED;
}



