/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "parser.h"

void ms_parserInit(Parser* self) {
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

char ms_parserPeekChar(Parser* self) {
	return *self->current_char;
}

char ms_parserPeekNextChar(Parser* self) {
	if (ms_parserPeekChar(self) == '\0') return '\0';
	return *(self->current_char + 1);
}

char ms_parserEatChar(Parser* self) {
	char c = ms_parserPeekChar(self);
	self->current_char++;
	if (c == '\n') self->current_line++;
	return c;
}

bool ms_parserMatchChar(Parser* self, char c) {
	if (ms_parserPeekChar(self) != c) return false;
	ms_parserEatChar(self);
	return true;
}

void ms_parserEatToken(Parser* self, TokenType type) {
	self->next.type = type;
	self->next.start = self->token_start;
	self->next.length = (int)(self->current_char - self->token_start);
	self->next.line = self->current_line - ((type == TK_LINE) ? 1 : 0);
}

void ms_parserEatTwoCharToken(Parser* self, char c, TokenType one,
                              TokenType two) {
	if (ms_parserMatchChar(self, c)) {
		ms_parserEatToken(self, two);
	} else {
		ms_parserEatToken(self, one);
	}
}