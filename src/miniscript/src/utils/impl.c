/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

/** A source file to implement the utility declarations and header only 
 * utilities. 
 */

#define VAR_IMPLEMENT
#include "var.h"

#define UTF8_IMPLEMENT
#include "utf8.h"

#include "utils.h"

int ms_powerOf2Ceil(int n) {
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;

	return n;
}

bool ms_isName(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

bool ms_isDigit(char c) {
	return ('0' <= c && c <= '9');
}
