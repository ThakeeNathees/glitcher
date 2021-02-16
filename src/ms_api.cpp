/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "ms_api.h"

void MiniScript::_register_methods() {
	register_method("_init", &MiniScript::_init);
	register_method("_process", &MiniScript::_process);
	register_method("test_hello", &MiniScript::test_hello);
}

/*****************************************************************************/
/* MINISCRIPT CONFIGURATIONS                                                 */
/*****************************************************************************/

static void errorPrint(MSVM* vm, MSErrorType type, const char* file, int line,
  const char* message) {
	MiniScript* self = NULL; // msGetUserData(vm);
	TODO; //fprintf(stderr, "%s", text);
}

static void writeFunction(MSVM* vm, const char* text) {
	TODO; //fprintf(stdout, "%s", text);
}

static void loadScriptDone(MSVM* vm, const char* path, void* user_data) {
	TODO; //free(user_data);
}

static MSLoadScriptResult loadScript(MSVM* vm, const char* path) {
	MSLoadScriptResult result;
	result.is_failed = false;
	result.source = ""; /* <------- */ TODO;
	result.user_data = NULL;
	return result;
}

void MiniScript::_init() {
	MSConfiguration config;
	config.user_data = (void*)this;
	config.error_fn = errorPrint;
	config.load_script_fn = loadScript;
	config.load_script_done_fn = loadScriptDone;
	vm = msNewVM(&config);
}

/*****************************************************************************/
/* MINISCRIPT API METHODS                                                    */
/*****************************************************************************/

