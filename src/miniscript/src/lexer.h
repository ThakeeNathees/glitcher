/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "miniscript.h"
#include "var.h"

typedef struct Parser Parser;

typedef enum {

	TK_ERROR = 0,
	TK_EOF,
	TK_LINE,

	// symbols
	TK_DOT,        // .
	TK_DOTDOT,     // ..
	TK_COMMA,       // ,
	TK_COLLON,     // :
	TK_SEMICOLLON, // ;
	TK_HASH,       // #
	TK_LPARAN,     // (
	TK_RPARAN,     // )
	TK_LBRACKET,   // [
	TK_RBRACKET,   // ]
	TK_LBRACE,     // {
	TK_RBRACE,     // }
	TK_PERCENT,    // %

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
	//TK_BANG,       // !  parsed as TK_NOT

	TK_EQEQ,       // ==
	TK_NOTEQ,      // !=
	TK_GTEQ,       // >=
	TK_LTEQ,       // <=

	TK_PLUSEQ,     // +=
	TK_MINUSEQ,    // -=
	TK_STAREQ,     // *=
	TK_DIVEQ,      // /=
	TK_SRIGHT,     // >>
	TK_SLEFT,      // <<

	//TODO:
	// >>= <<=
	//TK_PLUSPLUS,   // ++
	//TK_MINUSMINUS, // --
	//TK_MODEQ,      // %=
	//TK_XOREQ,      // ^=

	// Keywords.
	TK_TYPE,       // type
	TK_ENUM,       // enum
	TK_DEF,        // def
	TK_EXTERN,     // extern (C function declaration)
	TK_END,        // end

	TK_NULL,       // null
	TK_SELF,       // self
	TK_IS,         // is
	TK_IN,         // in
	TK_AND,        // and
	TK_OR,         // or
	TK_NOT,        // not
	TK_TRUE,       // true
	TK_FALSE,      // false

	TK_WHILE,      // while
	TK_FOR,        // for
	TK_IF,         // if
	TK_ELIF,       // elif
	TK_ELSE,       // else
	TK_BREAK,      // break
	TK_CONTINUE,   // continue
	TK_RETURN,     // return

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
	 // TK_STR_INTERP, //< not yet.

} TokenType;


typedef struct {
	TokenType type;

	const char* start; //< Begining of the token in the source.
	int length;        //< Number of chars of the token.
	int line;          //< Line number of the token (1 based).
	Var value;         //< Literal value of the token.
} Token;


// Returns the current char of the parser on.
char lexerPeekChar(Parser* parser);

// Returns the next char of the parser on.
char lexerPeekNextChar(Parser* parser);

// Advance the parser by 1 char.
char lexerEatChar(Parser* parser);

// Read and ignore chars till it reach new line or EOF.
void lexerSkipLineComment(Parser* parser);

// If the current char is [c] consume it and advance char by 1 and returns
// true otherwise returns false.
bool lexerMatchChar(Parser* parser, char c);

// If the current char is [c] eat the char and add token two otherwise eat
// append token one.
void lexerEatTwoCharToken(Parser* parser, char c, TokenType one,
	TokenType two);

// Initialize the next token as the type.
void lexerSetNextToken(Parser* parser, TokenType type);

// Initialize the next token as the type and assign the value.
void lexerSetNextValueToken(Parser* parser, TokenType type, Var value);

// Lex the next token and set it as the next token.
void lexerEatToken(Parser* parser);


#endif // LEXER_H