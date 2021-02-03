/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "lexer.h"
#include "miniscript.h"
#include "utils/utf8.h"
#include "utils/var.h"

typedef struct Parser {

	/* TODO:
	 *     VM* vm; // owner vm for reporting errors
	 */

	 // Currently compiled source.
	const char* source;

	// Start of the currently parsed token.
	const char* token_start;

	// Current char position in the source.
	const char* current_char;

	// Line number of the current char.
	int current_line;

	// Currently parsed tokens
	Token previous, current, next;

	// True if any syntex error occured at compile time.
	bool has_errors;
} Parser;

/** Parser function declarations. */

#endif // PARSER_H
