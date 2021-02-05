/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "lexer.h"
#include "miniscript.h"
#include "var.h"


struct Parser {
	VM* vm;             //< Owner of the parser (for reporting errors, etc).

	const char* source; //< Currently compiled source.

	const char* token_start;  //< Start of the currently parsed token.
	const char* current_char; //< Current char position in the source.
	int current_line;         //< Line number of the current char.

	Token previous, current, next; //< Currently parsed tokens.

	bool has_errors;    //< True if any syntex error occured at compile time.
};


// Initialize the parser.
void parserInit(Parser* self);


#endif // PARSER_H
