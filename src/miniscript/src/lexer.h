/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "miniscript.h"
#include "utils/utf8.h"
#include "utils/var.h"

typedef enum {

	TK_UNKNOWN = 0,
	TK_EOF,
	TK_LINE,

	// symbols
	TK_DOT,        // .
	TK_DOTDOT,     // ..
	TK_COMA,       // ,
	TK_COLLON,     // :
	TK_SEMICOLLON, // ;
	TK_HASH,       // #
	TK_LPARAN,     // (
	TK_RPARAN,     // )
	TK_LBRACKET,   // [
	TK_RBRACKET,   // ]
	TK_LBRACE,     // {
	TK_RBRACE,     // }
	TK_PRECENT,    // %

	TK_AMP,        // &
	TK_PIPE,       // |
	TK_CARET,      // ^

	TK_PLUS,       // +
	TK_MINUS,      // -
	TK_STAR,       // *
	TK_FSLASH,     // /
	TK_BSLASH,     // \.
	TK_EQ,         // =
	TK_GT,         // >
	TK_LT,         // <
	TK_BANG,       // !

	TK_PLUSPLUS,   // ++
	TK_MINUSMINUS, // --
	TK_PLUSEQ,     // +=
	TK_MINUSEQ,    // -=
	TK_STAREQ,     // *=
	TK_DIVEQ,      // /=
	TK_EQEQ,       // ==
	TK_GTEQ,       // >=
	TK_LTEQ,       // <=
	TK_MODEQ,      // %=
	TK_NOTEQ,      // !=
	TK_XOREQ,      // ^=
	TK_SRIGHT,     // >>
	TK_SLEFT,      // <<
	//TODO: >>= <<=

	// keywords
	TK_TYPE,
	TK_ENUM,
	TK_DEF,
	TK_EXTERN,     // C function declaration
	TK_END,

	TK_NULL,
	TK_SELF,
	TK_IS,
	TK_IN,
	TK_AND,
	TK_OR,
	TK_NOT,
	TK_TRUE,
	TK_FALSE,

	TK_WHILE,
	TK_FOR,
	TK_IF,
	TK_ELIF,
	TK_ELSE,
	TK_BREAK,
	TK_CONTINUE,
	TK_RETURN,

	TK_NAME,       // identifier

	TK_NUMBER,     // number literal
	TK_STRING,     // string literal

	/* String interpolation (reference wren-lang)
	 * but it doesn't support recursive ex: "a \(b + "\(c)")"
	 *  "a \(b) c \(d) e"
	 * tokenized as:
	 *   TK_STR_INTERP  "a "
	 *   TK_NAME        b
	 *   TK_STR_INTERP  " c "
	 *   TK_NAME        d
	 *   TK_STRING     " e" */
	TK_STR_INTERP,

} TokenType;

typedef struct Token {
	TokenType type;

	// Begining of the token in the source.
	const char* start;

	// Number of chars of the token.
	int length;

	// Literal value of the token.
	var value;
} Token;

/** Lexer function declarations. */

/** Returns true if `c` is [A-Za-z_]. */
bool ms_isName(char c);

/** Returns true if `c` is [0-9] */
bool ms_isDigit(char c);

#endif // LEXER_H
