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

	// Owner of the parser (for reporting errors, etc).
	VM* vm;

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


// Initialize the parser.
void ms_parserInit(Parser* self);

// Returns the current char of the parser on.
char ms_parserPeekChar(Parser* self);

// Returns the next char of the parser on.
char ms_parserPeekNextChar(Parser* self);

// Advance the parser by 1 char.
char ms_parserEatChar(Parser* self);

// If the current char is [c] consume it and advance char by 1 and returns
// true otherwise returns false.
bool ms_parserMatchChar(Parser* self, char c);

// Make and initialize the type as the next token.
void ms_parserEatToken(Parser* self, TokenType type);

// If the current char is [c] eat the char and add token two otherwise eat
// append token one.
void ms_parserEatTwoCharToken(Parser* self, char c, TokenType one,
                              TokenType two);




#endif // PARSER_H
