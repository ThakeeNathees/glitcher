/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include <stdio.h>

#define CLOGGER_IMPLEMENT
#include "clogger.h"

// FIXME:
#include "../src/common.h"

int main() {
	clogger_init();
	clogger_logfError("[DummyError] dummy error\n");
	clogger_logfWarning("[DummyWarning] dummy warning\n");
	return 0;
}
