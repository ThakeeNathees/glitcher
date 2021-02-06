/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "compiler.h"

#include "types/gen/byte_buffer.h"
#include "utils.h"

// The maximum number of variables (or global if compiling top level script) 
// to lookup from the compiling context. Also it's limited by it's opcode 
// which is using a single byte value to identify the local.
#define MAX_VARIABLES 256

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

	TK_TILD,       // ~
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
	//TK_TYPE,       // type
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

	// Type names for is test.
	// TK_NULL already defined.
	TK_BOOL_T,    // Bool
	TK_NUM_T,     // Num
	TK_STRING_T,  // String
	TK_ARRAY_T,   // Array
	TK_MAP_T,     // Map
	TK_RANGE_T,   // Range
	TK_FUNC_T,    // Function 
	TK_OBJ_T,     // Object (self, user data, etc.)

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

typedef struct {
	const char* identifier;
	TokenType tk_type;
} _Keyword;

// List of keywords mapped into their identifiers.
static _Keyword _keywords[] = {
	//{ "type",     TK_TYPE },
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

	// Type names.
	{ "Bool",     TK_BOOL_T },
	{ "Num",      TK_NUM_T },
	{ "String",   TK_STRING_T },
	{ "Array",    TK_ARRAY_T },
	{ "Map",      TK_MAP_T },
	{ "Range",    TK_RANGE_T },
	{ "Object",   TK_OBJ_T },
	{ "Function", TK_FUNC_T },

	{ NULL,      (TokenType)(0) }, // Sentinal to mark the end of the array
};

typedef struct {
	VM* vm;             //< Owner of the parser (for reporting errors, etc).

	const char* source; //< Currently compiled source.

	const char* token_start;  //< Start of the currently parsed token.
	const char* current_char; //< Current char position in the source.
	int current_line;         //< Line number of the current char.

	Token previous, current, next; //< Currently parsed tokens.

	bool has_errors;    //< True if any syntex error occured at compile time.
} Parser;

// Compiler Types ////////////////////////////////////////////////////////////

typedef struct Compiler Compiler;

// Precedence parsing references:
// https://en.wikipedia.org/wiki/Shunting-yard_algorithm
// TODO: I should explicitly state wren-lang as a reference "globaly".

typedef enum {
	PREC_NONE,
	PREC_LOWEST,
	PREC_ASSIGNMENT,    // =
	PREC_LOGICAL_OR,    // or
	PREC_LOGICAL_AND,   // and
	PREC_LOGICAL_NOT,   // not
	PREC_EQUALITY,      // == !=
	PREC_IN,            // in
	PREC_IS,            // is
	PREC_COMPARISION,   // < > <= >=
	PREC_BITWISE_OR,    // |
	PREC_BITWISE_XOR,   // ^
	PREC_BITWISE_AND,   // &
	PREC_BITWISE_SHIFT, // << >>
	PREC_RANGE,         // ..
	PREC_TERM,          // + -
	PREC_FACTOR,        // * / %
	PREC_UNARY,         // - ! ~
	PREC_CALL,          // ()
	PREC_SUBSCRIPT,     // []
	PREC_ATTRIB,        // .index
	PREC_PRIMARY,
} Precedence;

typedef void (*GrammarFn)(Compiler* compiler, bool can_assign);

typedef struct {
	GrammarFn prefix;
	GrammarFn infix;
	Precedence precedence;
} GrammarRule;

typedef struct {
	const char* name; //< Directly points into the source string.
	int length;       //< Length of the name.
	int depth;        //< The depth the local is defined in. (-1 means global)
} Variable;

typedef struct sLoop {

	// Index of the loop's start instruction where the execution will jump
	// back to once it reach the loop end.
	int start;

	// Index of the jump out address instruction to patch it's value once done
	// compiling the loop.
	int exit_jump;

	// Index of the first body instruction. Needed to start patching jump
	// address from which till the loop end.
	int body;

	// The outer loop of the current loop used to set and reset the compiler's
	// current loop context.
	struct sLoop* outer_loop;

} Loop;

struct Compiler {

	Parser parser;

	// Current depth the compiler in (-1 means top level) 0 means function
	// level and > 0 is inner scope.
	int scope_depth;

	Variable variables[MAX_VARIABLES]; //< Variables in the current context.
	int locals_count;                  //< Number of locals in [variables].

	Loop* loop;   //< Current loop.
	Function* fn; //< Current function.

};

/*****************************************************************************
 * LEXING                                                                    *
 *****************************************************************************/

// Forward declaration of lexer methods.

static char eatChar(Parser* parser);
static void setNextValueToken(Parser* parser, TokenType type, Var value);
static void setNextToken(Parser* parser, TokenType type);
static bool matchChar(Parser* parser, char c);

static void eatString(Parser* parser) {
	ByteBuffer buff;
	byteBufferInit(&buff);

	while (true) {
		char c = eatChar(parser);

		if (c == '"') break;

		if (c == '\0') {
			// TODO: syntaxError()

			// Null byte is required by TK_EOF.
			parser->current_char--;
			break;
		}

		if (c == '\\') {
			switch (eatChar(parser)) {
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
		(uint32_t)buff.count)->_super);

	byteBufferClear(&buff, parser->vm);

	setNextValueToken(parser, TK_STRING, string);
}

// Returns the current char of the parser on.
static char peekChar(Parser* parser) {
	return *parser->current_char;
}

// Returns the next char of the parser on.
static char peekNextChar(Parser* parser) {
	if (peekChar(parser) == '\0') return '\0';
	return *(parser->current_char + 1);
}

// Advance the parser by 1 char.
static char eatChar(Parser* parser) {
	char c = peekChar(parser);
	parser->current_char++;
	if (c == '\n') parser->current_line++;
	return c;
}

// Complete lexing an identifier name.
static void eatName(Parser* parser) {

	char c = peekChar(parser);
	while (utilIsName(c) || utilIsDigit(c)) {
		eatChar(parser);
		c = peekChar(parser);
	}

	const char* name_start = parser->token_start;

	TokenType type = TK_NAME;

	int length = (int)(parser->current_char - name_start);
	for (int i = 0; _keywords[i].identifier != NULL; i++) {
		if (strncmp(name_start, _keywords[i].identifier, length) == 0) {
			type = _keywords[i].tk_type;
			break;
		}
	}

	setNextToken(parser, type);
}

// Complete lexing a number literal.
static void eatNumber(Parser* parser) {

	// TODO: hex, binary and scientific literals.

	while (utilIsDigit(peekChar(parser)))
		eatChar(parser);

	if (matchChar(parser, '.')) {
		while (utilIsDigit(peekChar(parser)))
			eatChar(parser);
	}

	Var value = VAR_NUM(strtod(parser->token_start, NULL));
	setNextValueToken(parser, TK_NUMBER, value);
}

// Read and ignore chars till it reach new line or EOF.
static void skipLineComment(Parser* parser) {
	char c = eatChar(parser);

	while (c != '\n' && c != '\0') {
		c = eatChar(parser);
	}
}

// If the current char is [c] consume it and advance char by 1 and returns
// true otherwise returns false.
static bool matchChar(Parser* parser, char c) {
	if (peekChar(parser) != c) return false;
	eatChar(parser);
	return true;
}

// If the current char is [c] eat the char and add token two otherwise eat
// append token one.
static void setNextTwoCharToken(Parser* parser, char c, TokenType one,
	TokenType two) {
	if (matchChar(parser, c)) {
		setNextToken(parser, two);
	} else {
		setNextToken(parser, one);
	}
}

// Initialize the next token as the type.
static void setNextToken(Parser* parser, TokenType type) {
	parser->next.type = type;
	parser->next.start = parser->token_start;
	parser->next.length = (int)(parser->current_char - parser->token_start);
	parser->next.line = parser->current_line - ((type == TK_LINE) ? 1 : 0);
}

// Initialize the next token as the type and assign the value.
static void setNextValueToken(Parser* parser, TokenType type, Var value) {
	setNextToken(parser, type);
	parser->next.value = value;
}

// lexToken function's internal method.
static void _lexToken_internal(Parser* parser) {
	parser->previous = parser->current;
	parser->current = parser->next;

	if (parser->current.type == TK_EOF) return;

	while (peekChar(parser) != '\0') {
		parser->token_start = parser->current_char;
		char c = eatChar(parser);

		switch (c) {
			case ',': setNextToken(parser, TK_COMMA); return;
			case ':': setNextToken(parser, TK_COLLON); return;
			case ';': setNextToken(parser, TK_SEMICOLLON); return;
			case '#': setNextToken(parser, TK_HASH); return;
			case '(': setNextToken(parser, TK_LPARAN); return;
			case ')': setNextToken(parser, TK_RPARAN); return;
			case '[': setNextToken(parser, TK_LBRACKET); return;
			case ']': setNextToken(parser, TK_RBRACKET); return;
			case '{': setNextToken(parser, TK_LBRACE); return;
			case '}': setNextToken(parser, TK_RBRACE); return;
			case '%': setNextToken(parser, TK_PERCENT); return;

			case '~': setNextToken(parser, TK_TILD); return;
			case '&': setNextToken(parser, TK_AMP); return;
			case '|': setNextToken(parser, TK_PIPE); return;
			case '^': setNextToken(parser, TK_CARET); return;

			case '\n': setNextToken(parser, TK_LINE); return;

			case ' ':
			case '\t':
			case '\r': {
				char c = peekChar(parser);
				while (c == ' ' || c == '\t' || c == '\r') {
					eatChar(parser);
					c = peekChar(parser);
				}
				break;
			}

			case '.': // TODO: ".5" should be a valid number.
				setNextTwoCharToken(parser, '.', TK_DOT, TK_DOTDOT);
				return;

			case '=':
				setNextTwoCharToken(parser, '=', TK_EQ, TK_EQEQ);
				return;

			case '!':
				setNextTwoCharToken(parser, '=', TK_NOT, TK_NOTEQ);
				return;

			case '>':
				if (matchChar(parser, '>'))
					setNextToken(parser, TK_SRIGHT);
				else
					setNextTwoCharToken(parser, '=', TK_GT, TK_GTEQ);
				return;

			case '<':
				if (matchChar(parser, '<'))
					setNextToken(parser, TK_SLEFT);
				else
					setNextTwoCharToken(parser, '=', TK_LT, TK_LTEQ);
				return;

			case '+':
				setNextTwoCharToken(parser, '=', TK_PLUS, TK_PLUSEQ);
				return;

			case '-':
				setNextTwoCharToken(parser, '=', TK_MINUS, TK_MINUSEQ);
				return;

			case '*':
				setNextTwoCharToken(parser, '=', TK_STAR, TK_STAREQ);
				return;

			case '/':
				setNextTwoCharToken(parser, '=', TK_FSLASH, TK_DIVEQ);
				return;

			case '"': eatString(parser); return;

			default: {

				if (utilIsDigit(c)) {
					// TODO:
				} else if (utilIsName(c)) {
					eatName(parser);
				} else {
					if (c >= 32 && c <= 126) {
						// TODO: syntaxError("Invalid character %c", c);
					} else {
						// TODO: syntaxError("Invalid byte 0x%x", (uint8_t)c);
					}
					setNextToken(parser, TK_ERROR);
				}
				return;
			}
		}
	}

	setNextToken(parser, TK_EOF);
	parser->next.start = parser->current_char;
}

// Lex the next token and set it as the next token and returns the current
// token which will be the previous token after lexToken().
static TokenType lexToken(Parser* parser) {
	_lexToken_internal(parser);
	return parser->previous.type;
}


/*****************************************************************************
 * PARSING                                                                   *
 *****************************************************************************/

 // Initialize the parser.
static void parserInit(Parser* self) {
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

// Returns current token type.
static TokenType peek(Parser* self) {
	return self->current.type;
}

// Returns next token type.
static TokenType peekNext(Parser* self) {
	return self->next.type;
}

// Consume the current token if it's expected and lex for the next token
// and return true otherwise reutrn false.
static bool match(Parser* self, TokenType expected) {
	if (peek(self) != expected) return false;
	lexToken(self);
	return true;
}

// Consume the the current token and if it's not [expected] emits error log
// and continue parsing for more error logs.
static void eatToken(Parser* self, TokenType expected, const char* err_msg) {
	if (lexToken(self) != expected) {
		// TODO: syntaxError(err_msg);

		// If the next token is expected discard the current to minimize
		// cascaded errors and continue parsing.
		if (peek(self) == expected) {
			lexToken(self);
		}
	}
}

/*****************************************************************************
 * PARSING GRAMMAR                                                           *
 *****************************************************************************/

// Forward declaration of grammar functions.

static void exprAssignment(Compiler* compiler, bool can_assign);

// Bool, Num, String, Null, -and- bool_t, Array_t, String_t, ...
static void exprLiteral(Compiler* compiler, bool can_assign);
static void exprName(Compiler* compiler, bool can_assign);


static void exprBinaryOp(Compiler* compiler, bool can_assign);
static void exprUnaryOp(Compiler* compiler, bool can_assign);

static void exprGrouping(Compiler* compiler, bool can_assign);
static void exprArray(Compiler* compiler, bool can_assign);
static void exprMap(Compiler* compiler, bool can_assign);

static void exprCall(Compiler* compiler, bool can_assign);
static void exprAttrib(Compiler* compiler, bool can_assign);
static void exprSubscript(Compiler* compiler, bool can_assign);

#define NO_RULE { NULL,          NULL,          PREC_NONE }
#define NO_INFIX PREC_NONE

GrammarRule rules[] = {  // Prefix       Infix             Infix Precedence
	/* TK_ERROR		 */   NO_RULE,
	/* TK_EOF		 */   NO_RULE,
	/* TK_LINE		 */   NO_RULE,
	/* TK_DOT		 */ { exprAttrib,    NULL,             PREC_ATTRIB },
	/* TK_DOTDOT	 */ { NULL,          exprBinaryOp,     PREC_RANGE },
	/* TK_COMMA		 */   NO_RULE,
	/* TK_COLLON	 */   NO_RULE,
	/* TK_SEMICOLLON */   NO_RULE,
	/* TK_HASH		 */   NO_RULE,
	/* TK_LPARAN	 */ { exprGrouping,  exprCall,         PREC_CALL },
	/* TK_RPARAN	 */   NO_RULE,
	/* TK_LBRACKET	 */ { exprArray,     exprSubscript,    PREC_SUBSCRIPT },
	/* TK_RBRACKET	 */   NO_RULE,
	/* TK_LBRACE	 */ { exprMap,       NULL,             NO_INFIX },
	/* TK_RBRACE	 */   NO_RULE,
	/* TK_PERCENT	 */ { NULL,          exprBinaryOp,     PREC_FACTOR },
	/* TK_TILD		 */ { exprUnaryOp,   NULL,             NO_INFIX },
	/* TK_AMP		 */ { NULL,          exprBinaryOp,     PREC_BITWISE_AND },
	/* TK_PIPE		 */ { NULL,          exprBinaryOp,     PREC_BITWISE_OR },
	/* TK_CARET		 */ { NULL,          exprBinaryOp,     PREC_BITWISE_XOR },
	/* TK_PLUS		 */ { NULL,          exprBinaryOp,     PREC_TERM },
	/* TK_MINUS		 */ { NULL,          exprBinaryOp,     PREC_TERM },
	/* TK_STAR		 */ { NULL,          exprBinaryOp,     PREC_FACTOR },
	/* TK_FSLASH	 */ { NULL,          exprBinaryOp,     PREC_FACTOR },
	/* TK_BSLASH	 */   NO_RULE,
	/* TK_EQ		 */ { NULL,          exprAssignment,   PREC_ASSIGNMENT },
	/* TK_GT		 */ { NULL,          exprBinaryOp,     PREC_COMPARISION },
	/* TK_LT		 */ { NULL,          exprBinaryOp,     PREC_COMPARISION },
	/* TK_EQEQ		 */ { NULL,          exprBinaryOp,     PREC_EQUALITY },
	/* TK_NOTEQ		 */ { NULL,          exprBinaryOp,     PREC_EQUALITY },
	/* TK_GTEQ		 */ { NULL,          exprBinaryOp,     PREC_COMPARISION },
	/* TK_LTEQ		 */ { NULL,          exprBinaryOp,     PREC_COMPARISION },
	/* TK_PLUSEQ	 */ { NULL,          exprAssignment,   PREC_ASSIGNMENT },
	/* TK_MINUSEQ	 */ { NULL,          exprAssignment,   PREC_ASSIGNMENT },
	/* TK_STAREQ	 */ { NULL,          exprAssignment,   PREC_ASSIGNMENT },
	/* TK_DIVEQ		 */ { NULL,          exprAssignment,   PREC_ASSIGNMENT },
	/* TK_SRIGHT	 */ { NULL,          exprBinaryOp,     PREC_BITWISE_SHIFT },
	/* TK_SLEFT		 */ { NULL,          exprBinaryOp,     PREC_BITWISE_SHIFT },
	/* TK_ENUM		 */   NO_RULE,
	/* TK_DEF		 */   NO_RULE,
	/* TK_EXTERN	 */   NO_RULE,
	/* TK_END		 */   NO_RULE,
	/* TK_NULL		 */   NO_RULE,
	/* TK_SELF		 */   NO_RULE,
	/* TK_IS		 */ { NULL,          exprBinaryOp,     PREC_IS },
	/* TK_IN		 */ { NULL,          exprBinaryOp,     PREC_IN },
	/* TK_AND		 */ { NULL,          exprBinaryOp,     PREC_LOGICAL_AND },
	/* TK_OR		 */ { NULL,          exprBinaryOp,     PREC_LOGICAL_OR },
	/* TK_NOT		 */ { NULL,          exprUnaryOp,      PREC_LOGICAL_NOT },
	/* TK_TRUE		 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_FALSE		 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_BOOL_T	 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_NUM_T		 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_STRING_T	 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_ARRAY_T	 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_MAP_T		 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_RANGE_T	 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_FUNC_T	 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_OBJ_T		 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_WHILE		 */   NO_RULE,
	/* TK_FOR		 */   NO_RULE,
	/* TK_IF		 */   NO_RULE,
	/* TK_ELIF		 */   NO_RULE,
	/* TK_ELSE		 */   NO_RULE,
	/* TK_BREAK		 */   NO_RULE,
	/* TK_CONTINUE	 */   NO_RULE,
	/* TK_RETURN	 */   NO_RULE,
	/* TK_NAME		 */ { exprName,      NULL,             NO_INFIX },
	/* TK_NUMBER	 */ { exprLiteral,   NULL,             NO_INFIX },
	/* TK_STRING     */ { exprLiteral,   NULL,             NO_INFIX },
};

static GrammarRule* getRule(TokenType type) {
	return &(rules[(int)type]);
}

static void exprAssignment(Compiler* compiler, bool can_assign) { /*TODO*/ }

static void exprLiteral(Compiler* compiler, bool can_assign) { /*TODO*/ }
static void exprName(Compiler* compiler, bool can_assign) { /*TODO*/ }


static void exprBinaryOp(Compiler* compiler, bool can_assign) { /*TODO*/ }
static void exprUnaryOp(Compiler* compiler, bool can_assign) { /*TODO*/ }

static void exprGrouping(Compiler* compiler, bool can_assign) { /*TODO*/ }
static void exprArray(Compiler* compiler, bool can_assign) { /*TODO*/ }
static void exprMap(Compiler* compiler, bool can_assign) { /*TODO*/ }

static void exprCall(Compiler* compiler, bool can_assign) { /*TODO*/ }
static void exprAttrib(Compiler* compiler, bool can_assign) { /*TODO*/ }
static void exprSubscript(Compiler* compiler, bool can_assign) { /*TODO*/ }


/*****************************************************************************
 * COMPILING                                                                 *
 *****************************************************************************/

