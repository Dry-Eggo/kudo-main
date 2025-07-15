#pragma once

typedef enum {
	CMD_INIT,
	CMD_RUN,
	CMD_BUILD,
	CMD_HELP,
} CMD_OPTION;

typedef enum {
	FUNC,
	IDENTIFIER,
	INTEGER,

	// punctuations
	OPAREN, CPAREN,
	OCURLY, CCURLY,
	COMMA,
	SEMI,

	// operators
	EQ, EQEQ, NEQ, ADDEQ, SUBEQ, MULEQ, DIVEQ,
	ADD, INC /* ++ */,
	SUB, DEC /* -- */,
	
	EOS // End Of Stream	
} TokenKind;
