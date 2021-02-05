/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "lexer.h"

typedef struct {
	const char* identifier;
	TokenType tk_type;
} _Keyword;

// List of keywords mapped into their identifiers.
static _Keyword _keywords[] = {
	{ "type",     TK_TYPE },
	{ "enum",     TK_ENUM },
	{ "def",      TK_DEF },
	{ "extern",   TK_EXTERN },
	{ "end",      TK_END },
	{ "null",     TK_NULL },
	{ "self",     TK_SELF },
	{ "is",       TK_IS },
	{ "in",       TK_IN },
	{ "and",      TK_AND },
	{ "or",       TK_OR },
	{ "not",      TK_NOT },
	{ "true",     TK_TRUE },
	{ "false",    TK_FALSE },
	{ "while",    TK_WHILE },
	{ "for",      TK_FOR },
	{ "if",       TK_IF },
	{ "elif",     TK_ELIF },
	{ "else",     TK_ELSE },
	{ "break",    TK_BREAK },
	{ "continue", TK_CONTINUE },
	{ "return",   TK_RETURN },
	{ NULL,      (TokenType)(0) }, // Sentinal to mark the end of the array
};


