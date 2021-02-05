/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "lexer.h"
#include "parser.h"
#include "types/gen/byte_buffer.h"
#include "utils.h"

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



static void parserEatString(Parser* parser) {
	ByteBuffer buff;
	byteBufferInit(&buff);

	while (true) {
		char c = lexerEatChar(parser);

		if (c == '"') break;

		if (c == '\0') {
			// TODO: syntaxError()

			// Null byte is required by TK_EOF.
			parser->current_char--;
			break;
		}

		if (c == '\\') {
			switch (lexerEatChar(parser)) {
				case '"': byteBufferWrite(&buff, parser->vm, '"'); break;
				case '\\': byteBufferWrite(&buff, parser->vm, '\\'); break;
				case 'n': byteBufferWrite(&buff, parser->vm, '\n'); break;
				case 'r': byteBufferWrite(&buff, parser->vm, '\r'); break;
				case 't': byteBufferWrite(&buff, parser->vm, '\t'); break;

				default:
					// TODO: syntaxError("Error: invalid escape character")
					break;
			}
		} else {
			byteBufferWrite(&buff, parser->vm, c);
		}
	}

	// '\0' will be added by varNewSring();
	Var string = VAR_OBJ(&varNewString(parser->vm, (const char*)buff.data,
		buff.count)->_super);

	byteBufferClear(&buff, parser->vm);

	lexerSetNextValueToken(parser, TK_STRING, string);
}

char lexerPeekChar(Parser* parser) {
	return *parser->current_char;
}

char lexerPeekNextChar(Parser* parser) {
	if (lexerPeekChar(parser) == '\0') return '\0';
	return *(parser->current_char + 1);
}

char lexerEatChar(Parser* parser) {
	char c = lexerPeekChar(parser);
	parser->current_char++;
	if (c == '\n') parser->current_line++;
	return c;
}

void lexerSkipLineComment(Parser* parser) {
	char c = lexerEatChar(parser);

	while (c != '\n' && c != '\0') {
		c = lexerEatChar(parser);
	}
}

bool lexerMatchChar(Parser* parser, char c) {
	if (lexerPeekChar(parser) != c) return false;
	lexerEatChar(parser);
	return true;
}

void lexerEatTwoCharToken(Parser* parser, char c, TokenType one,
	TokenType two) {
	if (lexerMatchChar(parser, c)) {
		lexerSetNextToken(parser, two);
	} else {
		lexerSetNextToken(parser, one);
	}
}

void lexerSetNextToken(Parser* parser, TokenType type) {
	parser->next.type = type;
	parser->next.start = parser->token_start;
	parser->next.length = (int)(parser->current_char - parser->token_start);
	parser->next.line = parser->current_line - ((type == TK_LINE) ? 1 : 0);
}

void lexerSetNextValueToken(Parser* parser, TokenType type, Var value) {
	lexerSetNextToken(parser, type);
	parser->next.value = value;
}

void lexerEatToken(Parser* parser) {
	parser->previous = parser->current;
	parser->current = parser->next;

	if (parser->current.type == TK_EOF) return;

	while (lexerPeekChar(parser) != '\0') {
		parser->token_start = parser->current_char;
		char c = lexerEatChar(parser);

		switch (c) {
			case ',': lexerSetNextToken(parser, TK_COMMA); return;
			case ':': lexerSetNextToken(parser, TK_COLLON); return;
			case ';': lexerSetNextToken(parser, TK_SEMICOLLON); return;
			case '#': lexerSetNextToken(parser, TK_HASH); return;
			case '(': lexerSetNextToken(parser, TK_LPARAN); return;
			case ')': lexerSetNextToken(parser, TK_RPARAN); return;
			case '[': lexerSetNextToken(parser, TK_LBRACKET); return;
			case ']': lexerSetNextToken(parser, TK_RBRACKET); return;
			case '{': lexerSetNextToken(parser, TK_LBRACE); return;
			case '}': lexerSetNextToken(parser, TK_RBRACE); return;
			case '%': lexerSetNextToken(parser, TK_PERCENT); return;

			case '\n': lexerSetNextToken(parser, TK_LINE); return;

			case ' ':
			case '\t':
			case '\r': {
				char c = lexerPeekChar(parser);
				while (c == ' ' || c == '\t' || c == '\r') {
					lexerEatChar(parser);
					c = lexerPeekChar(parser);
				}
				break;
			}

			case '.': // TODO: ".5" should be a valid number.
				lexerEatTwoCharToken(parser, '.', TK_DOT, TK_DOTDOT);
				return;

			case '=':
				lexerEatTwoCharToken(parser, '=', TK_EQ, TK_EQEQ);
				return;

			case '!':
				lexerEatTwoCharToken(parser, '=', TK_NOT, TK_NOTEQ);
				return;

			case '>':
				if (lexerMatchChar(parser, '>'))
					lexerSetNextToken(parser, TK_SRIGHT);
				else
					lexerEatTwoCharToken(parser, '=', TK_GT, TK_GTEQ);
				return;

			case '<':
				if (lexerMatchChar(parser, '<'))
					lexerSetNextToken(parser, TK_SLEFT);
				else
					lexerEatTwoCharToken(parser, '=', TK_LT, TK_LTEQ);
				return;

			case '+':
				lexerEatTwoCharToken(parser, '=', TK_PLUS, TK_PLUSEQ);
				return;

			case '-':
				lexerEatTwoCharToken(parser, '=', TK_MINUS, TK_MINUSEQ);
				return;

			case '*':
				lexerEatTwoCharToken(parser, '=', TK_STAR, TK_STAREQ);
				return;

			case '/':
				lexerEatTwoCharToken(parser, '=', TK_FSLASH, TK_DIVEQ);
				return;

			case '"': parserEatString(parser); return;

			default: {

				if (utilIsDigit(c)) {

				} else if (utilIsName(c)) {

				} else {
					if (c >= 32 && c <= 126) {
						// TODO: syntaxError("Invalid character %c", c);
					} else {
						// TODO: syntaxError("Invalid byte 0x%x", (uint8_t)c);
					}
					lexerSetNextToken(parser, TK_ERROR);
				}
				return;
			}
		}
	}

	lexerSetNextToken(parser, TK_EOF);
	parser->next.start = parser->current_char;
}
