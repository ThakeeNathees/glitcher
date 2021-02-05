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
	//clogger_logfError("[DummyError] dummy error\n");
	//clogger_logfWarning("[DummyWarning] dummy warning\n");

	FILE* fp = fopen("test.ms", "r");
	if (fp == NULL) {
		clogger_logfError("[Error] cannot open file test.ms\n");
		return 1;
	}

	char buff[1024];
	size_t read = fread(buff, 1, sizeof(buff), fp);
	fclose(fp);
	buff[read] = '\0';

	printf("%s\n", buff);

	return 0;
}
